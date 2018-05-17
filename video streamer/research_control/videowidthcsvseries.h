#ifndef VIDEOWIDTHCSVSERIES_H
#define VIDEOWIDTHCSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class VideoWidthCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    VideoWidthCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // VIDEOWIDTHCSVSERIES_H
