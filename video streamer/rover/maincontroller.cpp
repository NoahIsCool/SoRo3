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
#include "soro_core/confloader.h"
#include "usbcameraenumerator.h"

#define LOG_TAG "ResearchRover"

namespace Soro {

MainController *MainController::_self = nullptr;

MainController::MainController(QObject *parent) : QObject(parent) { }

void MainController::panic(QString tag, QString message)
{
    LOG_E(LOG_TAG, QString("panic(): %1: %2").arg(tag, message));
    LOG_I(LOG_TAG, "Committing suicide...");
    delete _self;
    LOG_I(LOG_TAG, "Exiting with code 1");
    exit(1);
}

void MainController::init(QCoreApplication *app)
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
            Logger::rootLogger()->setLogfile(QCoreApplication::applicationDirPath()
                                             + "/../log/ResearchRover_" + QDateTime::currentDateTime().toString("M-dd_h.mm.ss_AP") + ".log");
            Logger::rootLogger()->setMaxFileLevel(Logger::LogLevelDebug);
            Logger::rootLogger()->setMaxStdoutLevel(Logger::LogLevelInformation);

            LOG_I(LOG_TAG, "*************Initializing core networking*****************");

            _self->_driveChannel = Channel::createServer(_self, NETWORK_ALL_DRIVE_CHANNEL_PORT, CHANNEL_NAME_DRIVE,
                                      Channel::UdpProtocol, QHostAddress::Any);
            _self->_mainChannel = Channel::createServer(_self, NETWORK_ALL_MAIN_CHANNEL_PORT, CHANNEL_NAME_MAIN,
                                      Channel::TcpProtocol, QHostAddress::Any);

            if (_self->_driveChannel->getState() == Channel::ErrorState) {
                panic(LOG_TAG, "The drive channel experienced a fatal error during initialization");
            }
            if (_self->_mainChannel->getState() == Channel::ErrorState) {
                panic(LOG_TAG, "The shared channel experienced a fatal error during initialization");
            }

            _self->_driveChannel->open();
            _self->_mainChannel->open();

            // observers for network channel connectivity changes
            connect(_self->_mainChannel, &Channel::stateChanged, _self, &MainController::mainChannelStateChanged);
            connect(_self->_driveChannel, &Channel::stateChanged, _self, &MainController::driveChannelStateChanged);


            LOG_I(LOG_TAG, "All network channels initialized successfully");

            LOG_I(LOG_TAG, "*****************Initializing MBED systems*******************");

            // create mbed channels
            _self->_mbed = new MbedChannel(SocketAddress(QHostAddress::Any, NETWORK_ROVER_MBED_PORT), MBED_ID, _self);

            // observers for mbed events
            connect(_self->_mbed, &MbedChannel::messageReceived, _self, &MainController::mbedMessageReceived);
            connect(_self->_mbed, &MbedChannel::stateChanged, _self, &MainController::mbedChannelStateChanged);

            // observers for network channels message received
            connect(_self->_driveChannel, &Channel::messageReceived, _self, &MainController::driveChannelMessageReceived);
            connect(_self->_mainChannel, &Channel::messageReceived, _self, &MainController::mainChannelMessageReceived);

            LOG_I(LOG_TAG, "*****************Initializing GPS system*******************");

            _self->_gpsServer = new GpsServer(SocketAddress(QHostAddress::Any, NETWORK_ROVER_GPS_PORT), _self);
            connect(_self->_gpsServer, &GpsServer::gpsUpdate, _self, &MainController::gpsUpdate);

            LOG_I(LOG_TAG, "*****************Initializing Video system*******************");

            _self->_frontCameraServer = new VideoServer(FRONT_CAMERA, NETWORK_ALL_FRONT_CAMERA_PORT, _self);
            _self->_backCameraServer = new VideoServer(BACK_CAMERA, NETWORK_ALL_BACK_CAMERA_PORT, _self);
            _self->_clawCameraServer = new VideoServer(CLAW_CAMERA, NETWORK_ALL_CLAW_CAMERA_PORT, _self);

            connect(_self->_frontCameraServer, &VideoServer::error, _self, &MainController::mediaServerError);
            connect(_self->_backCameraServer, &VideoServer::error, _self, &MainController::mediaServerError);
            connect(_self->_clawCameraServer, &VideoServer::error, _self, &MainController::mediaServerError);

            UsbCameraEnumerator cameras;
            cameras.loadCameras();

