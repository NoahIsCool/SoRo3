#include "mpvlauncher.h"

MPVLauncher::MPVLauncher(QObject *parent)
{
    ConfigReader reader("/opt/project-remoteVideo/config/missionControl.conf");
    if(!reader.exists()){
        LOG_W(LOG_TAG,"no config file found using defaults");
        rover = new QHostAddress("192.168.1.183");
        control = new socket(CONTROL_PORT);
        heartbeat = new socket(HEARTBEAT_PORT);

    }
    rover = new QHostAddress(reader.find("roverAddress"));
    connect(timer,SIGNAL(timeout()),this,SLOT(beatHeart()));
    timer = new QTimer;
    timer->start(200);

    QByteArray message;
    std::string option = "";
    std::cout << "Enter stream to see:\nfront \nback\nclaw\nhelp\n" << std::endl;
    while(true){
        std::cin >> option;

        if(option == "front"){
            message.append("front");
            control->sendUDP(*rover,message);
            system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
        }else if(option == "back"){
            message.append("front");
            control->sendUDP(*rover,message);
            system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
        }else if(option == "claw"){
            message.append("front");
            control->sendUDP(*rover,message);
            system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
        }else if(option == "help"){

        }else{
            std::cout << "dont recognize that one, noob" << std::endl;
        }
    }
}

void MPVLauncher::beatHeart(){
    heartbeat->sendUDP(*rover,"ping");
    timer->stop();
    timer->start(200);
}
