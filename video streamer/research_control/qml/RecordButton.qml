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

import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtGraphicalEffects 1.0

/* This is the test start/stop button. It also shows the elapsed time during a test,
  which it calculates itself using a timer. This doesn't need to be completely accurate
  as it only serves as a reference, the actual testing timestamps are saved elsewhere.
  */
ToolbarButton {
    id: recordButton
    width: label.width + busyIndicator.width + image.width
    height: parent.height
    state: "idle"

    property alias busyIndicator: busyIndicator
    property alias label: label

    BusyIndicator {
        id: busyIndicator
        width: 0
        height: 40
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        Material.accent: Material.foreground
        Universal.accent: Universal.foreground
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: image.right
        leftPadding: 4
        rightPadding: 4
        font.pointSize: 20
    }

    states: [
        State {
            name: "recording"
            PropertyChanges {
                target: recordButton
                cursorShape: Qt.PointingHandCursor
                image.source: "qrc:/icons/ic_stop_white_48px.svg"
                image.visible: true
                image.width: 40
                label.text: ""
                label.visible: true
                label.anchors.left: image.right
                busyIndicator.visible: false
                busyIndicator.width: 0
                busyIndicator.anchors.left: image.right
                tooltip.text: "Stop Recording Data"
            }
        },
        State {
            name: "idle"
            PropertyChanges {
                target: recordButton
                cursorShape: Qt.PointingHandCursor
                image.source: "qrc:/icons/ic_play_arrow_white_48px.svg"
                image.visible: true
                image.width: 40
                label.visible: false
                busyIndicator.visible: false
                busyIndicator.width: 0
                busyIndicator.anchors.left: image.right
                tooltip.text: "Start Recording Data"
            }
        },
        State {
            name: "waiting"
            PropertyChanges {
                target: recordButton
                cursorShape: Qt.BusyCursor
                busyIndicator.visible: true
                busyIndicator.width: 40
                image.visible: false
                image.width: 0
                label.visible: false
                label.anchors.left: busyIndicator.right
                tooltip.text: "Waiting..."
            }
        }
    ]
}
