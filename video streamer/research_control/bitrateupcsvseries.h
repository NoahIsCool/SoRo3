#ifndef BITRATEUPCSVSERIES_H
#define BITRATEUPCSVSERIES_H

#include "soro_core/csvrecorder.h"

namespace Soro {

class BitrateUpCsvSeries: public CsvDataSeries
{
    Q_OBJECT
public:
    BitrateUpCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void bitrateUpdate(int bpsUp);
};

} // namespace Soro

#endif // BITRATEUPCSVSERIES_H
