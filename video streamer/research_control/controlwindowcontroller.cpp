#include "controlwindowcontroller.h"
#include "maincontroller.h"
#include "soro_core/logger.h"
#include "soro_core/constants.h"

#include <QQmlComponent>

#define LOG_TAG "ControlWindowController"

namespace Soro {

ControlWindowController::ControlWindowController(QQmlEngine *engine, QObject *parent) : QObject(parent)
{
    _updateLatencyTimerId = TIMER_INACTIVE;
    _latency = 0;

    // Create UI for settings and control
    QQmlComponent qmlComponent(engine, QUrl("qrc:/qml/ControlWindow.qml"));
    _window = qobject_cast<QQuickWindow*>(qmlComponent.create());
    if (!qmlComponent.errorString().isEmpty() || !_window)
    {
        MainController::panic(LOG_TAG, "Cannot create control window QML: " + qmlComponent.errorString());
    }

    _window->show();

    connect(_window, SIGNAL(recordButtonClicked()), this, SIGNAL(recordButtonClicked()));
    connect(_window, SIGNAL(requestUiSync()), this, SIGNAL(requestUiSync()));
    connect(_window, SIGNAL(closed()), this, SIGNAL(closed()));
    connect(_window, SIGNAL(zeroOrientationButtonClicked()), this, SIGNAL(zeroOrientationButtonClicked()));
    connect(_window, SIGNAL(logCommentEntered(QString)), this, SIGNAL(logCommentEntered(QString)));
    connect(_window, SIGNAL(settingsApplied()), this, SIGNAL(settingsApplied()));

    START_TIMER(_updateLatencyTimerId, 500);
}

void ControlWindowController::updateSettingsModel(SettingsModel *model) const
{
    LOG_I(LOG_TAG, "Syncing settings with UI choices...");
    model->enableHud = _window->property("enableHud").toBool();
    model->enableVideo = _window->property("enableVideo").toBool();
    model->enableStereoVideo = _window->property("enableStereoVideo").toBool();
    model->enableAudio = _window->property("enableAudio").toBool();
    model->enableGps = _window->property("enableGps").toBool();
    model->selectedVideoEncoding = _window->property("selectedVideoEncoding").toInt();
    model->selectedVideoGrayscale = _window->property("selectedVideoGrayscale").toBool();
    model->selectedVideoWidth = _window->property("selectedVideoWidth").toInt();
    model->selectedVideoHeight = _window->property("selectedVideoHeight").toInt();
    model->selectedVideoFramerate = _window->property("selectedVideoFramerate").toInt();
    model->selectedVideoBitrate = _window->property("selectedVideoBitrate").toInt();
    model->selectedMjpegQuality = _window->property("selectedMjpegQuality").toInt();
    model->selectedCamera = _window->property("selectedCamera").toInt();
    model->selectedLatency = _window->property("selectedLatency").toInt();
    model->selectedHudParallax = _window->property("selectedHudParallax").toInt();
    model->selectedHudLatency = _window->property("selectedHudLatency").toInt();
}

void ControlWindowController::updateFromSettingsModel(const SettingsModel *model)
{
    LOG_I(LOG_TAG, "Syncing UI with settings...");

    // Prepare the view to be synced
    QMetaObject::invokeMethod(_window, "prepareForUiSync");

    _window->setProperty("videoEncodingNames", model->videoEncodingNames);
    _window->setProperty("cameraNames", model->cameraNames);
    _window->setProperty("roverAddress", model->roverAddress.toString());

    _window->setProperty("enableHud", model->enableHud);
    _window->setProperty("enableVideo", model->enableVideo);
    _window->setProperty("enableStereoVideo", model->enableStereoVideo);
    _window->setProperty("enableAudio", model->enableAudio);
    _window->setProperty("enableGps", model->enableGps);
    _window->setProperty("selectedVideoEncoding", model->selectedVideoEncoding);
    _window->setProperty("selectedVideoGrayscale", model->selectedVideoGrayscale);
    _window->setProperty("selectedVideoWidth", model->selectedVideoWidth);
    _window->setProperty("selectedVideoHeight", model->selectedVideoHeight);
    _window->setProperty("selectedVideoFramerate", model->selectedVideoFramerate);
    _window->setProperty("selectedVideoBitrate", model->selectedVideoBitrate);
    _window->setProperty("selectedMjpegQuality", model->selectedMjpegQuality);
    _window->setProperty("selectedCamera", model->selectedCamera);
    _window->setProperty("selectedLatency", model->selectedLatency);
    _window->setProperty("selectedHudParallax", model->selectedHudParallax);
    _window->setProperty("selectedHudLatency", model->selectedHudLatency);

    // Inform the view that the settings have been synced
    QMetaObject::invokeMethod(_window, "uiSyncComplete");
}

void ControlWindowController::onLatencyChanged(int latency)
{
    _latency = latency;
}

void ControlWindowController::ControlWindowController::updateGpsLocation(NmeaMessage msg)
{
    QMetaObject::invokeMethod(_window,
                              "updateGpsLocation",
                              Q_ARG(QVariant, msg.Latitude),
                              Q_ARG(QVariant, msg.Longitude));
}

void ControlWindowController::updateBitrate(int bpsUp, int bpsDown)
{
    QMetaObject::invokeMethod(_window,
                              "updateBitrate",
                              Q_ARG(QVariant, bpsUp),
                              Q_ARG(QVariant, bpsDown));
}

void ControlWindowController::notify(NotificationType type, QString title, QString message)
{
    QString typeString;
    switch (type)
    {
    case NotificationType_Error:
        typeString = "error";
        break;
    case NotificationType_Warning:
        typeString = "warning";
        break;
    case NotificationType_Info:
        typeString = "information";
        break;
    }

    QMetaObject::invokeMethod(_window,
                              "notify",
                              Q_ARG(QVariant, typeString),
                              Q_ARG(QVariant, title),
                              Q_ARG(QVariant, message));
}

void ControlWindowController::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _updateLatencyTimerId)
    {
        QMetaObject::invokeMethod(_window,
                                  "updatePing",
                                  Q_ARG(QVariant, _latency));
    }
}

