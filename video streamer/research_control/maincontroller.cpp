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

#include "maincontroller.h"
#include "soro_core/logger.h"
#include "soro_core/constants.h"
#include "soro_core/confloader.h"

#include "hudlatencygraphimpl.h"
#include "hudpowerimpl.h"
#include "hudorientationbackimpl.h"
#include "hudorientationsideimpl.h"
#include "qmlgstreamerglitem.h"
#include "qmlgstreamerpainteditem.h"

#include <QMessageBox>
#include <QQuickStyle>
#include <QProcessEnvironment>
#include <QtWebEngine>

#include <Qt5GStreamer/QGst/Init>

#define LOG_TAG "MainController"

namespace Soro {

MainController *MainController::_self = nullptr;

MainController::MainController(QObject *parent) : QObject(parent) { }

void MainController::panic(QString tag, QString message)
{
    LOG_E(LOG_TAG, QString("panic(): %1: %2").arg(tag, message));
    QMessageBox::critical(0, "Mission Control", tag + ": " + message);
    LOG_I(LOG_TAG, "Committing suicide...");
    delete _self;
    LOG_I(LOG_TAG, "Exiting with code 1");
    exit(1);
}

void MainController::init(QApplication *app)
{
    if (_self)
    {
        LOG_E(LOG_TAG, "init() called when already initialized");
    }
    else
    {
        _self = new MainController(app);

        // Use a timer to wait for the event loop to start
        QTimer::singleShot(0, _self, []()
        {
            QGst::init();
            QtWebEngine::initialize();

            // set root log output file
            Logger::rootLogger()->setLogfile(QCoreApplication::applicationDirPath()
                                             + "/../log/RoverControl_" + QDateTime::currentDateTime().toString("M-dd_h.mm_AP") + ".log");
            Logger::rootLogger()->setMaxFileLevel(Logger::LogLevelDebug);
            Logger::rootLogger()->setMaxStdoutLevel(Logger::LogLevelInformation);

            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "Starting...");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");

            //
            // Get settings from envvars
            //

            LOG_I(LOG_TAG, "Reading settings...");

            QFile confFile(QCoreApplication::applicationDirPath() + "/../config/research_control.conf");
            if (!confFile.exists()) {
                panic(LOG_TAG, "The configuration file ../config/research_control.conf does not exist.");
            }
            else {
                ConfLoader config;
                config.load(confFile);

                if (!config.valueAsIP("rover_address", &_self->_settings.roverAddress, false))
                {
                    panic(LOG_TAG, "Invalid value specified for rover_address in research_control.conf");
                }

                bool value;
                if (!config.valueAsBool("vaapi_enc_h264", &value))
                {
                    panic(LOG_TAG, "Invalid value specified for vaapi_enc_h264 in research_control.conf");
                }
                _self->_settings.useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_H264, value);

                if (!config.valueAsBool("vaapi_enc_mjpeg", &value))
                {
                    panic(LOG_TAG, "Invalid value specified for vaapi_enc_mjpeg in research_control.conf");
                }
                _self->_self->_settings.useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_MJPEG, value);

                if (!config.valueAsBool("vaapi_enc_h265", &value))
                {
                    panic(LOG_TAG, "Invalid value specified for vaapi_enc_h265 in research_control.conf");
                }
                _self->_settings.useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_H265, value);

                if (!config.valueAsBool("vaapi_enc_vp8", &value))
                {
                    panic(LOG_TAG, "Invalid value specified for vaapi_enc_vp8 in research_control.conf");
                }
                _self->_settings.useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_VP8, value);

