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

/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#include "mbedchannel.h"
#include "util.h"

#define MSG_TYPE_NORMAL 1
#define MSG_TYPE_LOG 2
//#define MSG_TYPE_BROADCAST 3
#define MSG_TYPE_HEARTBEAT 4
#define IDLE_CONNECTION_TIMEOUT 2000
#define MAX_PACKET_LEN 1024

namespace Soro {

#ifdef QT_CORE_LIB ///////////////////////////////////////////////////////////////////////

void MbedChannel::setChannelState(MbedChannel::State state)
{
    if (_state != state)
    {
        _state = state;
        Q_EMIT stateChanged(state);
    }
}

void MbedChannel::socketError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    LOG_E(LOG_TAG, "Error: " + _socket->errorString());
    START_TIMER(_resetConnectionTimerId, 500);
}

void MbedChannel::socketReadyRead()
{
    qint64 length;
    SocketAddress peer;
    while (_socket->hasPendingDatagrams())
    {
        length = _socket->readDatagram(_buffer, MAX_PACKET_LEN, &peer.host, &peer.port);
        if (peer.port != _host.port) continue; // Port must be the same on both sides
        if (_buffer[0] == '\0')
        {
            //'\0' is the message header for the server
            // It's possible we broadcasted our own message back to us
            continue;
        }
        if (length < 6)
        {
            LOG_W(LOG_TAG, "Received a message that was too short");
            continue;
        }
        if (length == MAX_PACKET_LEN)
        {
            LOG_E(LOG_TAG, "Received a packet that was too long, tell Jacob to up MAX_PACKET_LEN");
            continue;
        }
        if (_buffer[0] != _mbedId)
        {
            // Received a message from the wrong mbed. This may happen if more than one MbedChannel
            // are configured on the same port. Just ignore it.
            continue;
        }
        // By this point, we have verified the packet is from the mbed we want
        _mbed.host = peer.host;
        _mbed.port = peer.port;

        unsigned int sequence = Util::deserialize<unsigned int>(_buffer + 2);
        if (_state == ConnectingState)
        {
            LOG_I(LOG_TAG, "Connected to mbed client");
            setChannelState(ConnectedState);
        }
        else if (sequence < _lastReceiveId) continue; // Ignore packets that are older than the last one we got
        _lastReceiveId = sequence;
        _active = true; // Mark the mbed as active so it doesn't time out

        // See what type of message we got
        switch (static_cast<unsigned char>(_buffer[1]))
        {
        case MSG_TYPE_NORMAL: // Normal message, emit messageReceived
            if (length > 6)
            {
                Q_EMIT messageReceived(_buffer + 6, length - 6);
            }
            break;
        case MSG_TYPE_LOG: // Mbed wants to log something
            LOG_I(LOG_TAG, "Mbed:" + QString(_buffer + 6));
            break;
        /*case MSG_TYPE_BROADCAST: // Handshake message
            LOG_I(LOG_TAG, "Responding to handshake from mbed");
            sendMessage(nullptr, 0);
            break;*/
        case MSG_TYPE_HEARTBEAT: // Heartbeat message
            break;
        default:
            LOG_E(LOG_TAG, "Got message with unknown type");
            break;
        }
    }
}

void MbedChannel::resetConnection()
{
    LOG_I(LOG_TAG, "Connection is resetting...");
    setChannelState(ConnectingState);
    _lastReceiveId = 0;
    _active = false;
    _socket->abort();
    if (_socket->bind(_host.host, _host.port))
    {
        LOG_I(LOG_TAG, "Listening on UDP port " + _host.toString());
        _socket->open(QIODevice::ReadWrite);
    }
    else
    {
        LOG_E(LOG_TAG, "Failed to bind to " + _host.toString());
    }
}

MbedChannel::MbedChannel(SocketAddress host, unsigned char mbedId, QObject *parent) : QObject(parent)
{
    _host = host;
    _buffer = new char[MAX_PACKET_LEN];
    _socket = new QUdpSocket(this);
    _mbedId = static_cast<char>(mbedId);
    LOG_TAG = "Mbed(" + QString::number(mbedId) + ")";
    LOG_I(LOG_TAG, "Creating new mbed channel");
    connect(_socket, &QUdpSocket::readyRead, this, &MbedChannel::socketReadyRead);
    connect(_socket, static_cast<void (QUdpSocket::*)(QUdpSocket::SocketError)>(&QUdpSocket::error), this, &MbedChannel::socketError);
    resetConnection();
    START_TIMER(_watchdogTimerId, IDLE_CONNECTION_TIMEOUT);
}

