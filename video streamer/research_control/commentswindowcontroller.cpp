#include "commentswindowcontroller.h"
#include "maincontroller.h"
#include "soro_core/logger.h"
#include "soro_core/constants.h"

#include <QQmlComponent>

#define LOG_TAG "CommentsWindowController"

namespace Soro {

CommentsWindowController::CommentsWindowController(QQmlEngine *engine, QObject *parent) : QObject(parent)
{
    // Create UI for settings and control
    QQmlComponent qmlComponent(engine, QUrl("qrc:/qml/CommentsWindow.qml"));
    _window = qobject_cast<QQuickWindow*>(qmlComponent.create());
    if (!qmlComponent.errorString().isEmpty() || !_window)
    {
        MainController::panic(LOG_TAG, "Cannot create comments window QML: " + qmlComponent.errorString());
    }

    _window->show();

    connect(_window, SIGNAL(recordButtonClicked()), this, SIGNAL(recordButtonClicked()));
    connect(_window, SIGNAL(logCommentEntered(QString)), this, SIGNAL(logCommentEntered(QString)));
    connect(_window, SIGNAL(closed()), this, SIGNAL(closed()));
}

void CommentsWindowController::setConnectionState(Channel::State state)
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
    default: break;
    }
}

void CommentsWindowController::setRecordingState(RecordingState state)
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
    default: break;
    }
}

} // namespace SOro