                if (!config.valueAsBool("gst_hwrender", &_self->_settings.useHwRendering))
                {
                    panic(LOG_TAG, "Invalid value specified for gst_hwrender in research_control.conf");
                }
            }

            //
            // Initialize gamepad manager
            //

            LOG_I(LOG_TAG, "Initializing gamepad manager...");
            _self->_gamepad = new GamepadManager(QCoreApplication::applicationDirPath() + "/../config/gamecontrollerdb.txt", _self);

            //
            // Initialize core connections
            //

            LOG_I(LOG_TAG, "Initializing core connections...");
            _self->_mainChannel = Channel::createClient(_self, SocketAddress(_self->_settings.roverAddress, NETWORK_ALL_MAIN_CHANNEL_PORT), CHANNEL_NAME_MAIN,
            Channel::TcpProtocol, QHostAddress::Any);

            connect(_self->_mainChannel, &Channel::messageReceived,
                _self, &MainController::onMainChannelMessageReceived);

            _self->_driveSystem = new DriveControlSystem(_self->_settings.roverAddress, _self);
            _self->_driveSystem->setMode(DriveGamepadMode::SingleStickDrive);

            connect(_self->_gamepad, &GamepadManager::poll,
                _self->_driveSystem, &DriveControlSystem::gamepadPoll);
            connect(_self->_gamepad, &GamepadManager::gamepadChanged,
                _self->_driveSystem, &DriveControlSystem::gamepadChanged);
            connect(_self->_gamepad, &GamepadManager::poll, _self, [] (const GamepadManager::GamepadState *state) {
                _self->_gamepadXDataSeries->gamepadXChanged(state->axisLeftX);
                _self->_gamepadYDataSeries->gamepadYChanged(state->axisLeftY);
            });

            _self->_mainChannel->open();
            _self->_driveSystem->enable();

            //
            // Initialize media systems...
            //

            LOG_I(LOG_TAG, "Initializing audio/video systems...");

            _self->_frontVideoClient = new VideoClient(FRONT_CAMERA, SocketAddress(_self->_settings.roverAddress, NETWORK_ALL_FRONT_CAMERA_PORT), QHostAddress::Any, _self);
            _self->_backVideoClient = new VideoClient(BACK_CAMERA, SocketAddress(_self->_settings.roverAddress, NETWORK_ALL_BACK_CAMERA_PORT), QHostAddress::Any, _self);
            _self->_clawVideoClient = new VideoClient(CLAW_CAMERA, SocketAddress(_self->_settings.roverAddress, NETWORK_ALL_CLAW_CAMERA_PORT), QHostAddress::Any, _self);

            // Add localhost bounce to video streams so they can be recorded and played at the same time
            _self->_frontVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_FRONT_CAMERA_PORT));
            _self->_frontVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_MC_CAMERA_REC_PORT));
            _self->_backVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_BACK_CAMERA_PORT));
            _self->_backVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_MC_CAMERA_REC_PORT));
            _self->_clawVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_CLAW_CAMERA_PORT));
            _self->_clawVideoClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_MC_CAMERA_REC_PORT));

            connect(_self->_frontVideoClient, &VideoClient::stateChanged, _self, &MainController::onVideoClientStateChanged);
            connect(_self->_backVideoClient, &VideoClient::stateChanged, _self, &MainController::onVideoClientStateChanged);
            connect(_self->_clawVideoClient, &VideoClient::stateChanged, _self, &MainController::onVideoClientStateChanged);

            _self->_audioClient = new AudioClient(MEDIAID_AUDIO, SocketAddress(_self->_settings.roverAddress, NETWORK_ALL_AUDIO_PORT), QHostAddress::Any, _self);

            // Add localhost bounce to the media stream so the in-app player can display it from a udpsrc
            _self->_audioClient->addForwardingAddress(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_AUDIO_PORT));
            connect(_self->_audioClient, &AudioClient::stateChanged, _self, &MainController::onAudioClientStateChanged);

            _self->_audioPlayer = new AudioPlayer(_self);

            _self->_gstreamerRecorder = new GStreamerRecorder(SocketAddress(QHostAddress::LocalHost, NETWORK_MC_CAMERA_REC_PORT), "", _self);

            //
            // Initialize data recording system
            //

            LOG_I(LOG_TAG, "Initializing data recording systems...");
            _self->_sensorDataSeries = new SensorDataParser(_self);
            _self->_gpsDataSeries = new GpsCsvSeries(_self);
            _self->_connectionEventSeries = new ConnectionEventCsvSeries(_self);
            _self->_latencyDataSeries = new LatencyCsvSeries(_self);
            _self->_wheelSpeedLMDataSeries = new WheelSpeedCsvSeries(DriveMessage::Index_LeftMiddle, _self);
            _self->_wheelSpeedLODataSeries = new WheelSpeedCsvSeries(DriveMessage::Index_LeftOuter, _self);
            _self->_wheelSpeedRMDataSeries = new WheelSpeedCsvSeries(DriveMessage::Index_RightMiddle, _self);
            _self->_wheelSpeedRODataSeries = new WheelSpeedCsvSeries(DriveMessage::Index_RightOuter, _self);
            _self->_commentDataSeries = new CommentCsvSeries(_self);
            _self->_bitrateUpDataSeries = new BitrateUpCsvSeries(_self);
            _self->_bitrateDownDataSeries = new BitrateDownCsvSeries(_self);
            _self->_audioModeDataSeries = new AudioModeCsvSeries(_self);
            _self->_videoModeDataSeries = new VideoModeCsvSeries(_self);
            _self->_videoCodecDataSeries = new VideoCodecCsvSeries(_self);
            _self->_videoWidthDataSeries = new VideoWidthCsvSeries(_self);
            _self->_videoHeightDataSeries = new VideoHeightCsvSeries(_self);
            _self->_videoBitrateDataSeries = new VideoBitrateCsvSeries(_self);
            _self->_videoFramerateDataSeries = new VideoFramerateCsvSeries(_self);
            _self->_hudParallaxDataSeries = new HudParallaxCsvSeries(_self);
            _self->_hudLatencyDataSeries = new HudLatencyCsvSeries(_self);
            _self->_gamepadXDataSeries = new GamepadXCsvSeries(_self);
            _self->_gamepadYDataSeries = new GamepadYCsvSeries(_self);

            _self->_dataRecorder = new CsvRecorder("data", _self);
            _self->_dataRecorder->setUpdateInterval(50);

            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerASeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerBSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerCSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerDSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerESeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getWheelPowerFSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuRearYawSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuRearPitchSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuRearRollSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuMiddleYawSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuMiddlePitchSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuMiddleRollSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontYawSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontPitchSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontRollSeries());
            _self->_dataRecorder->addColumn(_self->_wheelSpeedLODataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedLMDataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedRODataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedRMDataSeries);
            _self->_dataRecorder->addColumn(_self->_gamepadXDataSeries);
            _self->_dataRecorder->addColumn(_self->_gamepadYDataSeries);
            _self->_dataRecorder->addColumn(_self->_gpsDataSeries->getLatitudeSeries());
            _self->_dataRecorder->addColumn(_self->_gpsDataSeries->getLongitudeSeries());
            _self->_dataRecorder->addColumn(_self->_bitrateUpDataSeries);
            _self->_dataRecorder->addColumn(_self->_bitrateDownDataSeries);
            _self->_dataRecorder->addColumn(_self->_latencyDataSeries->getRealLatencySeries());

            _self->_commentRecorder = new CsvRecorder("comments", _self);
            _self->_commentRecorder->addColumn(_self->_commentDataSeries);

            _self->_settingsRecorder = new CsvRecorder("settings", _self);
            _self->_settingsRecorder->addColumn(_self->_connectionEventSeries);
            _self->_settingsRecorder->addColumn(_self->_latencyDataSeries->getSimulatedLatencySeries());
            _self->_settingsRecorder->addColumn(_self->_videoModeDataSeries);
            _self->_settingsRecorder->addColumn(_self->_videoCodecDataSeries);
            _self->_settingsRecorder->addColumn(_self->_videoWidthDataSeries);
            _self->_settingsRecorder->addColumn(_self->_videoHeightDataSeries);
            _self->_settingsRecorder->addColumn(_self->_videoFramerateDataSeries);
            _self->_settingsRecorder->addColumn(_self->_videoBitrateDataSeries);
            _self->_settingsRecorder->addColumn(_self->_audioModeDataSeries);
            _self->_settingsRecorder->addColumn(_self->_hudParallaxDataSeries);
            _self->_settingsRecorder->addColumn(_self->_hudLatencyDataSeries);

            connect(_self->_mainChannel, &Channel::stateChanged,
                    _self->_connectionEventSeries, &ConnectionEventCsvSeries::mainChannelStateChanged);

            //
            // Initialize QML engine and register custom items
            //

            LOG_I(LOG_TAG, "Initializing QML and registering items...");
            qmlRegisterType<HudLatencyGraphImpl>("Soro", 1, 0, "HudLatencyGraphImpl");
            qmlRegisterType<HudPowerImpl>("Soro", 1, 0, "HudPowerImpl");
            qmlRegisterType<HudOrientationSideImpl>("Soro", 1, 0, "HudOrientationSideImpl");
            qmlRegisterType<HudOrientationBackImpl>("Soro", 1, 0, "HudOrientationBackImpl");
            if (_self->_settings.useHwRendering)
            {
                // Use the hardware opengl rendering surface, doesn't work on some hardware
                LOG_I(LOG_TAG, "Registering QmlGStreamerItem as GStreamerSurface...");
                qmlRegisterType<QmlGStreamerGlItem>("Soro", 1, 0, "GStreamerSurface");
            }
            else
            {
                // Use the software rendering surface, works everywhere but slower
                LOG_I(LOG_TAG, "Registering QmlGStreamerPaintedItem as GStreamerSurface...");
                qmlRegisterType<QmlGStreamerPaintedItem>("Soro", 1, 0, "GStreamerSurface");
            }

            _self->_qml = new QQmlEngine(_self);
            QQuickStyle::setStyle("Material");

            //
            // Create windows
            //
            LOG_I(LOG_TAG, "Creating windows...");

            _self->_controlWindow = new ControlWindowController(_self->_qml, _self);
            _self->_commentsWindow = new CommentsWindowController(_self->_qml, _self);
            _self->_mainWindow = new MainWindowController(_self->_qml, _self);
            //_self->_camera2Window = new MainWindowController(_self->_qml,_self);
            //_self->_camera3Window = new MainWindowController(_self->_qml,_self);

            connect(_self->_mainChannel, &Channel::rttChanged, _self, [](int rtt) {
                _self->_mainWindow->onLatencyChanged(rtt + _self->_settings.selectedLatency);
            });
            connect(_self->_mainChannel, &Channel::rttChanged,
                    _self->_latencyDataSeries, &LatencyCsvSeries::updateRealLatency);
            connect(_self->_mainChannel, &Channel::rttChanged,
                    _self->_controlWindow, &ControlWindowController::onLatencyChanged);
            connect(_self->_controlWindow, &ControlWindowController::requestUiSync,
                    _self, &MainController::onRequestUiSync);
            connect(_self->_controlWindow, &ControlWindowController::settingsApplied,
                    _self, &MainController::onSettingsApplied);
            connect(_self->_controlWindow, &ControlWindowController::zeroOrientationButtonClicked,
                    _self->_mainWindow, &MainWindowController::onZeroHudOrientationClicked);
            connect(_self->_gamepad, &GamepadManager::gamepadChanged,
                    _self->_controlWindow, &ControlWindowController::onGamepadChanged);
            connect(_self->_gamepad, &GamepadManager::poll,
                    _self->_mainWindow, &MainWindowController::onGamepadUpdate);
            connect(_self->_mainChannel, &Channel::stateChanged,
                    _self->_controlWindow, &ControlWindowController::setConnectionState);

            connect(_self->_controlWindow, &ControlWindowController::recordButtonClicked,
                    _self, &MainController::toggleDataRecording);
            connect (_self->_commentsWindow, &CommentsWindowController::recordButtonClicked,
                     _self, &MainController::toggleDataRecording);

            connect(_self->_sensorDataSeries, &SensorDataParser::dataParsed,
                    _self->_mainWindow, &MainWindowController::onSensorUpdate);
            connect(_self->_commentsWindow, &CommentsWindowController::logCommentEntered,
                    _self->_commentDataSeries, &CommentCsvSeries::onCommentEntered);

            connect(_self->_mainWindow, &MainWindowController::closed,
                    _self, &MainController::onWindowClosed);
            connect(_self->_controlWindow, &ControlWindowController::closed,
                    _self, &MainController::onWindowClosed);
            connect(_self->_commentsWindow, &CommentsWindowController::closed,
                    _self, &MainController::onWindowClosed);

            connect(_self->_mainWindow, &MainWindowController::gstreamerError, _self, [](QString message)
            {
               _self->_controlWindow->notify(NotificationType_Error, "Error Decoding Video", "Received an error decoding video: " + message);
            });

            connect(_self->_mainChannel, &Channel::stateChanged,
                    _self->_commentsWindow, &CommentsWindowController::setConnectionState);

            // Force an initial UI sync
            _self->onRequestUiSync();

            // Start bitrate calculate timer
            _self->_bitrateUpdateTimerId = _self->startTimer(1000);
        });
    }
}

