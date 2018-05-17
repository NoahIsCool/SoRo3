#ifndef VIDEOHEIGHTCSVSERIES_H
#define VIDEOHEIGHTCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoHeightCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoHeightCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOHEIGHTCSVSERIES_H
