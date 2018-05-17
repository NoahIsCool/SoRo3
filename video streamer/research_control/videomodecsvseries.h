#ifndef VIDEOMODECSVSERIES_H
#define VIDEOMODECSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoModeCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoModeCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOMODECSVSERIES_H