MbedChannel::~MbedChannel()
{
    _socket->abort();
    delete _socket;
    delete _buffer;
}

void MbedChannel::sendMessage(const char *message, int length)
{
    if ((_state == ConnectedState) && (length < 500))
    {
        _buffer[0] = '\0';
        _buffer[1] = _mbedId;
        Util::serialize<unsigned int>(_buffer + 2, _nextSendId++);
        memcpy(_buffer + 6, message, length);
        _socket->writeDatagram(_buffer, length + 6, _mbed.host, _mbed.port);
    }
}

void MbedChannel::timerEvent(QTimerEvent *e)
{
    QObject::timerEvent(e);
    if (e->timerId() == _watchdogTimerId)
    {
        if ((_state == ConnectedState) & !_active)
        {
            LOG_E(LOG_TAG, "Mbed client has timed out");
            setChannelState(ConnectingState);
        }
        _active = false;
    }
    else if (e->timerId() == _resetConnectionTimerId)
    {
        resetConnection();
        KILL_TIMER(_resetConnectionTimerId); //single shot
    }
}

MbedChannel::State MbedChannel::getState() const
{
    return _state;
}

#endif
#ifdef TARGET_LPC1768 ////////////////////////////////////////////////////////////////

DigitalOut _led1(LED1);
DigitalOut _led2(LED2);
DigitalOut _led3(LED3);
DigitalOut _led4(LED4);

extern "C" void mbed_reset();

void MbedChannel::panic()
{
    _led1 = 1;
    _led2 = 0;
    _led3 = 0;
    _led4 = 1;
    wait(5);
    reset();
}

void MbedChannel::reset()
{
    if (_resetCallback != NULL)
    {
        _resetCallback();
    }
    mbed_reset();
}

// We no longer send messages to a specific IP, we just broadcast. So this method is not used
/*void MbedChannel::loadConfig()
{
    LocalFileSystem local("local");
    FILE *configFile = fopen("/local/server.txt", "r");
    if (configFile != NULL)
    {
        char *line = new char[64];
        fgets(line, 64, configFile);
        fclose(configFile);
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == ':')
            {
                char *ip = new char[i + 1];
                strncpy(ip, line, i);
                ip[i] = '\0';
                unsigned int port = (unsigned int)atoi(line + i + 1);
                if ((port == 0) | (port > 65535)) break;
                _server.set_address(ip, port);
                return;
            }
        }
    }
    //an error occurred
    panic();
}*/

/*bool MbedChannel::setServerAddress()
{
    LocalFileSystem local("local");
    FILE *configFile = fopen("/local/server.txt", "r");
    if (configFile != NULL)
    {
        char *line = new char[64];
        fgets(line, 64, configFile);
        fclose(configFile);
        unsigned int port = (unsigned int)atoi(line);
        if ((port == 0) | (port > 65535))
        {
            return false;
        }
        char *ip = _eth->getIPAddress();
        char *broadcast = new char[strlen(ip) + 3]; //make sure we have enough room to add 2 more digits
        strcpy(broadcast, ip);
        strcpy(strrchr(broadcast, '.'), ".255");
        // set address as broadcast address and port from file
        _server.set_address(broadcast, port);
    }
    return true;
}*/

