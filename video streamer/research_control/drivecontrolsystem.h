#ifndef SORO_MISSIONCONTROL_DRIVECONTROLSYSTEM_H
#define SORO_MISSIONCONTROL_DRIVECONTROLSYSTEM_H

#include <QObject>
#include <QTimerEvent>

#include "soro_core/channel.h"
#include "soro_core/logger.h"
#include "soro_core/drivemessage.h"
#include "soro_core/constants.h"
#include "soro_core/enums.h"
#include "soro_core/channel.h"

#include "gamepadmanager.h"

namespace Soro {

class DriveControlSystem : public QObject {
    Q_OBJECT
public:
    explicit DriveControlSystem(const QHostAddress& roverAddress, QObject *parent = 0);

    void enable();
    void disable();

    void setMode(DriveGamepadMode mode);
    DriveGamepadMode getMode() const;

    void setMiddleSkidFactor(float factor);
    float getMiddleSkidFactor() const;

    void setSendInterval(int interval);
    int getSendInterval() const;

    void setDeadzone(float deadzone);
    float getDeadzone() const;

    Channel* getChannel();

protected:
    void timerEvent(QTimerEvent *e);

Q_SIGNALS:
    void driveMessageSent(const char *message, int size);

public Q_SLOTS:
    void gamepadPoll(const GamepadManager::GamepadState *state);
    void gamepadChanged(bool connected, QString name);

private:
    bool _gamepadConnected;
    float _deadzone;
    Channel *_channel = 0;
    int _interval;
    const GamepadManager::GamepadState *_gamepadState = 0;
    DriveGamepadMode _mode = DualStickDrive;
    int _controlSendTimerId = TIMER_INACTIVE;
    float _midSkidFactor; //The higher this is, the slower the middle wheels turn while skid steering
    char _buffer[256];
};

} // namespace Soro

#endif // SORO_MISSIONCONTROL_DRIVECONTROLSYSTEM_H
