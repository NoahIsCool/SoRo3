#ifndef GAMEPADYCSVSERIES_H
#define GAMEPADYCSVSERIES_H

#include "soro_core/csvrecorder.h"

namespace Soro {

class GamepadYCsvSeries: public CsvDataSeries
{
    Q_OBJECT
public:
    GamepadYCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void gamepadYChanged(int bpsDown);
};

} // namespace Soro

#endif // GAMEPADYCSVSERIES_H
