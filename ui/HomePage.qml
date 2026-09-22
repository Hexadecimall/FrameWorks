import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FrameWorks
import "components"

Item {
    id: home
    signal createDocument()
    signal openRecent()

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: Theme.chrome
            border.color: Theme.border

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 8

                Text {
                    text: "Start"
                    color: Theme.muted
                    font.pixelSize: 11
                    font.bold: true
                    font.letterSpacing: 1.4
                    Layout.bottomMargin: 5
                }
                StudioButton { Layout.fillWidth: true; text: "Home"; active: true }
                StudioButton { Layout.fillWidth: true; text: "New document"; onClicked: home.createDocument() }
                StudioButton { Layout.fillWidth: true; text: "Open document…"; enabled: false }
                Item { Layout.fillHeight: true }
                Text { text: "FrameWorks 0.1"; color: Theme.muted; font.pixelSize: 10 }
            }
        }

        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: content.implicitHeight + 96
            clip: true

            ColumnLayout {
                id: content
                width: Math.min(parent.width - 96, 920)
                x: 48
                y: 44
                spacing: 18

                Text {
                    text: "Create something precise."
                    color: Theme.text
                    font.pixelSize: 30
                    font.weight: Font.DemiBold
                }
                Text {
                    text: "Start clean or continue from a recent design."
                    color: Theme.muted
                    font.pixelSize: 14
                    Layout.bottomMargin: 10
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 14

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 154
                        radius: 12
                        color: "#242227"
                        border.color: Theme.border
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            Text { text: "+"; color: Theme.accent; font.pixelSize: 28 }
                            Text { text: "New document"; color: Theme.text; font.pixelSize: 16; font.bold: true }
                            Text { text: "Blank 1200 × 1500 canvas"; color: Theme.muted; font.pixelSize: 12 }
                            Item { Layout.fillHeight: true }
                            StudioButton { text: "Create"; active: true; onClicked: home.createDocument() }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 154
                        radius: 12
                        color: "#201f22"
                        border.color: Theme.border
                        opacity: 0.58
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            Text { text: "↗"; color: Theme.muted; font.pixelSize: 24 }
                            Text { text: "Open document"; color: Theme.text; font.pixelSize: 16; font.bold: true }
                            Text { text: "Available with the document format"; color: Theme.muted; font.pixelSize: 12 }
                            Item { Layout.fillHeight: true }
                            StudioButton { text: "Coming next"; enabled: false }
                        }
                    }
                }

                Text {
                    text: "Recent"
                    color: Theme.text
                    font.pixelSize: 14
                    font.bold: true
                    Layout.topMargin: 12
                }

                Rectangle {
                    Layout.preferredWidth: 310
                    Layout.preferredHeight: 190
                    radius: 12
                    color: recentMouse.containsMouse ? "#29272c" : "#242227"
                    border.color: recentMouse.containsMouse ? "#4a4650" : Theme.border

                    Rectangle {
                        x: 14
                        y: 14
                        width: 92
                        height: 115
                        radius: 4
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#ff5c35" }
                            GradientStop { position: 1; color: "#ff9457" }
                        }
                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: 34
                            width: 50
                            height: 50
                            radius: 25
                            gradient: Gradient {
                                GradientStop { position: 0; color: "#bffcff" }
                                GradientStop { position: 1; color: "#ba45ff" }
                            }
                        }
                        Text { x: 8; y: 83; text: "FORM"; color: "#181619"; font.pixelSize: 13; font.bold: true }
                    }
                    Column {
                        x: 124
                        y: 28
                        spacing: 5
                        Text { text: "Orbital Poster"; color: Theme.text; font.pixelSize: 15; font.bold: true }
                        Text { text: "1200 × 1500"; color: Theme.muted; font.pixelSize: 11 }
                        Text { text: "Starter document"; color: Theme.muted; font.pixelSize: 11 }
                    }
                    Text { x: 124; y: 145; text: "Open →"; color: Theme.accent; font.pixelSize: 12; font.bold: true }
                    MouseArea {
                        id: recentMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: home.openRecent()
                    }
                }
            }
        }
    }
}
