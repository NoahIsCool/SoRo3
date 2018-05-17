#ifndef SORO_MISSIONCONTROL_SETTINGSMODEL_H
#define SORO_MISSIONCONTROL_SETTINGSMODEL_H

#include <QtCore>
#include <QHash>
#include <QHostAddress>

#include "soro_core/constants.h"
#include "soro_core/gstreamerutil.h"

namespace Soro {

struct SettingsModel {

    SettingsModel();

    bool enableVideo;
    bool enableStereoVideo;
    bool enableAudio;
    bool enableHud;
    bool enableGps;

    int selectedVideoEncoding;
    int selectedVideoWidth;
    int selectedVideoHeight;
    bool selectedVideoGrayscale;
    int selectedVideoFramerate;
    int selectedVideoBitrate;
    int selectedMjpegQuality;
    int selectedCamera;
    int selectedLatency;
    int selectedHudParallax;
    int selectedHudLatency;

    int frontCameraIndex;
    int backCameraIndex;
    int clawCameraIndex;
    int allCamerasIndex;

    QHostAddress roverAddress;
    bool useHwRendering;
    QHash<quint8, bool> useVaapiEncodeForCodec;

    QStringList cameraNames;
    QStringList videoEncodingNames;
    GStreamerUtil::AudioProfile defaultAudioFormat;

    GStreamerUtil::VideoProfile getSelectedVideoProfile();
};

} // namespace Soro

#endif // SORO_MISSIONCONTROL_SETTINGSMODEL_H