void MainController::onWindowClosed()
{
    QApplication::quit();
}

void MainController::onRequestUiSync()
{
    _controlWindow->setConnectionState(_mainChannel->getState());
    _commentsWindow->setConnectionState(_mainChannel->getState());
    _controlWindow->updateFromSettingsModel(&_settings);
}

void MainController::startDataRecording()
{
    _recordStartTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    sendStartRecordCommandToRover();

    _controlWindow->setRecordingState(RecordingState_Waiting);
    _commentsWindow->setRecordingState(RecordingState_Waiting);
    _mainWindow->setRecordingState(RecordingState_Waiting);

    // Start a watchdog timer to make sure the rover responds
    QTimer::singleShot(5000, this, [this]
    {
        if (!_dataRecorder->isRecording())
        {
            // Rover did not respond to our record request in time
            stopDataRecording();
            _controlWindow->notify(NotificationType_Error,
                                   "Cannot Record Data",
                                   "The rover has not responded to the request to start data recording");
        }
    });
}

void MainController::stopDataRecording()
{
    _dataRecorder->stopLog();
    _commentRecorder->stopLog();
    _settingsRecorder->stopLog();
    _controlWindow->setRecordingState(RecordingState_Idle);
    _commentsWindow->setRecordingState(RecordingState_Idle);
    _mainWindow->setRecordingState(RecordingState_Idle);

    // Send stop command to rover as well
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_StopDataRecording;
    stream << static_cast<qint32>(messageType);

    _mainChannel->sendMessage(byteArray);
}

