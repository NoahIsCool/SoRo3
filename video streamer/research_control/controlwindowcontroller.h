#ifndef CONTROLWINDOWCONTROLLER_H
#define CONTROLWINDOWCONTROLLER_H

#include <QObject>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QTimerEvent>

#include "settingsmodel.h"
#include "soro_core/nmeamessage.h"
#include "soro_core/channel.h"
#include "soro_core/enums.h"

namespace Soro {

class ControlWindowController : public QObject
{
    Q_OBJECT
public:
    explicit ControlWindowController(QQmlEngine *engine, QObject *parent = 0);

    void updateSettingsModel(SettingsModel *model) const;
    void updateFromSettingsModel(const SettingsModel *model);

Q_SIGNALS:
    void recordButtonClicked();
    void requestUiSync();
    void closed();
    void zeroOrientationButtonClicked();
    void logCommentEntered(QString comment);
    void settingsApplied();

public Q_SLOTS:
    void onLatencyChanged(int ping);
    void updateGpsLocation(NmeaMessage msg);
    void updateBitrate(int bpsUp, int bpsDown);
    void notify(NotificationType type, QString title, QString message);
    void clearGps();
    void setMbedStatus(QString status);
    void setRecordingState(RecordingState state);
    void setConnectionState(Channel::State state);
    void onGamepadChanged(bool connected, QString name);
    void setDriveConnectionState(Channel::State state);

protected:
    void timerEvent(QTimerEvent *e);

private:
    QQuickWindow *_window;
    int _updateLatencyTimerId;
    int _latency;
};

} // namespace Soro

#endif // CONTROLWINDOWCONTROLLER_H
