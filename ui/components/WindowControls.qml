import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var targetWindow
    property bool macStyle: false
    implicitWidth: macStyle ? 66 : 138
    implicitHeight: 48

    component TrafficLight: Rectangle {
        required property color controlColor
        required property string symbol
        signal activated()
        width: 13
        height: 13
        radius: 7
        color: controlColor
        border.color: Qt.darker(controlColor, 1.18)
        Text {
            anchors.centerIn: parent
            text: parent.symbol
            color: "#4a2725"
            font.pixelSize: 9
            font.bold: true
            opacity: trafficMouse.containsMouse ? 0.8 : 0
        }
        MouseArea {
            id: trafficMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: parent.activated()
        }
    }

    component CaptionButton: Rectangle {
        required property string symbol
        property bool destructive: false
        signal activated()
        width: 46
        height: root.height
        color: captionMouse.containsMouse ? (destructive ? "#c42b1c" : "#353337") : "transparent"
        Text {
            anchors.centerIn: parent
            text: parent.symbol
            color: "#dedbe2"
            font.pixelSize: 15
        }
        MouseArea {
            id: captionMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: parent.activated()
        }
    }

    Row {
        visible: root.macStyle
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 9
        TrafficLight { controlColor: "#ff5f57"; symbol: "×"; onActivated: root.targetWindow.close() }
        TrafficLight { controlColor: "#febc2e"; symbol: "−"; onActivated: root.targetWindow.showMinimized() }
        TrafficLight {
            controlColor: "#28c840"
            symbol: "+"
            onActivated: root.targetWindow.toggleMaximized()
        }
    }

    Row {
        visible: !root.macStyle
        anchors.fill: parent
        CaptionButton { symbol: "−"; onActivated: root.targetWindow.showMinimized() }
        CaptionButton {
            symbol: root.targetWindow.isMaximized ? "❐" : "□"
            onActivated: root.targetWindow.toggleMaximized()
        }
        CaptionButton { symbol: "×"; destructive: true; onActivated: root.targetWindow.close() }
    }
}
