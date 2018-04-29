#ifndef GAMEPADMONITOR_H
#define GAMEPADMONITOR_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include "socket.h"

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

    qint8 overdrive = 0;
    qint8 leftSide=0;
    qint8 rightSide=0;
    qint8 gimbleUD=0;
    qint8 gimbleRL=0;

public slots:
    void onYAxis(double value);
    void onRYAxis(double value);
    void onButtonB(bool pressed);
    void onL2(double value);
    void onR2(double value);
    void onButtonUp(bool pressed);
    void onButtonDown(bool pressed);
    void sendUDP();
};

#endif // GAMEPADMONITOR_H
