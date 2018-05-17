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

MouseArea {
    width: image.width
    height: parent.height
    cursorShape: Qt.PointingHandCursor
    hoverEnabled: true

    onEntered: {
        tooltip.visible = true
    }

    onExited: {
        tooltip.visible = false
    }

    property alias image: image
    property alias tooltip: tooltip

    ToolTip {
        id: tooltip
        delay: 500
        timeout: 5000
    }

    Image {
        id: image
        sourceSize.height: height
        sourceSize.width: width
        fillMode: Image.PreserveAspectFit
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 48
        height: 48
    }

    ColorOverlay {
        id: colorOverlay
        anchors.fill: image
        source: image
        color: Material.foreground
    }
}
