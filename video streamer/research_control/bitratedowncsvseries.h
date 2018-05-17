#ifndef BITRATEDOWNCSVSERIES_H
#define BITRATEDOWNCSVSERIES_H

#include "soro_core/csvrecorder.h"

namespace Soro {

class BitrateDownCsvSeries: public CsvDataSeries
{
    Q_OBJECT
public:
    BitrateDownCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void bitrateUpdate(int bpsDown);
};

} // namespace Soro

#endif // BITRATEDOWNCSVSERIES_H
