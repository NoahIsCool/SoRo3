#ifndef HUDPARALLAXCSVSERIES_H
#define HUDPARALLAXCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class HudParallaxCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    HudParallaxCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // HUDPARALLAXCSVSERIES_H
