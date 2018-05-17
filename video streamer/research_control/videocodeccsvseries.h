#ifndef VIDEOCODECCSVSERIES_H
#define VIDEOCODECCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoCodecCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoCodecCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOCODECCSVSERIES_H
