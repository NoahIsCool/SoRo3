#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QUdpSocket>
#include <iostream>
#include <vector>

struct DataPacket{
    QHostAddress sender;
    int port;
    std::vector<uint8_t> message;
};

class socket : public QObject
{
    Q_OBJECT
public:
    explicit socket(QHostAddress addressToSendTo,int p,QObject *parent = nullptr);
    socket(int p,QObject *parent = nullptr);
    void sendUDP(QHostAddress to,std::vector<uint8_t> message,int port);

signals:
    void hasData(DataPacket);

public slots:
    void readUDP();
private:
    QUdpSocket* socketIn;
    QUdpSocket* socketOut;
    QHostAddress addressToSendTo;
    int port;
    DataPacket dataPacket;
    const qint64 MAX_SIZE = 256;
};

#endif // SOCKET_H
