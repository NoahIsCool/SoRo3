#include "gamepadmonitor.h"
#include<cmath>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include<stdio.h>


GamepadMonitor::GamepadMonitor( QObject *parent)
    : QObject(parent)
    , m_gamepad(0)
{
    //mySocket.sendUDP("hey");
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

void GamepadMonitor::onYAxis(double value){
    if (-0.1 > value || value > 0.1 ){
        if (value > 0){
             leftSide = -round(value * 80);
        } else{
             leftSide = -round(value * 90);
        }
    }
    else
        leftSide = 0;
    //printVals();
    //sendUDP();
}

void GamepadMonitor::onRYAxis(double value){

    if (-0.1 > value || value > 0.1 ){
        if (value > 0){
             rightSide = -round(value * 80);
        } else{
             rightSide = -round(value * 90);
        }
    }else{
        rightSide = 0;
    }
    //printVals(); you disconnect the red
    //sendUDP();

}

 void GamepadMonitor::onButtonB(bool pressed){
     if( pressed){
         overdrive = 1;
     }else{
         overdrive = 0;
     }
     //printVals();
     //sendUDP();
 }

void GamepadMonitor::onL2(double value){
    //if ( !(-0.05 < value && value < 0.05) ){
        gimbleRL = round(value * 5 );
   // }else{
     //       gimbleRL = 0;
   // }
    //printVals();
    //sendUDP();
}

void GamepadMonitor::onR2(double value){
  //  if (!( -0.05 < value && value < 0.05 )){
        gimbleRL = -round(value * 5 );
  //  }else{
  //          gimbleRL = 0;
  //  }
    //printVals();
    //sendUDP();
}

void GamepadMonitor::onButtonUp(bool pressed){
    if( pressed){
        gimbleUD = 1;
    }else{
        gimbleUD = 0;
    }
    //printVals();
    //sendUDP();
}

void GamepadMonitor::onButtonDown(bool pressed){

        if( pressed){
            gimbleUD = -1;
        }else{
            gimbleUD = 0;
        }
        //printVals();
        //sendUDP();

}

void GamepadMonitor::sendUDP(){
    QByteArray out;
    out.append(char(-127));
    out.append(char(0));
    out.append(overdrive);
    out.append(leftSide);
    out.append(rightSide);
    out.append(gimbleUD);
    out.append(gimbleRL);
    int hash = overdrive + leftSide + rightSide +gimbleUD + gimbleRL;
    hash = hash/5;
    out.append(hash);

    mySocket.sendUDP(out);
    printVals();
}

void GamepadMonitor::printVals(){
    char* buf;
    int hash = (overdrive + leftSide + rightSide +gimbleUD + gimbleRL)/5;

     //sprintf(buf, "%d, %d, %d, %d, %d, %d\n", overdrive, leftSide, rightSide, gimbleUD, gimbleRL, hash);
    //qDebug(buf);
    printf("%c, %d, %d, %d, %d, %d, %d, %d\n", char(-127), 0, overdrive, leftSide, rightSide, gimbleUD, gimbleRL, hash);
        //return;
    }/**/

GamepadMonitor::~GamepadMonitor()
{
    delete m_gamepad;
}

