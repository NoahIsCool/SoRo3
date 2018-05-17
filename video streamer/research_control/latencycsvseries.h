#ifndef LATENCYCSVSERIES_H
#define LATENCYCSVSERIES_H

#include <QObject>
#include <QFile>
#include <QDateTime>

#include "soro_core/nmeamessage.h"
#include "soro_core/csvrecorder.h"

namespace Soro {

class LatencyCsvSeries : public QObject
{
    Q_OBJECT
public:
    explicit LatencyCsvSeries(QObject *parent = 0);

    class RealLatencyCsvSeries : public CsvDataSeries { friend class LatencyCsvSeries;
    public: QString getSeriesName() const { return "Actual Latency"; }
            bool shouldKeepOldValues() const { return true; }
    };
    class SimulatedLatencyCsvSeries : public CsvDataSeries { friend class LatencyCsvSeries;
    public: QString getSeriesName() const { return "Simulated Latency"; }
            bool shouldKeepOldValues() const { return true; }
    };

    const RealLatencyCsvSeries* getRealLatencySeries() const;
    const SimulatedLatencyCsvSeries* getSimulatedLatencySeries() const;

public Q_SLOTS:
    void updateRealLatency(int latency);
    void updateSimulatedLatency(int latency);

private:
    RealLatencyCsvSeries _realLatencySeries;
    SimulatedLatencyCsvSeries _simulatedLatencySeries;
};

} // namespace Soro

#endif // LATENCYCSVSERIES_H
