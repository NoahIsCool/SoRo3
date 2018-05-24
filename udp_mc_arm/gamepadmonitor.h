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
    float wrist_rotation = 0;// in degrees
    float wrist_angle = 0;
    uint8_t clawL = 120;
    uint8_t clawR = 70;

    float phi;// for the wrist
    float theta;// for the wrist
    float left_x_axis;
    float left_y_axis;
    float right_x_axis;
    float right_y_axis;
    float coord_u = 18.5; //we will have to see what these are
    float coord_v = 9.5;

public slots:
    void onXAxis(double value);
    void onYAxis(double value);
    void onRYAxis(double value);
    void onRXAxis(double value);
    void onButtonB(bool pressed);
    void onL2(double value);
    void onR2(double value);
    void onButtonUp(bool pressed);
    void onButtonDown(bool pressed);
    void sendUDP();
    void message(QByteArray arr);

signals:
    void clawPosUpdated(float x, float y);
};

#endif // GAMEPADMONITOR_H
