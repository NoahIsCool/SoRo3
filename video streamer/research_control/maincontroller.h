#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QQmlEngine>

#include "soro_core/csvrecorder.h"
#include "soro_core/sensordataparser.h"
#include "soro_core/gpscsvseries.h"
#include "soro_core/channel.h"
#include "soro_core/wheelspeedcsvseries.h"

#include "latencycsvseries.h"
#include "commentcsvseries.h"
#include "connectioneventcsvseries.h"
#include "gamepadmanager.h"
#include "settingsmodel.h"
#include "mainwindowcontroller.h"
#include "controlwindowcontroller.h"
#include "drivecontrolsystem.h"
#include "commentswindowcontroller.h"
#include "audioclient.h"
#include "audioplayer.h"
#include "videoclient.h"
#include "bitratedowncsvseries.h"
#include "bitrateupcsvseries.h"
#include "gstreamerrecorder.h"
#include "audiomodecsvseries.h"
#include "videomodecsvseries.h"
#include "videocodeccsvseries.h"
#include "videoheightcsvseries.h"
#include "videowidthcsvseries.h"
#include "videobitratecsvseries.h"
#include "videoframeratecsvseries.h"
#include "hudparallaxcsvseries.h"
#include "hudlatencycsvseries.h"
#include "gamepadxcsvseries.h"
#include "gamepadycsvseries.h"

namespace Soro {

class MainController : public QObject
{
    Q_OBJECT
public:
    static void init(QApplication *app);
    static void panic(QString tag, QString message);

private:
    explicit MainController(QObject *parent=0);

    static MainController *_self;

    QQmlEngine *_qml = 0;
    GamepadManager *_gamepad = 0;
    SettingsModel _settings;
    DriveControlSystem *_driveSystem = 0;
    MainWindowController *_mainWindow = 0;
    MainWindowController *_camera2Window = 0;
    MainWindowController *_camera3Window = 0;
    ControlWindowController *_controlWindow = 0;
    CommentsWindowController *_commentsWindow = 0;
    Channel *_mainChannel = 0;

    // Video clients
    VideoClient *_frontVideoClient = 0;
    VideoClient *_backVideoClient = 0;
    VideoClient *_clawVideoClient = 0;

    // Video recorder
    GStreamerRecorder *_gstreamerRecorder = 0;

    // Audio stream subsystem
    AudioClient *_audioClient = 0;
    AudioPlayer *_audioPlayer = 0;
    GStreamerRecorder *_audioGStreamerRecorder = 0;

    // Data recording systems
    SensorDataParser *_sensorDataSeries = 0;
    GpsCsvSeries *_gpsDataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedLODataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedLMDataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedRODataSeries = 0;
    WheelSpeedCsvSeries *_wheelSpeedRMDataSeries = 0;
    AudioModeCsvSeries *_audioModeDataSeries = 0;
    VideoModeCsvSeries *_videoModeDataSeries = 0;
    VideoHeightCsvSeries *_videoHeightDataSeries = 0;
    VideoWidthCsvSeries *_videoWidthDataSeries = 0;
    VideoCodecCsvSeries *_videoCodecDataSeries = 0;
    VideoBitrateCsvSeries *_videoBitrateDataSeries = 0;
    VideoFramerateCsvSeries *_videoFramerateDataSeries = 0;
    HudParallaxCsvSeries *_hudParallaxDataSeries = 0;
    HudLatencyCsvSeries *_hudLatencyDataSeries = 0;
    ConnectionEventCsvSeries *_connectionEventSeries = 0;
    LatencyCsvSeries *_latencyDataSeries = 0;
    CommentCsvSeries *_commentDataSeries = 0;
    GamepadXCsvSeries *_gamepadXDataSeries = 0;
    GamepadYCsvSeries *_gamepadYDataSeries = 0;

    BitrateUpCsvSeries *_bitrateUpDataSeries = 0;
    BitrateDownCsvSeries *_bitrateDownDataSeries = 0;
    CsvRecorder *_dataRecorder = 0;
    CsvRecorder *_commentRecorder = 0;
    CsvRecorder *_settingsRecorder = 0;

    int _bitrateUpdateTimerId;

    qint64 _recordStartTime = 0;

private Q_SLOTS:
    void onMainChannelMessageReceived(const char *message, Channel::MessageSize length);
    void onWindowClosed();
    void onRequestUiSync();
    void onSettingsApplied();

    void startDataRecording();
    void stopDataRecording();
    void toggleDataRecording();

    void stopAllRoverCameras();
    void stopAudio();

    void startAudioStream(GStreamerUtil::AudioProfile profile);
    void startBackVideoStream(GStreamerUtil::VideoProfile profile,bool multiWindow);
    void startClawVideoStream(GStreamerUtil::VideoProfile profile,bool multiWindow);
    void startFrontCameraStream(GStreamerUtil::VideoProfile profile,bool multiWindow);

    void sendStopRecordCommandToRover();
    void sendStartRecordCommandToRover();

    void onAudioClientStateChanged(MediaClient *client, MediaClient::State state);
    void onVideoClientStateChanged(MediaClient *client, MediaClient::State state);

protected:
    void timerEvent(QTimerEvent *e);
};

} // namespace Soro

#endif // MAINCONTROLLER_H
