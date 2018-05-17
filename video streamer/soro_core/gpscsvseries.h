#ifndef GPSCSVSERIES_H
#define GPSCSVSERIES_H

#include <QObject>
#include <QFile>
#include <QDateTime>

#include "soro_core_global.h"
#include "nmeamessage.h"
#include "csvrecorder.h"

namespace Soro {

class SORO_CORE_EXPORT GpsCsvSeries : public QObject
{
    Q_OBJECT
public:
    explicit GpsCsvSeries(QObject *parent = 0);

    class LatitudeCsvSeries : public CsvDataSeries { friend class GpsCsvSeries;
    public:     QString getSeriesName() const { return "GPS Latitude"; }
                bool shouldKeepOldValues() const { return true; }
    private:    void update(NmeaMessage location) { CsvDataSeries::update(QVariant(location.Latitude)); }
    };
    class LongitudeCsvSeries : public CsvDataSeries { friend class GpsCsvSeries;
    public:     QString getSeriesName() const { return "GPS Longitude"; }
                bool shouldKeepOldValues() const { return true; }
    private:    void update(NmeaMessage location) { CsvDataSeries::update(QVariant(location.Longitude)); }
    };

    const LatitudeCsvSeries* getLatitudeSeries() const;
    const LongitudeCsvSeries* getLongitudeSeries() const;

Q_SIGNALS:
    void locationUpdated(NmeaMessage location);

public Q_SLOTS:
    void addLocation(NmeaMessage location);

private:
    LatitudeCsvSeries _latitudeSeries;
    LongitudeCsvSeries _longitudeSeries;
};

} // namespace Soro

#endif // GPSCSVSERIES_H
