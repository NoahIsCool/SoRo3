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
    /*connect(m_gamepad, &QGamepad::axisLeftXChanged, this, [](double value){
        qDebug() << "Left X" << value;
    });*/
    connect(m_gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(onYAxis(double)));
    connect(m_gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(onRYAxis(double)));
    connect(m_gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(onRXAxis(double)));
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

    clawL = 75;
    clawR = 150;
    udpTimer = new QTimer();
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(sendUDP()));
    udpTimer->start(100);
}

void GamepadMonitor::message(QByteArray arr){
    printf(arr);
}

void GamepadMonitor::onYAxis(double value){
    left_y_axis = value;

}

void GamepadMonitor::onRYAxis(double value){
    right_y_axis = value;
}

void GamepadMonitor::onRXAxis(double value){
    //printf("hello");
    phi = value;
    //qDebug() << phi;
    //else phi = 0;
}

 void GamepadMonitor::onButtonB(bool pressed){
     if (pressed){
        clawL = 35.0;
        clawR = 90.0;
    }
     else{
        clawL = 75.0;
        clawR = 150.0;
     }
     sendUDP();
 }

void GamepadMonitor::onL2(double value){

    //if( wrist_angle > 0 && value > .1)
    theta = -value;
    //else theta = 0;


}

void GamepadMonitor::onR2(double value){

    //if(value > .1)
    theta = value;
    //else theta = 0;

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
    //these units are in mm
    //max actuator length: 345.948mm = 145
    //min actuator length: 246.126mm = 35

    //do the calculations here and pray that they're correct
    float temp_u = coord_u-(left_y_axis*5)*(abs(left_y_axis) > .05);
    float temp_v = coord_v-(right_y_axis*5)*(abs(right_y_axis) > .05);
    float hypot2 = pow(temp_u, 2) + pow(temp_v, 2);
    float x_len = sqrt(160.6811-77.8123*cos( acos( (31.3201-hypot2)/(-36.0*sqrt(hypot2)) ) + atan(temp_v/temp_u)+.40602 ));
    float y_len = sqrt(134.8337-(85.8577*cos(2.91186-acos((hypot2-481.3801)/(-481.3)))));
    shoulder_length = uint8_t(round((x_len-246.126)*(95/99.822))+40);
    elbow_length = uint8_t(round((y_len-246.126)*(95/99.822))+40);

    //if the stuff is possible update the values
    if(true)//x_len > 246.126 && x_len < 345.948 && y_len > 246.126 && y_len < 345.948)
    {
        if(abs(left_y_axis) > .05)
            coord_u = temp_u;
        if(abs(right_y_axis) > .05)
            coord_v = temp_v;
    }

    wrist_angle = 127 * theta;
    if (wrist_angle < 10 && wrist_angle > -10) {
        wrist_angle = 0;
    }
    wrist_rotation = 127 * phi;
    if (wrist_rotation < 10 && wrist_rotation > -10) {
        wrist_rotation = 0;
    }

    QByteArray out;
    out.append(char(-127));
    out.append(char(1));
    out.append(shoulder_length);
    out.append(elbow_length);
    out.append(char(wrist_angle));
    out.append(char(wrist_rotation));
    out.append(clawL);
    out.append(clawR);
    out.append(uint8_t((shoulder_length+elbow_length+wrist_angle+wrist_rotation+clawL+clawR)/6));

    mySocket.sendUDP(out);
    qDebug() << "x:" << coord_u << "\ty:" << coord_v << "\tx len: " << x_len<< "\ty len"<< y_len << "\t1:" << shoulder_length << "\t2:" << elbow_length << "\tCl:" << clawL <<"\twrist angle: "<< wrist_angle
             <<"\twrist rotation: "<< wrist_rotation << "\thash: "<< uint8_t((coord_u+coord_v+wrist_angle+wrist_rotation+clawL+clawR)/6) << endl;


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

