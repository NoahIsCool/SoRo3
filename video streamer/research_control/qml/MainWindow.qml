import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

import Soro 1.0

ApplicationWindow {
    id: mainWindow

    width: 640
    height: 480

    title: "Mission Control"

    property alias gstreamerSurface: gstreamerSurface

    property int latency: 0
    property real gamepadX: 0
    property real gamepadY: 0
    property int latencyTolerance: 0
    property real rearRoll: 500
    property real frontRoll: 500
    property real rearRollZero: 500
    property real frontRollZero: 500
    property real rearPitch: 575
    property real frontPitch: 400
    property real rearPitchZero: 575
    property real frontPitchZero: 400
    property real middlePitch: 400
    property real middlePitchZero: 400
    property real middleRoll: 500
    property real middleRollZero: 500
    property int wheelFLPower: 0
    property int wheelFRPower: 0
    property int wheelMLPower: 0
    property int wheelMRPower: 0
    property int wheelBLPower: 0
    property int wheelBRPower: 0
    property real compassHeading: 0
    property real compassHeadingZero: 0

    property bool stereo: false
    property bool hudVisible: true
    property int hudParallax: 0
    property string recordingState: "idle"

    signal closed()

    onClosing: {
        if (recordingState !== "idle") {
            close.accepted = false
        }
        else {
            closed()
        }
    }

    function zero() {
        frontPitchZero = frontPitch
        rearPitchZero = rearPitch
        frontRollZero = frontRoll
        rearRollZero = rearRoll
        compassHeadingZero = compassHeading
    }

    GStreamerSurface {
        id: gstreamerSurface
        antialiasing: false
        anchors.fill: parent
    }

    Item {
        id: overlayItem
        anchors.fill: parent

        HudCompass {
            id: hudCompass
            x: stereo ? overlayItem.width / 4 - width / 2 : overlayItem.width / 2 - width / 2
            y: 12
            compassHeading: mainWindow.compassHeading
            compassHeadingZero: mainWindow.compassHeadingZero
            width: parent.height * 0.12
            height: width
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudPower {
            id: hudPower
            blurSource: gstreamerSurface
            wheelBLPower: mainWindow.wheelBLPower
            wheelBRPower: mainWindow.wheelBRPower
            wheelMLPower: mainWindow.wheelMLPower
            wheelMRPower: mainWindow.wheelMRPower
            wheelFLPower: mainWindow.wheelFLPower
            wheelFRPower: mainWindow.wheelFRPower
            x: stereo ? hudParallax : 0
            y: 0
            width: height * 0.8
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudOrientationSide {
            id: hudOrientationSide
            blurSource: gstreamerSurface
            rearPitch: mainWindow.rearPitch
            rearPitchZero: mainWindow.rearPitchZero
            frontPitch: mainWindow.frontPitch
            frontPitchZero: mainWindow.frontPitchZero
            middlePitch: mainWindow.middlePitch
            middlePitchZero: mainWindow.middlePitchZero
            x: stereo ? hudParallax : 0
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudOrientationBack {
            id: hudOrientationBack
            blurSource: gstreamerSurface
            rearRoll: mainWindow.rearRoll
            rearRollZero: mainWindow.rearRollZero
            frontRoll: mainWindow.frontRoll
            frontRollZero: mainWindow.frontRollZero
            middleRoll: mainWindow.middleRoll
            middleRollZero: mainWindow.middleRollZero
            x: stereo ? overlayItem.width / 2 - width / 2 : overlayItem.width - width
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: stereo
            visible: hudVisible
            enabled: visible
        }

        HudLatency {
            id: hudLatency
            blurSource: gstreamerSurface
            latency: mainWindow.latency
            latencyTolerance: mainWindow.latencyTolerance
            xValue: mainWindow.gamepadX
            yValue: mainWindow.gamepadY
            x: stereo ? overlayItem.width / 2 - width / 2 : overlayItem.width - width
            y: 0
            halfWidth: stereo
            width: height
            height: parent.height * 0.6
            visible: hudVisible
            enabled: visible
        }

        HudCompass {
            id: hudCompass2
            x: overlayItem.width / 4 * 3 - width / 2
            y: 12
            compassHeading: mainWindow.compassHeading
            compassHeadingZero: mainWindow.compassHeadingZero
            width: parent.height * 0.12
            height: width
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudPower {
            id: hudPower2
            wheelBLPower: mainWindow.wheelBLPower
            wheelBRPower: mainWindow.wheelBRPower
            wheelMLPower: mainWindow.wheelMLPower
            wheelMRPower: mainWindow.wheelMRPower
            wheelFLPower: mainWindow.wheelFLPower
            wheelFRPower: mainWindow.wheelFRPower
            blurSource: gstreamerSurface
            x: overlayItem.width / 2
            y: 0
            width: height * 0.8
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudOrientationSide {
            id: hudOrientationSide2
            rearPitch: mainWindow.rearPitch
            rearPitchZero: mainWindow.rearPitchZero
            frontPitch: mainWindow.frontPitch
            frontPitchZero: mainWindow.frontPitchZero
            middlePitch: mainWindow.middlePitch
            middlePitchZero: mainWindow.middlePitchZero
            blurSource: gstreamerSurface
            x: overlayItem.width / 2
            y: overlayItem.height - height
            width: height
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudOrientationBack {
            id: hudOrientationBack2
            rearRoll: mainWindow.rearRoll
            rearRollZero: mainWindow.rearRollZero
            frontRoll: mainWindow.frontRoll
            frontRollZero: mainWindow.frontRollZero
            middleRoll: mainWindow.middleRoll
            middleRollZero: mainWindow.middleRollZero
            blurSource: gstreamerSurface
            x: overlayItem.width - width / 2 - hudParallax
            y: overlayItem.height - height;
            width: height
            height: parent.height * 0.3
            halfWidth: true
            visible: hudVisible && stereo
            enabled: visible
        }

        HudLatency {
            id: hudLatency2
            latency: mainWindow.latency
            latencyTolerance: mainWindow.latencyTolerance
            xValue: mainWindow.gamepadX
            yValue: mainWindow.gamepadY
            blurSource: gstreamerSurface
            x: overlayItem.width - width / 2 - hudParallax
            y: 0;
            halfWidth: true
            width: height
            height: parent.height * 0.6
            visible: hudVisible && stereo
            enabled: visible
        }
    }
}
