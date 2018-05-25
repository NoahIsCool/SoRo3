#ifndef SORO_GLOBAL_H
#define SORO_GLOBAL_H

namespace soro{

const int FRONT_CAMERA_PORT     = 5555;
const int BACK_CAMERA_PORT      = 5556;
const int CLAW_CAMERA_PORT      = 5557;
const int CONTROL_PORT          = 6969;
const int HEARTBEAT_PORT        = 6970;
const int AUDIO_PORT            = 5558;

const int CONTROL_CLIENT_PORT   = 6971;
const int HEARTBEAT_CLIENT_PORT = 6972;

const QString FRONT   = "FRONT";
const QString BACK    = "BACK";
const QString CLAW    = "CLAW";


const QString INIT            = "INIT";
const QString FORWARD         = "FORWARD";
const QString CAMERA_TOGGLE   = "CAMERA_TOGGLE";
const QString HEARTBEAT       = "HEARTBEAT";
const QString AUDIO_TOGGLE    = "AUDIO_TOGGLE";
const QString EXIT            = "EXIT";

const QString START_CAMERA    = "START_CAMERA";
const QString STOP_CAMERA     = "STOP_CAMERA";
const QString CAMERA_STARTED  = "CAMERA_STARTED";
const QString CAMERA_STOPPED  = "CAMERA_STOPPED";
const QString START_AUDIO     = "START_AUDIO";
const QString STOP_AUDIO      = "STOP_AUDIO";
const QString AUDIO_STARTED   = "AUDIO_STARTED";
const QString AUDIO_STOPPED   = "AUDIO_STOPPED";

}

#endif // SORO_GLOBAL_H
