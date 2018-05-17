#ifndef AUDIOMODECSVSERIES_H
#define AUDIOMODECSVSERIES_H

#include "soro_core/csvrecorder.h"
#include "settingsmodel.h"

namespace Soro {

class AudioModeCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    AudioModeCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onSettingsChanged(const SettingsModel* settings);
};

} // namespace Soro

#endif // AUDIOMODECSVSERIES_H
