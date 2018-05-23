#include "videostreamer.h"

using namespace Logger;

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
    }else{
        QFile test("/dev/video0");
        if(test.exists()){
            frontDevice = "video0";
        }
    }

    connected = false;
    heartbeat = new socket(HEARTBEAT_PORT,this);
    timer = new QTimer(this);
    connect(heartbeat,SIGNAL(hasData(DataPacket)),this,SLOT(onHeartbeat(DataPacket)));

    //dont really know who will write to us so we just set it up to recieve
    control = new socket(CONTROL_PORT,this);
    connect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));

    //eventually want to load this from the config file but for now, just enter it
    //this loads the default profile. Unless you have a damn good reason, dont use a customized version
    profile = new GStreamerUtil::VideoProfile();
    profile->codec = GStreamerUtil::VIDEO_CODEC_H264;
}

void VideoStreamer::onMessage(DataPacket packet){
    LOG_I(LOG_TAG,"recieved message");
    QByteArray data;
    if(packet.message == "front"){
        startCamera(FRONT,packet.sender);
        data.append("ack");
    }else if(packet.message == "back"){
        startCamera(BACK,packet.sender);
        data.append("ack");
    }else if(packet.message == "claw"){
        startCamera(CLAW,packet.sender);
        data.append("ack");
    }else{
        data.append("nack");
    }
    control->sendUDP(packet.sender,data,packet.port);
}

void VideoStreamer::onHeartbeat(DataPacket packet){
    LOG_W(LOG_TAG,"got heartbeat");
    QByteArray data;
    data.append("ack");
    heartbeat->sendUDP(packet.sender,data,packet.port);
    if(connected){
        timer->stop();
    }else{
        connected = true;
        heartbeatAddress = packet.sender;
    }
    timer->start(1000);
}

void VideoStreamer::onTimeout(){
    LOG_W(LOG_TAG,"timedout");
    QByteArray data;
    data.append("timeout");
    heartbeat->sendUDP(heartbeatAddress,data,HEARTBEAT_CLIENT_PORT);
    connected = false;
    shutdownAllCameras();
    timer->stop();
}

void VideoStreamer::shutdownAllCameras(){
    frontPipeline->setState(QGst::StatePaused);
    frontPipeline->setState(QGst::StateNull);
    backPipeline->setState(QGst::StatePaused);
    backPipeline->setState(QGst::StateNull);
    clawPipeline->setState(QGst::StatePaused);
    clawPipeline->setState(QGst::StateNull);
}

void VideoStreamer::startCamera(CAMERA camera,QHostAddress client){
    QString clientAddress = client.toString();
    switch(camera){
    case FRONT:
        if(frontDevice != "NOT_FOUND"){
            LOG_I(LOG_TAG,"found Front Device!");
            QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(FRONT_CAMERA_PORT);
            //binStr += "! textoverlay text=\"Hello\" ! ffmpegcolorspace !";
            frontPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
            QGlib::connect(frontPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
            LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
            frontPipeline->bus()->addSignalWatch();
            QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(FRONT_CAMERA_PORT) +" caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
            LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
            frontPipeline->setState(QGst::StatePlaying);
        }
        break;
    case BACK:
        if(backDevice != "NOT_FOUND"){
            LOG_I(LOG_TAG,"found Back Device!");
            QString binStr = "v4l2src device=/dev/" + backDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(BACK_CAMERA_PORT);
            backPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
            QGlib::connect(backPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
            LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
            backPipeline->bus()->addSignalWatch();
            QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(BACK_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
            LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
            backPipeline->setState(QGst::StatePlaying);
        }
        break;
    case CLAW:
        if(clawDevice != "NOT_FOUND"){
            LOG_I(LOG_TAG,"found Claw Device!");
            QString binStr = "v4l2src device=/dev/" + clawDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
            clawPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
            QGlib::connect(clawPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
            LOG_I(LOG_TAG,"\n\n");
            LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
            clawPipeline->bus()->addSignalWatch();
            QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(CLAW_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
            LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
            clawPipeline->setState(QGst::StatePlaying);
        }
        break;
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
