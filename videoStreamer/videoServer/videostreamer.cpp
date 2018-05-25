#include "videostreamer.h"

using namespace Logger;
using namespace soro;

VideoStreamer::VideoStreamer(QObject *parent) : QObject(parent)
{

}

VideoStreamer::VideoStreamer(QString configFile){
    QGst::init();
    // create gstreamer command
    /*
     * incase you are wondering, this is here so you can enter stuff into a file like this
     * (for now) clientAddress=192.168.1.183
     * frontCamera=video0
     * backCamera=video1
     * clawCamera=video2
     * and it will be parsed and return the device needed.
     */
    ConfigReader reader(configFile);
    if(reader.exists()){
        frontDevice = reader.find("frontCamera");
        backDevice = reader.find("backCamera");
        clawDevice = reader.find("clawCamera");

        if(frontDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find front camera device. Unpredicted results will occure if you use this camera");
        }

        if(backDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find back camera device. Unpredicted results will occure if you use this camera");
        }

        if(clawDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find claw camera device. Unpredicted results will occure if you use this camera");
        }
    }else{
        QFile test("/dev/video0");
        if(test.exists()){
            frontDevice = "video0";
            backDevice = "video0";
        }
        LOG_W(LOG_TAG,"missig config file! Please put one at /home/soro/videoStreamer/conf/videoStreamer.conf");
    }

    frontPipeline = QGst::Pipeline::create();
    backPipeline = QGst::Pipeline::create();
    clawPipeline = QGst::Pipeline::create();
    primaryAudioPipeline = QGst::Pipeline::create();
    secondaryAudioPipeline = QGst::Pipeline::create();

    connected = false;
    heartbeat = new socket(HEARTBEAT_PORT,this);
    heartbeatTimeout = new QTimer(this);
    nope.append("CAMERA_IN_USE");

    control = new socket(CONTROL_PORT,this);

    //FIXME: segfault because the pipelines are created after the connect functions... need to figure out how to still use them
    connect(heartbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout()));
    connect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
    LOG_I(LOG_TAG,"setup sockets");
}

/*
 * control protocol:
 * client will send INIT
 * server will respond with INIT
 *
 * client will send MESSAGE_TYPE CAMERA_STATUS CAMERA_TYPE
 * server will start or stop the specified camera depending on the type
 */
void VideoStreamer::onMessage(DataPacket packet){
    QByteArray data;
    if(packet.message.startsWith(INIT)){
        data.append(INIT);
        connected = true;
        LOG_I(LOG_TAG,"connected");
        connect(heartbeat,SIGNAL(hasData(DataPacket)),this,SLOT(onHeartbeat(DataPacket)));
    }else if(packet.message.contains(START_CAMERA)){
        if(packet.message.contains(FRONT)){
            if(frontPipeEmpty){
                LOG_I(LOG_TAG,"starting front");
                startCamera(FRONT,packet.sender);
                data.append(CAMERA_TOGGLE);
                data.append(CAMERA_STARTED);
                data.append(FRONT);
            }else{
                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
            }
        }else if(packet.message.contains(BACK)){
            if(backPipeEmpty){
                LOG_I(LOG_TAG,"starting back");
                startCamera(BACK,packet.sender);
                data.append(CAMERA_TOGGLE);
                data.append(CAMERA_STARTED);
                data.append(BACK);
            }else{
                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
            }
        }else if(packet.message.contains(CLAW)){
            if(clawPipeEmpty){
                LOG_I(LOG_TAG,"starting claw");
                startCamera(CLAW,packet.sender);
                data.append(CAMERA_TOGGLE);
                data.append(CAMERA_STARTED);
                data.append(CLAW);
            }else{
                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
            }
        }
    }else if(packet.message.contains(STOP_CAMERA)){
        if(packet.message.contains(FRONT)){
            stopCamera(FRONT);
            data.append(CAMERA_TOGGLE);
            data.append(CAMERA_STOPPED);
            data.append(FRONT);
        }else if(packet.message.contains(BACK)){
            stopCamera(BACK);
            data.append(CAMERA_TOGGLE);
            data.append(CAMERA_STOPPED);
            data.append(BACK);
        }else if(packet.message.contains(CLAW)){
            stopCamera(CLAW);
            data.append(CAMERA_TOGGLE);
            data.append(CAMERA_STOPPED);
            data.append(CLAW);
        }
    }else if(packet.message.contains(AUDIO_TOGGLE)){
        if(packet.message.contains(START_AUDIO)){
            LOG_I(LOG_TAG,"starting audio");
            startAudio(packet.sender);
            data.append(AUDIO_TOGGLE);
            data.append(AUDIO_STARTED);
        }else if(packet.message.contains(STOP_AUDIO)){
            stopAudio();
        }else{
            LOG_I(LOG_TAG,"dont know brah" + packet.message);
        }
    }else if(packet.message.contains(EXIT)){
        //do nothing...let the timeout handle it...
    }
    LOG_I(LOG_TAG,"sending message to " + packet.sender.toString() + " at " + QString::number(CONTROL_CLIENT_PORT));
    control->sendUDP(packet.sender,data,CONTROL_CLIENT_PORT);
}

