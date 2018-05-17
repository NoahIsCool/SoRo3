#ifndef LATLNG_H
#define LATLNG_H

#define __EARTH_RADIUS 6371000
#define __RADIANS_PER_DEGREE 0.017453293
#define __FEET_PER_METER 3.2808399

#include "soro_global.h"

namespace Soro {

/* Struct for geolocation coordinates
 */
struct LatLng {
    double Latitude = 1000;
    double Longitude = 1000;

    LatLng() {
        Latitude = 1000;
        Longitude = 1000;
    }

    LatLng(double lat, double lng) {
        Latitude = lat;
        Longitude = lng;
    }

    inline bool isEmpty() const {
        return (Latitude == 1000) & (Longitude == 1000);
    }

    inline double latitudeInRadians() const {
        return Latitude * __RADIANS_PER_DEGREE;
    }
    inline double longitudeInRadians() const {
        return Longitude * __RADIANS_PER_DEGREE;
    }

    double metersTo(LatLng other) const {
        double lat1 = latitudeInRadians();
        double lng1 = longitudeInRadians();
        double lat2 = other.latitudeInRadians();
        double lng2 = other.longitudeInRadians();
        double deltaLat = lat2 - lat1;
        double deltaLng = lng2 - lng1;

        double a = pow(sin(deltaLat / 2.0), 2) + (cos(lat1) * cos(lat2)) + pow(sin(deltaLng / 2.0), 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return __EARTH_RADIUS * c;
    }

    inline double kmTo(LatLng other) const {
        return metersTo(other) / 1000;
    }

    inline double feetTo(LatLng other) const {
        return metersTo(other) * __FEET_PER_METER;
    }

    inline double milesTo(LatLng other) const {
        return feetTo(other) / 5280;
    }

    inline QString toString() const {
        return QString::number(Latitude) + ", " + QString::number(Longitude);
    }

    inline friend QDataStream& operator<<(QDataStream& stream, const LatLng& latlng) {
        stream << latlng.Latitude;
        stream << latlng.Longitude;
        return stream;
    }

    inline friend QDataStream& operator>>(QDataStream& stream, LatLng& latlng) {
        stream >> latlng.Latitude;
        stream >> latlng.Longitude;
        return stream;
    }

};

}

#endif // LATLNG_H
