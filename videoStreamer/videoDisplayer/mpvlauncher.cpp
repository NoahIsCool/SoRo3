#include "mpvlauncher.h"

using namespace Logger;

MPVLauncher::MPVLauncher(QObject *parent) : QObject(parent){

}

void MPVLauncher::start()
{
    ConfigReader reader("/opt/project-remoteVideo/config/missionControl.conf");
    if(!reader.exists()){
        LOG_W(LOG_TAG,"no config file found using defaults\nPlease put a config file at /home/soro/videoStreamer/config");
        rover = new QHostAddress("192.168.1.13");

    }
    rover = new QHostAddress(reader.find("roverAddress"));
    control = new socket(CONTROL_CLIENT_PORT);
    heartbeat = new socket(HEARTBEAT_CLIENT_PORT);
    controlPacket = new QByteArray;
    control->sendUDP(*rover,*controlPacket,CONTROL_PORT);
    timer = new QTimer(this);
    //timer->setInterval(200);
    connect(timer,SIGNAL(timeout()),this,SLOT(beatHeart()));
    connect(heartbeat,SIGNAL(hasData(DataPacket)),this,SLOT(checkHeartBeat(DataPacket)));
    timer->start(200);

    inputThread = new std::thread([this](){
        processInput();
    });
}

void MPVLauncher::processInput(){
    QByteArray message;
    QString option = "";
    std::string help = "Commands: \nstart \nstop \nhelp \nexit \nOptions:\nfront \nback \nclaw\n";
    std::cout << help << std::endl;
    QTextStream s(stdin);
    while(true){
        option = s.readLine();

        if(option.startsWith("start")){
            if(option.contains("front")){
                message.append("front");
                control->sendUDP(*rover,message,CONTROL_PORT);
                system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
            }else if(option.contains("back")){
                message.append("front");
                control->sendUDP(*rover,message,CONTROL_PORT);
                system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
            }else if(option.contains("claw")){
                message.append("front");
                control->sendUDP(*rover,message,CONTROL_PORT);
                system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
            }
        }else if(option == "help"){
            std::cout << help << std::endl;
        }else if(option == "exit"){
            exit(1);
        }else{
            std::cout << "dont recognize that one, noob" << std::endl;
        }
    }

    message.clear();
}

void MPVLauncher::beatHeart(){
    heartbeat->sendUDP(*rover,"ping",HEARTBEAT_PORT);
    timer->stop();
    timer->start(200);
}

void MPVLauncher::checkHeartBeat(DataPacket packet){
    if(packet.message == "nack"){
        LOG_E(LOG_TAG,"Disconnected from rover");
    }else{
        LOG_I(LOG_TAG,"Ponged from rover");
    }
}
