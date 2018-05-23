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
    connected = false;
    rover = new QHostAddress(reader.find("roverAddress"));
    control = new socket(CONTROL_CLIENT_PORT,this);
    heartbeat = new socket(HEARTBEAT_CLIENT_PORT,this);
    initMessage = new QByteArray;
    initMessage->append("init");
    control->sendUDP(*rover,*initMessage,CONTROL_PORT);
    qInfo() << "connecting...";
    connectTimer = new QTimer(this);
    heartbeatTimer = new QTimer(this);
    //timer->setInterval(200);
    connect(heartbeatTimer,SIGNAL(timeout()),this,SLOT(beatHeart()));
    connect(heartbeat,SIGNAL(hasData(DataPacket)),this,SLOT(checkHeartBeat(DataPacket)));
    connect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
    connect(connectTimer,SIGNAL(timeout()),this,SLOT(attemptConnection()));
    //heartbeatTimer->start(200);
    connectTimer->start(3000);
}

void MPVLauncher::processInput(){
    QByteArray message;
    QString option = "";
    QString help = "Commands: \nstart \nstop \nhelp \nexit \nOptions:\nfront \nback \nclaw\n";
    qInfo() << help;// << "\n";
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
            qInfo() << help;// << std::endl;
        }else if(option == "exit"){
            exit(1);
        }else{
            qInfo() << "dont recognize that one, noob";// << std::endl;
        }
    }

    message.clear();
}

void MPVLauncher::attemptConnection(){
    if(connected == false){
        qInfo() << ".";
        control->sendUDP(*rover,*initMessage,CONTROL_PORT);
        connectTimer->stop();
        connectTimer->start(3000);
    }
}

void MPVLauncher::onMessage(DataPacket packet){
    LOG_I(LOG_TAG,"got message " + packet.message);
    if(packet.message == "init"){
        connected = true;
        heartbeatTimer->start(200);
        connectTimer->stop();
        LOG_I(LOG_TAG,"connected to rover");
        inputThread = new std::thread([this](){
            processInput();
        });
    }else if(packet.message == "ack"){
        //all good
    }else if(packet.message == "nack"){
        LOG_E(LOG_TAG,"got nack...shit");
    }else if(packet.message == "timeout"){
        connected = false;
        LOG_E(LOG_TAG,"DISCONNECTED");
        heartbeatTimer->stop();
        connectTimer->start(1000);
    }else{
        LOG_W(LOG_TAG,"got some shit..." + packet.message);
    }
}

void MPVLauncher::beatHeart(){
    //LOG_I(LOG_TAG,"beating heart");
    heartbeat->sendUDP(*rover,"ping",HEARTBEAT_PORT);
    heartbeatTimer->stop();
    heartbeatTimer->start(200);
}

void MPVLauncher::checkHeartBeat(DataPacket packet){
    if(packet.message == "nack"){
        LOG_E(LOG_TAG,"Disconnected from rover");
        connected = false;
    }else if(packet.message == "timeout"){
        LOG_W(LOG_TAG,"timedout");
        connected = true;
    }else{
        LOG_I(LOG_TAG,"Ponged from rover");
    }
}