void MainController::timerEvent(QTimerEvent *e) {
    if (e->timerId() == _bitrateUpdateTimerId) {
        /*****************************************
         * This timer regularly updates the total bitrate count,
         * and also broadcasts it to slave mission controls since they
         * cannot calculate video bitrate
         */
        quint64 bpsRoverDown = 0, bpsRoverUp = 0;
        bpsRoverUp += _frontVideoClient->getBitrate();
        bpsRoverUp += _backVideoClient->getBitrate();
        bpsRoverUp += _clawVideoClient->getBitrate();
        bpsRoverUp += _audioClient->getBitrate();
        bpsRoverUp += _mainChannel->getBitsPerSecondDown();
        bpsRoverDown += _mainChannel->getBitsPerSecondUp();

        _bitrateUpDataSeries->bitrateUpdate(bpsRoverUp);
        _bitrateDownDataSeries->bitrateUpdate(bpsRoverDown);
        _controlWindow->updateBitrate(bpsRoverUp, bpsRoverDown);
    }
    else {
        QObject::timerEvent(e);
    }
}

void MainController::toggleDataRecording()
{
    if (_dataRecorder->isRecording())
    {
        stopDataRecording();
    }
    else
    {
        startDataRecording();
    }
}

void MainController::onSettingsApplied()
{
    _controlWindow->updateSettingsModel(&_settings);

    if (_settings.enableVideo)
    {
        LOG_I(LOG_TAG,"changing video streamer");
        if (_settings.selectedCamera == _settings.frontCameraIndex)
        {
            startFrontCameraStream(_settings.getSelectedVideoProfile(),false);
        }
        else if (_settings.selectedCamera == _settings.backCameraIndex)
        {
            startBackVideoStream(_settings.getSelectedVideoProfile(),false);
        }
        else if(_settings.selectedCamera == _settings.clawCameraIndex)
        {
            startClawVideoStream(_settings.getSelectedVideoProfile(),false);
        }
        else if(_settings.selectedCamera == _settings.allCamerasIndex)
        {
            startFrontCameraStream(_settings.getSelectedVideoProfile(),true);
            startBackVideoStream(_settings.getSelectedVideoProfile(),true);
            //startClawVideoStream(_settings.getSelectedVideoProfile());
        }
    }
    else
    {
        stopAllRoverCameras();
    }
    _mainWindow->setHudVisible(_settings.enableHud);
    _mainWindow->setHudParallax(_settings.selectedHudParallax);
    _mainWindow->setHudLatency(_settings.selectedHudLatency);
    if (_settings.enableAudio)
    {
        startAudioStream(_settings.defaultAudioFormat);
    }
    else
    {
        stopAudio();
    }
    _latencyDataSeries->updateSimulatedLatency(_settings.selectedLatency);

    _audioModeDataSeries->onSettingsChanged(&_settings);
    _videoModeDataSeries->onSettingsChanged(&_settings);
    _videoBitrateDataSeries->onSettingsChanged(&_settings);
    _videoWidthDataSeries->onSettingsChanged(&_settings);
    _videoHeightDataSeries->onSettingsChanged(&_settings);
    _videoCodecDataSeries->onSettingsChanged(&_settings);
    _videoFramerateDataSeries->onSettingsChanged(&_settings);
    _hudParallaxDataSeries->onSettingsChanged(&_settings);
    _hudLatencyDataSeries->onSettingsChanged(&_settings);
}

