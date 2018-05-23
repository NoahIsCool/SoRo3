#include "mpvlauncher.h"

using namespace Logger;

MPVLauncher::MPVLauncher(QObject *parent) : QObject(parent){

}

void MPVLauncher::start()
{
    ConfigReader reader("/home/soro/videoStreamer/config/missionControl.conf");
    if(!reader.exists()){
        LOG_W(LOG_TAG,"no config file found using defaults\nPlease put a config file at /home/soro/videoStreamer/config/missionControl.conf");
        rover = new QHostAddress("192.168.1.13");

    }
    connected = false;
    rover = new QHostAddress(reader.find("roverAddress"));
    control = new socket(CONTROL_CLIENT_PORT,this);
    heartbeat = new socket(HEARTBEAT_CLIENT_PORT,this);
    initMessage = new QByteArray;
    initMessage->append(INIT);
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
                if(frontVideoThread != nullptr){
                    LOG_W(LOG_TAG,"front video already running! close it, you fool!");
                    break;
                }
                message.append(CAMERA_TOGGLE);
                message.append(START_CAMERA);
                message.append(FRONT);
                LOG_I(LOG_TAG,"starting front camera");
            }else if(option.contains("back")){
                message.append(CAMERA_TOGGLE);
                message.append(START_CAMERA);
                message.append(BACK);
            }else if(option.contains("claw")){
                message.append(CAMERA_TOGGLE);
                message.append(START_CAMERA);
                message.append(CLAW);
            }
        }else if(option.startsWith("stop")){
            if(option.contains("front")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(FRONT);
            }else if(option.contains("back")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(BACK);
            }else if(option.contains("claw")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(CLAW);
            }
        }else if(option == "help"){
            qInfo() << help;// << std::endl;
            break;
        }else if(option == "exit"){
            message.append(EXIT);
            delete frontVideoThread;
            control->sendUDP(*rover,message,CONTROL_PORT);
            exit(1);
        }else{
            qInfo() << "dont recognize that one, noob";// << std::endl;
            break;
        }

        control->sendUDP(*rover,message,CONTROL_PORT);
        LOG_I(LOG_TAG,"message sent");
        message.clear();
    }
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
    if(packet.message.startsWith(INIT)){
        connected = true;
        heartbeatTimer->start(200);
        connectTimer->stop();
        LOG_I(LOG_TAG,"connected to rover");
        inputThread = new std::thread([this](){
            processInput();
        });
    }else if(packet.message.contains(CAMERA_TOGGLE)){
        if(packet.message.contains(CAMERA_STARTED)){
            if(packet.message.contains(FRONT)){
                if(frontVideoThread == nullptr){
                    frontVideoThread = new std::thread([this](){
                        system("gst-launch-1.0 -v udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
                        frontVideoThread = nullptr;
                    });
                }else{
                    LOG_W(LOG_TAG,"front video active! close it, fool");
                }
            }else if(packet.message.contains(BACK)){
                if(backVideoThread == nullptr){
                    backVideoThread = new std::thread([this](){
                        system("gst-launch-1.0 -v udpsrc port=5556 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
                        backVideoThread = nullptr;
                    });
                }else{
                    LOG_W(LOG_TAG,"back video active! close it, fool");
                }
            }else if(packet.message.contains(CLAW)){
                if(backVideoThread == nullptr){
                    clawVideoThread = new std::thread([this](){
                        system("gst-launch-1.0 -v udpsrc port=5557 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink");
                        clawVideoThread = nullptr;
                    });
                }else{
                    LOG_W(LOG_TAG,"claw video active! close it, fool");
                }
            }
        }
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
