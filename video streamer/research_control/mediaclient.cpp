/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mediaclient.h"

#include "soro_core/logger.h"

namespace Soro {

MediaClient::MediaClient(QString logTag, int mediaId, SocketAddress server, QHostAddress host, QObject *parent)
    : QObject(parent)
{

    LOG_TAG = logTag;
    _mediaId = mediaId;
    _server = server;

    LOG_I(LOG_TAG, "Creating new media client for server at " + server.toString());

    _controlChannel = Channel::createClient(this, _server, "soro_media" + QString::number(mediaId), Channel::TcpProtocol, host);
    _mediaSocket = new QUdpSocket(this);

    _buffer = new char[65536];

    connect(_controlChannel, &Channel::messageReceived, this, &MediaClient::controlMessageReceived);

    _controlChannel->open();

    // MUST connect this signal after opening the channel, otherwise the stateChanged()
    // signal will be emitted and trigger an invoking of a virtual method in
    // this class before its superclass implementation has been added to the vtable
    // in the superclass's own constructor
    connect(_controlChannel, &Channel::stateChanged, this, &MediaClient::controlChannelStateChanged);

    if (_controlChannel->getState() == Channel::ErrorState)
    {
        LOG_E(LOG_TAG, "The TCP channel could not be initialized");
    }

    if (!_mediaSocket->bind(host))
    {
        LOG_E(LOG_TAG, "Failed to bind to UDP socket");
    }

    _mediaSocket->open(QIODevice::ReadWrite);

    START_TIMER(_calculateBitrateTimerId, 1000);
}

MediaClient::~MediaClient()
{
    if (_controlChannel)
    {
        disconnect(_controlChannel, 0, 0, 0);
        _controlChannel->close();
        delete _controlChannel;
    }
    if (_mediaSocket)
    {
        disconnect(_mediaSocket, 0, 0, 0);
        if (_mediaSocket->isOpen()) _mediaSocket->close();
        delete _mediaSocket;
    }
}

void MediaClient::addForwardingAddress(SocketAddress address)
{
    for (SocketAddress existing : _forwardAddresses)
    {
        if (existing == address) return;
    }
    _forwardAddresses.append(address);
}

void MediaClient::removeForwardingAddress(SocketAddress address)
{
    int index = _forwardAddresses.indexOf(address);
    if (index >= 0)
    {
        _forwardAddresses.removeAt(index);
    }
}

void MediaClient::controlMessageReceived(const char *message, Channel::MessageSize size)
{
    Q_UNUSED(size);
    QByteArray byteArray = QByteArray::fromRawData(message, size);
    QDataStream stream(byteArray);
    stream.setByteOrder(QDataStream::BigEndian);
    QString messageType;
    stream >> messageType;
    LOG_E(LOG_TAG, "Got message: " + messageType);
    if (messageType.compare("start", Qt::CaseInsensitive) == 0)
    {
        LOG_I(LOG_TAG, "Server has notified us of a new media stream");
        disconnect(_mediaSocket, &QUdpSocket::readyRead, 0, 0);
        START_TIMER(_punchTimerId, 100);
        onServerStartMessageInternal();
        setState(ConnectedState);
    }
    else if (messageType.compare("streaming", Qt::CaseInsensitive) == 0)
    {
        // we were successful and are now receiving a media stream
        LOG_I(LOG_TAG, "Server has confirmed our address and should begin streaming");
        _errorString = ""; // clear error string since we have an active connection;
        mediaSocketReadyRead();
        connect(_mediaSocket, &QUdpSocket::readyRead, this, &MediaClient::mediaSocketReadyRead);
        KILL_TIMER(_punchTimerId);
        onServerStreamingMessageInternal(stream);
        setState(StreamingState);
    }
    else if (messageType.compare("eos", Qt::CaseInsensitive) == 0)
    {
        LOG_I(LOG_TAG, "Got EOS message from server");
        KILL_TIMER(_punchTimerId);
        disconnect(_mediaSocket, &QUdpSocket::readyRead, 0, 0);
        _lastBitrate = 0;
        onServerEosMessageInternal();
        setState(ConnectedState);
    }
    else if (messageType.compare("error", Qt::CaseInsensitive) == 0)
    {
        stream >> _errorString;
        LOG_I(LOG_TAG, "Got error message from server: " + _errorString);
        disconnect(_mediaSocket, &QUdpSocket::readyRead, 0, 0);
        _lastBitrate = 0;
        KILL_TIMER(_punchTimerId);
        onServerErrorMessageInternal();
        setState(ConnectedState);
    }
    else {
        LOG_E(LOG_TAG, "Got unknown message from media server");
    }
}

void MediaClient::mediaSocketReadyRead()
{
    qint64 size;
    while (_mediaSocket->hasPendingDatagrams())
    {
        size = _mediaSocket->readDatagram(_buffer, 65536);
        // update bit total
        _bitCount += size * 8;
        // forward the datagram to all specified addresses
        for (SocketAddress address : _forwardAddresses)
        {
            _mediaSocket->writeDatagram(_buffer, size, address.host, address.port);
        }
    }
}

void MediaClient::timerEvent(QTimerEvent *e)
{
    QObject::timerEvent(e);
    if (e->timerId() == _punchTimerId)
    {
        LOG_I(LOG_TAG, "punch timer tick");
        // send data to the the server so it can figure out our address
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        stream << QString("soro_media");
        stream << _mediaId;
        _mediaSocket->writeDatagram(message.constData(), message.size(), _server.host, _server.port);
    }
    else if (e->timerId() == _calculateBitrateTimerId)
    {
        // this timer runs twice per second to calculate the bitrate received by the client
        _lastBitrate = _bitCount;
        _bitCount = 0;
    }
}

void MediaClient::controlChannelStateChanged(Channel::State state)
{
    switch (state)
    {
    case Channel::ConnectedState:
        setState(ConnectedState);
        onServerConnectedInternal();
        break;
    default:
        setState(ConnectingState);
        disconnect(_mediaSocket, &QUdpSocket::readyRead, 0, 0);
        KILL_TIMER(_punchTimerId);
        onServerDisconnectedInternal();
        break;
    }
}

QString MediaClient::getErrorString() const
{
    return _errorString;
}

int MediaClient::getMediaId() const
{
    return _mediaId;
}

MediaClient::State MediaClient::getState() const
{
    return _state;
}

SocketAddress MediaClient::getServerAddress() const
{
    return _server;
}

SocketAddress MediaClient::getHostAddress() const
{
    return SocketAddress(_mediaSocket->localAddress(), _mediaSocket->localPort());
}

int MediaClient::getBitrate() const
{
    return _lastBitrate;
}

void MediaClient::setState(State state)
{
    if (_state != state)
    {
        _state = state;
        Q_EMIT stateChanged(this, _state);
    }
}


} // namespace Soro
