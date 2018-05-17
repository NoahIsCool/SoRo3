import QtQuick 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    property bool halfWidth: false
    property real compassHeading: 0
    property real compassHeadingZero: 0

    transform: Scale { xScale: halfWidth ? 0.5 : 1 }

    DropShadow {
        source: navDirectionImageColorOverlay
        anchors.fill: navDirectionImageColorOverlay
        radius: 10
        samples: radius
        rotation: compassHeading - compassHeadingZero
        spread: 0.2
        color: "#000000"
    }

    ColorOverlay {
        id: navDirectionImageColorOverlay
        anchors.fill: navDirectionImage
        source: navDirectionImage
        color: "#ffffff"
        rotation: compassHeading
    }

    Image {
        id: navDirectionImage
        anchors.fill: parent
        source: "qrc:/icons/ic_navigation_white_48px.svg"
        visible: false
        rotation: compassHeading
        sourceSize: Qt.size(width, height)
    }
}