void VideoStreamer::onHeartbeat(DataPacket packet){
    QByteArray data;
    data.append("ack");
    heartbeat->sendUDP(packet.sender,data,packet.port);
    if(connected){
        heartbeatTimeout->stop();
    }else{
        connected = true;
        heartbeatAddress = packet.sender;
    }
    heartbeatTimeout->start(1000);
}

void VideoStreamer::onTimeout(){
    LOG_W(LOG_TAG,"timed out");
    connected = false;
    shutdownAllCameras();
    heartbeatTimeout->stop();
}

void VideoStreamer::shutdownAllCameras(){
    stopCamera(FRONT);
    stopCamera(BACK);
    stopCamera(CLAW);

    //not a camera but should stop the audio streams as well
    //calling it twice ensures that both are shutdown
    stopAudio();
    stopAudio();

    //shutdown is only called when we disconnect.
    //we only have to do this when we disconnect.
    frontPipeEmpty = true;
    backPipeEmpty = true;
    clawPipeEmpty = true;
    frontPipeline = QGst::Pipeline::create();
    backPipeline = QGst::Pipeline::create();
    clawPipeline = QGst::Pipeline::create();
    disconnect(heartbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout()));
    disconnect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
    connect(heartbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout()));
    connect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
}

void VideoStreamer::stopCamera(QString camera){
    if(camera == FRONT){
        frontPipeline->setState(QGst::StatePaused);
        frontPipeline->setState(QGst::StateNull);
        frontPipeEmpty = true;
    }else if(camera == BACK){
        backPipeline->setState(QGst::StatePaused);
        backPipeline->setState(QGst::StateNull);
        backPipeEmpty = true;
    }else if(camera == CLAW){
        clawPipeline->setState(QGst::StatePaused);
        clawPipeline->setState(QGst::StateNull);
        clawPipeEmpty = true;
    }
}

