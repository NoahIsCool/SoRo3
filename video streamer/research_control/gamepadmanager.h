#ifndef SORO_MISSIONCONTROL_GAMEPADMANAGER_H
#define SORO_MISSIONCONTROL_GAMEPADMANAGER_H

#include <QObject>
#include <QTimerEvent>

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

#include "soro_core/logger.h"
#include "soro_core/constants.h"

namespace Soro {

/* Manages gamepad input for the application
 */
class GamepadManager : public QObject {
    Q_OBJECT
public:

    struct GamepadState
    {
        qint16 axisLeftX = 0, axisLeftY = 0, axisRightX = 0, axisRightY = 0,
            axisLeftTrigger = 0, axisRightTrigger = 0;
        bool buttonA = false, buttonB = false, buttonX = false, buttonY = false,
            buttonLeftShoulder = false, buttonRightShoulder = false,
            buttonLeftStick = false, buttonRightStick = false,
            buttonBack = false, buttonStart = false;

        bool dpadUp = false, dpadLeft = false, dpadRight = false, dpadDown = false;

    };

    explicit GamepadManager(QString gcdbPath, QObject *parent = 0);
    ~GamepadManager();

    bool init(int interval, QString *error);

    /* Gets the currently connected gamepad, or null if no gamepad is connected.
     */
    SDL_GameController* getGamepad();

    /* Gets the name of the currently connected gamepad, or an empty string
     * if no gamepad is connected.
     */
    QString getGamepadName() const;

    /* Returns true if a gamepad is current connected and being polled
     */
    bool isGamepadConnected() const;

    /* Gets the state of every button and axis on the gamepad the last time it was polled
     */
    const GamepadManager::GamepadState* getGamepadState() const;

Q_SIGNALS:
    /* Emitted when the gamepad changes
     */
    void gamepadChanged(bool connected, QString name);
    /* Emitted when new values are read from the gamepad
     */
    void poll(const GamepadManager::GamepadState* state);

protected:
    void timerEvent(QTimerEvent *event);

private:
    GamepadState _gamepadState;
    bool _sdlInitialized = false;
    QString _gamepadName;
    bool _initialized = false;
    int _interval;
    SDL_GameController *_gameController = nullptr;
    int _inputSelectorTimerId = TIMER_INACTIVE;
    int _updateTimerId = TIMER_INACTIVE;

    void setGamepad(SDL_GameController *controller);
    void resetGamepadState();

public:
};

} // namespace Soro

#endif // SORO_MISSIONCONTROL_GAMEPADMANAGER_H
