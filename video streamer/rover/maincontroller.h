#ifndef RESEARCHROVERPROCESS_H
#define RESEARCHROVERPROCESS_H

#include <QObject>

#include "soro_core/channel.h"
#include "soro_core/mbedchannel.h"
#include "soro_core/nmeamessage.h"
#include "soro_core/enums.h"
#include "soro_core/sensordataparser.h"
#include "soro_core/gpscsvseries.h"
#include "soro_core/drivemessage.h"
#include "soro_core/wheelspeedcsvseries.h"

#include "gpsserver.h"
#include "audioserver.h"
#include "videoserver.h"

namespace Soro {

class MainController : public QObject
{
    Q_OBJECT
public:
    static void init(QCoreApplication *app);
    static void panic(QString tag, QString message);

private:
    explicit MainController(QObject *parent=0);

    static MainController *_self;

    /* Connects to mission control for command and status communication
     */
    Channel *_driveChannel = 0;
    Channel *_mainChannel = 0;

    /* Interfaces with the mbed controlling the drive system and data collection system
     */
    MbedChannel *_mbed = 0;

    /* Provides GPS coordinates back to mission control
     */
    GpsServer *_gpsServer = 0;

    /* Provides audio back to mission control
     */
    AudioServer *_audioServer = 0;

    /* Handles video streaming from each individual camera
     */
    VideoServer *_frontCameraServer = 0;
    VideoServer *_backCameraServer = 0;
    VideoServer *_clawCameraServer = 0;
    WheelSpeedCsvSeries *_wheelSpeedLODataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedLMDataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedRODataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedRMDataSeries = 0;
    QString _FrontCameraDevice;
    QString _BackCameraDevice;
    QString _ClawCameraDevice;

    CsvRecorder *_dataRecorder;
    GpsCsvSeries *_gpsDataSeries;
    SensorDataParser *_sensorDataSeries;

private Q_SLOTS:
    void sendSystemStatusMessage();
    void mainChannelStateChanged(Channel::State state);
    void driveChannelStateChanged(Channel::State state);
    void mbedChannelStateChanged(MbedChannel::State state);
    void mbedMessageReceived(const char* message, int size);
    void driveChannelMessageReceived(const char* message, Channel::MessageSize size);
    void mainChannelMessageReceived(const char* message, Channel::MessageSize size);
    void gpsUpdate(NmeaMessage message);
    void mediaServerError(MediaServer* server, QString message);
    bool startDataRecording(QDateTime startTime);
    void stopDataRecording();
};

} // namespace Soro

#endif // RESEARCHROVERPROCESS_H
