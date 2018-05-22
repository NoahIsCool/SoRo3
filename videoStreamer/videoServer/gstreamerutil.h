#ifndef GSTREAMERUTIL_H
#define GSTREAMERUTIL_H

#include <QString>
#include <QHostAddress>

/* This namespace has some useful functions for constructing GStreamer pipeline descriptions, as well as
 * other GStreamer-related functionality
 */
namespace GStreamerUtil {

const quint8 VIDEO_CODEC_H264 = 0;
const quint8 VIDEO_CODEC_MPEG4 = 1;
const quint8 VIDEO_CODEC_VP8 = 2;
const quint8 VIDEO_CODEC_VP9 = 3;
const quint8 VIDEO_CODEC_H265 = 4;
const quint8 VIDEO_CODEC_MJPEG = 5;

const quint8 AUDIO_CODEC_AC3 = 100;

const quint8 CODEC_NULL = 255;

struct VideoProfile
{
    quint8 codec;
    quint16 width;
    quint16 height;
    quint16 framerate;
    quint32 bitrate;
    quint8 mjpeg_quality;
    bool grayscale;

    VideoProfile()
    {
        codec = VIDEO_CODEC_H265;
        width = 640;
        height = 480;
        framerate = 30;
        bitrate = 2048000;
        grayscale = false;
        mjpeg_quality = 50;
    }

    QString toString() const
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

    bool operator==(const VideoProfile& other) const
    {
        return (codec == other.codec) &&
                (width == other.width) &&
                (height == other.height) &&
                (framerate == other.framerate) &&
                (bitrate == other.bitrate) &&
                (mjpeg_quality == other.mjpeg_quality);
    }

    inline bool operator!=(const VideoProfile& other) const
    {
        return !(*this == other);
    }
};

struct AudioProfile
{
    quint8 codec;
    quint32 bitrate;

    AudioProfile()
    {
        codec = CODEC_NULL;
        bitrate = 32000;
    }

    QString toString() const
    {
        return QString("AP,%1,%2")
                .arg(QString::number(codec),
                     QString::number(bitrate));
    }

    bool operator==(const AudioProfile& other) const
    {
        return (codec == other.codec) &&
                (bitrate == other.bitrate);
    }

    inline bool operator!=(const AudioProfile& other) const
    {
        return !(*this == other);
    }
};

/* Creates a pipeline string that encodes ALSA audio into a RTP stream
 */
QString createRtpAlsaEncodeString(quint16 bindPort, QHostAddress address, quint16 port, AudioProfile profile);

/* Creates a pipeline string that encodes video from a camera into a RTP stream
 */
QString createRtpV4L2EncodeString(QString cameraDevice, quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi=false);

QString createRtpStereoV4L2EncodeString(QString leftCameraDevice, QString rightCameraDevice, quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi=false);

/* Creates a pipeline string that encodes raw video into a RTP stream
 */
QString createRtpVideoEncodeString(quint16 bindPort, QHostAddress address, quint16 port, VideoProfile profile, bool vaapi=false);

/* Creates a pipeline string that encodes raw audio into a RTP stream
 */
QString createRtpAudioEncodeString(quint16 bindPort, QHostAddress address, quint16 port, AudioProfile profile);

/* Creates a pipeline string that accepts an RTP video stream on a UDP port, and decodes it from the specified codec to a raw video stream
 */
QString createRtpVideoDecodeString(QHostAddress address, quint16 port, quint8 codec);

/* Creates a pipeline string that accepts an RTP video stream on a UDP port, and decodes it from the specified codec and
 * re-encodes it as an H264 video file at the specifed location. If desired, a timestamp and/or custom text can be
 * overlaid on the video.
 */
QString createRtpVideoFileSaveString(QHostAddress address, quint16 port, quint8 codec, QString filePath, bool timeOverlay, bool encodeVaapi=false);

/* Creates a pipeline string that outputs a video test pattern
 */
QString createVideoTestSrcString(QString pattern="snow", bool grayscale=false, quint16 width=640, quint16 height=480, quint16 framerate=30);

/* Creates a pipeline string that accepts an RTP audio stream on a UDP port, and decodes it from the specified codec to a raw audio stream
 */
QString createRtpAudioDecodeString(QHostAddress address, quint16 port, quint8 codec);

/* Creates a pipeline string that accepts an RTP audio stream on a UDP port, decodes it from the specified codec into a raw audio stream,
 * and plays it using autoaudiosink
 */
QString createRtpAudioPlayString(QHostAddress address, quint16 port, quint8 codec);

/* Creates a pipeline string that accepts an RTP stream on a UDP port, and depayloads it to an encoded video stream
 */
QString createRtpDepayString(QHostAddress address, quint16 port, quint8 codec);

/* Gets the element name and associated caps to RTP depayload a stream in the specified audio or video codec
 */
QString getRtpDepayElement(quint8 codec);

/* Gets the element name and associated caps to RTP payload a stream in the specified audio or video codec
 */
QString getRtpPayElement(quint8 codec);

/* Gets the element name and associated options to decode the specified video profile
 */
QString getVideoDecodeElement(quint8 codec);

/* Gets the element name and associated options to encode the specified video profile
 */
QString getVideoEncodeElement(VideoProfile profile, bool vaapi=false);

/* Gets the element name and associated options to decode the specified audio codec
 */
QString getAudioDecodeElement(quint8 codec);

/* Gets the element name and associated options to decode the specified audio codec
 */
QString getAudioEncodeElement(AudioProfile profile);

/* Gets the human-readable name of a codec
 */
QString getCodecName(quint8 codec);

} // namespace GStreamerUtil

#endif // GSTREAMERUTIL_H
