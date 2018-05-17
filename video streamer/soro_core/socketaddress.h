#ifndef SORO_SOCKETADDRESS_H
#define SORO_SOCKETADDRESS_H

#include "soro_core_global.h"

#include <QHostAddress>
#include <QDataStream>

namespace Soro {

/* Wrapper for a QHostAddress (IP) and quint16 (port) used to identify
 * the address of a network socket
 */
struct SORO_CORE_EXPORT SocketAddress {
    QHostAddress host;
    quint16 port;

    SocketAddress();
    SocketAddress(const SocketAddress& other);
    SocketAddress(const QHostAddress &address, quint16 port);

    ~SocketAddress();

    QString toString() const;

    bool operator==(const SocketAddress& other) const;

    inline bool operator!=(const SocketAddress& other) const {
        return !(*this == other);
    }

    friend QDataStream& operator<<(QDataStream& stream, const SocketAddress& address);
    friend QDataStream& operator>>(QDataStream& stream, SocketAddress& address);

};

}

Q_DECLARE_METATYPE(Soro::SocketAddress)

#endif // SORO_SOCKETADDRESS_H
