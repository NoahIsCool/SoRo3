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

#include "gstreamerrecorder.h"
#include "soro_core/logger.h"

#include <Qt5GStreamer/QGst/Bus>
#include <Qt5GStreamer/QGlib/Connect>

#define LOG_TAG "GStreamerRecorder" + _name

namespace Soro {

GStreamerRecorder::GStreamerRecorder(SocketAddress mediaAddress, QString name, QObject *parent) : QObject(parent)
{
    _name = name;
    _mediaAddress = mediaAddress;
}

bool GStreamerRecorder::begin(quint8 codec, QDateTime startTime, bool vaapiEncode)
{
    stop();

    QString filePath = QCoreApplication::applicationDirPath() + "/../research_media";

     if (!QDir(filePath).exists())
     {
        LOG_I(LOG_TAG, filePath + " directory does not exist, creating it");
        if (!QDir().mkpath(filePath))
        {
            LOG_E(LOG_TAG, "Cannot create " + filePath + " directory, video cannot be recorded");
            return false;
        }
    }

    filePath += "/" + startTime.toString("M-dd_h.mm.ss_AP") + ".avi";

    QString binStr = GStreamerUtil::createRtpVideoFileSaveString(_mediaAddress.host, _mediaAddress.port,
                                                                 codec,
                                                                 filePath,
                                                                 true,
                                                                 false);

    LOG_I(LOG_TAG, "Starting gstreamer recording with bin string " + binStr);
    _pipeline = QGst::Pipeline::create();
    _pipeline->bus()->addSignalWatch();
    QGlib::connect(_pipeline->bus(), "message", this, &GStreamerRecorder::onBusMessage);

    _bin = QGst::Bin::fromDescription(binStr);
    _pipeline->add(_bin);
    _pipeline->setState(QGst::StatePlaying);
}

void GStreamerRecorder::stop()
{
    if (!_pipeline.isNull())
    {
        LOG_I(LOG_TAG, "Stopping recording");
        _pipeline->bus()->removeSignalWatch();
        _pipeline->setState(QGst::StateNull);
        _pipeline.clear();
        _bin.clear();
    }
}

void GStreamerRecorder::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type())
    {
    case QGst::MessageEos:
        LOG_E(LOG_TAG, "onBusMessage(): Received end-of-stream message.");
        stop();
        break;
    case QGst::MessageError:
    {
        QString errorMessage = message.staticCast<QGst::ErrorMessage>()->error().message().toLatin1();
        LOG_E(LOG_TAG, "onBusMessage(): Received error message from gstreamer '" + errorMessage + "'");
        stop();
    }
    default:
        break;
    }
}

} // namespace Soro
