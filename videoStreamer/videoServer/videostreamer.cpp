#include "videostreamer.h"

using namespace Logger;
using namespace soro;

VideoStreamer::VideoStreamer(QObject *parent) : QObject(parent)
{

}

VideoStreamer::VideoStreamer(QString configFile){
    QGst::init();
    // I was trying to make something that is super general because thats kinda my thing. It would be great if we could reuse this code with very little fandangling.
    //Just check out soro2 and soro-research if you want to know what I mean. It took me an entire year to understand what was happening in that code.
    //Then it turns out, you cannot change shit. It is all dependent on everything else and if you delete something like the mbed code (used for the driving system)
    //Then the video streamer does work. Here, there is none of that. Here, you can change shit and not break unrelated shit. Youre welcome.
    /*
     * incase you are wondering, this is here so you can enter stuff into a file like this
     * (for now) clientAddress=192.168.1.183
     * frontCamera=video0
     * backCamera=video1
     * clawCamera=video2
     * topCamera=video3
     * and it will be parsed and return the device needed.
     */
    ConfigReader reader(configFile);
    if(reader.exists()){
        frontDevice = reader.find("frontCamera");
        backDevice = reader.find("backCamera");
        clawDevice = reader.find("clawCamera");
        topDevice = reader.find("topCamera");

        if(frontDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find front camera device. Unpredicted results will occure if you use this camera");
        }else{
            int ret = system(QString("ls /dev/" + frontDevice).toStdString().c_str());
            if(ret == 0){
                LOG_I(LOG_TAG,"Good news, everybody! I found the front camera!");
            }else{
                LOG_I(LOG_TAG,"Sweet File-not-found of Puget Sound! Cant find the front device. You should probably check to see if it is plugged in.");
            }
        }

        if(backDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find back camera device. Unpredicted results will occure if you use this camera");
        }else{
            int ret = system(QString("ls /dev/" + backDevice).toStdString().c_str());
            if(ret == 0){
                LOG_I(LOG_TAG,"Good news, everybody! I found the back camera!");
            }else{
                LOG_I(LOG_TAG,"Sweet File-not-found of Puget Sound! Cant find the back device. You should probably check to see if it is plugged in.");
            }
        }

        if(clawDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find claw camera device. Unpredicted results will occure if you use this camera");
        }else{
            int ret = system(QString("ls /dev/" + clawDevice).toStdString().c_str());
            if(ret == 0){
                LOG_I(LOG_TAG,"Good news, everybody! I found the claw camera!");
            }else{
                LOG_I(LOG_TAG,"Sweet File-not-found of Puget Sound! Cant find the claw device. You should probably check to see if it is plugged in.");
            }
        }

        if(topDevice == "NOT_FOUND"){
            LOG_I(LOG_TAG,"cannot find top camera device. Unpredicted results will occure if you use this camera");
        }else{
            int ret = system(QString("ls /dev/" + topDevice).toStdString().c_str());
            if(ret == 0){
                LOG_I(LOG_TAG,"Good news, everybody! I found the top camera!");
            }else{
                LOG_I(LOG_TAG,"Sweet File-not-found of Puget Sound! Cant find the top device. You should probably check to see if it is plugged in.");
            }
        }

    }else{
        QFile test("/dev/video0");
        if(test.exists()){
            frontDevice = "video0";
            topDevice = "video0";
        }
        LOG_W(LOG_TAG,"missig config file! Please put one at /home/soro/videoStreamer/conf/videoStreamer.conf");
    }

    frontPipeline = QGst::Pipeline::create();
    backPipeline = QGst::Pipeline::create();
    clawPipeline = QGst::Pipeline::create();
    topPipeline = QGst::Pipeline::create();
    primaryAudioPipeline = QGst::Pipeline::create();
    secondaryAudioPipeline = QGst::Pipeline::create();

    connected = false;
    heartbeat = new socket(HEARTBEAT_PORT,this);
    heartbeatTimeout = new QTimer(this);
    nope.push_back(CAMERA_NOT_FOUND);

    control = new socket(CONTROL_PORT,this);

    //something I found, it is super important to call the connect function last. I dont know how they build that shit but it only works with the variables on the stack before it.
    //so if you do something like this:
    /*
     * frontPipeline = QGst::Pipeline::create();
     * connect(heatbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout());
     * frontPipeline = QGst::Pipeline::create();
     *
     * shit will hit the fan. What are you laughing at? Im super cereal. (Joke from south park) QT only allows you to use connect with shit that was initialized before hand. If you
     * initialize something after the connect, you will have to call disconnect then connect again. Checkout onTimeout for an example.
     */
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
    std::vector<uint8_t> data;
    switch(packet.message[TYPE]){
        case INIT:
            data.push_back(INIT);
            connected = true;
            LOG_I(LOG_TAG,"connected");
            connect(heartbeat,SIGNAL(hasData(DataPacket)),this,SLOT(onHeartbeat(DataPacket)));
        break;
        case CAMERA_TOGGLE:
            switch(packet.message[COMMAND]){
                case START_CAMERA:
                    switch(packet.message[CAMERA]){
                        case FRONT:
                            if(frontPipeEmpty){
                                LOG_I(LOG_TAG,"starting front");
                                startCamera(FRONT,packet.sender);
                                data.push_back(CAMERA_TOGGLE);
                                data.push_back(CAMERA_STARTED);
                                data.push_back(FRONT);
                            }else{
                                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
                            }
                        break;
                        case BACK:
                            if(backPipeEmpty){
                                LOG_I(LOG_TAG,"starting back");
                                startCamera(BACK,packet.sender);
                                data.push_back(CAMERA_TOGGLE);
                                data.push_back(CAMERA_STARTED);
                                data.push_back(BACK);
                            }else{
                                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
                            }
                        break;
                        case CLAW:
                            if(clawPipeEmpty){
                                LOG_I(LOG_TAG,"starting claw");
                                startCamera(CLAW,packet.sender);
                                data.push_back(CAMERA_TOGGLE);
                                data.push_back(CAMERA_STARTED);
                                data.push_back(CLAW);
                            }else{
                                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
                            }
                        break;
                        case TOP:
                            if(topPipeEmpty){
                                LOG_I(LOG_TAG,"starting top");
                                startCamera(TOP,packet.sender);
                                data.push_back(CAMERA_TOGGLE);
                                data.push_back(CAMERA_STARTED);
                                data.push_back(TOP);
                            }else{
                                control->sendUDP(packet.sender,nope,CONTROL_CLIENT_PORT);
                            }
                        break;
                    };
                break;
                case STOP_CAMERA:
                    switch(packet.message[CAMERA]){
                        case FRONT:
                            stopCamera(FRONT);
                            data.push_back(CAMERA_TOGGLE);
                            data.push_back(CAMERA_STOPPED);
                            data.push_back(FRONT);
                        break;
                        case BACK:
                            stopCamera(BACK);
                            data.push_back(CAMERA_TOGGLE);
                            data.push_back(CAMERA_STOPPED);
                            data.push_back(BACK);
                        break;
                        case CLAW:
                            stopCamera(CLAW);
                            data.push_back(CAMERA_TOGGLE);
                            data.push_back(CAMERA_STOPPED);
                            data.push_back(CLAW);
                        break;
                        case TOP:
                            stopCamera(TOP);
                            data.push_back(CAMERA_TOGGLE);
                            data.push_back(CAMERA_STOPPED);
                            data.push_back(TOP);
                        break;
                    };
                break;
            };
        break;
        case AUDIO_TOGGLE:
            switch(packet.message[COMMAND]){
                case START_AUDIO:
                    LOG_I(LOG_TAG,"starting audio");
                    startAudio(packet.sender);
                    data.push_back(AUDIO_TOGGLE);
                    data.push_back(AUDIO_STARTED);
                break;
                case STOP_AUDIO:
                    stopAudio();
                break;
                default:
                    LOG_I(LOG_TAG,"dont know brah" + packet.message[COMMAND]);
                break;
            };
        break;
        case EXIT:
            //do nothing...let the timeout handle it...
        break;
    };
    LOG_I(LOG_TAG,"sending message to " + packet.sender.toString() + " at " + QString::number(CONTROL_CLIENT_PORT));
    control->sendUDP(packet.sender,data,CONTROL_CLIENT_PORT);
}

