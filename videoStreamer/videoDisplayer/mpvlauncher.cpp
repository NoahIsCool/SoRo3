#include "mpvlauncher.h"

using namespace Logger;

MPVLauncher::MPVLauncher(QObject *parent) : QObject(parent){

}

void MPVLauncher::start()
{
    ConfigReader reader("/home/soro/videoStreamer/config/missionControl.conf");
    if(!reader.exists()){
        LOG_W(LOG_TAG,"no config file found using defaults\nPlease put a config file at /home/soro/videoStreamer/config/missionControl.conf");
        rover = new QHostAddress("192.168.1.183");

    }else{
        rover = new QHostAddress(reader.find("roverAddress"));
    }
    connected = false;
    LOG_I(LOG_TAG,"rover address is " + rover->toString());
    control = new socket(CONTROL_CLIENT_PORT,this);
    heartbeat = new socket(HEARTBEAT_CLIENT_PORT,this);
    initMessage = new QByteArray;
    initMessage->append(INIT);
    control->sendUDP(*rover,*initMessage,CONTROL_PORT);
    std::cout << "connecting..." << std::endl;
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
    std::string help = "Commands: \tAvailable Cameras: \nstart \t\tfront \nstop \t\tback \nhelp \t\tclaw \nexit \n\t\taudio";
    std::cout << help << std::endl;
    while(true){
        std::string raw;
        std::getline(std::cin,raw);
        option = QString::fromStdString(raw);

        if(option.startsWith("start")){
            if(option.contains("front")){
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
            }else if(option.contains("audio")){
                message.append(AUDIO_TOGGLE);
                message.append(START_AUDIO);
            }
        }else if(option.startsWith("stop")){
            if(option.contains("front")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(FRONT);
                frontPipeline->setState(QGst::StatePaused);
                frontPipeline->setState(QGst::StateNull);
            }else if(option.contains("back")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(BACK);
                backPipeline->setState(QGst::StatePaused);
                backPipeline->setState(QGst::StateNull);
            }else if(option.contains("claw")){
                message.append(CAMERA_TOGGLE);
                message.append(STOP_CAMERA);
                message.append(CLAW);
                clawPipeline->setState(QGst::StatePaused);
                clawPipeline->setState(QGst::StateNull);
            }else if(option.contains("audio")){
                message.append(AUDIO_TOGGLE);
                message.append(START_AUDIO);
            }
        }else if(option == "help"){
            std::cout << help << std::endl;
            break;
        }else if(option == "exit"){
            message.append(EXIT);
            control->sendUDP(*rover,message,CONTROL_PORT);
            exit(1);
        }else{
            std::cout << "dont recognize that one" << std::endl;
            break;
        }

        control->sendUDP(*rover,message,CONTROL_PORT);
        LOG_I(LOG_TAG,"message sent");
        message.clear();
    }
}

void MPVLauncher::attemptConnection(){
    if(connected == false){
        std::cout << "." << std::endl;
        control->sendUDP(*rover,*initMessage,CONTROL_PORT);
        connectTimer->stop();
        connectTimer->start(3000);
    }
}

void MPVLauncher::onMessage(DataPacket packet){
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
                QString binStr = "udpsrc port=5555 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                frontPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                /*QString caps = "application/x-raw, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96";
                QString inBin = "udpsrc port=5555 ! rtph264depay ! decodebin ! videoconvert ! appsink name=\"MultiSink\"";// + caps;
                frontPipeline = QGst::Parse::launch(inBin).dynamicCast<QGst::Pipeline>();
                frontSink->setElement(frontPipeline->getElementByName("MultiSink"));
                QString displayBin = "appsrc name=\"MultiSink\"" + caps + "autovideosink";
                displayPipeline = QGst::Parse::launch(displayBin).dynamicCast<QGst::Pipeline>();
                frontPipeline->setState(QGst::StatePlaying);
                displayPipeline->setState(QGst::StatePlaying);*/
                frontPipeline->setState(QGst::StatePlaying);
            }else if(packet.message.contains(BACK)){
                QString binStr = "udpsrc port=5556 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                backPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                backPipeline->setState(QGst::StatePlaying);
            }else if(packet.message.contains(CLAW)){
                QString binStr = "udpsrc port=5556 caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                clawPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                clawPipeline->setState(QGst::StatePlaying);
            }
        }
    }else if(packet.message.startsWith(AUDIO_TOGGLE)){
        if(packet.message.contains(AUDIO_STARTED)){
            LOG_I(LOG_TAG,"starting audio");
            QString binStr = "udpsrc port=" + QString::number(AUDIO_PORT) + " caps=\"application/x-rtp,channels=(int)2,format=(string)S16LE,media=(string)audio,payload=(int)96,clock-rate=(int)44100,encoding-name=(string)L24\" ! rtpL24depay ! audioconvert ! autoaudiosink";
            audioPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
            audioPipeline->setState(QGst::StatePlaying);
        }

    }else if(packet.message.contains("CAMERA_IN_USE")){
        LOG_E(LOG_TAG,"someone is already using that camera");
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
