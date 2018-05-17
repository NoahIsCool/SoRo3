import QtQuick 2.0

/*
  Timer to keep track of how long a test has been running.
  */
Timer {
    id: recordingTimer
    interval: 1000
    repeat: true
    triggeredOnStart: true

    property string timeString
    property int elapsed

    onTriggered: {
        var elapsedHours = Math.floor(elapsed / 3600)
        var elapsedMinutes = Math.floor((elapsed - (elapsedHours * 3600)) / 60)
        var elapsedSeconds = Math.floor((elapsed - (elapsedHours * 3600)) - (elapsedMinutes * 60));

        if (elapsedMinutes.toString().length == 1) {
            elapsedMinutes = "0" + elapsedMinutes
        }
        if (elapsedSeconds.toString().length == 1) {
            elapsedSeconds = "0" + elapsedSeconds
        }

        timeString = elapsedHours + ":" + elapsedMinutes + ":" + elapsedSeconds

        recordToolbarButton.label.text = timeString
        elapsed++
    }
}