void VideoStreamer::startCamera(QString camera,QHostAddress client){
    QString clientAddress = client.toString();
    if(camera == FRONT){
        if(frontDevice != "NOT_FOUND"){
                LOG_I(LOG_TAG,"found Front Device!");
                //QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(FRONT_CAMERA_PORT);
                QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=20/1 ! x264enc threads=8 bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(FRONT_CAMERA_PORT);
                //frontPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
                frontPipeline->add(source);
                QGlib::connect(frontPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
                LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
                frontPipeline->bus()->addSignalWatch();
                QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(FRONT_CAMERA_PORT) +" caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
                frontPipeEmpty = false;
                frontPipeline->setState(QGst::StatePlaying);
        }
    }else if(camera == BACK){
        if(backDevice != "NOT_FOUND"){
            if(backPipeEmpty){
                LOG_I(LOG_TAG,"found Back Device!");
                //QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(BACK_CAMERA_PORT);
                QString binStr = "v4l2src device=/dev/" + backDevice + " ! video/x-raw,framerate=20/1 ! x264enc threads=8 bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(BACK_CAMERA_PORT);
                //backPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
                backPipeline->add(source);
                QGlib::connect(backPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
                LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
                backPipeline->bus()->addSignalWatch();
                QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(BACK_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
                backPipeEmpty = false;
                backPipeline->setState(QGst::StatePlaying);
            }else{
                control->sendUDP(client,nope,CONTROL_CLIENT_PORT);
            }
        }
    }else if(camera == CLAW){
        if(clawDevice != "NOT_FOUND"){
            if(clawPipeEmpty){
                LOG_I(LOG_TAG,"found Claw Device!");
                //QString binStr = "v4l2src device=/dev/" + clawDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
                QString binStr = "v4l2src device=/dev/" + clawDevice + " ! video/x-raw,framerate=20/1 ! x264enc threads=8 bitrate=500 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
                //clawPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
                clawPipeline->add(source);
                QGlib::connect(clawPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
                LOG_I(LOG_TAG,"\n\n");
                LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
                clawPipeline->bus()->addSignalWatch();
                QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(CLAW_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
                clawPipeEmpty = false;
                clawPipeline->setState(QGst::StatePlaying);
            }else{
                control->sendUDP(client,nope,CONTROL_CLIENT_PORT);
            }
        }
    }
}

void VideoStreamer::startAudio(QHostAddress client){
    if(primaryAudioPipelineEmpty){
        QString binStr = "autoaudiosrc ! audioconvert ! rtpL24pay ! udpsink host=" + client.toString() + " auto-multicast=true port=" + QString::number(AUDIO_PORT);
        QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
        primaryAudioPipeline->add(source);
        QGlib::connect(primaryAudioPipeline->bus(),"message::error",this,&VideoStreamer::onBusMessage);
        LOG_I(LOG_TAG,"\n\n");
        LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
        primaryAudioPipeline->bus()->addSignalWatch();
        QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(AUDIO_PORT) + " caps=\"application/x-rtp,channels=(int)2,format=(string)S16LE,media=(string)audio,payload=(int)96,clock-rate=(int)44100,encoding-name=(string)L24\" ! rtpL24depay ! audioconvert ! autoaudiosink";
        LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
        //primaryAudioPipelineEmpty = false;
        primaryAudioPipeline->setState(QGst::StatePlaying);
        primaryAudioPipelineEmpty = false;
    }else if(secondaryAudioPipelineEmpty){
        QString binStr = "autoaudiosrc ! audioconvert ! rtpL24pay ! udpsink host=" + client.toString() + " auto-multicast=true port=" + QString::number(AUDIO_PORT);
        QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
        secondaryAudioPipeline->add(source);
        QGlib::connect(secondaryAudioPipeline->bus(),"message::error",this,&VideoStreamer::onBusMessage);
        LOG_I(LOG_TAG,"\n\n");
        LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
        secondaryAudioPipeline->bus()->addSignalWatch();
        QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(AUDIO_PORT) + " caps=\"application/x-rtp,channels=(int)2,format=(string)S16LE,media=(string)audio,payload=(int)96,clock-rate=(int)44100,encoding-name=(string)L24\" ! rtpL24depay ! audioconvert ! autoaudiosink";
        LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
        //primaryAudioPipelineEmpty = false;
        secondaryAudioPipeline->setState(QGst::StatePlaying);
        secondaryAudioPipelineEmpty = false;
    }else{
        LOG_I(LOG_TAG,"well shit brah, no more audio pipelines");
    }

}

void VideoStreamer::stopAudio(){
    if(!secondaryAudioPipelineEmpty){
        secondaryAudioPipeline->setState(QGst::StatePaused);
        secondaryAudioPipeline->setState(QGst::StateNull);
        secondaryAudioPipelineEmpty = true;
    }else if(!primaryAudioPipelineEmpty){
        primaryAudioPipeline->setState(QGst::StatePaused);
        primaryAudioPipeline->setState(QGst::StateNull);
        primaryAudioPipelineEmpty = true;
    }
}

void VideoStreamer::onBusMessage(const QGst::MessagePtr &message){
    switch (message->type()) {
    case QGst::MessageEos:
        LOG_I(LOG_TAG,"EOS reached");
        break;
    case QGst::MessageError:
        LOG_I(LOG_TAG,"some sort of error");
        break;
    default:
        break;
    }
}
