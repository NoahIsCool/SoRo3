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

#include "socketaddress.h"
#include <cstring>

namespace Soro {

SocketAddress::SocketAddress(const QHostAddress &address, quint16 port)
{
    this->host = address;
    this->port = port;
}

SocketAddress::SocketAddress(const SocketAddress &other)
{
    this->host = other.host;
    this->port = other.port;
}

SocketAddress::SocketAddress()
{
    this->host = QHostAddress::Any;
    this->port = 0;
}

SocketAddress::~SocketAddress() { }

QString SocketAddress::toString() const {
    return host.toString() + ":" + QString::number(port);
}

bool SocketAddress::operator==(const SocketAddress& other) const
{
    //In a situations where we are working with IPv4 addresses, but the :ffff: (IPv6)
    //prefix is added to only one of them, simple 'a == b' will fail
    Q_IPV6ADDR a = host.toIPv6Address();
    Q_IPV6ADDR b = other.host.toIPv6Address();
    return (std::memcmp(&a, &b, sizeof(Q_IPV6ADDR)) == 0)
            & (port == other.port);
}

QDataStream& operator<<(QDataStream& stream, const SocketAddress& address)
{
    stream << address.host.toString();
    stream << address.port;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, SocketAddress& address)
{
    QString hostStr;

    stream >> hostStr;
    stream >> address.port;

    address.host.setAddress(hostStr);
    return stream;
}

}
