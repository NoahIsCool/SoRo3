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

#include "videoclient.h"

namespace Soro {

void VideoClient::onServerStreamingMessageInternal(QDataStream& stream)
{
    QString profileString;
    bool isStereo;
    stream >> profileString;
    stream >> isStereo;
    _profile = GStreamerUtil::VideoProfile(profileString);
    _stereo = isStereo;
}

void VideoClient::onServerStartMessageInternal()
{
    _profile = GStreamerUtil::VideoProfile();
    _stereo = false;
}

void VideoClient::onServerEosMessageInternal()
{
    _profile = GStreamerUtil::VideoProfile();
    _stereo = false;
}

void VideoClient::onServerErrorMessageInternal()
{
    _profile = GStreamerUtil::VideoProfile();
    _stereo = false;
}

void VideoClient::onServerDisconnectedInternal()
{
    _profile = GStreamerUtil::VideoProfile();
    _stereo = false;
}

VideoClient::VideoClient(int mediaId, SocketAddress server, QHostAddress host, QObject *parent)
    : MediaClient("VideoClient " + QString::number(mediaId), mediaId, server, host, parent) { }

GStreamerUtil::VideoProfile VideoClient::getVideoProfile() const
{
    return _profile;
}

bool VideoClient::getIsStereo() const
{
    return _stereo;
}

void VideoClient::onServerConnectedInternal() { }

} // namespace Soro
