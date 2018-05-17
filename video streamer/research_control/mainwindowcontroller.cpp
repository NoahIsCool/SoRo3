/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mainwindowcontroller.h"
#include "maincontroller.h"
#include "qmlgstreamerglitem.h"
#include "soro_core/sensordataparser.h"
#include "soro_core/logger.h"
#include "soro_core/constants.h"

#include <QQmlComponent>

#define LOG_TAG "MainWindowController"

namespace Soro {

MainWindowController::MainWindowController(QQmlEngine *engine, QObject *parent) : QObject(parent)
{
    _updateLatencyTimerId = TIMER_INACTIVE;
    _driveMode = DriveGamepadMode::DualStickDrive;
    _latency = 0;
    _playing = false;

    // Create UI for settings and control
    QQmlComponent qmlComponent(engine, QUrl("qrc:/qml/MainWindow.qml"));
    _window = qobject_cast<QQuickWindow*>(qmlComponent.create());
    if (!qmlComponent.errorString().isEmpty() || !_window)
    {
        MainController::panic(LOG_TAG, "Cannot create main window QML: " + qmlComponent.errorString());
    }

    _window->show();

    connect(_window, SIGNAL(closed()), this, SIGNAL(closed()));

    stopVideo();

    START_TIMER(_updateLatencyTimerId, 500);
}

MainWindowController::~MainWindowController()
{
    resetPipeline();
}

void MainWindowController::resetPipeline()
{
    LOG_I(LOG_TAG, "Resetting gstreamer pipeline");
    if (!_pipeline.isNull())
    {
        QGlib::disconnect(_pipeline->bus(), "message", this, &MainWindowController::onBusMessage);
        _pipeline->setState(QGst::StatePaused);
        _pipeline->setState(QGst::StateNull);
        _pipeline.clear();
    }
}

void MainWindowController::playVideo(SocketAddress address, GStreamerUtil::VideoProfile profile)
{
    LOG_E(LOG_TAG,"playing from port " + address.port);
    resetPipeline();

    if (profile.codec == GStreamerUtil::CODEC_NULL) {
        LOG_E(LOG_TAG, "play(): Given unusable video profile, refusing to play");
        stopVideo();
        return;
    }
    _videoProfile = profile;

    _pipeline = QGst::Pipeline::create();
    _pipeline->bus()->addSignalWatch();
    QGlib::connect(_pipeline->bus(), "message", this, &MainWindowController::onBusMessage);

    // create a udpsrc to receive the stream
    QString binStr = GStreamerUtil::createRtpVideoDecodeString(address.host, address.port, profile.codec);
    LOG_I(LOG_TAG, "Starting video surface with bin string " + binStr);

    // create a gstreamer bin from the description
    QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
    // create a gstreamer sink
    QGst::ElementPtr sink = getVideoSink();

    _pipeline->add(source, sink);
    source->link(sink);

    _playing = true;
    _pipeline->setState(QGst::StatePlaying);
}

void MainWindowController::stopVideo()
{
    _videoProfile = GStreamerUtil::VideoProfile();
    resetPipeline();
    //create videotestsrc pipeline for coolness
    QString binStr = GStreamerUtil::createVideoTestSrcString("smpte", true, 800, 600, 30);

    _pipeline = QGst::Pipeline::create();
    QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
    QGst::ElementPtr sink = getVideoSink();

    _pipeline->add(source, sink);
    source->link(sink);

    _playing = false;
    _pipeline->setState(QGst::StatePlaying);
}

void MainWindowController::onBusMessage(const QGst::MessagePtr &message)
{
    switch (message->type()) {
    case QGst::MessageEos:
        LOG_E(LOG_TAG, "onBusMessage(): Received EOS message from gstreamer");
        Q_EMIT gstreamerError("Received unexpected EOS message");
        break;
    case QGst::MessageError: {
        QString msg = message.staticCast<QGst::ErrorMessage>()->debugMessage();
        LOG_E(LOG_TAG, "onBusMessage(): Received error message from gstreamer '" + msg + "'");
        Q_EMIT gstreamerError(msg);
        break;
    }
    case QGst::MessageInfo: {
        QString msg = message.staticCast<QGst::InfoMessage>()->debugMessage();
        LOG_I(LOG_TAG, "onBusMessage(): Received info message from gstreamer '" + msg + "'");
        break;
    }
    default:
        break;
    }
}

void MainWindowController::onZeroHudOrientationClicked()
{
    QMetaObject::invokeMethod(_window, "zero");
}

void MainWindowController::setDriveGamepadMode(DriveGamepadMode mode)
{
    _driveMode = mode;
}

DriveGamepadMode MainWindowController::getDriveGamepadMode() const
{
    return _driveMode;
}

void MainWindowController::onLatencyChanged(int latency)
{
    _latency = latency;
}

QGst::ElementPtr MainWindowController::getVideoSink()
{
    return qvariant_cast<QmlGStreamerGlItem*>(_window->property("gstreamerSurface"))->videoSink();
}

void MainWindowController::onSensorUpdate(char tag, int value)
{
    switch (tag) {
    case SensorDataParser::DATATAG_WHEELPOWER_A:
        _window->setProperty("wheelMLPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_B:
        _window->setProperty("wheelFLPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_C:
        _window->setProperty("wheelFRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_D:
        _window->setProperty("wheelMRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_E:
        _window->setProperty("wheelBRPower", value);
        break;
    case SensorDataParser::DATATAG_WHEELPOWER_F:
        _window->setProperty("wheelBLPower", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_REAR_PITCH:
        _window->setProperty("rearPitch", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_REAR_ROLL:
        _window->setProperty("rearRoll", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_FRONT_PITCH:
        _window->setProperty("frontPitch", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_FRONT_ROLL:
        _window->setProperty("frontRoll", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_MIDDLE_PITCH:
        _window->setProperty("middlePitch", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_MIDDLE_ROLL:
        _window->setProperty("middleRoll", value);
        break;
    case SensorDataParser::DATATAG_IMUDATA_FRONT_YAW:
       // _window->setProperty("compassHeading", (value - 100) * 360.0f / 800.0f); // original code
        _window->setProperty("compassHeading", ((100 - value) * 360.0f / 800.0f)-130);  //Modified July 31 2017- works
       //_window->setProperty("compassHeading", ((value - 100) * 360.0f / 800.0f*(-1.0))-50); //modified by MAG before Jul 31 2017
        break;
    }
}

void MainWindowController::onGamepadUpdate(const GamepadManager::GamepadState *state)
{
    float x = 0, y = 0;
    switch (_driveMode)
    {
    case DriveGamepadMode::SingleStickDrive:
        x = GamepadUtil::axisShortToAxisFloat(state->axisLeftX);
        y = GamepadUtil::axisShortToAxisFloat(state->axisLeftY);
        break;
    case DriveGamepadMode::DualStickDrive:
        // TODO Unsupported currently, this is placeholder code
        x = 0;
        y = GamepadUtil::axisShortToAxisFloat(state->axisLeftY);
        break;
    default: break;
    }

    _window->setProperty("gamepadX", x);
    _window->setProperty("gamepadY", y);
}

void MainWindowController::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _updateLatencyTimerId)
    {
        int latency = _latency;
        if (latency >= 0) { // Don't add to latency if there's no connection
            latency += _window->property("hudLatency").toInt();
        }
        _window->setProperty("latency", latency);
    }
}

void MainWindowController::setRecordingState(RecordingState state)
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

void MainWindowController::setHudVisible(bool visible)
{
    _window->setProperty("hudVisible", visible);
}

bool MainWindowController::isHudVisible() const
{
    return _window->property("hudVisible").toBool();
}

void MainWindowController::setHudParallax(int parallax)
{
    _window->setProperty("hudParallax", parallax);
}

int MainWindowController::getHudParallax() const
{
    return _window->property("hudParallax").toInt();
}

void MainWindowController::setHudLatency(int latency)
{
    _window->setProperty("hudLatency", latency);
}

int MainWindowController::getHudLatency() const
{
    return _window->property("hudLatency").toInt();
}

void MainWindowController::setSideBySideStereo(bool stereo)
{
    _window->setProperty("stereo", stereo);
}

bool MainWindowController::getSideBySideStereo() const
{
    return _window->property("stereo").toBool();
}

} // namespace Soro