/*
 * You know that old saying: "if its stupid and works. Its not stupid? Well that quote is actually describing this hot mess...
 * Anyways, I orrigionally wrote this code so only one client could use the server at a time. Then I realized I was an idiot and
 * it needs to support any number of clients. So now, it just responds to whoever or whomever wants to send it a message.
 * Probably not the best way because now we dont know if a single client is having problems but you know, its not stupid because it works.
 * NO ITS NOT, I KNOW WHAT YOU ARE THINKING!
 */
void VideoStreamer::onHeartbeat(DataPacket packet){
    std::vector<uint8_t> data;
    data.push_back(ACK);
    heartbeat->sendUDP(packet.sender,data,packet.port);
    if(connected){
        heartbeatTimeout->stop();
    }else{
        connected = true;
        heartbeatAddress = packet.sender;
    }
    heartbeatTimeout->start(1000);
}

/*
 * The timeout is called when no heartbeat messages are resceved within a second. I wanted this so that the rover wouldnt be streaming video or audio
 * when there was no one there to recieve it. Thats a pretty big waist (waste? Whatever, I'm an engineer, not a spellerist, fuck you) of both energy and bandwith.
 */
void VideoStreamer::onTimeout(){
    LOG_W(LOG_TAG,"timed out");
    connected = false;
    shutdownAllCameras();
    heartbeatTimeout->stop();
}

