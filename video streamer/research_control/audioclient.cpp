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

#include "audioclient.h"

namespace Soro {

void AudioClient::onServerStreamingMessageInternal(QDataStream& stream) {
    QString profileString;
    stream >> profileString;
    _profile = GStreamerUtil::AudioProfile(profileString);
}

void AudioClient::onServerStartMessageInternal() {
    _profile = GStreamerUtil::AudioProfile();
}

void AudioClient::onServerEosMessageInternal() {
    _profile = GStreamerUtil::AudioProfile();
}

void AudioClient::onServerErrorMessageInternal() {
    _profile = GStreamerUtil::AudioProfile();
}

AudioClient::AudioClient(int mediaId, SocketAddress server, QHostAddress host, QObject *parent)
    : MediaClient("AudioClient " + QString::number(mediaId), mediaId, server, host, parent) {
}

GStreamerUtil::AudioProfile AudioClient::getAudioProfile() const {
    return _profile;
}

void AudioClient::onServerConnectedInternal() { }

void AudioClient::onServerDisconnectedInternal() {
    _profile = GStreamerUtil::AudioProfile();
}

} // namespace Soro
