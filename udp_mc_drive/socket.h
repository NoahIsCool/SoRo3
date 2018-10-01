
#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QUdpSocket>

class socket : public QObject
{
    Q_OBJECT
public:
    explicit socket(QObject *parent = nullptr);
    void sendUDP(QByteArray message);

public slots:
    void readUDP();

private:
    QUdpSocket* udpSocket;
};

#endif // SOCKET_H
