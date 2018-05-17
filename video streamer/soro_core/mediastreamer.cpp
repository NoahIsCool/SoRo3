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

#include "mediastreamer.h"
#include "logger.h"
#include "constants.h"

namespace Soro {

MediaStreamer::MediaStreamer(QString LOG_TAG, QObject *parent) : QObject(parent) {
    this->LOG_TAG = LOG_TAG;
}

MediaStreamer::~MediaStreamer() {
    stop();
}

void MediaStreamer::stop() {
    if (_pipeline) {
        LOG_I(LOG_TAG, "stop(): setting pipeline to StateNull");
        _pipeline->bus()->removeSignalWatch();
        _pipeline->setState(QGst::StateNull);
        _pipeline.clear();
    }
    if (_ipcSocket) {
        LOG_I(LOG_TAG, "stop(): deleting IPC socket");
        disconnect(_ipcSocket, 0, this, 0);
        delete _ipcSocket;
        _ipcSocket = nullptr;
    }
}

bool MediaStreamer::connectToParent(quint16 port) {
    LOG_I(LOG_TAG, "connectToParent(): Creating new TCP socket on port " + QString::number(port));
    _ipcSocket = new QTcpSocket(this);

    connect(_ipcSocket, &QTcpSocket::readyRead, this, &MediaStreamer::ipcSocketReadyRead);
    connect(_ipcSocket, &QTcpSocket::disconnected, this, &MediaStreamer::ipcSocketDisconnected);
    connect(_ipcSocket, static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error), this, &MediaStreamer::ipcSocketError);

    _ipcSocket->connectToHost(QHostAddress::LocalHost, port);
    if (!_ipcSocket->waitForConnected(1000)) {
        LOG_E(LOG_TAG, "connectToParent(): Unable to connect to parent");
        QCoreApplication::exit(0);
        return false;
    }
    return true;
}

void MediaStreamer::ipcSocketReadyRead() {
    char buffer[512];
    while (_ipcSocket->bytesAvailable() > 0) {
        _ipcSocket->readLine(buffer, 512);
        if (QString(buffer).compare("stop") == 0) {
            LOG_I(LOG_TAG, "ipcSocketReadyRead(): Got stop request from parent");
            stop();
            QCoreApplication::exit(0);
        }
        else {
            LOG_W(LOG_TAG, "ipcSocketReadyRead(): Got unknown request from parent '" + QString(buffer) + "'");
        }
    }
}

QGst::PipelinePtr MediaStreamer::createPipeline() {
    QGst::PipelinePtr pipeline = QGst::Pipeline::create();
    pipeline->bus()->addSignalWatch();
    QGlib::connect(pipeline->bus(), "message", this, &MediaStreamer::onBusMessage);

    LOG_I(LOG_TAG, "createPipeline(): pipeline created successfully");
    return pipeline;
}

void MediaStreamer::onBusMessage(const QGst::MessagePtr & message) {
    LOG_I(LOG_TAG, "onBusMessage(): Got bus message type " + message->typeName());
    QByteArray errorMessage;
    switch (message->type()) {
    case QGst::MessageEos:
        LOG_E(LOG_TAG, "onBusMessage(): Received EOS message from gstreamer");
        QCoreApplication::exit(STREAMPROCESS_ERR_GSTREAMER_EOS);
        break;
    case QGst::MessageError:
        errorMessage = message.staticCast<QGst::ErrorMessage>()->error().message().toLatin1();
        LOG_E(LOG_TAG, "onBusMessage(): Received error message from gstreamer '" + errorMessage + "'");
        QCoreApplication::exit(STREAMPROCESS_ERR_GSTREAMER_ERROR);
        break;
    default:
        break;
    }
}

void MediaStreamer::ipcSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    LOG_E(LOG_TAG, "ipcSocketError(): Socket error");
    stop();
    QCoreApplication::exit(STREAMPROCESS_ERR_SOCKET_ERROR);
}

void MediaStreamer::ipcSocketDisconnected() {
    LOG_E(LOG_TAG, "ipcSocketDisconnected(): Socket disconnected");
    stop();
    QCoreApplication::exit(STREAMPROCESS_ERR_SOCKET_ERROR);
}

} // namespace Soro

