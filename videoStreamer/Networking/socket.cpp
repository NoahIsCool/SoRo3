#include "socket.h"
// my laptop is 192.168.1.9
// soro desktop is 192.168.1.5

socket::socket(QHostAddress address,int p,QObject *parent) : QObject(parent)
{
    socketIn = new QUdpSocket(this);
    socketOut = new QUdpSocket(this);
    addressToSendTo = address;
    port = p;
    if(socketIn->bind(QHostAddress::LocalHost, port))
    //if(socketIn->bind(QHostAddress::LocalHost,1237))
    {
        qDebug() << "Bound to port " + QString::number(port);
    } else {
        qDebug() << "Error binding to port:" << socketIn->errorString();
    }
    connect(socketIn, SIGNAL(readyRead()), this, SLOT(readUDP()));
}

socket::socket(int p,QObject *parent){
    socketOut = new QUdpSocket(this);
    socketIn = new QUdpSocket(this);
    port = p;
    if(socketIn->bind(QHostAddress::AnyIPv4,port)){
        qDebug() << "Bound to port " << QString::number(port);
    }else{
        qDebug() << "Error binding to port" << socketIn->errorString();
    }
    connect(socketIn, SIGNAL(readyRead()), this, SLOT(readUDP()));
}

void socket::sendUDP(QHostAddress to,std::vector<uint8_t> data,int p)
{
    char *out = new char[data.size()];
    for(int i = 0; i < data.size(); i++){
        out[i] = data[i];
    }

    socketOut->writeDatagram(out,data.size(),to,p);

}

void socket::readUDP()
{
    // when data comes in
    int size = socketIn->pendingDatagramSize();
    char *buffer = new char[size];

    QHostAddress sender;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    socketIn->readDatagram(buffer,MAX_SIZE,&sender,&senderPort);
    addressToSendTo = sender;

    std::vector<uint8_t> data;
    data.resize(size);
    for(int i = 0; i < data.size(); i++){
        data[i] = (unsigned int)buffer[i];
    }

    dataPacket.sender = sender;
    dataPacket.port = senderPort;
    dataPacket.message = data;

    emit hasData(dataPacket);
}
