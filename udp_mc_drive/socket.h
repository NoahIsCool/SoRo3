
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

signals:

public slots:
    void readUDP();
private:
    QUdpSocket* socketIn;
    QUdpSocket* socketOut;
};

#endif // SOCKET_H
