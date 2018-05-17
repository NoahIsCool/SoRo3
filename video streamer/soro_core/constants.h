/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SORO_CONSTANTS_H
#define SORO_CONSTANTS_H

#ifdef QT_CORE_LIB
#include <QCoreApplication>
#endif

/* Timer macros
 */
#define TIMER_INACTIVE -1
#define START_TIMER(X,Y) if (X == TIMER_INACTIVE) X = startTimer(Y)
#define KILL_TIMER(X) if (X != TIMER_INACTIVE) { killTimer(X); X = TIMER_INACTIVE; }

#define SORO_SETTINGS_DIR QCoreApplication::applicationDirPath() + "/../config"

#define SORO_DBUS_SERVICE_NAME_ROOT "edu.ou.soonerrover"
#define SORO_DBUS_VIDEO_PARENT_SERVICE_NAME "edu.ou.soonerrover.video_parent"
#define SORO_DBUS_VIDEO_CHILD_SERVICE_NAME(pid) "edu.ou.soonerrover.video_child_" + pid
#define SORO_DBUS_AUDIO_PARENT_SERVICE_NAME "edu.ou.soonerrover.audio_parent"
#define SORO_DBUS_AUDIO_CHILD_SERVICE_NAME(pid) "edu.ou.soonerrover.audio_child_" + pid

#define IPV4_REGEX "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
#define IPV6_REGEX "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))"

/* These define the channel names for communication between
 * the mission control and rover.
 */
#define CHANNEL_NAME_DRIVE              "Soro_DriveChannel"
#define CHANNEL_NAME_MAIN               "Soro_MainChannel"

/* These are return codes that audio/video streaming processes will return
 * depending on the error they encountered.
 */
#define STREAMPROCESS_ERR_NOT_ENOUGH_ARGUMENTS  91
#define STREAMPROCESS_ERR_INVALID_ARGUMENT      92
#define STREAMPROCESS_ERR_GSTREAMER_EOS         93
#define STREAMPROCESS_ERR_GSTREAMER_ERROR       94
#define STREAMPROCESS_ERR_UNKNOWN_CODEC         95
#define STREAMPROCESS_ERR_SOCKET_ERROR          96

/* These are little messages used for IPC between the main
 * rover processes and their streaming processes
 */
#define STREAMPROCESS_IPC_START     's'
#define STREAMPROCESS_IPC_STREAMING 'v'
#define STREAMPROCESS_IPC_EXIT      'e'

/* These define the network ports used for all network
 * communication. They were specifically chosen as unassigned
 * ports.
 */
#define NETWORK_ALL_DRIVE_CHANNEL_PORT  5501
#define NETWORK_ALL_MAIN_CHANNEL_PORT   5502
#define NETWORK_ALL_AUDIO_PORT          5503
#define NETWORK_ROVER_MBED_PORT         5504
#define NETWORK_ROVER_GPS_PORT          5505
//#define NETWORK_ALL_MAIN_CAMERA_PORT    5530
//#define NETWORK_ALL_AUX1_CAMERA_PORT    5531
#define NETWORK_ALL_FRONT_CAMERA_PORT   5530
#define NETWORK_ALL_BACK_CAMERA_PORT    5531
#define NETWORK_ALL_CLAW_CAMERA_PORT    5532
#define NETWORK_MC_CAMERA_REC_PORT      5540

#define MEDIAID_AUDIO           50
//#define MEDIAID_MAIN_CAMERA     0
//#define MEDIAID_AUX1_CAMERA     1
#define FRONT_CAMERA            0
#define BACK_CAMERA             1
#define CLAW_CAMERA             2

/* These are used to ID communication packets to/from each mbed. Each mbed
 * has it's own port as well, these ID's are just used for an extra layer
 * of protection against packets ending up in the wrong place.
 */
#define MBED_ID     13

#endif // SORO_CONSTANTS_H