            QFile camFile(QCoreApplication::applicationDirPath() + "/../config/research_cameras.conf");
            if (!camFile.exists()) {
                panic(LOG_TAG, "The camera configuration file ../config/research_cameras.conf does not exist.");
            }
            else {
                ConfLoader camConfig;
                camConfig.load(camFile);

                const UsbCamera* frontCamera = cameras.find(camConfig.value("fr_matchName"),
                                                camConfig.value("f_matchDevice"),
                                                camConfig.value("f_matchVendorId"),
                                                camConfig.value("f_matchProductId"),
                                                camConfig.value("f_matchSerial"));

                const UsbCamera* backCamera = cameras.find(camConfig.value("b_matchName"),
                                                camConfig.value("b_matchDevice"),
                                                camConfig.value("b_matchVendorId"),
                                                camConfig.value("b_matchProductId"),
                                                camConfig.value("b_matchSerial"));

                const UsbCamera* clawCamera = cameras.find(camConfig.value("c_matchName"),
                                                camConfig.value("c_matchDevice"),
                                                camConfig.value("c_matchVendorId"),
                                                camConfig.value("c_matchProductId"),
                                                camConfig.value("c_matchSerial"));

                if (frontCamera) {
                    _self->_FrontCameraDevice = frontCamera->device;
                    _self->_FrontCameraDevice.remove("/dev/");
                    LOG_I(LOG_TAG, "Front camera found: " + frontCamera->toString());
                }
                else {
                    LOG_E(LOG_TAG, "Front camera couldn't be found using provided definition.");
                }
                if (backCamera) {
                    _self->_BackCameraDevice = backCamera->device;
                    _self->_BackCameraDevice.remove("/dev/");
                    LOG_I(LOG_TAG, "Back camera found: " + backCamera->toString());
                }
                else {
                    LOG_E(LOG_TAG, "Back camera couldn't be found using provided definition.");
                }
                if (clawCamera) {
                    _self->_ClawCameraDevice = clawCamera->device;
                    _self->_ClawCameraDevice.remove("/dev/");
                    LOG_I(LOG_TAG, "Claw camera found: " + clawCamera->toString());
                }
                else {
                    LOG_E(LOG_TAG, "Claw camera couldn't be found using provided definition.");
                }

            }

            LOG_I(LOG_TAG, "*****************Initializing Audio system*******************");

            _self->_audioServer = new AudioServer(MEDIAID_AUDIO, NETWORK_ALL_AUDIO_PORT, _self);

            connect(_self->_audioServer, &AudioServer::error, _self, &MainController::mediaServerError);

            LOG_I(LOG_TAG, "*****************Initializing Data Recording System*******************");

            _self->_sensorDataSeries = new SensorDataParser(_self);
            _self->_gpsDataSeries = new GpsCsvSeries(_self);
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
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontYawSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontPitchSeries());
            _self->_dataRecorder->addColumn(_self->_sensorDataSeries->getImuFrontRollSeries());
            _self->_dataRecorder->addColumn(_self->_wheelSpeedLODataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedLMDataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedRODataSeries);
            _self->_dataRecorder->addColumn(_self->_wheelSpeedRMDataSeries);
            _self->_dataRecorder->addColumn(_self->_gpsDataSeries->getLatitudeSeries());
            _self->_dataRecorder->addColumn(_self->_gpsDataSeries->getLongitudeSeries());
            connect(_self->_gpsServer, &GpsServer::gpsUpdate, _self->_gpsDataSeries, &GpsCsvSeries::addLocation);
            connect(_self->_mbed, &MbedChannel::messageReceived, _self->_sensorDataSeries, &SensorDataParser::newData);

            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "Initialization complete");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
            LOG_I(LOG_TAG, "-------------------------------------------------------");
        });
    }
}

void MainController::mainChannelStateChanged(Channel::State state) {
    if (state == Channel::ConnectedState) {
        // send all status information since we just connected
        // TODO there is an implementation bug where a Channel will not send messages immediately after it connects
        QTimer::singleShot(1000, this, SLOT(sendSystemStatusMessage()));
    }
}

void MainController::sendSystemStatusMessage() {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    MainMessageType messageType = MainMessageType_RoverStatusUpdate;

    stream << static_cast<qint32>(messageType);
    stream << (_mbed->getState() == MbedChannel::ConnectedState);
    _mainChannel->sendMessage(message);
}

bool MainController::startDataRecording(QDateTime startTime) {
    LOG_I(LOG_TAG, "Starting test log with start time of " + QString::number(startTime.toMSecsSinceEpoch()));

    return _dataRecorder->startLog(startTime, CsvRecorder::RECORDING_MODE_ON_INTERVAL);
}

void MainController::stopDataRecording() {
    LOG_I(LOG_TAG, "Ending test log");

    _dataRecorder->stopLog();
}

