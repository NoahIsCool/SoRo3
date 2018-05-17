#ifndef SORO_ROVER_GPSSERVER_H
#define SORO_ROVER_GPSSERVER_H

#include <QtCore>
#include <QObject>
#include <QUdpSocket>

#include "soro_core/constants.h"
#include "soro_core/socketaddress.h"
#include "soro_core/nmeamessage.h"
#include "soro_core/constants.h"

namespace Soro {

/* Listens for NMEA formatted GPS messages on a UDP socket
 */
class GpsServer : public QObject {
    Q_OBJECT
public:
    explicit GpsServer(SocketAddress hostAddress, QObject *parent = nullptr);
    ~GpsServer();

private:
    QUdpSocket *_socket;
    char _buffer[1024];
    void resetConnection();
    SocketAddress _hostAddress;
    int _resetConnectionTimerId = TIMER_INACTIVE;

Q_SIGNALS:
    void connectionError(QAbstractSocket::SocketError err);
    void gpsUpdate(NmeaMessage update);

private Q_SLOTS:
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError err);

protected:
    void timerEvent(QTimerEvent *e);
};

} // namespace Soro

#endif // SORO_ROVER_GPSSERVER_H
