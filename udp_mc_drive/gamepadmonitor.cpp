#include "gamepadmonitor.h"
#include <cmath>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include <stdio.h>


GamepadMonitor::GamepadMonitor( QObject *parent)
    : QObject(parent)
    , m_gamepad(nullptr)
{
    // make sure gamepads are connected
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        return;
    }

    // set up signals and slots to react to changes in xbox controller buttons/joysticks
    m_gamepad = new QGamepad(*gamepads.begin(), this);
    connect(m_gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(onYAxis(double)));
    connect(m_gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(onRYAxis(double)));
    connect(m_gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(onButtonB(bool)));
    connect(m_gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(onButtonUp(bool)));
    connect(m_gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(onButtonDown(bool)));
    connect(m_gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(onL2(double)));
    connect(m_gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(onR2(double)));

    // set up a timer to go off every 100 ms, and when that timer goes off send control data to rover
    udpTimer = new QTimer();
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(sendUDP()));
    udpTimer->start(100);
}

void GamepadMonitor::onYAxis(double value){
    // value (from controller) will be between -1 and 1

    if (-0.1 > value || value > 0.1 ){ // deadzone
        leftSide = static_cast<qint8>(-round(value * 90));
    }else{
        leftSide = 0;
    }
}

void GamepadMonitor::onRYAxis(double value){
    // value (from controller) will be between -1 and 1

    if (-0.1 > value || value > 0.1 ){ // deadzone
        rightSide = static_cast<qint8>(-round(value * 90));
    }else{
        rightSide = 0;
    }
}

void GamepadMonitor::onButtonB(bool pressed){
    // change this!
    if(pressed){
         modifiers = 1;
     }else{
         modifiers = 0;
     }
}

void GamepadMonitor::onL2(double value){
        gimbleRL = static_cast<qint8>(-round(value * 5));
}

void GamepadMonitor::onR2(double value){
        gimbleRL = static_cast<qint8>(round(value * 5));
}

void GamepadMonitor::onButtonUp(bool pressed){
    if(pressed){
        gimbleUD = 1;
    }else{
        gimbleUD = 0;
    }
}

void GamepadMonitor::onButtonDown(bool pressed){
    if(pressed){
        gimbleUD = -1;
    }else{
        gimbleUD = 0;
    }
}

void GamepadMonitor::sendUDP(){
    QByteArray out;
    out.append(char(-127));
    out.append(char(0));
    out.append(modifiers);
    out.append(leftSide);
    out.append(rightSide);
    out.append(gimbleUD);
    out.append(gimbleRL);
    qint8 hash = (modifiers + leftSide + rightSide +gimbleUD + gimbleRL)/5;
    out.append(hash);

    mySocket.sendUDP(out);
    printVals();
}

void GamepadMonitor::printVals(){
    int hash = (modifiers + leftSide + rightSide +gimbleUD + gimbleRL)/5;
    printf("%c, %d, %d, %d, %d, %d, %d, %d\n", char(-127), 0, modifiers, leftSide, rightSide, gimbleUD, gimbleRL, hash);
}

GamepadMonitor::~GamepadMonitor()
{
    delete m_gamepad;
}

