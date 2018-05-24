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
    connect(m_gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(onXAxis(double)));
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

void GamepadMonitor::onXAxis(double value){
    left_x_axis = value;

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
    // control the coordinate based on controller input
    // the number 0.2 below means its top speed is 0.2 inches/second
    float temp_u = coord_u;
    float temp_v = coord_v;
    if(abs(left_x_axis) > .05)
        temp_u += left_x_axis*0.2;
    if(abs(left_y_axis) > .05)
        temp_v -= left_y_axis*0.2;

    //make sure it stays in bounds:
    // x^2 + y^2 <= 956.4557
    if( hypot(temp_u, temp_v) > 30.93 )
    {
        temp_u += temp_u*((30.93/hypot(temp_u, temp_v))-1);
        temp_v += temp_v*((30.93/hypot(temp_u, temp_v))-1);
    }
    // x^2 + y^2 >= 427.3300
    if( hypot(temp_u, temp_v) < 20.67 )
    {
        temp_u += temp_u*((20.67/hypot(temp_u, temp_v))-1);
        temp_v += temp_v*((20.67/hypot(temp_u, temp_v))-1);
    }
    // (x-14.8678)^2 + (y-1.9870)^2 <= 324.3601
    if( hypot(temp_u-14.8678, temp_v-1.9870) > 18.01 )
    {
        temp_u += (temp_u-14.8678)*((18.01/hypot((temp_u-14.8678), (temp_v-1.9870)))-1);
        temp_v += (temp_v-1.9870)*((18.01/hypot((temp_u-14.8678), (temp_v-1.9870)))-1);
    }
    // (x-1.2187)^2 + (y-14.9504)^2 >= 324.3601
    if( hypot(temp_u-1.2187, temp_v-14.9504) < 18.01 )
    {
        temp_u += (temp_u-1.2187)*((18.01/hypot((temp_u-1.2187), (temp_v-14.9504)))-1);
        temp_v += (temp_v-14.9504)*((18.01/hypot((temp_u-1.2187), (temp_v-14.9504)))-1);
    }
    emit clawPosUpdated(temp_u, temp_v);

    // calculate the length of each actuator based on the cartesian coordinate (u,v)
    float hypot2 = pow(temp_u, 2) + pow(temp_v, 2);
    float x_len = sqrt(160.6811-77.8123*cos( acos( (99.3601-hypot2)/(-30.0*sqrt(hypot2)) ) + atan(temp_v/temp_u)+.40602 ));
    float y_len = sqrt(180.5948-(100.9791*cos(2.96241-acos((hypot2-549.3601)/(-540.3)))));

    // convert inches to degrees for the servos
    // max: 13.62 in = 135 deg
    // min: 9.69 in = 40 deg
    shoulder_length = uint8_t(round((x_len-9.69)*(95/3.93))+40);
    elbow_length = uint8_t(round((y_len-9.69)*(95/3.93))+40);

    //only update the position if its physically possible for the claw to be there
    if(true)//x_len > 9.69 && x_len < 13.62 && y_len > 9.69 && y_len < 13.62)
    {
            coord_u = temp_u;
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
    qDebug() << "x:" << coord_u << "\ty:" << coord_v << "\tx len: " << x_len<< "\ty len:"<< y_len << "\t1:" << shoulder_length << "\t2:" << elbow_length << "\tCl:" << clawL <<"\twrist angle: "<< wrist_angle
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

