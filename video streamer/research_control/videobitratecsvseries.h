#ifndef VIDEOBITRATECSVSERIES_H
#define VIDEOBITRATECSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoBitrateCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoBitrateCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOBITRATECSVSERIES_H
