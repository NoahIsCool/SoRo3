/*
 * Copyright 2016 The University of Oklahoma.
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

#include "audioserver.h"
#include "soro_core/logger.h"

namespace Soro {

AudioServer::AudioServer(int mediaId, quint16 bindPort, QObject *parent)
    : MediaServer("AudioServer " + QString::number(mediaId), mediaId, QCoreApplication::applicationDirPath() + "/audio_streamer" , bindPort, parent) {
}

void AudioServer::onStreamStoppedInternal() {
    if (!_starting) {
        _profile.codec = GStreamerUtil::CODEC_NULL;
    }
}

void AudioServer::start(GStreamerUtil::AudioProfile profile) {
    LOG_I(LOG_TAG, "start()");
    _profile = profile;

    // prevent onStreamStoppedInternal from resetting the stream parameters
    // in the event a running stream must be stopped
    _starting = true;
    initStream();
    _starting = false;
}

void AudioServer::constructChildArguments(QStringList& outArgs, quint16 bindPort, SocketAddress address, quint16 ipcPort) {
    outArgs << _profile.toString();
    outArgs << QHostAddress(address.host.toIPv4Address()).toString();
    outArgs << QString::number(address.port);
    outArgs << QString::number(bindPort);
    outArgs << QString::number(ipcPort);

    QString binString = GStreamerUtil::createRtpAlsaEncodeString(bindPort, address.host, address.port, _profile);
    LOG_I(LOG_TAG, "Child is about to start using gstreamer bin string " + binString);
}

void AudioServer::constructStreamingMessage(QDataStream& stream) {
    stream << _profile.toString();
}

GStreamerUtil::AudioProfile AudioServer::getAudioProfile() const {
    return _profile;
}

} // namespace Soro
