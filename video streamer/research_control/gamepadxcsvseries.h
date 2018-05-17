#ifndef GAMEPADXCSVSERIES_H
#define GAMEPADXCSVSERIES_H

#include "soro_core/csvrecorder.h"

namespace Soro {

class GamepadXCsvSeries: public CsvDataSeries
{
    Q_OBJECT
public:
    GamepadXCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void gamepadXChanged(int bpsDown);
};

} // namespace Soro

#endif // GAMEPADXCSVSERIES_H
