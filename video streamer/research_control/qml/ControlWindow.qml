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

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Controls.Universal 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtWebEngine 1.3
import QtQuick.Window 2.2

ApplicationWindow {
    id: controlWindow
    width: 1000
    height: 1200

    property alias gamepadField: gamepadField
    property alias roverAddressField: roverAddressField
    property alias notificationTitleLabel: notificationTitleLabel
    title: "Mission Control"

    // Alias properties
    property alias busyIndicator: busyIndicator
    property alias statusImage: statusImage
    property alias statusLabel: statusLabel
    property alias bitrateLabel: bitrateLabel
    property alias avGroupBox: avGroupBox
    property alias settingsPane: settingsPane
    property alias gpsGroupBox: gpsGroupBox
    property alias simulationGroupBox: simulationGroupBox
    property alias notificationImageColorOverlay: notificationImageColorOverlay
    property alias notificationPane: notificationPane
    property alias notificationImage: notificationImage
    property alias notificationLabel: notificationLabel
    property alias revertSettingsButton: revertSettingsButton
    property alias applySettingsButton: applySettingsButton
    property alias simLatencySpinBox: simLatencySpinBox
    property alias enableHudSwitch: enableHudSwitch
    property alias hudParallaxSpinBox: hudParallaxSpinBox
    property alias activeCameraCombo: activeCameraCombo
    property alias videoEncodingCombo: videoEncodingCombo
    property alias grayscaleVideoSwitch: grayscaleVideoSwitch
    property alias stereoVideoSwitch: stereoVideoSwitch
    property alias enableAudioSwitch: enableAudioSwitch
    property alias enableVideoSwitch: enableVideoSwitch
    property alias gpsClearButton: gpsClearButton
    property alias gpsHistoryField: gpsHistoryField
    property alias gpsLocationField: gpsLocationField
    property alias enableGpsSwitch: enableGpsSwitch
    property alias webEngineView: webEngineView
    property alias settingsFooterPane: settingsFooterPane
    property alias connectionState: connectionStateGroup.state
    property alias recordingState: recordingStateGroup.state
    property alias fullscreenState: fullscreenStateGroup.state
    property alias sidePaneVisibilityState: sidePaneVisibilityStateGroup.state
    property alias fullscreenToolbarButton: fullscreenToolbarButton
    property alias recordToolbarButton: recordToolbarButton
    property alias sidebarToolbarButton: sidebarToolbarButton
    property alias hudLatencySpinBox: hudLatencySpinBox
    property alias videoFramerateSpinBox: videoFramerateSpinBox
    property alias videoBitrateSpinBox: videoBitrateSpinBox
    property alias mjpegQualitySpinBox: mjpegQualitySpinBox

    // Settings properties

    property alias enableVideo: enableVideoSwitch.checked
    property alias enableStereoVideo: stereoVideoSwitch.checked
    property alias enableHud: enableHudSwitch.checked
    property alias selectedCamera: activeCameraCombo.currentIndex
    property alias selectedVideoEncoding: videoEncodingCombo.currentIndex
    property alias selectedVideoGrayscale: grayscaleVideoSwitch.checked
    property alias selectedVideoWidth: videoWidthSpinBox.value
    property alias selectedVideoHeight: videoHeightSpinBox.value
    property alias selectedVideoFramerate: videoFramerateSpinBox.value
    property alias selectedVideoBitrate: videoBitrateSpinBox.value
    property alias selectedMjpegQuality: mjpegQualitySpinBox.value
    property alias enableAudio: enableAudioSwitch.checked
    property alias selectedLatency: simLatencySpinBox.value
    property alias selectedHudParallax: hudParallaxSpinBox.value
    property alias enableGps: enableGpsSwitch.checked
    property alias selectedHudLatency: hudLatencySpinBox.value

    // Configuration properties

    property alias videoEncodingNames: videoEncodingCombo.model
    property alias cameraNames: activeCameraCombo.model
    property string roverAddress: "0.0.0.0"
    property string gamepad: "None"
    property string mbedStatus: "Unknown"

    // Internal properties

    property color theme_yellow: "#FBC02D"
    property color theme_red: "#d32f2f"
    property color theme_green: "#388E3C"
    property color theme_blue: "#1976D2"
    property color accentColor: "#616161"
    property int gpsDataPoints: 0

    // Signals

    signal requestUiSync()
    signal settingsApplied()
    signal logCommentEntered(string comment)
    signal recordButtonClicked()
    signal zeroOrientationButtonClicked()
    signal closed()

    // Public functions

    /*
      Should be called before the backend has started syncing
      the state of the UI to reflect the current settings
      */
    function prepareForUiSync() {
        settingsPane.enabled = false
        settingsFooterPane.visible = false
    }

    /*
      Should be called when the backend has finished syncing the
      state of the UI to reflect the current settings
      */
    function uiSyncComplete() {
        settingsFooterPane.state = "hidden"
        settingsPane.enabled = true
        settingsFooterPane.visible = true
    }

    /*
      Updates the ping (latency) information displayed
      */
    function updatePing(ping) {
        if (connectionState == "connected") {
            statusLabel.text = "Connected, " + ping + "ms"
        }
    }

    /*
      Updates the GPS location displayed on the map
      */
    function updateGpsLocation(lat, lng, heading) {
        webEngineView.runJavaScript("updateLocation(" + lat + ", " + lng + ", " + heading + ");")
        gpsLocationField.text = degToDms(lat, false) + ", " + degToDms(lng, true)
        gpsDataPoints++
    }

    /*
      Updates the bitrate status label
      */
    function updateBitrate(bpsUp, bpsDown) {
        if (connectionState == "connected") {
            var upUnits, downUnits;
            if (bpsUp > 1000000) {
                upUnits = "Mb/s"
                bpsUp = Math.round(bpsUp / 10000) / 100
            }
            else if (bpsUp > 1000) {
                upUnits = "Kb/s"
                bpsUp = Math.round(bpsUp / 10) / 100
            }
            else {
                upUnits = "b/s"
            }
            if (bpsDown > 1000000) {
                downUnits = "Mb/s"
                bpsDown = Math.round(bpsDown / 10000) / 100
            }
            else if (bpsDown > 1000) {
                downUnits = "Kb/s"
                bpsDown = Math.round(bpsDown / 10) / 100
            }
            else {
                downUnits = "b/s"
            }
            bitrateLabel.text =
                    "▲ <b>" + bpsUp + "</b> " + upUnits +
                    "<br>" +
                    "▼ <b>" + bpsDown + "</b> " + downUnits
        }
    }

    /*
      Displays a popup notification for a few seconds.
      Valid types are: 'error', 'warning', or 'information'
      */
    function notify(type, title, message) {
        notificationPane.state = "hidden"
        switch (type) {
        case "error":
            notificationImage.source = "qrc:/icons/ic_error_white_48px.svg"
            notificationImageColorOverlay.color = theme_red
            break
        case "warning":
            notificationImage.source = "qrc:/icons/ic_warning_white_48px.svg"
            notificationImageColorOverlay.color = theme_yellow
            break;
        case "information":
        default:
            notificationImage.source = "qrc:/icons/ic_info_white_48px.svg"
            notificationImageColorOverlay.color = theme_blue
            break;
        }
        notificationTitleLabel.text = title
        notificationLabel.text = message
        notificationPane.state = "visible"
        notificationTimer.restart()
    }

    /*
      Internal function to convert degress to degree/minute/seconds
      */
    function degToDms(D, lng){
        return "" + 0|(D<0?D=-D:D) + "° "
                + 0|D%1*60 + "' "
                + (0|D*60%1*6000)/100 + "\" "
                + D<0?lng?'W':'S':lng?'E':'N'
    }

    /*
      Reloads the GPS map to clear all location points currently displayed
      */
    function clearGps() {
        gpsDataPoints = 0
        webEngineView.runJavaScript("resetLocations()");
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    /*
      Theme settings
      */
    Material.theme: Material.Dark
    Material.accent: accentColor
    Universal.theme: Universal.Dark
    Universal.accent: accentColor

    onVisibleChanged: {
        if (visible) {
            // Request for the UI to be synced since it just became visible
            prepareForUiSync()
            requestUiSync()
        }
    }

    onGpsDataPointsChanged: {
        // Update the gps history field to reflect the current # of samples
        gpsHistoryField.text = "" + gpsDataPoints + " Samples"
    }

    onRoverAddressChanged: {
        // Update the rover address field
        roverAddressField.text = roverAddress
    }

    onGamepadChanged: {
        // Update the gamepad field
        if (gamepad == "") {
            gamepadField.text = "None"
        }
        else {
            gamepadField.text = gamepad
        }
    }

    onClosing: {
        if (recordingState !== "idle") {
            close.accepted = false
            onWindowCloseDialog.visible = true
        }
        else {
            closed()
        }
    }

    StateGroup {
        id: fullscreenStateGroup
        state: "normal"
        states: [
            State {
                name: "fullscreen"
                PropertyChanges {
                    target: fullscreenToolbarButton
                    tooltip.text: "Exit Fullscreen"
                    image.source: "qrc:/icons/ic_fullscreen_exit_white_48px.svg"
                }
                StateChangeScript {
                    script: controlWindow.showFullScreen()
                }
            },
            State {
                name: "normal"
                PropertyChanges {
                    target: fullscreenToolbarButton
                    tooltip.text: "Show Fullscreen"
                    image.source: "qrc:/icons/ic_fullscreen_white_48px.svg"
                }
                StateChangeScript {
                    script: controlWindow.showNormal()
                }
            }
        ]
    }


    /* State group to control the visibility of the options sidebar
      */
    StateGroup {
        id: sidePaneVisibilityStateGroup
        state: "visible"

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: asidePane
                    anchors.leftMargin: -asidePane.width
                }
                PropertyChanges {
                    target: sidebarToolbarButton
                    tooltip.text: "Show Sidebar"
                    image.source: "qrc:/icons/ic_keyboard_arrow_right_white_48px.svg"
                }
            },
            State {
                name: "visible"
                PropertyChanges {
                    target: asidePane
                    anchors.leftMargin: 0
                }
                PropertyChanges {
                    target: sidebarToolbarButton
                    tooltip.text: "Hide Sidebar"
                    image.source: "qrc:/icons/ic_keyboard_arrow_left_white_48px.svg"
                }
            }

        ]

        transitions: [
            Transition {
                from: "visible"
                to: "hidden"
                PropertyAnimation {
                    properties: "anchors.leftMargin"
                    duration: 100
                }
            },
            Transition {
                from: "hidden"
                to: "visible"
                PropertyAnimation {
                    properties: "anchors.leftMargin"
                    duration: 100
                }
            }
        ]
    }

    /*
      State group to update the UI to reflect a change in the rover's connection status
      Can be accessed from the backend with the connectionState property
      */
    StateGroup {
        id: connectionStateGroup
        state: "connecting"
        states: [
            State {
                name: "connecting"
                PropertyChanges {
                    target: controlWindow
                    accentColor: theme_yellow
                    busyIndicator.visible: true
                    statusImage.visible: false
                    bitrateLabel.visible: false
                    statusLabel.text: "Connecting..."
                    avGroupBox.enabled: false
                    simulationGroupBox.enabled: false
                    gpsGroupBox.enabled: false
                }
            },
            State {
                name: "connected"
                PropertyChanges {
                    target: controlWindow
                    accentColor: theme_green
                    busyIndicator.visible: false
                    statusImage.visible: true
                    bitrateLabel.visible: true
                    statusLabel.text: "Connected"
                    statusImage.source: "qrc:/icons/ic_check_circle_white_48px.svg"
                    avGroupBox.enabled: true
                    simulationGroupBox.enabled: true
                    gpsGroupBox.enabled: true
                }
            },
            State {
                name: "error"
                PropertyChanges {
                    target: controlWindow
                    accentColor: theme_red
                    busyIndicator.visible: false
                    statusImage.visible: true
                    bitrateLabel.visible: false
                    statusLabel.text: "Error"
                    statusImage.source: "qrc:/icons/ic_error_white_48px.svg"
                    settingsPane.enabled: false
                }
            }
        ]
        transitions: [
            Transition {
                ColorAnimation {
                    duration: 250
                }
            }
        ]
    }

    /*
      State group to show the recording status in the UI.
      */
    StateGroup {
        id: recordingStateGroup
        state: "idle"
        states: [
            State {
                name: "recording"
                PropertyChanges {
                    target: recordToolbarButton
                    state: "recording"
                }
                PropertyChanges {
                    target: recordingTimer
                    running: true
                    elapsed: 0
                }
                StateChangeScript {
                    script: clearGps()
                }
            },
            State {
                name: "idle"
                PropertyChanges {
                    target: recordToolbarButton
                    state: "idle"
                }
                PropertyChanges {
                    target: recordingTimer
                    running: false
                }
            },
            State {
                name: "waiting"
                PropertyChanges {
                    target: recordToolbarButton
                    state: "waiting"
                }
                PropertyChanges {
                    target: recordingTimer
                    running: false
                }
            }
        ]
    }

    onConnectionStateChanged: {
        if (recordingState === "recording") {
            // Append the status to the test comment log
            switch (connectionState) {
            case "connected":
                recordComment("[System Message] The rover is now connected", "system");
                break;
            case "error":
                recordComment("[System Message] The communication channel with the rover has experienced a fatal error", "system");
                break;
            case "connecting":
            default:
                recordComment("[System Message] Connection to the rover has been lost", "system");
                break;
            }
        }
    }

    /*
      Timer to dismiss notifications after a set amout of time
      */
    Timer {
        id: notificationTimer
        interval: 7000
        running: false
        repeat: false
        onTriggered: notificationPane.state = "hidden"
    }

    RecordingTimer {
        id: recordingTimer

        onTimeStringChanged: {
            recordToolbarButton.label.text = timeString
        }
    }

    /////////////////////////////////////////////////////////////////////////////////

    WebEngineView {
        id: webEngineView
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: asidePane.right
        anchors.bottom: parent.bottom
        url: "qrc:/html/map.html"
    }

    DropShadow {
        id: asideShadow
        anchors.fill: asidePane
        source: asidePane
        radius: 15
        samples: 20
        color: "#000000"
    }

    Pane {
        id: asidePane
        width: 450
        bottomPadding: 0
        rightPadding: 0
        leftPadding: 0
        topPadding: 0
        clip: true
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: headerPane.bottom
        anchors.topMargin: 0

        Flickable {
            id: settingsFlickable
            clip: false
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: settingsFooterPane.height
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 8
            contentHeight: settingsPane.height

            Pane {
                id: settingsPane
                width: parent.width
                height: gpsGroupBox.y + gpsGroupBox.height + topPadding + bottomPadding
                x: 0
                y: 0

                GroupBox {
                    id: infoGroupBox
                    width: 200
                    height: mbedStatusField.y + mbedStatusField.height + topPadding + bottomPadding
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    title: "Information"

                    Label {
                        id: roverAddressLabel
                        y: 6
                        width: 100
                        text: "Rover Address"
                        anchors.verticalCenter: roverAddressField.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Label {
                        id: roverAddressField
                        text: "Unknown"
                        anchors.left: roverAddressLabel.right
                        anchors.leftMargin: 12
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: 0
                    }

                    Label {
                        id: gamepadLabel
                        y: 55
                        width: 100
                        text: "Input Device"
                        anchors.verticalCenter: gamepadField.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Label {
                        id: gamepadField
                        text: "None"
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.left: gamepadLabel.right
                        anchors.leftMargin: 12
                        anchors.top: roverAddressField.bottom
                        anchors.topMargin: 8
                    }

                    Label {
                        id: mbedStatusLabel
                        x: 5
                        y: 60
                        width: 100
                        text: "Mbed Status"
                        anchors.verticalCenterOffset: 0
                        anchors.leftMargin: 0
                        anchors.verticalCenter: mbedStatusField.verticalCenter
                        anchors.left: parent.left
                    }

                    Label {
                        id: mbedStatusField
                        text: mbedStatus
                        anchors.leftMargin: 12
                        anchors.left: mbedStatusLabel.right
                        anchors.rightMargin: 0
                        anchors.top: gamepadField.bottom
                        anchors.topMargin: 8
                        anchors.right: parent.right
                    }
                }

                GroupBox {
                    id: interfaceGroupBox
                    x: -12
                    y: -12
                    height: hudOrientationZeroButton.y + hudOrientationZeroButton.height + topPadding + bottomPadding
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.top: infoGroupBox.bottom
                    anchors.topMargin: 8
                    title: "User Interface"

                    Label {
                        id: enableHudLabel
                        y: 17
                        width: 100
                        text: "Enable HUD"
                        anchors.verticalCenter: enableHudSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Switch {
                        id: enableHudSwitch
                        text: checked ? "HUD On" : "HUD Off"
                        anchors.left: enableHudLabel.right
                        anchors.leftMargin: 12
                        anchors.top: parent.top
                        anchors.topMargin: 8
                        onCheckedChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: hudParallaxLabel
                        y: 27
                        width: 100
                        text: "HUD Parallax"
                        anchors.verticalCenter: hudParallaxSpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: hudParallaxSpinBox
                        anchors.left: hudParallaxLabel.right
                        anchors.leftMargin: 12
                        anchors.top: enableHudSwitch.bottom
                        anchors.topMargin: 0
                        stepSize: 10
                        editable: true
                        from: 0
                        to: 500
                        onValueChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: hudLatencyLabel
                        width: 100
                        text: "HUD Latency (ms)"
                        anchors.verticalCenter: hudLatencySpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: hudLatencySpinBox
                        anchors.left: hudLatencyLabel.right
                        anchors.leftMargin: 12
                        anchors.top: hudParallaxSpinBox.bottom
                        anchors.topMargin: 0
                        stepSize: 1
                        editable: true
                        from: 0
                        to: 2000
                        onValueChanged: settingsFooterPane.state = "visible"
                    }

                    Button {
                        id: hudOrientationZeroButton
                        width: 200
                        text: "Zero Orientation"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: hudLatencySpinBox.bottom
                        anchors.topMargin: 8

                        onClicked: zeroOrientationButtonClicked()
                    }
                }

                GroupBox {
                    id: avGroupBox
                    x: -12
                    y: 200
                    height: videoNotesLabel.y + videoNotesLabel.height + topPadding + bottomPadding
                    title: "Audio/Video"
                    clip: false
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.top: interfaceGroupBox.bottom
                    anchors.topMargin: 8

                    Switch {
                        id: enableVideoSwitch
                        y: 96
                        text: checked ? "Video On" : "Video Off"
                        anchors.left: enableVideoLabel.right
                        anchors.leftMargin: 12
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        onCheckedChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: enableVideoLabel
                        y: 77
                        width: 100
                        text: "Enable Video"
                        anchors.verticalCenter: enableVideoSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Switch {
                        id: enableAudioSwitch
                        x: 100
                        y: 248
                        text: checked ? "Audio On" : "Audio Off"
                        anchors.left: enableAudioLabel.right
                        anchors.leftMargin: 12
                        anchors.top: enableVideoSwitch.bottom
                        anchors.topMargin: 8
                        onCheckedChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: enableAudioLabel
                        x: -12
                        y: 259
                        width: 100
                        text: "Enable Audio"
                        anchors.verticalCenter: enableAudioSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    ComboBox {
                        id: activeCameraCombo
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.top: enableAudioSwitch.bottom
                        anchors.topMargin: 8
                        anchors.left: activeCameraLabel.right
                        anchors.leftMargin: 12
                        onCurrentIndexChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: activeCameraLabel
                        y: 125
                        width: 100
                        text: "Active Camera"
                        anchors.verticalCenter: activeCameraCombo.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    ComboBox {
                        id: videoEncodingCombo
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        anchors.left: videoEncodingLabel.right
                        anchors.leftMargin: 12
                        anchors.top: activeCameraCombo.bottom
                        anchors.topMargin: 8
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        onCurrentIndexChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: videoEncodingLabel
                        width: 100
                        text: "Encoding"
                        anchors.verticalCenter: videoEncodingCombo.verticalCenter
                        anchors.leftMargin: 0
                        anchors.left: parent.left
                    }

                    SpinBox {
                        id: videoWidthSpinBox
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        stepSize: 10
                        to: 1920
                        from: 1
                        value: 640
                        anchors.left: videoWidthLabel.right
                        anchors.leftMargin: 12
                        anchors.top: videoEncodingCombo.bottom
                        anchors.topMargin: 8
                        onValueChanged: settingsFooterPane.state = "visible"
                        editable: true
                    }

                    Label {
                        id: videoWidthLabel
                        width: 100
                        text: "Width"
                        anchors.verticalCenter: videoWidthSpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: videoHeightSpinBox
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        stepSize: 10
                        to: 1080
                        from: 1
                        value: 480
                        anchors.left: videoHeightLabel.right
                        anchors.leftMargin: 12
                        anchors.top: videoWidthSpinBox.bottom
                        anchors.topMargin: 8
                        onValueChanged: settingsFooterPane.state = "visible"
                        editable: true
                    }

                    Label {
                        id: videoHeightLabel
                        width: 100
                        text: "Height"
                        anchors.verticalCenter: videoHeightSpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: videoBitrateSpinBox
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked & videoEncodingCombo.currentText !== "MJPEG"
                        stepSize: 10
                        to: 10000
                        from: 1
                        value: 500
                        anchors.left: videoBitrateLabel.right
                        anchors.leftMargin: 12
                        anchors.top: videoHeightSpinBox.bottom
                        anchors.topMargin: 8
                        onValueChanged: settingsFooterPane.state = "visible"
                        editable: true
                    }

                    Label {
                        id: videoBitrateLabel
                        width: 100
                        text: "Bitrate (Kb/s)"
                        anchors.verticalCenter: videoBitrateSpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: mjpegQualitySpinBox
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked & videoEncodingCombo.currentText === "MJPEG"
                        to: 100
                        from: 1
                        value: 50
                        anchors.left: mjpegQualityLabel.right
                        anchors.leftMargin: 12
                        anchors.top: videoBitrateSpinBox.bottom
                        anchors.topMargin: 8
                        onValueChanged: settingsFooterPane.state = "visible"
                        editable: true
                    }

                    Label {
                        id: mjpegQualityLabel
                        width: 100
                        text: "MJPEG: Quality"
                        anchors.verticalCenter: mjpegQualitySpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: videoFramerateSpinBox
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        from: 1
                        to: 30
                        value: 30
                        stepSize: 1
                        anchors.left: videoFramerateLabel.right
                        anchors.leftMargin: 12
                        anchors.top: mjpegQualitySpinBox.bottom
                        anchors.topMargin: 8
                        onValueChanged: settingsFooterPane.state = "visible"
                        editable: true
                    }

                    Label {
                        id: videoFramerateLabel
                        width: 100
                        text: "Framerate (fps)"
                        anchors.verticalCenter: videoFramerateSpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Switch {
                        id: grayscaleVideoSwitch
                        y: 96
                        text: checked ? "Grayscale On" : "Grayscale Off"
                        anchors.left: grayscaleVideoLabel.right
                        anchors.leftMargin: 12
                        anchors.top: videoFramerateSpinBox.bottom
                        anchors.topMargin: 8
                        onCheckedChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: grayscaleVideoLabel
                        y: 77
                        width: 100
                        text: "Grayscale Video"
                        anchors.verticalCenter: grayscaleVideoSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Switch {
                        id: stereoVideoSwitch
                        text: checked ? "Stereo On" : "Stereo Off"
                        enabled: enableVideoSwitch.enabled & enableVideoSwitch.checked
                        anchors.left: stereoVideoLabel.right
                        anchors.leftMargin: 12
                        anchors.top: grayscaleVideoSwitch.bottom
                        anchors.topMargin: 8
                        onCheckedChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: stereoVideoLabel
                        width: 100
                        height: 18
                        text: "Stereo Video"
                        anchors.verticalCenter: stereoVideoSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Label {
                        id: videoNotesLabel
                        text: "<b>Stereo Video</b> has the same target bitrate as mono video, with half the horizontal resolution per eye."
                        textFormat: Text.RichText
                        verticalAlignment: Text.AlignBottom
                        anchors.top: stereoVideoSwitch.bottom
                        anchors.topMargin: 8
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                        wrapMode: Text.WordWrap
                    }
                }

                GroupBox {
                    id: simulationGroupBox
                    height: simNotesLabel.y + simNotesLabel.height + topPadding + bottomPadding
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.top: avGroupBox.bottom
                    anchors.topMargin: 8
                    title: "Simulation"

                    Label {
                        id: simLatencyLabel
                        y: 27
                        width: 100
                        text: "Latency (ms)"
                        anchors.verticalCenter: simLatencySpinBox.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    SpinBox {
                        id: simLatencySpinBox
                        anchors.left: simLatencyLabel.right
                        anchors.leftMargin: 12
                        anchors.top: parent.top
                        anchors.topMargin: 0
                        stepSize: 50
                        editable: true
                        from: 0
                        to: 10000
                        onValueChanged: settingsFooterPane.state = "visible"
                    }

                    Label {
                        id: simNotesLabel
                        text: "<b>Latency</b> only delays driving commands. Any operations performed on this screen will be unaffected."
                        anchors.top: simLatencySpinBox.bottom
                        anchors.topMargin: 8
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                        textFormat: Text.RichText
                        wrapMode: Text.WordWrap
                    }
                }

                GroupBox {
                    id: gpsGroupBox
                    width: 200
                    height: gpsHistoryField.y + gpsHistoryField.height + topPadding + bottomPadding
                    anchors.top: simulationGroupBox.bottom
                    anchors.topMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    title: "GPS"

                    Label {
                        id: enableGpsLabel
                        y: 1
                        width: 100
                        text: "Enable GPS"
                        anchors.verticalCenter: enableGpsSwitch.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                    }

                    Switch {
                        id: enableGpsSwitch
                        text: checked ? "GPS On" : "GPS Off"
                        anchors.left: enableGpsLabel.right
                        anchors.leftMargin: 12
                        anchors.top: parent.top
                        anchors.topMargin: 0
                    }

                    Label {
                        id: gpsLocationLabel
                        y: 76
                        width: 100
                        text: "Last Location"
                        anchors.left: parent.left
                        anchors.leftMargin: 0
                        anchors.verticalCenter: gpsLocationField.verticalCenter
                    }

                    TextField {
                        id: gpsLocationField
                        readOnly: true
                        text: "Nothing Received"
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.left: gpsLocationLabel.right
                        anchors.leftMargin: 12
                        anchors.top: enableGpsSwitch.bottom
                        anchors.topMargin: 8
                    }

                    Label {
                        id: gpsHistoryLabel
                        x: 0
                        y: 76
                        width: 100
                        text: "Data Points"
                        anchors.verticalCenterOffset: 0
                        anchors.leftMargin: 0
                        anchors.verticalCenter: gpsHistoryField.verticalCenter
                        anchors.left: parent.left
                    }

                    TextField {
                        id: gpsHistoryField
                        text: "Nothing Received"
                        anchors.leftMargin: 12
                        anchors.left: gpsHistoryLabel.right
                        anchors.rightMargin: 12
                        anchors.top: gpsLocationField.bottom
                        anchors.topMargin: 8
                        anchors.right: gpsClearButton.left
                        readOnly: true
                    }

                    Button {
                        id: gpsClearButton
                        x: 296
                        y: 142
                        text: "Clear"
                        anchors.verticalCenter: gpsHistoryField.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        onClicked: clearGps()
                    }
                }

            }

            ScrollIndicator.vertical:  ScrollIndicator { }
        }

        DropShadow {
            id: footerShadow
            anchors.fill: settingsFooterPane
            visible: settingsFooterPane.visible
            source: settingsFooterPane
            horizontalOffset: -15
            radius: 15
            samples: 20
            color: "#000000"
        }

        Pane {
            id: settingsFooterPane
            //Material.theme: Material.Light
            //Universal.theme: Universal.Light
            height: 64
            visible: true
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.bottom: parent.bottom
            state: "hidden"

            Button {
                id: applySettingsButton
                text: "Apply"
                Material.background: Material.accent
                Universal.background: Universal.accent
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                onClicked: {
                    settingsFooterPane.state = "hidden"
                    settingsApplied()
                }
            }

            Label {
                id: settingsFooterLabel
                text: "Settings have been changed."
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: revertSettingsButton.left
                anchors.rightMargin: 12
                anchors.left: parent.left
                anchors.leftMargin: 0
            }

            Button {
                id: revertSettingsButton
                text: "Revert"
                anchors.rightMargin: 12
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.right: applySettingsButton.left
                onClicked: {
                    settingsFooterPane.state = "hidden"
                    requestUiSync()
                }
            }

            states: [
                State{
                    name: "hidden"
                    PropertyChanges {
                        target: settingsFooterPane
                        anchors.bottomMargin: -height - footerShadow.radius
                    }
                },
                State {
                    name: "visible"
                    PropertyChanges {
                        target: settingsFooterPane
                        anchors.bottomMargin: 0
                    }
                }
            ]

            transitions: [
                Transition {
                    PropertyAnimation {
                        properties: "anchors.bottomMargin"
                        duration: 250
                        easing.type: Easing.InOutExpo
                    }
                }
            ]
        }

    }

    DropShadow {
        id: headerShadow
        anchors.fill: headerPane
        visible: headerPane.visible
        source: headerPane
        radius: 15
        samples: 20
        color: "#000000"
    }

    Pane {
        id: headerPane
        Material.background: Material.accent
        Material.foreground: "#ffffff"
        Universal.background: Universal.accent
        Universal.foreground: "#ffffff"
        height: 64
        clip: false
        bottomPadding: 12
        rightPadding: 12
        leftPadding: 12
        topPadding: 12
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        BusyIndicator {
            id: busyIndicator
            width: 40
            height: 40
            visible: true
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.verticalCenter: parent.verticalCenter
            Material.accent: Material.foreground
            Universal.accent: Universal.foreground
        }

        Image {
            id: statusImage
            width: 40
            height: 40
            visible: false
            sourceSize.height: height
            sourceSize.width: width
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.verticalCenter: parent.verticalCenter
        }

        ColorOverlay {
            id: statusImageColorOverlay
            anchors.fill: statusImage
            source: statusImage
            color: "#ffffff"
            visible: statusImage.visible
        }

        Label {
            id: statusLabel
            y: 16
            text: "Please Wait..."
            font.pointSize: 22
            anchors.verticalCenter: busyIndicator.verticalCenter
            anchors.left: busyIndicator.right
            anchors.leftMargin: 12
        }

        Label {
            id: bitrateLabel
            x: 308
            y: 11
            text: "▲ <b>Up</b> Mb/s<br>▼ <b>Down</b> Mb/s"
            visible: false
            anchors.verticalCenterOffset: 0
            textFormat: Text.RichText
            font.pointSize: 10
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: headerSeparator.left
            anchors.rightMargin: 12
        }

        Pane {
            id: headerSeparator
            x: 437
            Material.background: Material.foreground
            Universal.background: Universal.foreground
            width: 1
            anchors.left: parent.left
            anchors.leftMargin: 438
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
        }

        ToolbarButton {
            id: sidebarToolbarButton
            anchors.left: headerSeparator.right

            onClicked: {
                if (sidePaneVisibilityState === "visible") {
                    sidePaneVisibilityState = "hidden"
                }
                else {
                    sidePaneVisibilityState = "visible"
                }
            }
        }

        RecordButton {
            id: recordToolbarButton
            anchors.right: fullscreenToolbarButton.left
            onClicked: {
                switch (recordingState) {
                case "recording":
                    confirmRecordStopDialog.visible = true
                    break
                case "idle":
                    recordButtonClicked()
                    break
                case "waiting":
                default:
                    break
                }
            }
        }

        ToolbarButton {
            id: fullscreenToolbarButton
            anchors.right: parent.right
            onClicked: {
                if (fullscreenState === "fullscreen") {
                    fullscreenState = "normal"
                }
                else {
                    fullscreenState = "fullscreen"
                }
            }
        }
    }

    DropShadow {
        id: notificationShadow
        anchors.fill: notificationPane
        source: notificationPane
        radius: 15
        visible: notificationPane.visible
        opacity: notificationPane.opacity
        samples: 20
        color: "#000000"
    }

    Pane {
        id: notificationPane
        //Material.theme: Material.Light
        //Universal.theme: Universal.Light
        x: 800
        y: 181
        width: 400
        height: 100
        state: "hidden"
        anchors.right: parent.right


        Label {
            id: notificationTitleLabel
            text: "Network Driving Disabled"
            font.pointSize: 12
            font.bold: true
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: notificationImage.right
            anchors.leftMargin: 12
            anchors.top: parent.top
            anchors.topMargin: 0
        }

        Label {
            id: notificationLabel
            text: "The rover is being driven by serial override. Network drive commands will not be accepted."
            anchors.topMargin: 0
            anchors.leftMargin: 12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: parent.right
            anchors.left: notificationImage.right
            anchors.top: notificationTitleLabel.bottom
            verticalAlignment: Text.AlignTop
            wrapMode: Text.WordWrap
            font.pointSize: 10
        }


        Image {
            id: notificationImage
            width: height
            sourceSize.height: height
            sourceSize.width: width
            fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            source: "qrc:/icons/ic_info_white_48px.svg"
        }


        ColorOverlay {
            id: notificationImageColorOverlay
            anchors.fill: notificationImage
            source: notificationImage
            color: "#ff0000"
        }


        MouseArea {
            id: notificationMouseArea
            anchors.fill: parent
            onClicked: notificationPane.state = "hidden"
            cursorShape: Qt.PointingHandCursor
            anchors.topMargin: -notificationPane.topPadding
            anchors.bottomMargin: -notificationPane.bottomPadding
            anchors.leftMargin: -notificationPane.leftPadding
            anchors.rightMargin: -notificationPane.rightPadding
        }


        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: notificationPane
                    anchors.rightMargin: -width - notificationShadow.radius
                    opacity: 0
                }
            },
            State {
                name: "visible"
                PropertyChanges {
                    target: notificationPane
                    anchors.rightMargin: 0
                    opacity: 0.7
                }
            }

        ]

        transitions: [
            Transition {
                from: "hidden"
                to: "visible"
                PropertyAnimation {
                    properties: "anchors.rightMargin,opacity"
                    duration: 500
                    easing.type: Easing.InOutExpo
                }
            },
            Transition {
                from: "visible"
                to: "hidden"
                PropertyAnimation {
                    properties: "opacity"
                    duration: 100
                }
            }
        ]
    }

    ConfirmRecordStopDialog {
        id: confirmRecordStopDialog
        onAccepted: recordButtonClicked()
    }

    OnWindowCloseDialog {
        id: onWindowCloseDialog
    }
}
