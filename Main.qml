import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: qsTr("VigilantHub")

    property bool hasData: false
    property bool isProcessorReady: false

    // Dark mode colors with a futuristic, sci-fi aesthetic
    property color backgroundColor: "#0A0E1A"  // Deep, almost black with a slight blue tint
    property color gradientEndColor: "#1A2333"  // Subtle gradient for depth
    property color textColor: "#E0E7FF"  // High-contrast, slightly blue-tinted white
    property color panelColor: Qt.rgba(0.15, 0.18, 0.25, 0.7)  // Glassmorphism: translucent dark panel
    property color panelGradientEnd: Qt.rgba(0.2, 0.23, 0.3, 0.7)
    property color borderColor: "#4A5A8A"  // Subtle blue-gray border
    property color videoBorderColor: "#00D4FF"  // Neon cyan for a futuristic touch
    property color warningGlowColor: "#FF4D6A"  // Neon magenta for warnings
    property color shadowColor: Qt.rgba(0.05, 0.05, 0.1, 0.5)  // Subtle shadow for depth

    // Accent color customization (can be changed to "#FF4D6A" for magenta or "#00FFAA" for green)
    property color accentColor: "#00D4FF"  // Default: Neon cyan

    property real baseFontSize: window.width * 0.015

    // Properties for storing the latest valid values
    property real lastYawnPercentage: 0.0
    property real lastHeadPosePercentage: 0.0
    property real lastEyeClosurePercentage: 0.0
    property real lastPhonePercentage: 0.0

    // Precomputed properties for text and colors
    property string yawnTextValue: "Yawn n.a."
    property string headPoseTextValue: "Head Pose n.a."
    property string eyeClosureTextValue: "Eye Closure n.a."
    property string phoneTextValue: "Phone n.a."
    property string warningTextValue: ""
    property color yawnTextColor: textColor
    property color headPoseTextColor: textColor
    property color eyeClosureTextColor: textColor
    property color phoneTextColor: textColor
    property bool warningVisible: false

    function getTextColor(hasData, percentage) {
        if (!hasData) return textColor
        if (percentage > 70) return warningGlowColor
        if (percentage > 30) return "#FF8C5A"  // Warm orange for medium alerts
        return "#00FFAA"  // Bright green for safe values
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: backgroundColor }
            GradientStop { position: 1.0; color: gradientEndColor }
        }
    }

    Text {
        id: titleText
        text: "Vigilant Hub"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: window.height * 0.05
        color: textColor
        font.pixelSize: baseFontSize * 2.5
        font.weight: Font.Bold
    }

    Image {
        id: videoStream
        width: parent.width * 0.65
        height: parent.height * 0.65
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -window.height * 0.03
        source: "image://cameraProvider/frame"
        cache: false
        asynchronous: true

        property int frameCounter: 0
        onFrameCounterChanged: source = "image://cameraProvider/frame?" + frameCounter

        Timer {
            interval: 200  // 5 FPS
            running: true
            repeat: true
            onTriggered: videoStream.frameCounter++
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: videoBorderColor
            border.width: 3
            radius: 12
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: videoBorderColor
                border.width: 1
                radius: 14
                opacity: 0.6
            }
        }
    }

    Rectangle {
        id: bottomPanel
        width: parent.width * 0.65
        height: window.height * 0.08
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: window.height * 0.03
        gradient: Gradient {
            GradientStop { position: 0.0; color: panelColor }
            GradientStop { position: 1.0; color: panelGradientEnd }
        }
        radius: 10
        border.color: borderColor
        border.width: 1

        Rectangle {
            anchors.fill: parent
            anchors.margins: -5
            z: -1
            color: shadowColor
            radius: 10
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: window.width * 0.01
            spacing: window.width * 0.02

            Text {
                id: yawnText
                text: yawnTextValue
                color: yawnTextColor
                font.pixelSize: baseFontSize
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth
            }

            Text {
                id: headPoseText
                text: headPoseTextValue
                color: headPoseTextColor
                font.pixelSize: baseFontSize
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth
            }

            Text {
                id: eyeClosureText
                text: eyeClosureTextValue
                color: eyeClosureTextColor
                font.pixelSize: baseFontSize
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth
            }

            Text {
                id: phoneText
                text: phoneTextValue
                color: phoneTextColor
                font.pixelSize: baseFontSize
                font.weight: Font.Medium
                Layout.alignment: Qt.AlignRight
                Layout.fillWidth: true
                Layout.preferredWidth: implicitWidth
            }
        }
    }

    Rectangle {
        id: warningBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: bottomPanel.top
        anchors.bottomMargin: window.height * 0.015
        width: warningText.implicitWidth + 20
        height: warningText.implicitHeight + 10
        color: Qt.rgba(warningGlowColor.r, warningGlowColor.g, warningGlowColor.b, 0.3)
        radius: 5
        visible: warningVisible

        Text {
            id: warningText
            text: warningTextValue
            anchors.centerIn: parent
            color: warningGlowColor
            font.pixelSize: baseFontSize * 1.2
            font.weight: Font.Bold
        }
    }

    Connections {
        target: mlProcessor
        function onResultChanged() {
            console.log("Result changed - Yawn:", mlProcessor ? mlProcessor.yawnPercentage : "undefined",
                        "Head Pose:", mlProcessor ? mlProcessor.headPosePercentage : "undefined",
                        "Eye Closure:", mlProcessor ? mlProcessor.eyeClosurePercentage : "undefined",
                        "Phone:", mlProcessor ? mlProcessor.phonePercentage : "undefined")

            if (mlProcessor) {
                let yawn = mlProcessor.yawnPercentage
                let headPose = mlProcessor.headPosePercentage
                let eyeClosure = mlProcessor.eyeClosurePercentage
                let phone = mlProcessor.phonePercentage

                if (yawn !== undefined && yawn >= 0 &&
                    headPose !== undefined && headPose >= 0 &&
                    eyeClosure !== undefined && eyeClosure >= 0 &&
                    phone !== undefined && phone >= 0) {
                    lastYawnPercentage = yawn
                    lastHeadPosePercentage = headPose
                    lastEyeClosurePercentage = eyeClosure
                    lastPhonePercentage = phone
                    hasData = true
                    isProcessorReady = true

                    // Precompute text and color values
                    yawnTextValue = "Yawn " + lastYawnPercentage.toFixed(1) + "%"
                    headPoseTextValue = "Head Pose " + lastHeadPosePercentage.toFixed(1) + "%"
                    eyeClosureTextValue = "Eye Closure " + lastEyeClosurePercentage.toFixed(1) + "%"
                    phoneTextValue = "Phone " + lastPhonePercentage.toFixed(1) + "%"
                    yawnTextColor = getTextColor(hasData, lastYawnPercentage)
                    headPoseTextColor = getTextColor(hasData, lastHeadPosePercentage)
                    eyeClosureTextColor = getTextColor(hasData, lastEyeClosurePercentage)
                    phoneTextColor = getTextColor(hasData, lastPhonePercentage)

                    // Update warning message
                    let warnings = [];
                    if (lastYawnPercentage > 10) warnings.push("Yawning");
                    if (lastHeadPosePercentage > 10) warnings.push("Looking Away");
                    if (lastEyeClosurePercentage > 10) warnings.push("Eyes Closed");
                    if (lastPhonePercentage > 10) warnings.push("Phone Usage");
                    if (warnings.length > 0) {
                        warningTextValue = "WARNING: " + warnings.join(" and ") + " Detected!"
                        warningVisible = true
                    } else {
                        warningTextValue = ""
                        warningVisible = false
                    }
                } else {
                    hasData = false
                    isProcessorReady = false
                    yawnTextValue = "Yawn n.a."
                    headPoseTextValue = "Head Pose n.a."
                    eyeClosureTextValue = "Eye Closure n.a."
                    phoneTextValue = "Phone n.a."
                    yawnTextColor = textColor
                    headPoseTextColor = textColor
                    eyeClosureTextColor = textColor
                    phoneTextColor = textColor
                    warningTextValue = ""
                    warningVisible = false
                    console.log("Invalid data received from mlProcessor")
                }
            } else {
                hasData = false
                isProcessorReady = false
                yawnTextValue = "Yawn n.a."
                headPoseTextValue = "Head Pose n.a."
                eyeClosureTextValue = "Eye Closure n.a."
                phoneTextValue = "Phone n.a."
                yawnTextColor = textColor
                headPoseTextColor = textColor
                eyeClosureTextColor = textColor
                phoneTextColor = textColor
                warningTextValue = ""
                warningVisible = false
                console.log("mlProcessor is null")
            }
        }
    }
}
