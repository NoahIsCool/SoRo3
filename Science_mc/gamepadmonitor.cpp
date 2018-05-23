#include "gamepadmonitor.h"
#include <cmath>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include <stdio.h>


GamepadMonitor::GamepadMonitor( QObject *parent)
    : QObject(parent)
    , m_gamepad(0)
{
    this->phi = 0.0;
    this->wrist_rotation = 0.0;
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        return;
    }

    m_gamepad = new QGamepad(*gamepads.begin(), this);


    //connect(m_gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(onXAxis(double)));
    connect(m_gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(onYAxis(double)));
    //connect(m_gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(onRYAxis(double)));
    //connect(m_gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(onRXAxis(double)));
    connect(m_gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(onButtonB(bool)));
    connect(m_gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(onButtonA(bool)));
    connect(m_gamepad, SIGNAL(buttonXChanged(bool)), this, SLOT(onButtonX(bool)));
    //connect(m_gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(onButtonUp(bool)));
    //connect(m_gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(onButtonDown(bool)));
    connect(m_gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(onL2(double)));
    connect(m_gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(onR2(double)));


    connect(&mySocket, SIGNAL(UDPrecived(QByteArray)), this, SLOT(message(QByteArray)));


    clawL = 75;
    clawR = 150;
    udpTimer = new QTimer();
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(sendUDP()));
    udpTimer->start(100);
}

void GamepadMonitor::message(QByteArray arr){
    printf(arr);
}

/*void GamepadMonitor::onXAxis(double value){
    left_x_axis = value;

}*/

void GamepadMonitor::onYAxis(double value){
    left_y_axis = -value;
}
/*
void GamepadMonitor::onRYAxis(double value){
    right_y_axis = value;
}

void GamepadMonitor::onRXAxis(double value){
    //printf("hello");
    phi = value;
    //qDebug() << phi;
    //else phi = 0;
}
*/
 void GamepadMonitor::onButtonB(bool pressed){
     overdrive = pressed;
 }

 void GamepadMonitor::onButtonA(bool pressed){
     if (pressed){
         spin = 0;// stop the drill
     }
     //qDebug() << "a changed";
 }

 void GamepadMonitor::onButtonX(bool pressed){
     if(pressed){// don't change anything when the button is released
         if (XToggle){
             // it was true before
             // stop spinning the fan
             XToggle = false;
             fan = 0;
         }
         else{
             // spin the fan
             XToggle = true;
             fan = fanSpeed;
         }
     }
 }

void GamepadMonitor::onL2(double value){
    trigger = -value;
    qDebug() << value;
}

void GamepadMonitor::onR2(double value){
    trigger = value;
}
/*
void GamepadMonitor::onButtonUp(bool pressed){

}

void GamepadMonitor::onButtonDown(bool pressed){

}*/

void GamepadMonitor::sendUDP(){

    if(abs(left_y_axis) > .05){
        if (left_y_axis > 0)
            actuator = 0;// move drill forward
        else {
            actuator = 1;// move drill back
        }
    }
    else{
        actuator = 2; // stop moving the drill
    }
    // apply rate of change
    spin += trigger * 2;
    if(spin > 90 || spin < -90)
        spin -= trigger * 2;

    //fan speed is set in button press

    QByteArray out;
    out.append(char(-127));
    out.append(char(2));
    out.append(char(actuator));
    out.append(char(actuatorSpeed));
    out.append(char(spin));
    out.append(char(overdrive));
    out.append(char(fan));
    int hash = (actuator+actuatorSpeed+spin+overdrive+fan)/5;
    out.append(char(hash));

    mySocket.sendUDP(out);
    qDebug()<< "Actuator: "   << actuator  << "\tSpeed: "  <<  actuatorSpeed<<"\tDrill Speed: " << spin
            << "\tOverdrie: " << int(overdrive) << "\tFan Speed: " <<  fan  <<  "\tHash: "<< hash;
}

void GamepadMonitor::printVals(){
    /*
    int hash = (overdrive + leftSide + rightSide +gimbleUD + gimbleRL)/5;

    //sprintf(buf, "%d, %d, %d, %d, %d, %d\n", overdrive, leftSide, rightSide, gimbleUD, gimbleRL, hash);
    //qDebug(buf);
    printf("%c, %d, %d, %d, %d, %d, %d, %d\n", char(-127), 0, overdrive, leftSide, rightSide, gimbleUD, gimbleRL, hash);
    */
}

GamepadMonitor::~GamepadMonitor()
{
    delete m_gamepad;
}