void ControlWindowController::setMbedStatus(QString status)
{
    _window->setProperty("mbedStatus", status);
}

void ControlWindowController::setConnectionState(Channel::State state)
{
    switch (state)
    {
    case Channel::ErrorState:
        _window->setProperty("connectionState", "error");
        break;
    case Channel::ConnectedState:
        _window->setProperty("connectionState", "connected");
        break;
    case Channel::ConnectingState:
        _window->setProperty("connectionState", "connecting");
        break;
    case Channel::ReadyState: break;
    }
}

void ControlWindowController::setDriveConnectionState(Channel::State state)
{
    switch (state)
    {
    case Channel::ErrorState:
        notify(NotificationType_Error,
               "Drive Channel Error",
               "An unrecoverable netowork error occurred. Please exit and check the log.");
        setMbedStatus("Network Error");
        break;
    case Channel::ConnectedState:
        notify(NotificationType_Info,
               "Drive Channel Connected",
               "You are now connected to the rover's drive system.");
        break;
    case Channel::ConnectingState:
        notify(NotificationType_Error,
               "Drive Channel Disconnected",
               "The network connection to the rover's drive system is not available.");
        setMbedStatus("Unknown");
        break;
    case Channel::ReadyState: break;
    }
}

void ControlWindowController::setRecordingState(RecordingState state)
{
    switch (state)
    {
    case RecordingState_Idle:
        _window->setProperty("recordingState", "idle");
        break;
    case RecordingState_Waiting:
        _window->setProperty("recordingState", "waiting");
        break;
    case RecordingState_Recording:
        _window->setProperty("recordingState", "recording");
        break;
    }
}

void ControlWindowController::clearGps()
{
    QMetaObject::invokeMethod(_window, "clearGps");
}

void ControlWindowController::onGamepadChanged(bool connected, QString name)
{
    _window->setProperty("gamepad", name);
    if (connected)
    {
        notify(NotificationType_Info, "Input Device Connected", name + " is connected and ready for use.");
    }
    else
    {
        notify(NotificationType_Warning, "No Input Device", "Driving will be unavailable until a compatible controller is connected.");
    }
}

} // namespace Soro