void MainController::onVideoClientStateChanged(MediaClient *client, MediaClient::State state)
{
    if (client == _backVideoClient)     //back camera
    {
        LOG_I(LOG_TAG, "back video client is changing states");
        switch (state) {
        case VideoClient::StreamingState: {
            LOG_I(LOG_TAG,"playing from back video");
            GStreamerUtil::VideoProfile profile = _backVideoClient->getVideoProfile();
            _mainWindow->playVideo(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_BACK_CAMERA_PORT), profile);
            _settings.enableVideo = true;
            _mainWindow->setSideBySideStereo(false);
            _settings.enableStereoVideo = false;
            _controlWindow->updateFromSettingsModel(&_settings);
            _gstreamerRecorder->begin(profile.codec, QDateTime::currentDateTime(), false);
        }
            break;
        default: {
            LOG_I(LOG_TAG,"Killing back video stream");
            _mainWindow->stopVideo();
            _settings.enableVideo = false;
            _mainWindow->setSideBySideStereo(false);
            _settings.enableStereoVideo = false;
            _controlWindow->updateFromSettingsModel(&_settings);
            _gstreamerRecorder->stop();
        }
            break;
        }
    }
    else if (client == _frontVideoClient)    //forward camera
    {
        LOG_I(LOG_TAG, "front video client is changing states");
        switch (state) {
        case VideoClient::StreamingState: {
            GStreamerUtil::VideoProfile profile = _frontVideoClient->getVideoProfile();
            _mainWindow->playVideo(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_FRONT_CAMERA_PORT), profile);
            _settings.enableVideo = true;
            _mainWindow->setSideBySideStereo(_frontVideoClient->getIsStereo());
            _settings.enableStereoVideo = _frontVideoClient->getIsStereo();
            _controlWindow->updateFromSettingsModel(&_settings);
            _gstreamerRecorder->begin(profile.codec, QDateTime::currentDateTime(), false);
        }
            break;
        default: {
            if (_settings.enableVideo)
            {
                LOG_I(LOG_TAG,"Killing front video");
                _mainWindow->stopVideo();
                _settings.enableVideo = false;
                _mainWindow->setSideBySideStereo(false);
                _settings.enableStereoVideo = false;
                _controlWindow->updateFromSettingsModel(&_settings);
                _gstreamerRecorder->stop();
            }
        }
            break;
        }
    }else if (client == _clawVideoClient)   //claw camera
    {
        LOG_I(LOG_TAG, "claw video client is changing states");
        switch (state) {
        case VideoClient::StreamingState: {
            GStreamerUtil::VideoProfile profile = _clawVideoClient->getVideoProfile();
            _mainWindow->playVideo(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_CLAW_CAMERA_PORT), profile);
            _settings.enableVideo = true;
            _mainWindow->setSideBySideStereo(false);
            _settings.enableStereoVideo = false;
            _controlWindow->updateFromSettingsModel(&_settings);
            _gstreamerRecorder->begin(profile.codec, QDateTime::currentDateTime(), false);
        }
            break;
        default: {
            _mainWindow->stopVideo();
            _settings.enableVideo = false;
            _mainWindow->setSideBySideStereo(false);
            _settings.enableStereoVideo = false;
            _controlWindow->updateFromSettingsModel(&_settings);
            _gstreamerRecorder->stop();
        }
            break;
        }
    }
}