/*
 * Pretty simple here, but I thought I should mention that the disconnect and connect functions are there because I was having a dickons of a time
 * getting this to shut things down without crashing the program. So everytime we create a pipeline with the '=' we need to disconnect then connect.
 * Not my fault, qt is just a bitch.
 */
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
    topPipeEmpty = true;
    frontPipeline = QGst::Pipeline::create();
    backPipeline = QGst::Pipeline::create();
    clawPipeline = QGst::Pipeline::create();
    topPipeline = QGst::Pipeline::create();
    disconnect(heartbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout()));
    disconnect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
    connect(heartbeatTimeout,SIGNAL(timeout()),this,SLOT(onTimeout()));
    connect(control,SIGNAL(hasData(DataPacket)),this,SLOT(onMessage(DataPacket)));
}

void VideoStreamer::stopCamera(soro::CAMERA_TYPE camera){
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
    }else if(camera == TOP){
        topPipeline->setState(QGst::StatePaused);
        topPipeline->setState(QGst::StateNull);
        topPipeEmpty = true;
    }
}

/*
 * feel free to look up this shit they call gstreamer but Im telling you this works. Change it up, use it differently but these descriptions work.
 * If you really want to look into it, try to find the gstreamer bin descriptions. Maybe you will find one that is a little faster than the ones I have.
 * ffmpeg also has one that might be worth looking into.
 * Anyways, how this works is it generates a pipeline with the binstr. Then you have to add it to the pipeline (make sure to initialize the pipeline before adding the source)
 * then connect the pipelines bus to a method of your own, like my onBusMessage, to get errors and such. To be honest, its total shit. I cant figure out how to get anything out of their message pointer
 * and theres absolutly no documentation about it. I feel like gstreamer was just a big fuck you...
 * Anyways, then you add a signal watch and when you are ready to start streaming it, set the state to playing. I dont think it matters too much on if you start playing on the client side or the server side
 * but I think a good practice would be to start the streaming on the server side first. But what do I know? I'm not the idiot who built this shit show, I'm just the one who's guessing game is strong enough
 * to throw crap at a wall and somehow make the mona lisa.
 *
 * By the way, my guessing game is pretty strong. Thats how I passed most of my classes.
 */
