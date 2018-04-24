#ifndef GAMEPADMONITOR_H
#define GAMEPADMONITOR_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include "socket.h"
#include <cmath>

QT_BEGIN_NAMESPACE
class QGamepad;
QT_END_NAMESPACE

class GamepadMonitor : public QObject
{
    Q_OBJECT
public:
    //int buttons[10];
    explicit GamepadMonitor(QObject *parent = 0);
    ~GamepadMonitor();

private:
    QGamepad *m_gamepad;
    socket mySocket;
    QTimer *udpTimer;

    void printVals();

    uint8_t base_rotation = 0; //all these numbers are between 0 and 180
    uint8_t shoulder_length = 0;
    uint8_t elbow_length = 0;
    uint8_t wrist_rotation = 0;
    uint8_t wrist_angle = 0;
    uint8_t clawL = 90;
    uint8_t clawR = 70;

    float left_x_axis;
    float left_y_axis;
    float right_x_axis;
    float right_y_axis;
    float coord_u = 90; //we will have to see what these are
    float coord_v = 90;

public slots:
    void onYAxis(double value);
    void onRYAxis(double value);
    void onButtonB(bool pressed);
    void onL2(double value);
    void onR2(double value);
    void onButtonUp(bool pressed);
    void onButtonDown(bool pressed);
    void sendUDP();
    void message(QByteArray arr);
};

#endif // GAMEPADMONITOR_H
