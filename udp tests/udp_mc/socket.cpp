#include "socket.h"
// my laptop is 192.168.1.9
// soro desktop is 192.168.1.5

socket::socket(QObject *parent) : QObject(parent)
{
    socketIn = new QUdpSocket(this);
    socketOut = new QUdpSocket(this);
    if(socketIn->bind(QHostAddress::LocalHost, 1237))
    {
        qDebug() << "Bound to port 1237";
    } else {
        qDebug() << "Error binding to port:" << socketIn->errorString();
    }
    connect(socketIn, SIGNAL(readyRead()), this, SLOT(readUDP()));
}

void socket::sendUDP(const char* message)
{
    QByteArray Data;
    Data.append(message);

    // Sends the datagram datagram
    // to the host address and at port.
    // qint64 QUdpSocket::writeDatagram(const QByteArray & datagram,
    //                      const QHostAddress & host, quint16 port)
    socketOut->writeDatagram(Data, QHostAddress::LocalHost, 1234);
}

void socket::readUDP()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(socketIn->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socketIn->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}
