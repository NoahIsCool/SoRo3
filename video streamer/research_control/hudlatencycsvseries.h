#ifndef HUDLATENCYCSVSERIES_H
#define HUDLATENCYCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class HudLatencyCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    HudLatencyCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // HUDLATENCYCSVSERIES_H
