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

#include "nmeamessage.h"

namespace Soro {

NmeaMessage::NmeaMessage() { }

NmeaMessage::NmeaMessage(QString nmea)
{
    int ggaPos = nmea.indexOf("$GPGGA,");
    int vtgPos = nmea.indexOf("$GPVTG,");
    if (ggaPos >= 0)
    {
        QStringList ggaList = nmea.mid(ggaPos).split(",");
        QString time = ggaList[1];
        QString latitude = ggaList[2];
        QString latDirection = ggaList[3];
        QString longitude = ggaList[4];
        QString lngDirection = ggaList[5];
        QString fixQuality = ggaList[6];
        QString satellites = ggaList[7];
        QString hdop = ggaList[8];
        QString altitude = ggaList[9];
        QString altitudeUnits = ggaList[10];

        // Hardcoded for northern/western hemisphere TODO

        Latitude = latitude.isEmpty() ? -1 :
                                        (latitude.mid(0, latitude.indexOf(".") - 2).toDouble()
                                                    + (latitude.mid(latitude.indexOf(".") - 2).toDouble() / 60.0));
        Longitude = longitude.isEmpty() ? -1 :
                                        (longitude.mid(0, longitude.indexOf(".") - 2).toDouble()
                                                    + (longitude.mid(longitude.indexOf(".") - 2).toDouble() / 60.0));
        Longitude = -Longitude;

        Satellites = satellites.isEmpty() ? 0 : satellites.toInt();
        Altitude = altitude.isEmpty() ? -1 :  qRound(altitude.toDouble());
    }
    else
    {
        Latitude = 0;
        Longitude = 0;
        Satellites = 0;
        Altitude = -1;
    }
    if (vtgPos >= 0)
    {
        QStringList vtgList = nmea.mid(vtgPos).split(",");
        QString trackTrueNorth = vtgList[1];
        QString trackMagNorth = vtgList[3];
        QString groundSpeedKnots = vtgList[5];
        QString groundSpeedKph = vtgList[7];

        Heading = trackMagNorth.isEmpty() ? -1 : qRound(trackMagNorth.toDouble());
        GroundSpeed = groundSpeedKnots.isEmpty() ? -1 : qRound(groundSpeedKph.toDouble());
    }
    else
    {
        Heading = -1;
        GroundSpeed = -1;
    }
}

QDataStream& operator<<(QDataStream& stream, const NmeaMessage& message)
{
    stream << message.Latitude;
    stream << message.Longitude;
    stream << (qint32)message.Satellites;
    stream << (qint32)message.Altitude;
    stream << (qint32)message.Heading;
    stream << message.GroundSpeed;

    return stream;
}

QDataStream& operator>>(QDataStream& stream, NmeaMessage& message) {
    stream >> message.Latitude;
    stream >> message.Longitude;
    qint32 temp;
    stream >> temp;
    message.Satellites = (int)temp;
    stream >> temp;
    message.Altitude = (int)temp;
    stream >> temp;
    message.Heading = (int)temp;
    stream >> message.GroundSpeed;

    return stream;
}

}
