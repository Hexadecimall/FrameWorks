import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FrameWorks
import FrameWorks.Native
import "components"

ApplicationWindow {
    id: window
    width: 1440
    height: 920
    minimumWidth: 920
    minimumHeight: 640
    visible: true
    title: "FrameWorks"
    color: Theme.background
    property bool desktopWindow: Qt.platform.os !== "wasm"
    property bool macWindow: Qt.platform.os === "osx"
    property bool workspaceOpen: false
    property bool customMaximized: false
    property real restoredX: 0
    property real restoredY: 0
    property real restoredWidth: 1440
    property real restoredHeight: 920
    readonly property bool isMaximized: macWindow ? customMaximized : visibility === Window.Maximized
    flags: desktopWindow ? Qt.Window | Qt.FramelessWindowHint : Qt.Window

    function toggleMaximized() {
        if (!desktopWindow) return
        if (!macWindow) {
            if (visibility === Window.Maximized) showNormal()
            else showMaximized()
            return
        }
        if (customMaximized) {
            x = restoredX
            y = restoredY
            width = restoredWidth
            height = restoredHeight
            customMaximized = false
        } else {
            restoredX = x
            restoredY = y
            restoredWidth = width
            restoredHeight = height
            const area = screen.availableGeometry
            x = area.x
            y = area.y
            width = area.width
            height = area.height
            customMaximized = true
        }
    }

    DocumentController { id: document }
    property string activeTool: "Move"
    property var tools: [
        ["↖", "Move"], ["◇", "Node"], ["⌁", "Pen"], ["✎", "Brush"],
        ["□", "Rectangle"], ["○", "Ellipse"], ["T", "Text"], ["H", "Hand"], ["Z", "Zoom"]
    ]

    header: Rectangle {
        id: titleBar
        height: 48
        color: Theme.chrome
        border.color: Theme.border

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onPressed: if (window.desktopWindow) window.startSystemMove()
            onDoubleClicked: {
                if (!window.desktopWindow) return
                window.toggleMaximized()
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 10
            WindowControls {
                visible: window.desktopWindow && window.macWindow
                Layout.preferredWidth: visible ? implicitWidth : 0
                targetWindow: window
                macStyle: true
            }
            RowLayout {
                spacing: 0
                Rectangle {
                    Layout.preferredWidth: 28; Layout.preferredHeight: 28
                    radius: 8; color: Theme.accent
                    Text { anchors.centerIn: parent; text: "FW"; color: "white"; font.pixelSize: 10; font.bold: true }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: window.workspaceOpen = false
                    }
                }
                Item { Layout.preferredWidth: 14 }
                Row {
                    spacing: 0
                    Text { text: "Frame"; color: Theme.text; font.pixelSize: 16; font.bold: true }
                    Text { text: "Works"; color: Theme.accent; font.pixelSize: 16; font.bold: true }
                }
                Item { Layout.preferredWidth: 10 }
                Row {
                    visible: window.workspaceOpen
                    spacing: 4
                    Repeater {
                        model: ["File", "Edit", "Layer", "Select", "View"]
                        StudioButton { text: modelData }
                    }
                }
            }
            Item { Layout.fillWidth: true }
            Rectangle { visible: window.workspaceOpen; width: 6; height: 6; radius: 3; color: "#68d391" }
            Text { visible: window.workspaceOpen; text: document.starterPoster ? "Orbital Poster" : "Untitled"; color: Theme.text; font.pixelSize: 13 }
            Text { visible: window.workspaceOpen; text: "1200 × 1500"; color: Theme.muted; font.pixelSize: 12 }
            Item { Layout.fillWidth: true }
            StudioButton { visible: window.workspaceOpen; text: "↶"; enabled: document.canUndo; onClicked: document.undo() }
            StudioButton { visible: window.workspaceOpen; text: "↷"; enabled: document.canRedo; onClicked: document.redo() }
            StudioButton { visible: window.workspaceOpen; text: "Export"; active: true }
            WindowControls {
                visible: window.desktopWindow && !window.macWindow
                Layout.preferredWidth: visible ? implicitWidth : 0
                Layout.fillHeight: true
                targetWindow: window
                macStyle: false
            }
        }
        ResizeHandle { edges: Qt.TopEdge; anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right; height: 5 }
        ResizeHandle { edges: Qt.LeftEdge | Qt.TopEdge; anchors.left: parent.left; anchors.top: parent.top; width: 9; height: 9 }
        ResizeHandle { edges: Qt.RightEdge | Qt.TopEdge; anchors.right: parent.right; anchors.top: parent.top; width: 9; height: 9 }
    }

    component ResizeHandle: MouseArea {
        required property int edges
        visible: window.desktopWindow && !window.isMaximized
        z: 1000
        cursorShape: edges === (Qt.LeftEdge | Qt.TopEdge) || edges === (Qt.RightEdge | Qt.BottomEdge)
                     ? Qt.SizeFDiagCursor
                     : edges === (Qt.RightEdge | Qt.TopEdge) || edges === (Qt.LeftEdge | Qt.BottomEdge)
                       ? Qt.SizeBDiagCursor
                       : edges === Qt.LeftEdge || edges === Qt.RightEdge
                         ? Qt.SizeHorCursor : Qt.SizeVerCursor
        onPressed: window.startSystemResize(edges)
    }

    ResizeHandle { edges: Qt.LeftEdge; anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom; width: 5 }
    ResizeHandle { edges: Qt.RightEdge; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom; width: 5 }
    ResizeHandle { edges: Qt.BottomEdge; anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right; height: 5 }
    ResizeHandle { edges: Qt.LeftEdge | Qt.BottomEdge; anchors.left: parent.left; anchors.bottom: parent.bottom; width: 9; height: 9 }
    ResizeHandle { edges: Qt.RightEdge | Qt.BottomEdge; anchors.right: parent.right; anchors.bottom: parent.bottom; width: 9; height: 9 }

    ColumnLayout {
        anchors.fill: parent
        visible: window.workspaceOpen
        spacing: 0

        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 44
            color: Theme.chrome; border.color: Theme.border
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 12; spacing: 8
                Text { text: "↖"; color: Theme.accent; font.pixelSize: 19 }
                Text { text: activeTool; color: Theme.text; font.pixelSize: 13; font.bold: true }
                Item { Layout.preferredWidth: 8 }
                Repeater {
                    model: [["X", document.selectionX], ["Y", document.selectionY],
                            ["W", document.selectionWidth], ["H", document.selectionHeight]]
                    RowLayout {
                        spacing: 5
                        Text { text: modelData[0]; color: Theme.muted; font.pixelSize: 12 }
                        TextField {
                            implicitWidth: 68; implicitHeight: 30
                            text: Math.round(modelData[1]).toString()
                            color: Theme.text; font.pixelSize: 12
                            background: Rectangle { color: "#151416"; radius: 5; border.color: Theme.border }
                            onEditingFinished: {
                                const value = Number(text)
                                if (modelData[0] === "X") document.selectionX = value
                                else if (modelData[0] === "Y") document.selectionY = value
                                else if (modelData[0] === "W") document.selectionWidth = value
                                else document.selectionHeight = value
                            }
                        }
                    }
                }
                Item { Layout.fillWidth: true }
            }
        }

        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0

            Rectangle {
                Layout.preferredWidth: 58; Layout.fillHeight: true
                color: "#1c1b1e"; border.color: Theme.border
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 7; spacing: 4
                    Repeater {
                        model: window.tools
                        StudioButton {
                            Layout.preferredWidth: 42; Layout.preferredHeight: 42
                            text: modelData[0]; active: activeTool === modelData[1]
                            ToolTip.visible: hovered; ToolTip.text: modelData[1]
                            onClicked: activeTool = modelData[1]
                        }
                    }
                    Item { Layout.fillHeight: true }
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 28; height: 28; radius: 7
                        color: document.selectionFill; border.color: "white"; border.width: 2
                    }
                }
            }

            Rectangle {
                id: stage
                Layout.fillWidth: true; Layout.fillHeight: true
                color: "#27252a"
                clip: true
                Canvas {
                    anchors.fill: parent
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        ctx.fillStyle = "#3a373d"
                        for (let y = 12; y < height; y += 16)
                            for (let x = 12; x < width; x += 16) ctx.fillRect(x, y, 1, 1)
                    }
                }
                Rectangle {
                    id: artboard
                    property real zoomFactor: 1.0
                    readonly property real fittedWidth: Math.min(stage.width * 0.62, stage.height * 0.66 * 0.8)
                    width: fittedWidth * zoomFactor
                    height: width * 1.25
                    anchors.centerIn: parent
                    color: "white"
                    layer.enabled: true
                    layer.samples: 4

                    ArtworkCanvas { anchors.fill: parent; document: document }
                    Text {
                        x: parent.width * 0.075; y: parent.height * 0.07
                        visible: document.starterPoster
                        text: "OBJECTS IN MOTION · 2026"; color: "#181619"
                        font.pixelSize: Math.max(9, parent.width * 0.022); font.bold: true; font.letterSpacing: 4
                    }
                    Text {
                        x: parent.width * 0.07; y: parent.height * 0.69
                        visible: document.starterPoster
                        text: "FORM"; color: "#181619"
                        font.pixelSize: parent.width * 0.14; font.weight: Font.Black
                    }
                    Text {
                        x: parent.width * 0.07; y: parent.height * 0.79
                        visible: document.starterPoster
                        text: "FOLLOWS"; color: "transparent"
                        style: Text.Outline; styleColor: "#181619"
                        font.pixelSize: parent.width * 0.13; font.weight: Font.Black
                    }
                    MouseArea {
                        anchors.fill: parent
                        onPressed: document.selectLayer(document.starterPoster ? 3 : 0)
                    }
                }
                Rectangle {
                    anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 14
                    width: 112; height: 36; radius: 8; color: "#1b1a1d"; border.color: Theme.border
                    RowLayout {
                        anchors.fill: parent
                        StudioButton { text: "−"; onClicked: artboard.zoomFactor = Math.max(0.5, artboard.zoomFactor - 0.1) }
                        Text { text: Math.round(artboard.width / 12) + "%"; color: Theme.text; font.pixelSize: 12 }
                        StudioButton { text: "+"; onClicked: artboard.zoomFactor = Math.min(2.0, artboard.zoomFactor + 0.1) }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 300; Layout.fillHeight: true
                color: "#1d1c1f"; border.color: Theme.border
                ColumnLayout {
                    anchors.fill: parent; spacing: 0
                    Item {
                        Layout.fillWidth: true; Layout.preferredHeight: 44
                        Row {
                            anchors.centerIn: parent
                            spacing: 4
                            Repeater {
                                model: ["Design", "Assets", "History"]
                                StudioButton {
                                    width: 84
                                    height: 36
                                    text: modelData
                                    active: index === 0
                                }
                            }
                        }
                    }
                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }
                    ColumnLayout {
                        Layout.fillWidth: true; Layout.margins: 12; spacing: 10
                        Text { text: "Appearance"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                        RowLayout {
                            Text { text: "Fill"; color: Theme.muted; Layout.preferredWidth: 62 }
                            Rectangle { width: 24; height: 24; radius: 5; color: document.selectionFill; border.color: Theme.border }
                            TextField {
                                Layout.fillWidth: true; text: document.selectionFill.toString()
                                color: Theme.text; font.pixelSize: 12
                                background: Rectangle { color: "#171619"; radius: 5; border.color: Theme.border }
                                onEditingFinished: document.selectionFill = text
                            }
                        }
                        RowLayout {
                            Text { text: "Opacity"; color: Theme.muted; Layout.preferredWidth: 62 }
                            Slider { Layout.fillWidth: true; from: 0; to: 1; value: document.selectionOpacity; onMoved: document.selectionOpacity = value }
                            Text { text: Math.round(document.selectionOpacity * 100) + "%"; color: Theme.text; font.pixelSize: 12 }
                        }
                    }
                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }
                    ColumnLayout {
                        Layout.fillWidth: true; Layout.margins: 12; spacing: 9
                        Text { text: "Transform"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                        RowLayout {
                            StudioButton { Layout.fillWidth: true; text: "Duplicate"; onClicked: document.duplicateSelected() }
                            StudioButton { Layout.fillWidth: true; text: "Delete"; onClicked: document.removeSelected() }
                        }
                    }
                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }
                    Text { Layout.margins: 12; text: "Layers"; color: Theme.text; font.pixelSize: 13; font.bold: true }
                    ListView {
                        id: layerList
                        Layout.fillWidth: true; Layout.fillHeight: true
                        Layout.leftMargin: 8; Layout.rightMargin: 8
                        model: document
                        clip: true
                        delegate: Rectangle {
                            required property int index
                            required property string layerName
                            required property string layerType
                            required property bool layerVisible
                            required property bool layerLocked
                            required property bool layerSelected
                            width: layerList.width; height: 54; radius: 7
                            color: layerSelected ? "#343138" : layerMouse.containsMouse ? "#29272c" : "transparent"
                            Rectangle { width: 2; height: parent.height - 12; anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; color: layerSelected ? Theme.accent : "transparent" }
                            RowLayout {
                                z: 1
                                anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8; spacing: 9
                                StudioButton { text: layerVisible ? "●" : "○"; onClicked: document.toggleVisibility(index) }
                                Rectangle {
                                    width: 32; height: 30; radius: 5; color: "#252328"; border.color: Theme.border
                                    Text { anchors.centerIn: parent; text: layerType === "Text" ? "T" : layerType.startsWith("Ellipse") ? "○" : layerType === "Curve" ? "⌁" : "□"; color: Theme.text }
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true; spacing: 1
                                    Text { text: layerName; color: Theme.text; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                    Text { text: layerType; color: Theme.muted; font.pixelSize: 10 }
                                }
                                Text { text: layerLocked ? "⌑" : "›"; color: Theme.muted }
                            }
                            MouseArea { id: layerMouse; anchors.fill: parent; hoverEnabled: true; onClicked: document.selectLayer(index) }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 30
            color: Theme.chrome; border.color: Theme.border
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                Text { text: activeTool + " tool"; color: Theme.muted; font.pixelSize: 11 }
                Item { Layout.fillWidth: true }
                Text { text: "RGB/8 · sRGB IEC61966-2.1"; color: Theme.muted; font.pixelSize: 11 }
                Item { Layout.fillWidth: true }
                Text { text: document.selectedIndex >= 0 ? "Layer selected" : "Nothing selected"; color: Theme.muted; font.pixelSize: 11 }
            }
        }
    }

    HomePage {
        anchors.fill: parent
        visible: !window.workspaceOpen
        onCreateDocument: {
            document.newDocument()
            window.workspaceOpen = true
        }
        onOpenRecent: {
            document.openStarterDocument()
            window.workspaceOpen = true
        }
    }

    Shortcut { enabled: window.workspaceOpen; sequences: [StandardKey.Undo]; onActivated: document.undo() }
    Shortcut { enabled: window.workspaceOpen; sequences: [StandardKey.Redo]; onActivated: document.redo() }
    Shortcut { enabled: window.workspaceOpen; sequence: Qt.platform.os === "osx" ? "Meta+D" : "Ctrl+D"; onActivated: document.duplicateSelected() }
    Shortcut { enabled: window.workspaceOpen; sequence: "Delete"; onActivated: document.removeSelected() }
}
