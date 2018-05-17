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

ApplicationWindow {
    id: commentsWindow

    width: 640
    height: 480
    title: qsTr("Research Control - Comments")

    property alias connectionState: connectionStateGroup.state
    property alias recordingState: recordingStateGroup.state
    property alias recordToolbarButton: recordToolbarButton
    property alias recordingTimer: recordingTimer

    signal logCommentEntered(string comment)
    signal recordButtonClicked()
    signal closed()

    // Internal properties

    property color theme_yellow: "#FBC02D"
    property color theme_red: "#d32f2f"
    property color theme_green: "#388E3C"
    property color theme_blue: "#1976D2"
    property color accentColor: "#616161"

    function recordComment(text, type) {
        commentsListModel.append({"commentText": "At " + recordingTimer.timeString + ":", "messageType": "time"})
        commentsListModel.append({"commentText": text, "messageType": type})
        commentsListView.positionViewAtEnd()
        if (type === "user") {
            // Send signal to backend
            logCommentEntered(text)
        }
    }

    /*
      Theme settings
      */
    Material.theme: Material.Dark
    Material.accent: accentColor
    Universal.theme: Universal.Dark
    Universal.accent: accentColor

    onClosing: {
        if (recordingState !== "idle") {
            close.accepted = false
            onWindowCloseDialog.visible = true
        }
        else {
            closed()
        }
    }


    RecordingTimer {
        id: recordingTimer

        onTimeStringChanged: {
            recordToolbarButton.label.text = timeString
        }
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
                    target: commentsWindow
                    accentColor: theme_yellow
                }
            },
            State {
                name: "connected"
                PropertyChanges {
                    target: commentsWindow
                    accentColor: theme_green
                }
            },
            State {
                name: "error"
                PropertyChanges {
                    target: commentsWindow
                    accentColor: theme_red
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
      State group to update the UI to reflect a change in the rover's connection status
      Can be accessed from the backend with the connectionState property
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
                PropertyChanges {
                    target: commentsTextArea
                    enabled: true
                }
                StateChangeScript {
                    script: commentsListModel.clear()
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
                PropertyChanges {
                    target: commentsTextArea
                    enabled: false
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
                PropertyChanges {
                    target: commentsTextArea
                    enabled: false
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

    Item {
        id: commentsPane
        anchors.top: headerPane.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottomMargin: 8
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        ListView {
            id: commentsListView
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height * 0.7
            clip: true

            /* This list model accepts two parameters:
              - text: the comment text
              - type: can be either time, system, or user. Use user for user-entered comments,
               and system for system status messages.
               */
            model: ListModel {
                id: commentsListModel
            }

            delegate:Label {
                width: parent.width
                text: commentText
                topPadding: 10
                leftPadding: 10
                rightPadding: 10
                bottomPadding: timeMessage ? 5 : 10
                wrapMode: Text.WordWrap
                color: messageType == "time" ? Material.accent : Material.foreground
                font.italic: messageType == "time"
                font.bold: messageType == "system"
            }
        }

        GroupBox {
            anchors.top: commentsListView.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 10
            TextArea {
                id: commentsTextArea
                anchors.fill: parent
                placeholderText: "Enter your comments here"
                wrapMode: Text.WordWrap
                enabled: false

                Keys.onReturnPressed: {
                    recordComment(text.trim(), "user")
                    text = "";
                }
            }
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

        Label {
            id: headerLabel
            y: 16
            text: qsTr("Comments")
            font.pointSize: 22
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
        }

        RecordButton {
            id: recordToolbarButton
            anchors.right: parent.right

            onClicked: {
                switch (recordingState) {
                case "idle":
                    recordButtonClicked()
                    break
                case "recording":
                    confirmRecordStopDialog.visible = true
                    break
                case "waiting":
                    break
                }
            }
        }
    }

    ConfirmRecordStopDialog {
        id: confirmRecordStopDialog
        onAccepted: recordButtonClicked()
    }

    OnWindowCloseDialog {
        id: onWindowCloseDialog
    }
}
