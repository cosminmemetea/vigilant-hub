import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: window
    width: 800
    height: 480
    visible: true
    title: qsTr("VigilantHub")
    color: "#1E1E1E"

    Image {
        id: videoStream
        width: parent.width * 0.5
        height: parent.height * 0.67
        anchors.centerIn: parent
        source: "image://cameraProvider/frame"
        cache: false
        asynchronous: true

        property int frameCounter: 0
        onFrameCounterChanged: {
            source = "image://cameraProvider/frame?" + frameCounter;
        }

        Timer {
            interval: 66  // 15 FPS
            running: true
            repeat: true
            onTriggered: {
                videoStream.frameCounter++;
            }
        }

        onStatusChanged: {
            if (status === Image.Ready) {
                console.log("Image loaded successfully:", width, "x", height);
            } else if (status === Image.Error) {
                console.log("Failed to load image from cameraProvider.");
            } else if (status === Image.Loading) {
                console.log("Image is loading...");
            }
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#00FFFF"
            border.width: 2
        }
    }

    Rectangle {
        id: omsWidget
        width: parent.width * 0.25
        height: parent.height
        anchors.left: parent.left
        color: Qt.rgba(0.54, 0.17, 0.89, 0.2)
        border.color: "#8A2BE2"
        border.width: 2

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Seatbelt: YES"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Presence: Adult"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
        }
    }

    Rectangle {
        id: dmsWidget
        width: parent.width * 0.25
        height: parent.height
        anchors.right: parent.right
        color: Qt.rgba(0.12, 0.56, 1.0, 0.2)
        border.color: "#1E90FF"
        border.width: 2

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Driver ID: Adam"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Eye Openness: 85%"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Blinks/Min: 16"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Gaze: -18°, 7°"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Distraction: NO"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Drowsiness: NO"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Cellphone: NO"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: "Smoking: NO"
                color: "#FFFFFF"
                font.family: "Arial"
                font.pixelSize: 16
                font.bold: true
            }
        }
    }
}
