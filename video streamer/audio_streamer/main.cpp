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

#include "soro_core/socketaddress.h"
#include "soro_core/gstreamerutil.h"
#include "soro_core/constants.h"
#include "soro_core/logger.h"

#include "audiostreamer.h"

#define LOG_TAG "Main"

using namespace Soro;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Logger::rootLogger()->setLogfile(QCoreApplication::applicationDirPath()
                                     + "/../log/Audio_" + QDateTime::currentDateTime().toString("M-dd_h.mm.ss_AP") + ".log");
    Logger::rootLogger()->setMaxFileLevel(Logger::LogLevelDebug);
    Logger::rootLogger()->setMaxStdoutLevel(Logger::LogLevelDisabled);

    LOG_I(LOG_TAG, "Starting...");
    QGst::init();

    if (argc < 6) {
        LOG_E(LOG_TAG, "Not enough arguments (expected 6, got " + QString::number(argc) + ")");
        return STREAMPROCESS_ERR_NOT_ENOUGH_ARGUMENTS;
    }

    bool ok;
    GStreamerUtil::AudioProfile profile;
    QString device;
    SocketAddress address;
    quint16 bindPort;
    quint16 ipcPort;

    /*
     * Parse audio profile
     */
    QString profileString = QString(argv[1]);
    LOG_I(LOG_TAG, "Profile: " + profileString);
    profile = GStreamerUtil::AudioProfile(profileString);

    /*
     * Parse address
     */
    address.host = QHostAddress(argv[2]);
    address.port = QString(argv[3]).toInt(&ok);
    if ((address.host == QHostAddress::Null) | (address.host == QHostAddress::Any) | !ok) {
        // invalid address
        LOG_E(LOG_TAG, "Invalid address '" + QString(argv[2]) + ":" + QString(argv[3]) + "'");
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "Address: " + address.toString());

    /*
     * Parse bind port
     */
    bindPort = QString(argv[4]).toInt(&ok);
    if (!ok) {
        // invalid host
        LOG_E(LOG_TAG, "Invalid bind port '" + QString(argv[4]) + "'");
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "Bind port: " + QString::number(bindPort));

    /*
     * Parse IPC port
     */
    ipcPort = QString(argv[5]).toInt(&ok);
    if (!ok) {
        // invalid IPC port
        LOG_E(LOG_TAG, "Invalid IPC port '" + QString(argv[5]) + "'");
        return STREAMPROCESS_ERR_INVALID_ARGUMENT;
    }
    LOG_I(LOG_TAG, "IPC Port: " + QString::number(ipcPort));

    a.setApplicationName("AudioStream for " + device + " to " + address.toString());

    LOG_I(LOG_TAG, "Creating stream object");
    AudioStreamer stream(profile, bindPort, address, ipcPort, &a);
    LOG_I(LOG_TAG, "Stream object created");
    return a.exec();
}