void MbedChannel::initConnection()
{
    Serial pc(USBTX, USBRX);
    pc.printf("Port: %u\r\n", _serverPort); // DEBUG
    //initialize ethernet interface
    _led1 = 1;
    if (_eth->init() != 0)
    {
        panic();
    }
    // connect to ethernet and get address through DHCP
    _led2 = 1;
    if (_eth->connect() != 0)
    {
        panic();
    }
    _led3 = 1;

    // Ensure the network allows broadcasting
    if (strcmp(strrchr(_eth->getNetworkMask(), '.'), ".0") != 0)
    {
        // Network doesn't allow broadcasting
        panic();
    }

    // Get the broadcast address
    char *ip = _eth->getIPAddress();
    pc.printf("IP: %s\r\n", ip); // DEBUG
    char *broadcast = new char[strlen(ip) + 3]; //make sure we have enough room to add 2 more digits
    strcpy(broadcast, ip);
    strcpy(strrchr(broadcast, '.'), ".255");
    pc.printf("Broadcast: %s\r\n", broadcast); // DEBUG
    _server.set_address(broadcast, _serverPort);

    setTimeout(IDLE_CONNECTION_TIMEOUT / 3);
    //initialize socket
    while (_socket->bind(_serverPort) != 0)
    {
        wait(0.2);
        _led3 = 0;
        wait(0.2);
        _led3 = 1;
    }
    if (_socket->set_broadcasting(true) != 0)
    {
        // Cannot set broadcasting on socket
        panic();
    }

    _led4 = 1;
    wait(1);
    _led1 = 0;
    _led2 = 0;
    _led3 = 0;
    _led4 = 0;

    /*Endpoint peer;
    while (1)
    {
        //send broadcast handshake
        sendMessage(NULL, 0, MSG_TYPE_BROADCAST);
        pc.printf("Sending handshake message...\r\n"); // DEBUG
        while (1)
        {
            //recieve any responses
            int len = _socket->receiveFrom(peer, _buffer, MAX_PACKET_LEN);
            if ((len < 6) || (len == MAX_PACKET_LEN)) break;
            if (_buffer[0] == '\0' && _buffer[1] == _mbedId)
            {
                //received a response from the server
                _server.set_address(peer.get_address(), _serverPort);
                if (_socket->set_broadcasting(false) != 0)
                {
                    panic();
                }
                _led1 = 0;
                _led2 = 0;
                _led3 = 0;
                _led4 = 0;
                return;
            }
        }
        wait(0.2);
        _led4 = 0;
        wait(0.2);
        _led4 = 1;
    }*/
}

MbedChannel::MbedChannel(unsigned char mbedId, unsigned int port)
{
    _resetCallback = NULL;
    _buffer = new char[MAX_PACKET_LEN];
    _mbedId = static_cast<char>(mbedId);
    _eth = new EthernetInterface;
    _socket = new UDPSocket;
    _serverPort = port;
    initConnection();
    _lastSendTime = time(NULL);
    _lastReceiveId = 0;
    _nextSendId = 0;
}

MbedChannel::~MbedChannel()
{
    _socket->close();
    delete _socket;
    delete _buffer;
    // This class is managing the ethernet
    _eth->disconnect();
    delete _eth;
}

void MbedChannel::setTimeout(unsigned int millis)
{
    if (millis < IDLE_CONNECTION_TIMEOUT / 2)
        _socket->set_blocking(false, millis);
}

void MbedChannel::sendMessage(char *message, int length)
{
    sendMessage(message, length, MSG_TYPE_NORMAL);
}

/* Sends a log message to the server.
 */
void MbedChannel::log(char *message)
{
    sendMessage(message, strlen(message) + 1, MSG_TYPE_LOG);
}

void MbedChannel::sendMessage(char *message, int length, unsigned char type)
{
    if (!isEthernetActive())
    {
        panic();
    }
    _buffer[0] = _mbedId;
    _buffer[1] = static_cast<char>(type);
    Util::serialize<unsigned int>(_buffer + 2, _nextSendId++);
    memcpy(_buffer + 6, message, length);
    _socket->sendTo(_server, _buffer, length + 6);
    _lastSendTime = time(NULL);
    if (_led2 == 1) _led2 = 0;
    else _led2 = 1;
}

void MbedChannel::setResetListener(void (*callback)(void))
{
    _resetCallback = callback;
}

int MbedChannel::read(char *outMessage, int maxLength)
{
    if (!isEthernetActive())
    {
        panic();
    }
    Endpoint peer;
    //Check if a heartbeat should be sent
    if (time(NULL) - _lastSendTime >= 1)
    {
        sendMessage(NULL, 0, MSG_TYPE_HEARTBEAT);
    }
    int len = _socket->receiveFrom(peer, _buffer, maxLength);
    unsigned int sequence = Util::deserialize<unsigned int>(_buffer + 2);
    if ((len < 6)
            || (peer.get_port() != _serverPort)
            || (_buffer[0] != '\0')
            || (_buffer[1] != _mbedId))
    {
        return -1;
    }
    if (_led1 == 1) _led1 = 0;
    else _led1 = 1;
    if ((sequence < _lastReceiveId) && (time(NULL) - _lastReceiveTime < 2))
    {
        return -1;
    }
    _lastReceiveId = sequence;
    _lastReceiveTime = time(NULL);
    memcpy(outMessage, _buffer + 6, len - 6);
    return len - 6;
}
#endif

}
