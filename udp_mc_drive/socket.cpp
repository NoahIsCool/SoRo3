#include "socket.h"
// my laptop is 192.168.1.9
// soro desktop is 192.168.1.5

socket::socket(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    if(udpSocket->bind(QHostAddress("192.168.1.101"), 1237))
    {
        qDebug() << "Bound to port 1237";
    } else {
        qDebug() << "Error binding to port:" << udpSocket->errorString();
    }
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readUDP()));
}

void socket::sendUDP(QByteArray Data)
{
    udpSocket->writeDatagram(Data, QHostAddress("192.168.1.102"), 1234);
}

void socket::readUDP()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(static_cast<int>(udpSocket->pendingDatagramSize()));

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize, QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}