void VideoStreamer::startCamera(soro::CAMERA_TYPE camera,QHostAddress client){
    QString clientAddress = client.toString();
    switch(camera){
        case FRONT:
            if(frontDevice != "NOT_FOUND"){
                    LOG_I(LOG_TAG,"found Front Device!");
                    //QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(FRONT_CAMERA_PORT);
                    QString binStr = "v4l2src device=/dev/" + frontDevice + " ! videoscale method=0 ! videorate drop-only=true ! videoconvert ! video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! x264enc speed-preset=ultrafast tune=zerolatency bitrate=2048 ! rtph264pay config-interval=3 pt=96 ! udpsink host=" + clientAddress + " port=" + QString::number(FRONT_CAMERA_PORT);
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
        break;
        case BACK:
            if(backDevice != "NOT_FOUND"){
                if(backPipeEmpty){
                    LOG_I(LOG_TAG,"found Back Device!");
                    //QString binStr = "v4l2src device=/dev/" + frontDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(BACK_CAMERA_PORT);
                    QString binStr = "v4l2src device=/dev/" + backDevice + " ! videoscale method=0 ! videorate drop-only=true ! videoconvert ! video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! x264enc speed-preset=ultrafast tune=zerolatency bitrate=2048 ! rtph264pay config-interval=3 pt=96 ! udpsink host=" + clientAddress + " port=" + QString::number(BACK_CAMERA_PORT);
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
        break;
        case CLAW:
            if(clawDevice != "NOT_FOUND"){
                if(clawPipeEmpty){
                    LOG_I(LOG_TAG,"found Claw Device!");
                    //QString binStr = "v4l2src device=/dev/" + clawDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
                    QString binStr = "v4l2src device=/dev/" + clawDevice + " ! videoscale method=0 ! videorate drop-only=true ! videoconvert ! video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! x264enc speed-preset=ultrafast tune=zerolatency bitrate=2048 ! rtph264pay config-interval=3 pt=96 ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
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
        break;
        case TOP:
            if(topDevice != "NOT_FOUND"){
                if(topPipeEmpty){
                    LOG_I(LOG_TAG,"found Top Device!");
                    //QString binStr = "v4l2src device=/dev/" + topDevice + " ! video/x-raw,framerate=20/1 ! videoscale ! videoconvert ! x264enc threads=4 tune=zerolatency bitrate=5000 speed-preset=superfast ! rtph264pay ! udpsink host=" + clientAddress + " port=" + QString::number(CLAW_CAMERA_PORT);
                    QString binStr = "v4l2src device=/dev/" + topDevice + " ! videoscale method=0 ! videorate drop-only=true ! videoconvert ! video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! x264enc speed-preset=ultrafast tune=zerolatency bitrate=2048 ! rtph264pay config-interval=3 pt=96 ! udpsink host=" + clientAddress + " port=" + QString::number(TOP_CAMERA_PORT);
                    //clawPipeline = QGst::Parse::launch(binStr).dynamicCast<QGst::Pipeline>();
                    QGst::BinPtr source = QGst::Bin::fromDescription(binStr);
                    topPipeline->add(source);
                    QGlib::connect(topPipeline->bus(), "message::error", this, &VideoStreamer::onBusMessage);
                    LOG_I(LOG_TAG,"\n\n");
                    LOG_I(LOG_TAG, "Created gstreamer bin " + binStr + "\n\n");
                    topPipeline->bus()->addSignalWatch();
                    QString bin2Str = "gst-launch-1.0 -v udpsrc port=" + QString::number(TOP_CAMERA_PORT) + " caps = \"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96\" ! rtph264depay ! decodebin ! videoconvert ! autovideosink";
                    LOG_I(LOG_TAG,"Gstream code to use: " + bin2Str + "\n\n");
                    topPipeEmpty = false;
                    topPipeline->setState(QGst::StatePlaying);
                }else{
                    control->sendUDP(client,nope,CONTROL_CLIENT_PORT);
                }
            }
        break;
    };
}

void VideoStreamer::startAudio(QHostAddress client){
    if(primaryAudioPipelineEmpty){
        QString binStr = "autoaudiosrc ! mulawenc ! rtppcmupay ! udpsink host=" + client.toString() + "  port=" + QString::number(AUDIO_PORT);
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
