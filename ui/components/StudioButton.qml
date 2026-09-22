import QtQuick
import QtQuick.Controls
import FrameWorks

Button {
    id: control
    property bool active: false
    implicitHeight: 32
    implicitWidth: Math.max(38, contentItem.implicitWidth + 18)
    hoverEnabled: true

    contentItem: Text {
        text: control.text
        color: control.active || control.hovered ? Theme.text : "#c8c4ca"
        font.pixelSize: 13
        font.weight: control.active ? Font.DemiBold : Font.Normal
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: Theme.radius
        color: control.active ? "#353238" : control.hovered ? "#302e33" : "transparent"
        border.color: control.active ? "#47434b" : "transparent"
    }
}