void MainController::onAudioClientStateChanged(MediaClient *client, MediaClient::State state)
{
    Q_UNUSED(client);
    LOG_I(LOG_TAG, "Audio client is changing states");

    switch (state)
    {
    case AudioClient::StreamingState: {
        GStreamerUtil::AudioProfile profile = _audioClient->getAudioProfile();
        _audioPlayer->play(SocketAddress(QHostAddress::LocalHost, NETWORK_ALL_AUDIO_PORT), profile);
        _settings.enableAudio = true;
        _controlWindow->updateFromSettingsModel(&_settings);
        break;
    }
    default:
        if (_settings.enableAudio)
        {
            _audioPlayer->stop();
            _settings.enableAudio = false;
            _controlWindow->updateFromSettingsModel(&_settings);
        }
        break;
    }
}

void MainController::sendStartRecordCommandToRover()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_StartDataRecording;
    stream << static_cast<qint32>(messageType);
    stream << _recordStartTime;
    _mainChannel->sendMessage(message);
}

void MainController::sendStopRecordCommandToRover()
{
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_StopDataRecording;
    stream << static_cast<qint32>(messageType);
    _mainChannel->sendMessage(message);
}

void MainController::onMainChannelMessageReceived(const char *message, Channel::MessageSize size)
{
    QByteArray byteArray = QByteArray::fromRawData(message, size);
    QDataStream stream(byteArray);
    MainMessageType messageType;

    LOG_D(LOG_TAG, "Getting shared channel message");

    stream >> reinterpret_cast<qint32&>(messageType);
    switch (messageType)
    {
    case MainMessageType_RoverStatusUpdate: {
        bool mbedStatus;
        stream >> mbedStatus;
        if (!mbedStatus)
        {
            _controlWindow->notify(NotificationType_Error,
                                   "Mbed Error",
                                   "The rover has lost connection to the mbed. Driving and data collection will no longer work.");
            _controlWindow->setMbedStatus("Error");
        }
        else
        {
            _controlWindow->setMbedStatus("Normal");
        }
    }
        break;
    case MainMessageType_RoverMediaServerError: {
        qint32 mediaId;
        QString error;
        stream >> mediaId;
        stream >> error;

        if (mediaId == _audioClient->getMediaId())
        {
            _controlWindow->notify(NotificationType_Warning,
                                   "Audio Stream Error",
                                   "The rover encountered an error trying to stream audio.");
            LOG_E(LOG_TAG, "Audio streaming error: " + error);
        }
        else
        {
            _controlWindow->notify(NotificationType_Warning,
                                   "Video Stream Error",
                                   "The rover encountered an error trying to stream this camera.");
            LOG_E(LOG_TAG, "Streaming error on camera " + QString::number(mediaId) + ": " + error);
        }
    }
        break;
    case MainMessageType_RoverGpsUpdate: {
        NmeaMessage location;
        stream >> location;
        // Forward to UI
        _controlWindow->updateGpsLocation(location);
        // Forward to logger
        _gpsDataSeries->addLocation(location);
    }
        break;
    case MainMessageType_RoverDriveOverrideStart:
        _controlWindow->notify(NotificationType_Info,
                               "Network Driving Disabled",
                               "The rover is being driven by serial override. Network drive commands will not be accepted.");
        _controlWindow->setMbedStatus("Manual Override");
        break;
    case MainMessageType_RoverDriveOverrideEnd:
        _controlWindow->notify(NotificationType_Info,
                               "Network Driving Enabled",
                               "The rover has resumed accepting network drive commands.");
        _controlWindow->setMbedStatus("Normal");
        break;
    case MainMessageType_SensorUpdate: {
        QByteArray data;
        stream >> data;
        // This raw data should be sent to an MbedParser to be decoded
        _sensorDataSeries->newData(data.constData(), data.length());
        break;
    }
    case MainMessageType_StartDataRecording: {
        // Rover has responed that they are starting data recording, start ours
        if (!_dataRecorder->startLog(
                    QDateTime::fromMSecsSinceEpoch(_recordStartTime),
                    CsvRecorder::RECORDING_MODE_ON_INTERVAL) ||
            !_commentRecorder->startLog(QDateTime::fromMSecsSinceEpoch(_recordStartTime),
                    CsvRecorder::RECORDING_MODE_ON_DEMAND) ||
            !_settingsRecorder->startLog(QDateTime::fromMSecsSinceEpoch(_recordStartTime),
                    CsvRecorder::RECORDING_MODE_ON_DEMAND))
        {
            stopDataRecording();
            _controlWindow->notify(NotificationType_Error,
                                   "Cannot Record Data",
                                   "An error occurred attempting to start data logging.");

            // Try to tell the rover to stop their recording too
            sendStopRecordCommandToRover();
        }
        _controlWindow->setRecordingState(RecordingState_Recording);
        _commentsWindow->setRecordingState(RecordingState_Recording);
        break;
    }
    default:
        LOG_E(LOG_TAG, "Got unknown message header on shared channel");
        break;
    }
}

