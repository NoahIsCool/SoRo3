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

#include "audiostreamer.h"
#include "soro_core/constants.h"
#include "soro_core/logger.h"

namespace Soro {

AudioStreamer::AudioStreamer(GStreamerUtil::AudioProfile profile, quint16 bindPort, SocketAddress address, quint16 ipcPort, QObject *parent)
        : MediaStreamer("AudioStreamer", parent) {
    if (!connectToParent(ipcPort)) return;

    LOG_I(LOG_TAG, "Creating pipeline");
    _pipeline = createPipeline();

    // create gstreamer command
    QString binStr = GStreamerUtil::createRtpAlsaEncodeString(bindPort, address.host, address.port, profile);

    LOG_I(LOG_TAG, "Creating gstreamer bin " + binStr);

    QGst::BinPtr encoder = QGst::Bin::fromDescription(binStr);

    LOG_I(LOG_TAG, "Created gstreamer bin " + binStr);

    _pipeline->add(encoder);

    LOG_I(LOG_TAG, "Elements linked on pipeline");

    // play
    _pipeline->setState(QGst::StatePlaying);

    LOG_I(LOG_TAG, "Stream started");

}

} // namespace Soro
