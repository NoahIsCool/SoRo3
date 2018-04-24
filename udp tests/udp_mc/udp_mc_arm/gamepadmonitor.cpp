#include "gamepadmonitor.h"
#include <cmath>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include <stdio.h>


GamepadMonitor::GamepadMonitor( QObject *parent)
    : QObject(parent)
    , m_gamepad(0)
{
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        return;
    }

    m_gamepad = new QGamepad(*gamepads.begin(), this);
    /*connect(m_gamepad, &QGamepad::axisLeftXChanged, this, [](double value){
        qDebug() << "Left X" << value;
    });*/
    connect(m_gamepad,SIGNAL(axisLeftYChanged(double)), this, SLOT(onYAxis(double)));

    /*connect(m_gamepad, &QGamepad::axisRightXChanged, this, [](double value){
        qDebug() << "Right X" << value;
    });*/
    connect(m_gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(onRYAxis(double)));
    connect(m_gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(onButtonB(bool)));
    connect(m_gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(onButtonUp(bool)));
    connect(m_gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(onButtonDown(bool)));
    connect(m_gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(onL2(double)));
    connect(m_gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(onR2(double)));
    connect(&mySocket, SIGNAL(UDPrecived(QByteArray)), this, SLOT(message(QByteArray)));
    /*
    connect(m_gamepad, &QGamepad::buttonAChanged, this, [](bool pressed){
        qDebug() << "Button A" << pressed;
    });
     connect(m_gamepad, &QGamepad::buttonXChanged, this, [](bool pressed){
        qDebug() << "Button X" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonYChanged, this, [](bool pressed){
        qDebug() << "Button Y" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonL1Changed, this, [](bool pressed){
        qDebug() << "Button L1" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonR1Changed, this, [](bool pressed){
        qDebug() << "Button R1" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonL2Changed, this, [](double value){
        qDebug() << "Button L2: " << value;
    });
    connect(m_gamepad, &QGamepad::buttonR2Changed, this, [](double value){
        qDebug() << "Button R2: " << value;
    });
    connect(m_gamepad, &QGamepad::buttonSelectChanged, this, [](bool pressed){
        qDebug() << "Button Select" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonStartChanged, this, [](bool pressed){
        qDebug() << "Button Start" << pressed;
    });
    connect(m_gamepad, &QGamepad::buttonGuideChanged, this, [](bool pressed){
        qDebug() << "Button Guide" << pressed;
    });*/

    udpTimer = new QTimer();
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(sendUDP()));
    udpTimer->start(100);
}

void GamepadMonitor::message(QByteArray arr){
    printf(arr);
}

void GamepadMonitor::onYAxis(double value){
    left_y_axis = value;
    sendUDP();
}

void GamepadMonitor::onRYAxis(double value){
    right_y_axis = value;
    sendUDP();
}

 void GamepadMonitor::onButtonB(bool pressed){
     /*
     if( pressed){
         overdrive = 1;
     }else{
         overdrive = 0;
     }
     //printVals();*gamepads.begin()
     //sendUDP();
     */
 }

void GamepadMonitor::onL2(double value){
    /*
    //if ( !(-0.05 < value && value < 0.05) ){
        gimbleRL = round(value * 5 );//*gamepads.begin() ???
   // }else{
     //       gimbleRL = 0;
   // }
    //printVals();
    //sendUDP();
    */
}

void GamepadMonitor::onR2(double value){
    qDebug() << value << endl;
    clawL = 180 - round(value * 100.0);
    clawR = 80 - round(value * 50.0);

    sendUDP();

}

void GamepadMonitor::onButtonUp(bool pressed){
    /*
    if( pressed){
        gimbleUD = 1;
    }else{
        gimbleUD = 0;
    }
    //printVals();
    //sendUDP();
    */
}

void GamepadMonitor::onButtonDown(bool pressed){
/*
        if( pressed){
            gimbleUD = -1;
        }else{
            gimbleUD = 0;
        }
        //printVals();
        //sendUDP();
*/
}

void GamepadMonitor::sendUDP(){
 /*
    //do the calculations here and pray that they're correct
    float hypot = pow(coord_u-(left_y_axis*5)*(abs(left_y_axis) > .05), 2) + pow(coord_v-(right_y_axis*5)*(abs(right_y_axis) > .05), 2);
    float x_len = sqrt(115913.23-40797.380*cos(acos((74882.01-hypot)/(-764*sqrt(hypot)))+atan(coord_u/coord_v)+0.7062655093));
    float y_len = sqrt(97180.86-37707.52883*cos(4.519421862-acos((hypot-366730.01)/(-359003.6))));

    shoulder_length = uint8_t(round((x_len-246.126)*(100/99.822))+45);
    elbow_length = uint8_t(round((y_len-246.126)*(100/99.822))+45);

    //if the stuff is possible update the values
    if(x_len > 246.126 && x_len < 345.948 && y_len > 246.126 && y_len < 345.948)
    {
        if(abs(left_y_axis) > .05)
            coord_u += -(left_y_axis*5);
        if(abs(right_y_axis) > .05)
            coord_v += -(right_y_axis*5);

        QByteArray out;
        out.append(char(-127));
        out.append(char(1));
        out.append(shoulder_length);
        out.append(elbow_length);
        out.append(uint8_t((shoulder_length+elbow_length)/2));

        mySocket.sendUDP(out);
    }

    qDebug() << "x:" << coord_u << "\ty:" << coord_v << "\t1:" << shoulder_length << "\t2:" << elbow_length;
*/

    if(abs(left_y_axis) > .05)
        coord_u += -(left_y_axis);
    if(abs(right_y_axis) > .05)
        coord_v += -(right_y_axis);
    if(coord_u > 180)
        coord_u = 180;
    if(coord_v > 180)
        coord_v = 180;
    if(coord_u < 0)
        coord_u = 0;
    if(coord_v < 0)
        coord_v = 0;

    QByteArray out;
    out.append(char(-127));
    out.append(char(1));
    out.append(uint8_t(coord_u));
    out.append(uint8_t(coord_v));
    out.append(clawL);
    out.append(clawR);
    out.append(uint8_t((coord_u+coord_v+clawL+clawR)/4));

    mySocket.sendUDP(out);
    qDebug() << "x:" << coord_u << "\ty:" << coord_v << "\tCl:" << clawL;


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