void MainController::stopAllRoverCameras() {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_StopAllCameraStreams;

    _mainWindow->stopVideo();
    if(_camera2Window != 0){
        _camera2Window->stopVideo();
        _camera3Window->stopVideo();
        _camera2Window = 0;
        _camera3Window = 0;
        //FIXME: figure out how to destroy windows
    }
    stream << static_cast<qint32>(messageType);
    _mainChannel->sendMessage(message);
}

void MainController::stopAudio() {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_RequestDeactivateAudioStream;

    stream << static_cast<qint32>(messageType);
    _mainChannel->sendMessage(message);
}

void MainController::startBackVideoStream(GStreamerUtil::VideoProfile profile,bool multiWindow) {
    if (profile.codec != GStreamerUtil::CODEC_NULL)
    {
        if(multiWindow){
            _camera2Window = new MainWindowController(_self->_qml,_self);
        }else{
            if(_camera2Window != 0){
                _camera2Window->stopVideo();
                _camera2Window = 0;
            }

            if(_camera3Window != 0){
                _camera3Window->stopVideo();
                _camera3Window = 0;
            }
        }
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        MainMessageType messageType;
        messageType = MainMessageType_StartBackCameraStream;

        stream << static_cast<qint32>(messageType);
        stream << profile.toString();
        stream << _settings.useVaapiEncodeForCodec.value(profile.codec, false);
        _mainChannel->sendMessage(message);
    }
    else {
        LOG_E(LOG_TAG, "startAux1VideoStream(): This format is not useable. If you want to stop the video stream, call stopAllRoverCameras() instead");
    }
}

