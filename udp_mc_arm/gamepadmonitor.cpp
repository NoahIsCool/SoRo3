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
    connect(m_gamepad,SIGNAL(axisLeftYChanged(double)), this, SLOT(onYAxis(double)));

    /*connect(m_gamepad, &QGamepad::axisRightXChanged, this, [](double value){
        qDebug() << "Right X" << value;
    });*/
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
    //qDebug() << value << endl;
    if (value > .1 || value < -.1){
        phi = value;
    }
    else phi = 0;
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
    if( wrist_angle > 0 && value > .1)
    theta = -value;
    else theta = 0;


}

void GamepadMonitor::onR2(double value){
    // old claw code, needed to use this for the wrist
    //clawL = 180 - round(value * 100.0);
   // clawR = 80 - round(value * 50.0);
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
    float x_len = sqrt(115912.15-53752.93*cos( acos( (47776.0-hypot2)/(-764.0*sqrt(hypot2)) ) + atan(temp_v/temp_u)+.7062655 ));
    float y_len = sqrt(97698.97-40191.66*cos(3.124-acos((hypot2-339624.0)/(-336246.807))));
    shoulder_length = uint8_t(round((x_len-246.126)*(95/99.822))+40);
    elbow_length = uint8_t(round((y_len-246.126)*(95/99.822))+40);

    //if the stuff is possible update the values
    if(x_len > 246.126 && x_len < 345.948 && y_len > 246.126 && y_len < 345.948)
    {
        if(abs(left_y_axis) > .05)
            coord_u = temp_u;
        if(abs(right_y_axis) > .05)
            coord_v = temp_v;
    }

    wrist_angle += theta;
    if (wrist_angle < 0.0){
        wrist_angle = 0.0;
    }
    wrist_rotation += phi;
    QByteArray out;
    out.append(char(-127));
    out.append(char(1));
    out.append(shoulder_length);
    out.append(elbow_length);
    out.append(wrist_angle);
    out.append(wrist_rotation);
    out.append(clawL);
    out.append(clawR);
    out.append(uint8_t((shoulder_length+elbow_length+wrist_angle+wrist_rotation+clawL+clawR)/6));

    mySocket.sendUDP(out);
    qDebug() << "x:" << coord_u << "\ty:" << coord_v << "\t1:" << shoulder_length << "\t2:" << elbow_length << "\tCl:" << clawL <<"\twrist angle: "<< wrist_angle
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

