#ifndef SENSORDATAPARSER_H
#define SENSORDATAPARSER_H

#include "soro_core_global.h"
#include "mbedchannel.h"
#include "csvrecorder.h"

namespace Soro {

/* This class is responsible for logging the data sent back by the research mbed, including
 * IMU and power consumption data. These values are stored in a binary logfile and timestamped;
 * additionally they can be accessed immediately by attaching to the newData() signal.
 */
class SORO_CORE_EXPORT SensorDataParser : public QObject
{
    Q_OBJECT

public:
    static const char DATATAG_WHEELPOWER_A = 'A';
    static const char DATATAG_WHEELPOWER_B = 'B';
    static const char DATATAG_WHEELPOWER_C = 'C';
    static const char DATATAG_WHEELPOWER_D = 'D';
    static const char DATATAG_WHEELPOWER_E = 'E';
    static const char DATATAG_WHEELPOWER_F = 'F';
    static const char DATATAG_IMUDATA_REAR_YAW = 'L';
    static const char DATATAG_IMUDATA_REAR_PITCH = 'J';
    static const char DATATAG_IMUDATA_REAR_ROLL = 'K';
    static const char DATATAG_IMUDATA_FRONT_YAW = 'P';
    static const char DATATAG_IMUDATA_FRONT_PITCH = 'Q';
    static const char DATATAG_IMUDATA_FRONT_ROLL = 'R';
    static const char DATATAG_IMUDATA_MIDDLE_YAW = 'X';
    static const char DATATAG_IMUDATA_MIDDLE_PITCH = 'Y';
    static const char DATATAG_IMUDATA_MIDDLE_ROLL = 'Z';
    static const char DATATAG_ERROR = '?';

    class WheelPowerACsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel A Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class WheelPowerBCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel B Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class WheelPowerCCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel C Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class WheelPowerDCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel D Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class WheelPowerECsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel E Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class WheelPowerFCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Wheel F Power"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuRearYawCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Rear Yaw"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuRearPitchCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Rear Pitch"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuRearRollCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Rear Roll"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuFrontYawCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Front Yaw"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuFrontPitchCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Front Pitch"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuFrontRollCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Front Roll"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuMiddleYawCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Middle Yaw"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuMiddlePitchCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Middle Pitch"; }
                bool shouldKeepOldValues() const { return true; }
    };
    class ImuMiddleRollCsvSeries : public CsvDataSeries { friend class SensorDataParser;
    public:     QString getSeriesName() const { return "Middle Roll"; }
                bool shouldKeepOldValues() const { return true; }
    };

    explicit SensorDataParser(QObject *parent=0);

    bool isValidTag(char c);
    QString getSeriesName() const;

    const WheelPowerACsvSeries* getWheelPowerASeries() const;
    const WheelPowerBCsvSeries* getWheelPowerBSeries() const;
    const WheelPowerCCsvSeries* getWheelPowerCSeries() const;
    const WheelPowerDCsvSeries* getWheelPowerDSeries() const;
    const WheelPowerECsvSeries* getWheelPowerESeries() const;
    const WheelPowerFCsvSeries* getWheelPowerFSeries() const;
    const ImuRearYawCsvSeries* getImuRearYawSeries() const;
    const ImuRearPitchCsvSeries* getImuRearPitchSeries() const;
    const ImuRearRollCsvSeries* getImuRearRollSeries() const;
    const ImuFrontYawCsvSeries* getImuFrontYawSeries() const;
    const ImuFrontPitchCsvSeries* getImuFrontPitchSeries() const;
    const ImuFrontRollCsvSeries* getImuFrontRollSeries() const;
    const ImuMiddleYawCsvSeries* getImuMiddleYawSeries() const;
    const ImuMiddlePitchCsvSeries* getImuMiddlePitchSeries() const;
    const ImuMiddleRollCsvSeries* getImuMiddleRollSeries() const;

private:
    QByteArray _buffer;
    WheelPowerACsvSeries _wheelPowerASeries;
    WheelPowerBCsvSeries _wheelPowerBSeries;
    WheelPowerCCsvSeries _wheelPowerCSeries;
    WheelPowerDCsvSeries _wheelPowerDSeries;
    WheelPowerECsvSeries _wheelPowerESeries;
    WheelPowerFCsvSeries _wheelPowerFSeries;
    ImuRearYawCsvSeries _imuRearYawSeries;
    ImuRearPitchCsvSeries _imuRearPitchSeries;
    ImuRearRollCsvSeries _imuRearRollSeries;
    ImuFrontYawCsvSeries _imuFrontYawSeries;
    ImuFrontPitchCsvSeries _imuFrontPitchSeries;
    ImuFrontRollCsvSeries _imuFrontRollSeries;
    ImuMiddleYawCsvSeries _imuMiddleYawSeries;
    ImuMiddlePitchCsvSeries _imuMiddlePitchSeries;
    ImuMiddleRollCsvSeries _imuMiddleRollSeries;

    void parseBuffer();

public Q_SLOTS:
    void newData(const char* data, int len);

Q_SIGNALS:
    void dataParsed(char tag, int value);
    void parseError();
};

} // namespace Soro

#endif // SENSORDATAPARSER_H
