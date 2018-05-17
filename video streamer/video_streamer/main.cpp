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

#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>

#include <Qt5GStreamer/QGst/Init>
#include <Qt5GStreamer/QGst/Element>
#include <Qt5GStreamer/QGst/ElementFactory>

#include "soro_core/socketaddress.h"
#include "soro_core/enums.h"
#include "soro_core/constants.h"
#include "soro_core/logger.h"

#include "videostreamer.h"

#define LOG_TAG "Main"

using namespace Soro;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Logger::rootLogger()->setLogfile(QCoreApplication::applicationDirPath()
                                     + "/../log/Video_" + QDateTime::currentDateTime().toString("M-dd_h.mm.ss_AP") + ".log");
    Logger::rootLogger()->setMaxFileLevel(Logger::LogLevelDebug);
    Logger::rootLogger()->setMaxStdoutLevel(Logger::LogLevelDisabled);

    LOG_I(LOG_TAG, "Starting...");
    QGst::init();

    if (argc < 9) {
        LOG_E(LOG_TAG, "Not enough arguments (expected 9, got " + QString::number(argc) + ")");
        return STREAMPROCESS_ERR_NOT_ENOUGH_ARGUMENTS;
    }

    bool ok;
    GStreamerUtil::VideoProfile profile;
    QString device;
    bool isStereo;
    bool vaapi;
    SocketAddress address;
    quint16 bindPort;
    quint16 ipcPort;

    /*
     * Parse device
     */
    device = argv[1];
    LOG_I(LOG_TAG, "Device: " + device);

    /*
     * Parse stereo
     */
    isStereo = atoi(argv[2]) == 1;

    /*
     * Parse profile
     */
    QString profileString = QString(argv[3]);
    LOG_I(LOG_TAG, "Profile: " + profileString);
    profile = GStreamerUtil::VideoProfile(profileString);

    /*
     * Parse VAAPI
     */
    vaapi = atoi(argv[4]) == 1;

    /*
     * Parse destination address
     */
    address.host = QHostAddress(argv[5]);
    address.port = QString(argv[6]).toInt(&ok);
    if ((address.host == QHostAddress::Null) | (address.host == QHostAddress::Any) | !ok) {
        LOG_E(LOG_TAG, "Invalid address '" + QString(argv[5]) + ":" + QString(argv[6]) + "'");
        // invalid address
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "Address: " + address.toString());

    /*
     * Parse bind port
     */
    bindPort = QString(argv[7]).toInt(&ok);
    if (!ok) {
        // invalid host
        LOG_E(LOG_TAG, "Invalid bind port '" + QString(argv[7]) + "'");
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "Bind port: " + QString::number(bindPort));

    /*
     * Parse IPC Port
     */
    ipcPort = QString(argv[8]).toInt(&ok);
    if (!ok) {
        // invalid IPC port
        LOG_E(LOG_TAG, "Invalid IPC port '" + QString(argv[8]) + "'");
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "IPC Port: " + QString::number(ipcPort));

    a.setApplicationName("VideoStream for " + device + " to " + address.toString());

    LOG_I(LOG_TAG, "Creating stream object");
    if (isStereo)
    {
        // For stereo streams, the device field is two devices split with a comma
        VideoStreamer stream(device.mid(0, device.indexOf(",")), device.mid(device.indexOf(",") + 1), profile, bindPort, address, ipcPort, vaapi, &a);
        LOG_I(LOG_TAG, "Stream object created");
        return a.exec();
    }
    else
    {
        VideoStreamer stream(device, profile, bindPort, address, ipcPort, vaapi, &a);
        LOG_I(LOG_TAG, "Stream object created");
        return a.exec();
    }
}
