#ifndef WHEELSPEEDCSVSERIES_H
#define WHEELSPEEDCSVSERIES_H

#include "csvrecorder.h"

namespace Soro {

class WheelSpeedCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    /* Wheel index should be one of the DriveMessage::Index_* constants
     */
    WheelSpeedCsvSeries(int wheelIndex, QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onDriveCommand(const char *driveMessage);

private:
    int _wheelIndex;
};

} // namespace Soro

#endif // WHEELSPEEDCSVSERIES_H
