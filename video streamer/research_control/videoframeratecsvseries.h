#ifndef VIDEOFRAMERATETCSVSERIES_H
#define VIDEOFRAMERATETCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoFramerateCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoFramerateCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOFRAMERATETCSVSERIES_H
