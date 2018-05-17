/*
 * Copyright 2017 The University of Oklahoma
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

#include "gstreamerutil.h"

namespace Soro {
namespace GStreamerUtil {

VideoProfile::VideoProfile()
{
    codec = CODEC_NULL;
    width = 640;
    height = 480;
    framerate = 30;
    bitrate = 2048000;
    grayscale = false;
    mjpeg_quality = 50;
}

VideoProfile::VideoProfile(QString description)
{
    QStringList items = description.split(',');
    if ((items[0] == "VP") && (items.size() == 8))
    {
        codec = items[1].toUInt();
        width = items[2].toUInt();
        height = items[3].toUInt();
        framerate = items[4].toUInt();
        bitrate = items[5].toUInt();
        mjpeg_quality = items[6].toUInt();
        grayscale = items[7] == "g";
    }
    else
    {
        codec = CODEC_NULL;
        width = 640;
        height = 480;
        framerate = 30;
        bitrate = 2048000;
        mjpeg_quality = 50;
        grayscale = false;
    }
}

AudioProfile::AudioProfile(QString description)
{
    QStringList items = description.split(',');
    if ((items[0] == "AP") && (items.size() == 3))
    {
        codec = items[1].toUInt();;
        bitrate = items[2].toUInt();
    }
    else
    {
        codec = CODEC_NULL;
        bitrate = 32000;
    }
}

QString VideoProfile::toString() const
{
    return QString("VP,%1,%2,%3,%4,%5,%6,%7")
            .arg(QString::number(codec),
                 QString::number(width),
                 QString::number(height),
                 QString::number(framerate),
                 QString::number(bitrate),
                 QString::number(mjpeg_quality),
                 grayscale ? "g" : "c");
}

bool VideoProfile::operator==(const VideoProfile& other) const
{
    return (codec == other.codec) &&
            (width == other.width) &&
            (height == other.height) &&
            (framerate == other.framerate) &&
            (bitrate == other.bitrate) &&
            (mjpeg_quality == other.mjpeg_quality);
}

AudioProfile::AudioProfile()
{
    codec = CODEC_NULL;
    bitrate = 32000;
}

QString AudioProfile::toString() const
{
    return QString("AP,%1,%2")
            .arg(QString::number(codec),
                 QString::number(bitrate));
}

bool AudioProfile::operator==(const AudioProfile& other) const
{
    return (codec == other.codec) &&
            (bitrate == other.bitrate);
}

QString createRtpAlsaEncodeString(quint16 bindPort,  QHostAddress address, quint16 port, AudioProfile profile)
{
    return "alsasrc ! audioconvert ! " + createRtpAudioEncodeString(bindPort, address, port, profile);
}

QString createRtpV4L2EncodeString(QString cameraDevice, quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi)
{
    return QString("v4l2src device=/dev/%1 ! "
                   "videoscale method=0 add-borders=true ! "
                   "videorate drop-only=true ! "
                   "%2")
            .arg(cameraDevice,
                 createRtpVideoEncodeString(bindPort, address, port, profile, vaapi));
}

QString createRtpStereoV4L2EncodeString(QString leftCameraDevice, QString rightCameraDevice, quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi)
{
    return QString("v4l2src device=/dev/%5 ! "
                   "videoscale method=0 add-borders=true ! "
                   "videorate drop-only=true ! "
                   "video/x-raw,width=%1,height=%2,framerate=%3/1 ! "
                   "videoscale method=0 add-borders=false ! "
                   "video/x-raw,width=%4,height=%2 ! "
                   "videobox left=-%4 ! "
                   "videomixer name=mix background=black ! "
                   "%7 "
                   "v4l2src device=/dev/%6 ! "
                   "videoscale method=0 add-borders=true ! "
                   "videorate drop-only=true ! "
                   "video/x-raw,width=%1,height=%2,framerate=%3/1 ! "
                   "videoscale method=0 add-borders=false ! "
                   "video/x-raw,width=%4,height=%2 ! "
                   "mix.").arg(
                QString::number(profile.width),
                QString::number(profile.height),
                QString::number(profile.framerate),
                QString::number(profile.width / 2),
                rightCameraDevice,
                leftCameraDevice,
                createRtpVideoEncodeString(bindPort, address, port, profile, vaapi));
}

QString createRtpVideoEncodeString(quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi)
{
    return QString("%1 ! "
                   "video/x-raw,format=I420,width=%2,height=%3,framerate=%4/1 ! "
                   "%5 ! "
                   "%6 ! "
                   "udpsink bind-port=%7 host=%8 port=%9")
            .arg(profile.grayscale ? "videoconvert ! video/x-raw,format=GRAY8 ! videoconvert"
                                   : "videoconvert",
                 QString::number(profile.width),
                 QString::number(profile.height),
                 QString::number(profile.framerate),
                 getVideoEncodeElement(profile, vaapi),
                 getRtpPayElement(profile.codec),
                 QString::number(bindPort),
                 address.toString(),
                 QString::number(port));
}

QString createRtpAudioEncodeString(quint16 bindPort, QHostAddress address, quint16 port, AudioProfile profile)
{
    return QString("%1 ! %2 ! udpsink bind-port=%3 host=%4 port=%5")
            .arg(getAudioEncodeElement(profile),
                 getRtpPayElement(profile.codec),
                 QString::number(bindPort),
                 address.toString(),
                 QString::number(port));
}

QString createRtpAudioPlayString(QHostAddress address, quint16 port, quint8 codec)
{
    return createRtpAudioDecodeString(address, port, codec) + " ! audioconvert ! autoaudiosink";
}

QString createRtpAudioDecodeString(QHostAddress address, quint16 port, quint8 codec)
{
    return createRtpDepayString(address, port, codec) + " ! " + getAudioDecodeElement(codec);
}

QString createRtpVideoDecodeString(QHostAddress address, quint16 port, quint8 codec)
{
    return createRtpDepayString(address, port, codec) + " ! " + getVideoDecodeElement(codec) + " ! videoconvert ! video/x-raw,format=RGB ! videoconvert";
}

QString createRtpVideoFileSaveString(QHostAddress address, quint16 port, quint8 codec, QString filePath, bool timeOverlay, bool encodeVaapi)
{
    QString bin = createRtpDepayString(address, port, codec)
            + " ! " + getVideoDecodeElement(codec)
            + " ! videoconvert ! videoscale method=0 add-borders=true ! videorate ! video/x-raw,format=I420,width=1920,height=1080,framerate=30/1 ! ";
    if (timeOverlay)
    {
        bin += "timeoverlay halignment=center valignment=top ! ";
    }

    VideoProfile encodeProfile;
    encodeProfile.codec = VIDEO_CODEC_H264;

    bin += getVideoEncodeElement(encodeProfile, encodeVaapi) + QString(" ! queue ! avimux ! filesink location=\"%1\"").arg(filePath);

    return bin;
}

QString createVideoTestSrcString(QString pattern, bool grayscale, quint16 width, quint16 height, quint16 framerate)
{
    return QString("videotestsrc pattern=%1 ! video/x-raw,format=%2,width=%3,height=%4,framerate=%5/1 ! videoconvert")
            .arg(pattern, grayscale ? "GRAY8" : "RGB",  QString::number(width), QString::number(height), QString::number(framerate));
}

QString createRtpDepayString(QHostAddress address, quint16 port, quint8 codec)
{
    return QString("udpsrc address=%1 port=%2 ! %3").arg(
                address.toString(),
                QString::number(port),
                getRtpDepayElement(codec));
}

QString getRtpPayElement(quint8 codec)
{
    switch (codec)
    {
    case VIDEO_CODEC_MPEG4:
        return "rtpmp4vpay config-interval=3 pt=96";
    case VIDEO_CODEC_H264:
        return "rtph264pay config-interval=3 pt=96";
    case VIDEO_CODEC_MJPEG:
        return "rtpjpegpay";
    case VIDEO_CODEC_VP8:
        return "rtpvp8pay pt=96";
    case VIDEO_CODEC_VP9:
        return "rtpvp9pay pt=96";
    case VIDEO_CODEC_H265:
        return "rtph265pay config-interval=3 pt=96";
    case AUDIO_CODEC_AC3:
        return "rtpac3pay";
    default:
        // unknown codec
        return "";
    }
}

QString getRtpDepayElement(quint8 codec)
{
    switch (codec)
    {
    case VIDEO_CODEC_MPEG4:
        return "application/x-rtp,media=video,encoding-name=MP4V-ES,clock-rate=90000,profile-level-id=1,payload=96 ! rtpmp4vdepay";
    case VIDEO_CODEC_H264:
        return "application/x-rtp,media=video,encoding-name=H264,clock-rate=90000,payload=96 ! rtph264depay";
    case VIDEO_CODEC_MJPEG:
        return "application/x-rtp,media=video,encoding-name=JPEG,payload=26 ! rtpjpegdepay";
    case VIDEO_CODEC_VP8:
        return "application/x-rtp,media=video,encoding-name=VP8,clock-rate=90000,payload=96 ! rtpvp8depay";
    case VIDEO_CODEC_VP9:
        return "application/x-rtp,media=video,encoding-name=VP9,clock-rate=90000,payload=96 ! rtpvp9depay";
    case VIDEO_CODEC_H265:
        return "application/x-rtp,media=video,encoding-name=H265,clock-rate=90000,payload=96 ! rtph265depay";
    case AUDIO_CODEC_AC3:
        return "application/x-rtp,media=audio,clock-rate=44100,encoding-name=AC3 ! rtpac3depay";
    default:
        // unknown codec
        return "";
    }
}

QString getAudioEncodeElement(AudioProfile profile)
{
    switch (profile.codec)
    {
    case AUDIO_CODEC_AC3:
        return QString("avenc_ac3 bitrate=%1")
                .arg(profile.bitrate);
    default:
        // unknown codec
        return "";
    }
}

QString getAudioDecodeElement(quint8 codec)
{
    switch (codec)
    {
    case AUDIO_CODEC_AC3:
        return "a52dec";
    default:
        // unknown codec
        return "";
    }
}

QString getVideoEncodeElement(VideoProfile profile, bool vaapi)
{
    if (vaapi)
    {
        switch (profile.codec)
        {
        case VIDEO_CODEC_MPEG4:
        case VIDEO_CODEC_VP9:
            // No VAAPI encoder for these formats
            return getVideoEncodeElement(profile, false);
        case VIDEO_CODEC_H264:
            return QString("vaapih264enc bitrate=%1")
                    .arg(QString::number(profile.bitrate / 1000)); // Bitrate wanted in kbit/sec
        case VIDEO_CODEC_MJPEG:
            return QString("vaapijpegenc bitrate=%1 quality=%2")
                    .arg(QString::number(profile.bitrate / 1000), // Bitrate wanted in kbit/sec
                         QString::number(profile.mjpeg_quality));
        case VIDEO_CODEC_VP8:
            return QString("vaapivp8enc bitrate=%1")
                    .arg(QString::number(profile.bitrate / 1000)); // Bitrate wanted in kbit/sec
        case VIDEO_CODEC_H265:
            return QString("vaapih265enc bitrate=%1")
                    .arg(QString::number(profile.bitrate / 1000)); // Bitrate wanted in kbit/sec
        default:
            // unknown codec
            return "";
        }
    }
    else
    {
        switch (profile.codec)
        {
        case VIDEO_CODEC_MPEG4:
            return QString("avenc_mpeg4 bitrate=%1")
                    .arg(QString::number(profile.bitrate));
        case VIDEO_CODEC_H264:
            return QString("x264enc speed-preset=ultrafast tune=zerolatency bitrate=%1")
                    .arg(QString::number(profile.bitrate / 1000)); // Bitrate wanted in kbit/sec
        case VIDEO_CODEC_MJPEG:
            return QString("jpegenc quality=%1")
                    .arg(QString::number(profile.mjpeg_quality));
        case VIDEO_CODEC_VP8:
            return QString("vp8enc target-bitrate=%1")
                    .arg(QString::number(profile.bitrate));
        case VIDEO_CODEC_VP9:
            return QString("vp9enc target-bitrate=%1")
                    .arg(QString::number(profile.bitrate));
        case VIDEO_CODEC_H265:
            return QString("x265enc speed-preset=ultrafast tune=zerolatency bitrate=%1")
                    .arg(QString::number(profile.bitrate / 1000)); // Bitrate wanted in kbit/sec
        default:
            // unknown codec
            return "";
        }
    }
}

QString getVideoDecodeElement(quint8 codec)
{
    switch (codec)
    {
    case VIDEO_CODEC_MPEG4:
        return "avdec_mpeg4";
    case VIDEO_CODEC_H264:
        return "avdec_h264";
    case VIDEO_CODEC_MJPEG:
        return "jpegdec";
    case VIDEO_CODEC_VP8:
        return "avdec_vp8";
    case VIDEO_CODEC_VP9:
        return "avdec_vp9";
    case VIDEO_CODEC_H265:
        return "avdec_h265";
    default:
        // unknown codec
        return "";
    }
}

QString getCodecName(quint8 codec)
{
    switch (codec)
    {
    case VIDEO_CODEC_MPEG4:
        return "MPEG4";
    case VIDEO_CODEC_H264:
        return "H264";
    case VIDEO_CODEC_MJPEG:
        return "MJPEG";
    case VIDEO_CODEC_VP8:
        return "VP8";
    case VIDEO_CODEC_VP9:
        return "VP9";
    case VIDEO_CODEC_H265:
        return "H265";
    case AUDIO_CODEC_AC3:
        return "AC3";
    default:
        // unknown codec
        return "INVALID";
    }
}

} // namespace GStreamerUtil
} // namespace Soro
