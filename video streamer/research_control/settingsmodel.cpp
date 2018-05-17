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

#include "settingsmodel.h"

namespace Soro {

SettingsModel::SettingsModel()
{
    roverAddress = roverAddress;
    enableHud = true;
    enableVideo = false;
    enableStereoVideo = false;
    enableAudio = false;
    enableGps = true;
    selectedVideoEncoding = 0;
    selectedVideoWidth = 640;
    selectedVideoHeight = 480;
    selectedVideoBitrate = 500;
    selectedVideoGrayscale = false;
    selectedVideoFramerate = 30;
    selectedMjpegQuality = 50;
    selectedCamera = 0;
    selectedLatency = 0;
    selectedHudParallax = 0;
    selectedHudLatency = 100;

    defaultAudioFormat.codec = GStreamerUtil::AUDIO_CODEC_AC3;
    defaultAudioFormat.bitrate = 32000;

    cameraNames << "Front Camera";      frontCameraIndex = 0;
    cameraNames << "Back Camera";       backCameraIndex = 1;
    cameraNames << "Claw Camera";       clawCameraIndex = 2;
    cameraNames << "All cameras";       allCamerasIndex = 3;

    videoEncodingNames << "H264";
    videoEncodingNames << "MPEG4";
    videoEncodingNames << "VP8";
    videoEncodingNames << "VP9";
    videoEncodingNames << "H265";
    videoEncodingNames << "MJPEG";

    useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_H264, false);
    useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_MPEG4, false);
    useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_H265, false);
    useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_MJPEG, false);
    useVaapiEncodeForCodec.insert(GStreamerUtil::VIDEO_CODEC_VP8, false);
}

GStreamerUtil::VideoProfile SettingsModel::getSelectedVideoProfile()
{
    GStreamerUtil::VideoProfile profile;
    profile.codec = selectedVideoEncoding;
    profile.bitrate = selectedVideoBitrate * 1000;
    profile.framerate = selectedVideoFramerate;
    profile.mjpeg_quality = selectedMjpegQuality;
    profile.width = selectedVideoWidth;
    profile.grayscale = selectedVideoGrayscale;
    profile.height = selectedVideoHeight;
    return profile;
}

} // namespace Soro
