#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QTimerEvent>

#include <Qt5GStreamer/QGst/Element>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/Message>

#include "gamepadmanager.h"

#include "soro_core/enums.h"
#include "soro_core/gstreamerutil.h"
#include "soro_core/socketaddress.h"

namespace Soro {

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowController(QQmlEngine *engine, QObject *parent = 0);
    ~MainWindowController();

    bool isHudVisible() const;
    int getHudParallax() const;
    int getHudLatency() const;
    bool getSideBySideStereo() const;
    QGst::ElementPtr getVideoSink();
    DriveGamepadMode getDriveGamepadMode() const;
    void playVideo(SocketAddress address, GStreamerUtil::VideoProfile profile);
    void stopVideo();

Q_SIGNALS:
    void closed();
    void gstreamerError(QString message);

public Q_SLOTS:
    void onLatencyChanged(int latency);
    void onZeroHudOrientationClicked();
    void onSensorUpdate(char tag, int value);
    void setRecordingState(RecordingState state);
    void setSideBySideStereo(bool stereo);
    void onGamepadUpdate(const GamepadManager::GamepadState *state);
    void setHudVisible(bool visible);
    void setHudParallax(int parallax);
    void setHudLatency(int latency);
    void setDriveGamepadMode(DriveGamepadMode mode);

protected:
    void timerEvent(QTimerEvent *e);

private:
    void resetPipeline();
    void onBusMessage(const QGst::MessagePtr & message);

    int _latency;
    QGst::PipelinePtr _pipeline;
    bool _playing;
    GStreamerUtil::VideoProfile _videoProfile;
    QQuickWindow *_window = 0;
    DriveGamepadMode _driveMode;
    int _updateLatencyTimerId;
};

} // namespace Soro

#endif // MAINWINDOWCONTROLLER_H
