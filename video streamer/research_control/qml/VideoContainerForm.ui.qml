import QtQuick 2.7
import QtGStreamer 1.0

Rectangle {
    id: videoContainer

    VideoItem {
        id: videoItemStereoL
        width: parent.width / 2
        height: parent.height
        x: 0
        y: 0
        surface: videoSurfaceStereoL // Set as context property in C++
    }

    VideoItem {
        id: videoItemStereoR
        width: parent.width / 2
        height: parent.height
        x: parent.width / 2
        y: 0
        surface: videoSurfaceStereoR // Set as context property in C++
    }

    VideoItem {
        id: videoItemMono
        anchors.fill: parent
        surface: videoSurfaceMono // Set as context property in C++
    }
}
