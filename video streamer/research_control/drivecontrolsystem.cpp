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

#include "drivecontrolsystem.h"
#include "maincontroller.h"

#include "soro_core/constants.h"

#define LOG_TAG "DriveControlSystem"

namespace Soro {

DriveControlSystem::DriveControlSystem(const QHostAddress& roverAddress, QObject *parent) : QObject(parent)
{
    _channel = Channel::createClient(this, SocketAddress(roverAddress, NETWORK_ALL_DRIVE_CHANNEL_PORT), CHANNEL_NAME_DRIVE,
                        Channel::UdpProtocol, QHostAddress::Any);
    if (_channel->getState() == Channel::ErrorState)
    {
        MainController::panic(LOG_TAG, "Could not open channel for drive control");
    }
    _channel->open();
    _midSkidFactor = 0.2;
    _deadzone = 0.1;
    _gamepadConnected = false;
    setSendInterval(100);
}

void DriveControlSystem::enable()
{
    START_TIMER(_controlSendTimerId, _interval);
}

void DriveControlSystem::disable()
{
    KILL_TIMER(_controlSendTimerId);
}

void DriveControlSystem::setSendInterval(int interval)
{
    _interval = interval;
    KILL_TIMER(_controlSendTimerId);
    START_TIMER(_controlSendTimerId, _interval);
}

void DriveControlSystem::setMiddleSkidFactor(float factor)
{
    _midSkidFactor = factor;
}

void DriveControlSystem::setDeadzone(float deadzone)
{
    _deadzone = deadzone;
}

void DriveControlSystem::setMode(DriveGamepadMode mode)
{
    _mode = mode;
}

void DriveControlSystem::gamepadChanged(bool connected, QString name)
{
    Q_UNUSED(name);
    _gamepadConnected = connected;
}

void DriveControlSystem::timerEvent(QTimerEvent *e)
{
    QObject::timerEvent(e);
    if ((e->timerId() == _controlSendTimerId) && _channel && _gamepadConnected && _gamepadState)
    {
        //send the rover a drive gamepad packet
        switch (_mode)
        {
        case SingleStickDrive:
            DriveMessage::setGamepadData_SingleStick(_buffer, _gamepadState->axisLeftX, _gamepadState->axisLeftY, _midSkidFactor, _deadzone);
            qDebug() << (int)_buffer[1] << " " << (int)_buffer[2] << " " << (int)_buffer[3] << " " << (int)_buffer[4];
            break;
        case DualStickDrive:
            DriveMessage::setGamepadData_DualStick(_buffer, _gamepadState->axisLeftY, _gamepadState->axisRightX, _midSkidFactor, _deadzone);
            break;
        }
        _channel->sendMessage(_buffer, DriveMessage::RequiredSize);
        Q_EMIT driveMessageSent(_buffer, DriveMessage::RequiredSize);
    }
}

void DriveControlSystem::gamepadPoll(const GamepadManager::GamepadState *state)
{
    _gamepadState = state;
}

DriveGamepadMode DriveControlSystem::getMode() const
{
    return _mode;
}

float DriveControlSystem::getMiddleSkidFactor() const
{
    return _midSkidFactor;
}

float DriveControlSystem::getDeadzone() const
{
    return _deadzone;
}

Channel* DriveControlSystem::getChannel()
{
    return _channel;
}

} // namespace Soro
