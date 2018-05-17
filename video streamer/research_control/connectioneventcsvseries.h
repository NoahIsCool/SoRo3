#ifndef CONNECTIONEVENTCSVSERIES_H
#define CONNECTIONEVENTCSVSERIES_H

#include <QObject>
#include <QTimerEvent>

#include "soro_core/csvrecorder.h"
#include "soro_core/channel.h"

namespace Soro {

class ConnectionEventCsvSeries: public CsvDataSeries
{
    Q_OBJECT
public:
    ConnectionEventCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void driveChannelStateChanged(Channel::State state);
    void mainChannelStateChanged(Channel::State state);
};

} // namespace Soro

#endif // CONNECTIONEVENTCSVSERIES_H