void MainController::driveChannelStateChanged(Channel::State state) {
    if (state != Channel::ConnectedState) {
        //Send a stop command to the rover
        char stopMessage[DriveMessage::RequiredSize];
        DriveMessage::setGamepadData_DualStick(stopMessage, 0, 0, 0);
        _mbed->sendMessage(stopMessage, DriveMessage::RequiredSize);
    }
}

void MainController::mbedChannelStateChanged(MbedChannel::State state) {
    Q_UNUSED(state);
    sendSystemStatusMessage();
}

void MainController::driveChannelMessageReceived(const char* message, Channel::MessageSize size) {
    char header = message[0];
    MbedMessageType messageType;
    reinterpret_cast<qint32&>(messageType) = (qint32)reinterpret_cast<unsigned char&>(header);
    switch (messageType) {
    case MbedMessage_Drive:
        _wheelSpeedLMDataSeries->onDriveCommand(message);
        _wheelSpeedLODataSeries->onDriveCommand(message);
        _wheelSpeedRMDataSeries->onDriveCommand(message);
        _wheelSpeedRODataSeries->onDriveCommand(message);
        _mbed->sendMessage(message, (int)size);
        break;
    default:
        LOG_E(LOG_TAG, "Received invalid message from mission control on drive control channel");
        break;
    }
}

void MainController::mediaServerError(MediaServer *server, QString message) {
    QByteArray byeArray;
    QDataStream stream(&byeArray, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    MainMessageType messageType = MainMessageType_RoverMediaServerError;

    stream << messageType;
    stream <<(qint32)server->getMediaId();
    stream << message;
    _mainChannel->sendMessage(byeArray);
}

/* Main channel message handler
 */
void MainController::mainChannelMessageReceived(const char* message, Channel::MessageSize size) {
    QByteArray byteArray = QByteArray::fromRawData(message, size);
    QDataStream stream(byteArray);
    MainMessageType messageType;

    stream >> reinterpret_cast<qint32&>(messageType);
    switch (messageType) {
    case MainMessageType_RequestActivateAudioStream: {
        //
        // Start audio stream
        //
        QString profileString;
        stream >> profileString;
        GStreamerUtil::AudioProfile profile(profileString);
        _audioServer->start(profile);
    }
        break;
    case MainMessageType_RequestDeactivateAudioStream:
        //
        // Stop audio stream
        //
        _audioServer->stop();
        break;
    case MainMessageType_StartFrontCameraStream: {
        //
        // Start MONO main camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_FrontCameraDevice.isEmpty()) {
            _frontCameraServer->start(_FrontCameraDevice, profile, vaapi);
        }
    }
        break;
    case MainMessageType_StopAllCameraStreams:
        //
        // Stop all camera streams
        //
        _frontCameraServer->stop();
        _backCameraServer->stop();
        _clawCameraServer->stop();
        break;
    case MainMessageType_StartBackCameraStream:{
        //
        // Start AUX1 camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_BackCameraDevice.isEmpty()) {
            _backCameraServer->start(_BackCameraDevice, profile, vaapi);
        }
    }
        break;
    case MainMessageType_StartClawCameraStream:{
        //
        // Start AUX1 camera stream
        //
        QString profileString;
        bool vaapi;
        stream >> profileString;
        stream >> vaapi;
        GStreamerUtil::VideoProfile profile(profileString);
        if (!_ClawCameraDevice.isEmpty()) {
            _clawCameraServer->start(_ClawCameraDevice, profile, vaapi);
        }
    }
    case MainMessageType_StartDataRecording: {
        //
        // Start data recording
        //
        qint64 startTime;
        stream >> startTime;
        if (startDataRecording(QDateTime::fromMSecsSinceEpoch(startTime))) {
            // Echo the message back to mission control
            QByteArray byteArray;
            QDataStream stream(&byteArray, QIODevice::WriteOnly);
            MainMessageType messageType = MainMessageType_StartDataRecording;
            stream << static_cast<qint32>(messageType);
            _mainChannel->sendMessage(byteArray);
        }
    }
        break;
    case MainMessageType_StopDataRecording:
        //
        // Stop data recording
        //
        stopDataRecording();
        break;
    default:
        LOG_W(LOG_TAG, "Got unknown shared channel message");
        break;
    }
}

void MainController::mbedMessageReceived(const char* message, int size) {
    // Forward the message to mission control (MbedDataParser instance will take care of logging it)

    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_SensorUpdate;

    stream << static_cast<qint32>(messageType);
    stream << QByteArray(message, size);

    _mainChannel->sendMessage(byteArray);
}

void MainController::gpsUpdate(NmeaMessage message) {
    // Forward this update to mission control
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    MainMessageType messageType = MainMessageType_RoverGpsUpdate;
    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<qint32>(messageType);
    stream << message;

    _mainChannel->sendMessage(byteArray);
}

} // namespace Soro

