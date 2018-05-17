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

#include "gpsserver.h"
#include "soro_core/logger.h"

#define LOG_TAG "GpsServer"

#define RECOVERY_DELAY 500

namespace Soro {

GpsServer::GpsServer(SocketAddress hostAddress, QObject *parent) : QObject(parent) {
    _socket = new QUdpSocket(this);
    connect(_socket, &QUdpSocket::readyRead, this, &GpsServer::socketReadyRead);
    connect(_socket, static_cast<void (QUdpSocket::*)(QUdpSocket::SocketError)>(&QUdpSocket::error), this, &GpsServer::socketError);
    LOG_I(LOG_TAG, "Creating GPS server with host address " + hostAddress.toString());
    _hostAddress = hostAddress;
    resetConnection();
}

GpsServer::~GpsServer() {
    delete _socket;
}

void GpsServer::socketReadyRead() {
    LOG_D(LOG_TAG, "socketReadyRead() called");
    SocketAddress address;
    qint64 status;
    while (_socket->hasPendingDatagrams()) {
        //read in a datagram
        status = _socket->readDatagram(_buffer, 1024, &address.host, &address.port);
        if (status < 0) {
            //an error occurred reading from the socket, the onSocketError slot will handle it
            return;
        }
        QString nmea = QString(_buffer);
        NmeaMessage nmeaMessage(nmea);

        if ((nmeaMessage.Latitude != 0) && (nmeaMessage.Longitude != 0)) {
            LOG_I(LOG_TAG, "Received GPS packet");
            Q_EMIT gpsUpdate(nmeaMessage);
        }
        else {
            LOG_W(LOG_TAG, "Received invalid GPS message on server port");
        }
    }
}

void GpsServer::socketError(QAbstractSocket::SocketError err) {
    Q_EMIT connectionError(err);
    LOG_E(LOG_TAG, "Server Error: " + _socket->errorString());
    START_TIMER(_resetConnectionTimerId, RECOVERY_DELAY);
}

void GpsServer::resetConnection() {
    _socket->abort();
    _socket->bind(_hostAddress.host, _hostAddress.port);
    _socket->open(QIODevice::ReadWrite);
}

void GpsServer::timerEvent(QTimerEvent *e) {
    QObject::timerEvent(e);
    if (e->timerId() == _resetConnectionTimerId) {
        resetConnection();
        KILL_TIMER(_resetConnectionTimerId);
    }
}

} // namespace Soro
