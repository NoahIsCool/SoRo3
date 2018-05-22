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
     * frontCamera=video0
     * backCamera=video1
     * clawCamera=video2
     * and it will be parsed and return the device needed.
     */
    ConfigReader reader(configFile);
    QString frontDevice = "NOT_FOUND";
    QString backDevice = "NOT_FOUND";
    QString clawDevice = "NOT_FOUND";
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
    //eventually want to load this from the config file but for now, just enter it
    //this loads the default profile. Unless you have a damn good reason, dont use a customized version
    profile = new GStreamerUtil::VideoProfile();
    profile->codec = GStreamerUtil::VIDEO_CODEC_H264;

    if(frontDevice != "NOT_FOUND"){
        LOG_I(LOG_TAG,"found Front Device!");
        QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=192.168.1.183 port=" + QString::number(FRONT_CAMERA_PORT);
        //binStr += "! textoverlay text=\"Hello\" ! ffmpegcolorspace !";
        frontPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
        QGlib::connect(frontPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
        LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
        frontPipeline->bus()->addSignalWatch();
        QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(FRONT_CAMERA_PORT) +" caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
        LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str);
        frontPipeline->setState(QGst::StatePlaying);
    }

    if(backDevice != "NOT_FOUND"){
        LOG_I(LOG_TAG,"found Back Device!");
        QString binStr = "v4l2src device=/dev/" + backDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=192.168.1.183 port=" + QString::number(BACK_CAMERA_PORT);
        backPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
        QGlib::connect(backPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
        LOG_I(LOG_TAG, "Created gstreamer bin " + binStr);
        backPipeline->bus()->addSignalWatch();
        QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(BACK_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
        LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str);
        backPipeline->setState(QGst::StatePlaying);
    }

    if(clawDevice != "NOT_FOUND"){
        LOG_I(LOG_TAG,"found Claw Device!");
        QString binStr = "v4l2src device=/dev/" + clawDevice + " ! video/x-raw,framerate=30/1 ! videoscale ! videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=192.168.1.13 port=" + QString::number(CLAW_CAMERA_PORT);
        clawPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
        QGlib::connect(clawPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
        LOG_I(LOG_TAG,"\n\n");
        LOG_I(LOG_TAG, "Created gstreamer bin " + binStr);
        clawPipeline->bus()->addSignalWatch();
        QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(CLAW_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
        LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str);
        clawPipeline->setState(QGst::StatePlaying);
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