void MainController::startClawVideoStream(GStreamerUtil::VideoProfile profile,bool multiWindow) {
    if (profile.codec != GStreamerUtil::CODEC_NULL)
    {
        if(multiWindow){
            _camera3Window = new MainWindowController(_self->_qml,_self);
        }else{
            if(_camera2Window != 0){
                _camera2Window->stopVideo();
                _camera2Window = 0;
            }

            if(_camera3Window != 0){
                _camera3Window->stopVideo();
                _camera3Window = 0;
            }
        }
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        MainMessageType messageType;
        messageType = MainMessageType_StartClawCameraStream;

        stream << static_cast<qint32>(messageType);
        stream << profile.toString();
        stream << _settings.useVaapiEncodeForCodec.value(profile.codec, false);
        _mainChannel->sendMessage(message);
    }
    else {
        LOG_E(LOG_TAG, "startClawStream(): This format is not useable. If you want to stop the video stream, call stopAllRoverCameras() instead");
    }
}

//main camera is the front camera
void MainController::startFrontCameraStream(GStreamerUtil::VideoProfile profile,bool multiWindow) {
    if (profile.codec != GStreamerUtil::CODEC_NULL)
    {
        if(!multiWindow){
            if(_camera2Window != 0){
                _camera2Window->stopVideo();
                _camera2Window = 0;
            }

            if(_camera3Window != 0){
                _camera3Window->stopVideo();
                _camera3Window = 0;
            }
        }
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        MainMessageType messageType;

        messageType = MainMessageType_StartFrontCameraStream;

        stream << static_cast<qint32>(messageType);
        stream << profile.toString();
        stream << _settings.useVaapiEncodeForCodec.value(profile.codec, false);
        _mainChannel->sendMessage(message);
    }
    else {
        LOG_E(LOG_TAG, "startMainCameraStream(): This format is not useable. If you want to stop the audio stream, call stopAllRoverCameras() instead");
    }
}

void MainController::startAudioStream(GStreamerUtil::AudioProfile profile) {
    if (profile.codec != GStreamerUtil::CODEC_NULL)
    {
        QByteArray message;
        QDataStream stream(&message, QIODevice::WriteOnly);
        MainMessageType messageType;
        messageType = MainMessageType_RequestActivateAudioStream;

        stream << static_cast<qint32>(messageType);
        stream << profile.toString();
        _mainChannel->sendMessage(message);
    }
    else {
        LOG_E(LOG_TAG, "startAudioStream(): This format is not useable. If you want to stop the audio stream, call stopAudio() instead");
    }
}

} // namespace Soro
