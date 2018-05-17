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

#include "videoserver.h"
#include "soro_core/logger.h"

namespace Soro {

VideoServer::VideoServer(int mediaId, quint16 bindPort, QObject *parent)
    : MediaServer("VideoServer " + QString::number(mediaId), mediaId, QCoreApplication::applicationDirPath() + "/video_streamer" , bindPort, parent) {
}

void VideoServer::onStreamStoppedInternal() {
    if (!_starting) {
        _videoDevice = "";
        _profile.codec = GStreamerUtil::CODEC_NULL;
    }
}

void VideoServer::start(QString deviceName, GStreamerUtil::VideoProfile profile, bool vaapi) {
    LOG_I(LOG_TAG, "start(): Streaming " + deviceName);
    _videoDevice = deviceName;
    _profile = profile;
    _vaapi = vaapi;
    _stereo = false;

    // prevent onStreamStoppedInternal from resetting the stream parameters
    // in the event a running stream must be stopped
    _starting = true;
    initStream();
    _starting = false;
}

void VideoServer::start(QString leftDeviceName, QString rightDeviceName, GStreamerUtil::VideoProfile profile, bool vaapi) {
    _videoDevice = leftDeviceName + "," + rightDeviceName;
    LOG_I(LOG_TAG, "start(): Streaming " + _videoDevice + " with profile " + profile.toString());
    _profile = profile;
    _vaapi = vaapi;
    _stereo = true;

    // prevent onStreamStoppedInternal from resetting the stream parameters
    // in the event a running stream must be stopped
    _starting = true;
    initStream();
    _starting = false;
}

void VideoServer::constructChildArguments(QStringList& outArgs, quint16 bindPort, SocketAddress address, quint16 ipcPort) {
    outArgs << _videoDevice;
    outArgs << (_stereo ? "1" : "0");
    outArgs << _profile.toString();
    outArgs << (_vaapi ? "1" : "0");
    outArgs << QHostAddress(address.host.toIPv4Address()).toString();
    outArgs << QString::number(address.port);
    outArgs << QString::number(bindPort);
    outArgs << QString::number(ipcPort);

    QString binString;
    if (_stereo)
    {
        binString = GStreamerUtil::createRtpStereoV4L2EncodeString(_videoDevice.mid(0, _videoDevice.indexOf(",")),
                                                             _videoDevice.mid(_videoDevice.indexOf(",") + 1),
                                                             bindPort,
                                                             address.host,
                                                             address.port,
                                                             _profile,
                                                             _vaapi);
    }
    else
    {
        binString = GStreamerUtil::createRtpV4L2EncodeString(_videoDevice,
                                                             bindPort,
                                                             address.host,
                                                             address.port,
                                                             _profile,
                                                             _vaapi);
    }
    LOG_I(LOG_TAG, "Child is about to start using gstreamer bin string " + binString);
}

void VideoServer::constructStreamingMessage(QDataStream& stream) {
    stream << _profile.toString();
    stream << _stereo;
}

GStreamerUtil::VideoProfile VideoServer::getVideoProfile() const {
    return _profile;
}

} // namespace Soro
