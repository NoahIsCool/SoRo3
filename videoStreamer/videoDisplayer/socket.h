#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QUdpSocket>

struct DataPacket{
    QHostAddress sender;
    int port;
    QString message;
};

class socket : public QObject
{
    Q_OBJECT
public:
    explicit socket(QHostAddress addressToSendTo,int p,QObject *parent = nullptr);
    socket(int p,QObject *parent = nullptr);
    void sendUDP(QHostAddress to,QByteArray message,int port);

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
};

#endif // SOCKET_H
