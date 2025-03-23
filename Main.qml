import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: qsTr("VigilantHub")
    color: "#1A1F2B"  // Dark Tesla-like blue-gray

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1A1F2B" }
            GradientStop { position: 1.0; color: "#2E3440" }
        }
    }

    // Header
    Text {
        text: "Vigilant Hub"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 40
        color: "#D8DEE9"
        font.family: "Montserrat"
        font.pixelSize: 40
        font.weight: Font.Bold
    }

    // Video Stream
    Image {
        id: videoStream
        width: parent.width * 0.65
        height: parent.height * 0.65
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20
        source: "image://cameraProvider/frame"
        cache: false
        asynchronous: true

        property int frameCounter: 0
        onFrameCounterChanged: source = "image://cameraProvider/frame?" + frameCounter

        Timer {
            interval: 66  // 15 FPS
            running: true
            repeat: true
            onTriggered: videoStream.frameCounter++
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#5E81AC"
            border.width: 3
            radius: 12
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "#5E81AC"
                border.width: 1
                radius: 14
                opacity: 0.6
            }
        }
    }

    // Bottom Panel (Yawn + Head Pose + Eye Closure + Phone)
    Rectangle {
        id: bottomPanel
        width: parent.width * 0.65
        height: 60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        color: Qt.rgba(0.15, 0.18, 0.25, 0.9)
        radius: 10
        border.color: "#4C566A"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 20

            Text {
                id: yawnText
                text: "Yawn " + (mlProcessor.yawnPercentage ? mlProcessor.yawnPercentage.toFixed(1) : "0.0") + "%"
                color: {
                    if (mlProcessor.yawnPercentage > 70) return "#BF616A"
                    if (mlProcessor.yawnPercentage > 30) return "#D08770"
                    return "#A3BE8C"
                }
                font.family: "Montserrat"
                font.pixelSize: 18
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignLeft
                Behavior on text { PropertyAnimation { duration: 500 } }
            }

            Text {
                id: headPoseText
                text: "Head Pose " + (mlProcessor.headPosePercentage ? mlProcessor.headPosePercentage.toFixed(1) : "0.0") + "%"
                color: {
                    if (mlProcessor.headPosePercentage > 70) return "#BF616A"
                    if (mlProcessor.headPosePercentage > 30) return "#D08770"
                    return "#A3BE8C"
                }
                font.family: "Montserrat"
                font.pixelSize: 18
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
                Behavior on text { PropertyAnimation { duration: 500 } }
            }

            Text {
                id: eyeClosureText
                text: "Eye Closure " + (mlProcessor.eyeClosurePercentage ? mlProcessor.eyeClosurePercentage.toFixed(1) : "0.0") + "%"
                color: {
                    if (mlProcessor.eyeClosurePercentage > 70) return "#BF616A"
                    if (mlProcessor.eyeClosurePercentage > 30) return "#D08770"
                    return "#A3BE8C"
                }
                font.family: "Montserrat"
                font.pixelSize: 18
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
                Behavior on text { PropertyAnimation { duration: 500 } }
            }

            Text {
                id: phoneText
                text: "Phone " + (mlProcessor.phonePercentage ? mlProcessor.phonePercentage.toFixed(1) : "0.0") + "%"
                color: {
                    if (mlProcessor.phonePercentage > 70) return "#BF616A"
                    if (mlProcessor.phonePercentage > 30) return "#D08770"
                    return "#A3BE8C"
                }
                font.family: "Montserrat"
                font.pixelSize: 18
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignRight
                Behavior on text { PropertyAnimation { duration: 500 } }
            }
        }
    }

    // Warning Message
    Text {
        id: warningText
        text: {
            let warnings = [];
            if (mlProcessor.yawnPercentage > 10) warnings.push("Yawning");
            if (mlProcessor.headPosePercentage > 10) warnings.push("Looking Away");
            if (mlProcessor.eyeClosurePercentage > 10) warnings.push("Eyes Closed");
            if (mlProcessor.phonePercentage > 10) warnings.push("Phone Usage");
            if (warnings.length > 0) {
                return "WARNING: " + warnings.join(" and ") + " Detected!"
            }
            return ""
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: bottomPanel.top
        anchors.bottomMargin: 10
        color: "#BF616A"
        font.family: "Montserrat"
        font.pixelSize: 20
        font.weight: Font.Bold
        visible: mlProcessor.yawnPercentage > 10 || mlProcessor.headPosePercentage > 10 || mlProcessor.eyeClosurePercentage > 10 || mlProcessor.phonePercentage > 10
        Behavior on text { PropertyAnimation { duration: 500 } }
        Behavior on opacity { PropertyAnimation { duration: 500 } }
    }

    // Debug Binding Issues
    Connections {
        target: mlProcessor
        function onResultChanged() {
            console.log("Result changed - Yawn:", mlProcessor.yawnPercentage, "Head Pose:", mlProcessor.headPosePercentage, "Eye Closure:", mlProcessor.eyeClosurePercentage, "Phone:", mlProcessor.phonePercentage)
        }
    }

    // Subtle glow animation for video stream
    SequentialAnimation {
        running: true
        loops: Animation.Infinite
        PropertyAnimation {
            target: videoStream
            property: "opacity"
            from: 1.0
            to: 0.95
            duration: 2000
        }
        PropertyAnimation {
            target: videoStream
            property: "opacity"
            from: 0.95
            to: 1.0
            duration: 2000
        }
    }
}
