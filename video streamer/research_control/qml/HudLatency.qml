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

import QtQuick 2.4
import QtGraphicalEffects 1.0
import Soro 1.0

Item {
    id: root
    property bool halfWidth: false
    property int latency: 0
    property real xValue: 0
    property real yValue: 0
    property int latencyTolerance: 0
    property Item blurSource

    height: 600
    width: height

    onLatencyToleranceChanged: {
        graphX.latencyTolerance = latencyTolerance
        graphY.latencyTolerance = latencyTolerance
    }

    onLatencyChanged: {
        graphX.latency = latency
        graphY.latency = latency
        if (latency < 0) {
            latencyText.text = "Delay<br><b>N/A</b>"
        }
        else {
            latencyText.text = "Delay<br><b>" + latency.toString() + "ms</b>"
        }
    }

    onXValueChanged: {
        graphX.value = xValue
    }

    onYValueChanged: {
        graphY.value = yValue
    }

    transform: Scale { xScale: halfWidth ? 0.5 : 1 }

    FastBlur {
        source: ShaderEffectSource {
           sourceItem: root.blurSource
           sourceRect: Qt.rect(root.x + xBackground.x,
                               root.y + xBackground.y,
                               halfWidth ? xBackground.width / 2 : xBackground.width,
                               xBackground.height)
        }
        radius: 64
        transparentBorder: true
        anchors.margins: 4
        anchors.fill: xBackground
    }

    FastBlur {
        source: ShaderEffectSource {
           sourceItem: root.blurSource
           sourceRect: Qt.rect(root.x + yBackground.x,
                               root.y + yBackground.y,
                               halfWidth ? yBackground.width / 2 : yBackground.width,
                               yBackground.height)
        }
        radius: 64
        transparentBorder: true
        anchors.margins: 4
        anchors.fill: yBackground
    }

    HudBackground {
        id: xBackground
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        width:  parent.height / 4
        opacity: 0.8
    }

    HudBackground {
        id: yBackground
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 0
        height: parent.height / 4
        opacity: 0.8
    }

    HudLatencyGraphImpl {
        id: graphX
        anchors.rightMargin: anchors.bottomMargin
        anchors.bottomMargin: 16 + yBackground.height / 10
        anchors.leftMargin: anchors.bottomMargin
        anchors.topMargin: latencyBackground.height + xBackground.width / 10
        anchors.fill: xBackground
        mode: "vertical"
    }

    HudLatencyGraphImpl {
        id: graphY
        anchors.fill: yBackground
        anchors.topMargin: 16 + yBackground.height / 10;
        anchors.bottomMargin: anchors.topMargin
        anchors.leftMargin: anchors.topMargin
        anchors.rightMargin: latencyBackground.width + yBackground.height / 10
        mode: "horizontal"
    }

    HudBackground {
        id: latencyBackground
        backdrop.color: "#4CAF50"
        backdrop.radius: 0
        opacity: 1
        width: height
        height: parent.height / 4
        anchors.right: parent.right
        anchors.top: parent.top

        Text {
            id: latencyText
            anchors.centerIn: parent
            font.pointSize: Math.max(parent.width / 8, 1)
            text: "Delay<br><b>N/A</b>"
            horizontalAlignment: Text.AlignHCenter
            color: "white"
        }
    }
}
