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

#include "gamepadmanager.h"
#include "maincontroller.h"

#define LOG_TAG "GamepadManager"

namespace Soro {

GamepadManager::GamepadManager(QString gcdbPath, QObject *parent) : QObject(parent)
{
    _interval = 50;
    LOG_I(LOG_TAG, "Calling SDL::init...");
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) == 0)
    {
        // Add gamepad map
        LOG_I(LOG_TAG, "Loading mappings from " + gcdbPath);
        int mapCount = SDL_GameControllerAddMappingsFromFile(gcdbPath.toLocal8Bit().constData());
        if (mapCount == -1)
        {
            MainController::panic(LOG_TAG, QString("Unable to apply game controller map from %1.\n\nError: %2").arg(
                                      gcdbPath,
                                      SDL_GetError()));
        }
        LOG_I(LOG_TAG, QString("Added mappings for %1 game controllers").arg(mapCount));

        START_TIMER(_inputSelectorTimerId, 1000);
    }
    else
    {
        MainController::panic(LOG_TAG, "SDL failed to initialize: " + QString(SDL_GetError()));
    }
    resetGamepadState();
}

GamepadManager::~GamepadManager()
{
    SDL_Quit();
}

SDL_GameController* GamepadManager::getGamepad()
{
    return _gameController;
}

QString GamepadManager::getGamepadName() const
{
    return _gamepadName;
}

bool GamepadManager::isGamepadConnected() const
{
    return _gameController != nullptr;
}

void GamepadManager::timerEvent(QTimerEvent *e)
{
    QObject::timerEvent(e);
    if (e->timerId() == _inputSelectorTimerId)
    {
        /***************************************
         * This timer querys SDL at regular intervals to look for a
         * suitable controller
         */
        SDL_GameControllerUpdate();
        //LOG_I(LOG_TAG, "Looking for usable joysticks...");
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            char guid_str[256];
            SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(i);
            const char * name = SDL_JoystickNameForIndex(i);
            SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
            LOG_I(LOG_TAG, QString("Considering joystick %1 (%2) with GUID %3...").arg(QString::number(i), name, QString(guid_str)));
            if (SDL_IsGameController(i))
            {
                SDL_GameController *controller = SDL_GameControllerOpen(i);
                if (controller) {
                    //this gamepad will do
                    setGamepad(controller);
                    KILL_TIMER(_inputSelectorTimerId);
                    START_TIMER(_updateTimerId, _interval);
                    break;
                }
                else
                {
                    LOG_E(LOG_TAG, "Failed to open joystick " + QString::number(i));
                }
                SDL_GameControllerClose(controller);
            }
            else
            {
                LOG_I(LOG_TAG, QString("Joystick %1 is not a valid game controller").arg(QString::number(i)));
            }
        }
    }
    else if (e->timerId() == _updateTimerId)
    {
        SDL_GameControllerUpdate();
        if (_gameController && SDL_GameControllerGetAttached(_gameController))
        {
            // Update gamepad data
            _gamepadState.axisLeftX           = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_LEFTX);
            _gamepadState.axisLeftY           = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_LEFTY);
            _gamepadState.axisRightX          = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_RIGHTX);
            _gamepadState.axisRightY          = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_RIGHTY);
            _gamepadState.axisLeftTrigger     = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            _gamepadState.axisRightTrigger    = SDL_GameControllerGetAxis(_gameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            _gamepadState.buttonA             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_A);
            _gamepadState.buttonB             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_B);
            _gamepadState.buttonX             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_X);
            _gamepadState.buttonY             = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_Y);
            _gamepadState.buttonLeftShoulder  = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            _gamepadState.buttonRightShoulder = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            _gamepadState.buttonStart         = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_START);
            _gamepadState.buttonBack          = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_BACK);
            _gamepadState.buttonLeftStick     = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_LEFTSTICK);
            _gamepadState.buttonRightStick    = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
            _gamepadState.dpadUp              = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
            _gamepadState.dpadLeft            = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            _gamepadState.dpadDown            = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            _gamepadState.dpadRight           = SDL_GameControllerGetButton(_gameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            Q_EMIT poll(&_gamepadState);
        }
        else
        {
            // Controller is no longer attached
            resetGamepadState();
            setGamepad(nullptr);
            KILL_TIMER(_updateTimerId);
            START_TIMER(_inputSelectorTimerId, 1000);
            LOG_I(LOG_TAG, "The gamepad has been disconnected");
        }
    }
}

void GamepadManager::setGamepad(SDL_GameController *controller)
{
    if (_gameController != controller)
    {
        _gameController = controller;
        _gamepadName = _gameController ? QString(SDL_GameControllerName(_gameController)) : "";
        if (controller)
        {
            LOG_I(LOG_TAG, "Active controller is \'" + _gamepadName + "\'");
        }
        else
        {
            LOG_W(LOG_TAG, "No usable controller connected");
        }
        Q_EMIT gamepadChanged(isGamepadConnected(), _gamepadName);
    }
}

void GamepadManager::resetGamepadState()
{
    _gamepadState.axisLeftX           = 0;
    _gamepadState.axisLeftY           = 0;
    _gamepadState.axisRightX          = 0;
    _gamepadState.axisRightY          = 0;
    _gamepadState.axisLeftTrigger     = 0;
    _gamepadState.axisRightTrigger    = 0;
    _gamepadState.buttonA             = false;
    _gamepadState.buttonB             = false;
    _gamepadState.buttonX             = false;
    _gamepadState.buttonY             = false;
    _gamepadState.buttonLeftShoulder  = false;
    _gamepadState.buttonRightShoulder = false;
    _gamepadState.buttonStart         = false;
    _gamepadState.buttonBack          = false;
    _gamepadState.buttonLeftStick     = false;
    _gamepadState.buttonRightStick    = false;
    _gamepadState.dpadUp              = false;
    _gamepadState.dpadLeft            = false;
    _gamepadState.dpadDown            = false;
    _gamepadState.dpadRight           = false;
}

} // namespace Soro
