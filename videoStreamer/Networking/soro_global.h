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

/*const QString FRONT   = "FRONT";
const QString BACK    = "BACK";
const QString CLAW    = "CLAW";*/


/*const QString INIT            = "INIT";
const QString CAMERA_TOGGLE   = "CAMERA_TOGGLE";
const QString HEARTBEAT       = "HEARTBEAT";
const QString AUDIO_TOGGLE    = "AUDIO_TOGGLE";
const QString EXIT            = "EXIT";*/

/*
 * these are the different types of messages that can be sent to and from the rover.
 * I would like to add one for error messages but I cant figure out how to use the damn message pointer...
 *
 */
enum MESSAGE_TYPE : uint8_t{
    INIT = 0,
    CAMERA_TOGGLE,
    HEARTBEAT,
    AUDIO_TOGGLE,
    CAMERA_NOT_FOUND,
    EXIT,
};

/*
 * Ya, I know, I should have divided these up by what type of message they belong to but now-a-days, shutup.
 * Just as a clarification, the ping, ack and nack messages are for the heartbeat socket. Check out the heartbeat description
 * its kinda interesting and not my best work of code.
 */
enum COMMAND : uint8_t {
    START_CAMERA = 0,
    STOP_CAMERA,
    CAMERA_STARTED,
    CAMERA_STOPPED,
    START_AUDIO,
    STOP_AUDIO,
    AUDIO_STARTED,
    AUDIO_STOPPED,
    PING,
    ACK,
    NACK,
    TIMEOUT,
};

/*
 * Kinda ambiguous, I know, but these are the locations of the message types in the QByteArray.
 * I just thought it would be easier if they are always in the same spot.
 */
enum MESSAGE_LOCATION : uint8_t {
    TYPE = 0,
    COMMAND = 1,
    CAMERA = 2,
};

enum CAMERA_TYPE : uint8_t {
    FRONT = 0,
    BACK,
    CLAW,
};

/*const QString START_CAMERA    = "START_CAMERA";
const QString STOP_CAMERA     = "STOP_CAMERA";
const QString CAMERA_STARTED  = "CAMERA_STARTED";
const QString CAMERA_STOPPED  = "CAMERA_STOPPED";
const QString START_AUDIO     = "START_AUDIO";
const QString STOP_AUDIO      = "STOP_AUDIO";
const QString AUDIO_STARTED   = "AUDIO_STARTED";
const QString AUDIO_STOPPED   = "AUDIO_STOPPED";*/

}

#endif // SORO_GLOBAL_H
