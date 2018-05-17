#ifndef SORO_NMEAMESSAGE_H
#define SORO_NMEAMESSAGE_H

#include <QDataStream>

#include "soro_core_global.h"

namespace Soro {

/* Structure for holding GPS information and parsing NMEA messages
 */
struct NmeaMessage {

    double Latitude, Longitude;
    /* Number of fixed satellites
     */
    int Satellites;
    /* Altitude in meters above sea level
     */
    int Altitude;

    /* Heading, in degrees relative to north
     */
    int Heading;

    /* Ground speed in kilometers per hour
     */
    double GroundSpeed;

    NmeaMessage();
    NmeaMessage(QString nmea);

    friend QDataStream& operator<<(QDataStream& stream, const NmeaMessage& message);
    friend QDataStream& operator>>(QDataStream& stream, NmeaMessage& message);
};

}

#endif // SORO_NMEAMESSAGE_H
