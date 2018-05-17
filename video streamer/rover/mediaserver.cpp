/*
 * Copyright 2016 The University of Oklahoma.
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

#include "mediaserver.h"
#include "soro_core/logger.h"

namespace Soro {

MediaServer::MediaServer(QString logTag, int mediaId, QString childProcessPath, quint16 bindPort, QObject *parent) : QObject(parent) {
    LOG_TAG = logTag;
    LOG_I(LOG_TAG, "MediaServer(): Creating new media server on port " + QString::number(bindPort) + " with child process '" + childProcessPath + "'");

    _bindPort = bindPort;
    _mediaId = mediaId;

    _controlChannel = Channel::createServer(this, bindPort, "soro_media" + QString::number(mediaId), Channel::TcpProtocol);
    _controlChannel->open();

    connect(_controlChannel, &Channel::stateChanged, this, &MediaServer::controlChannelStateChanged);

    _mediaSocket = new QUdpSocket(this);

    _ipcServer = new QTcpServer(this);
    _ipcServer->listen(QHostAddress::LocalHost);
    connect(_ipcServer, &QTcpServer::newConnection, this, &MediaServer::ipcServerClientAvailable);

    _child.setProgram(childProcessPath);

    _state = IdleState;

    // Learn a little bit about the streaming binary we were given
    QFileInfo childBinInfo(childProcessPath);
    if (!childBinInfo.exists() || !childBinInfo.isReadable()) {
        LOG_E(LOG_TAG, "!!!! The binary '" + childProcessPath + "' does not exist or cannot be accessed. Streaming WILL NOT work !!!!");
    }
    else if (!childBinInfo.isExecutable()) {
        LOG_W(LOG_TAG, "!!!! The binary '" + childProcessPath + "' does not have execute permission. Executing 'chmod +x' in attempt to fix !!!!");
        QProcess chmod;
        chmod.start("chmod", QStringList() << "+x" << childProcessPath);
        chmod.waitForFinished(1000);
    }
}

MediaServer::~MediaServer() {
    LOG_I(LOG_TAG, "~MediaServer(): Stopping");
    stop();
}

void MediaServer::beginStream(SocketAddress address) {
    QStringList args;
    constructChildArguments(args, _bindPort, address, _ipcServer->serverPort());
    _child.setArguments(args);

    connect(&_child, &QProcess::stateChanged, this, &MediaServer::childStateChanged);
    _child.start();

    LOG_I(LOG_TAG, "beginStream(): Sending streaming message to client");
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << QString("streaming");
    constructStreamingMessage(stream);
    LOG_I(LOG_TAG, "beginStream(): Sending stream configuration to client");
    _controlChannel->sendMessage(message.constData(), message.size());

    setState(StreamingState);
}

void MediaServer::stop() {
    if (_state == IdleState) {
        LOG_I(LOG_TAG, "stop(): Server is already stopped");
        return;
    }
    if (_child.state() != QProcess::NotRunning) {
        LOG_I(LOG_TAG, "stop(): Asking the streaming process to stop");
        disconnect(&_child, &QProcess::stateChanged, this, &MediaServer::childStateChanged);
        if (_ipcSocket) {
            _ipcSocket->write("stop");
            _ipcSocket->flush();
            if (!_child.waitForFinished(1000)) {
                LOG_E(LOG_TAG, "stop(): Streaming process did not respond to stop request, terminating it");
                _child.terminate();
                _child.waitForFinished();
                LOG_I(LOG_TAG, "stop(): Streaming process has been terminated");
            }
            else {
                LOG_I(LOG_TAG, "stop(): Streaming process has exited gracefully");
            }
        }
        else {
            LOG_E(LOG_TAG, "stop(): Streaming process is not connected to the rover process, terminating it");
            _child.terminate();
            _child.waitForFinished();
            LOG_I(LOG_TAG, "stop(): Streaming process has been terminated");
        }
    }

    if (_ipcSocket) {
        LOG_I(LOG_TAG, "stop(): IPC socket is active, closing it");
        disconnect(_ipcSocket, 0, 0, 0);
        _ipcSocket->abort();
        delete _ipcSocket;
        _ipcSocket = nullptr;
    }

    onStreamStoppedInternal();

    if (_controlChannel->getState() == Channel::ConnectedState) {
        // notify the client that the server is stopping the stream
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << QString("eos");
        _controlChannel->sendMessage(message.constData(), message.size());
    }
    _mediaSocket->abort();
    setState(IdleState);
}

void MediaServer::initStream() {
    if (_state != IdleState) {
        LOG_I(LOG_TAG, "initStream(): Stream is not idle, but you still want to start it. The stream will be stopped and then restarted with the new configuration.");
        stop();
    }
    LOG_I(LOG_TAG, "initStream(): Starting handshake process");
    setState(WaitingState);
    beginClientHandshake();
}

void MediaServer::beginClientHandshake() {
    if (_state != WaitingState) return;
    if (_controlChannel->getState() == Channel::ConnectedState) {
        _mediaSocket->abort();
        if (!_mediaSocket->bind(_bindPort)) {
            LOG_E(LOG_TAG, "beginClientHandshake(): Cannot bind to UDP socket " + QString::number(_bindPort) + ": " + _mediaSocket->errorString());
            QTimer::singleShot(500, this, SLOT(beginClientHandshake()));
            return;
        }
        connect(_mediaSocket, &QUdpSocket::readyRead, this, &MediaServer::mediaSocketReadyRead);
        _mediaSocket->open(QIODevice::ReadWrite);
        // notify a connected client that there is about to be a stream change
        // and they should verify their UDP address
        LOG_I(LOG_TAG, "beginClientHandshake(): Sending stream start message to client");
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << QString("start");
        _controlChannel->sendMessage(message.constData(), message.size());
        // client must respond on its UDP address within a certain time or the process will start again
        QTimer::singleShot(3000, this, SLOT(beginClientHandshake()));

    }
    else {
        LOG_I(LOG_TAG, "beginClientHandshake(): Waiting for client to respond...");
        QTimer::singleShot(500, this, SLOT(beginClientHandshake()));
    }
}

void MediaServer::ipcServerClientAvailable() {
    if (!_ipcSocket) {
        _ipcSocket = _ipcServer->nextPendingConnection();
        LOG_I(LOG_TAG, "ipcServerClientAvailable(): Streaming process is connected to its parent through TCP");
    }
}

void MediaServer::mediaSocketReadyRead() {
    if (!_mediaSocket | (_state == StreamingState)) return;
    SocketAddress peer;
    char buffer[100];
    int length = _mediaSocket->readDatagram(&buffer[0], 100, &peer.host, &peer.port);

    QByteArray byteArray = QByteArray::fromRawData(buffer, length);
    QDataStream stream(byteArray);
    QString tag;
    int mediaId;
    stream >> tag;
    stream >> mediaId;

    if (tag.compare("soro_media", Qt::CaseInsensitive) != 0) {
        LOG_E(LOG_TAG, "mediaSocketReadyRead(): Got invalid handshake packet on UDP media port");
        return;
    }
    if (mediaId != _mediaId) {
        LOG_E(LOG_TAG, "mediaSocketReadyRead(): Got wrong media ID during UDP handshake, check your port configuration");
        return;
    }
    LOG_I(LOG_TAG, "Client has completed handshake on its UDP address");
    // Disconnect the media UDP socket so udpsink can bind to it
    disconnect(_mediaSocket, &QUdpSocket::readyRead, this, &MediaServer::mediaSocketReadyRead);
    _mediaSocket->abort(); // MUST ABORT THE SOCKET!!!!
    beginStream(peer);
}

void MediaServer::childStateChanged(QProcess::ProcessState state) {
    switch (state) {
    case QProcess::NotRunning:
        disconnect(&_child, &QProcess::stateChanged, this, &MediaServer::childStateChanged);

        switch (_child.exitCode()) {
        case 0:
        case STREAMPROCESS_ERR_GSTREAMER_EOS:
            LOG_I(LOG_TAG, "childStateChanged(): Streaming process has exited normally");
            Q_EMIT eos(this);
            break;
        case STREAMPROCESS_ERR_GSTREAMER_ERROR:
            LOG_E(LOG_TAG, "childStateChanged(): The streaming processes exited due to a gstreamer error");
            Q_EMIT error(this, "Streaming process exited due to a gstreamer error");
            break;
        case STREAMPROCESS_ERR_INVALID_ARGUMENT:
        case STREAMPROCESS_ERR_NOT_ENOUGH_ARGUMENTS:
        case STREAMPROCESS_ERR_UNKNOWN_CODEC:
            LOG_E(LOG_TAG, "childStateChanged(): Streaming processes exited due to an argument error");
            Q_EMIT error(this, "Streaming processes exited due to an argument error");
            break;
        case STREAMPROCESS_ERR_SOCKET_ERROR:
            LOG_E(LOG_TAG, "childStateChanged(): Streaming process exited because it lost contact with the parent cameraprocess");
            Q_EMIT error(this, "Streaming process exited because it lost contact with the parent process");
            break;
        default:
            LOG_E(LOG_TAG, "childStateChanged(): Streaming process exited due to an unknown error (exit code " + QString::number(_child.exitCode()) + ")");
            Q_EMIT error(this, "Streaming process exited due to an unknown error (exit code " + QString::number(_child.exitCode()) + ")");
            break;
        }

        stop();
        break;
    case QProcess::Starting:
        LOG_I(LOG_TAG, "childStateChanged(): Child is starting...");
        break;
    case QProcess::Running:
        LOG_I(LOG_TAG, "childStateChanged(): Child is running");
        break;
    }
}

void MediaServer::controlChannelStateChanged(Channel::State state) {
    if (state != Channel::ConnectedState) {
        stop();
    }
}

int MediaServer::getMediaId() {
    return _mediaId;
}

MediaServer::State MediaServer::getState() const {
    return _state;
}

void MediaServer::setState(MediaServer::State state) {
    if (_state != state) {
        LOG_I(LOG_TAG, "Changing to state " + QString::number(static_cast<qint32>(state)));
        _state = state;
        Q_EMIT stateChanged(this, state);
    }
}

} // namespace Soro
