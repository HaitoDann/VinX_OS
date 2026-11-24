import QtQuick
import Vinx.Theme
import Vinx.Controls
import Vinx.System

// Icônes du bureau (ADR 0048) : contenu du dossier « Bureau », en colonnes depuis le coin haut droit.
// Double-clic : ouvrir ; les autres actions (renommer, supprimer) se font dans Fichiers.
Item {
    id: root
    property var entries: []
    property string selected: ""
    readonly property int cell: 92
    function refresh() { const e = Session.desktopEntries(); if (JSON.stringify(e) !== JSON.stringify(entries)) entries = e }
    Component.onCompleted: refresh()
    Timer { interval: 2500; running: root.visible; repeat: true; onTriggered: root.refresh() }

    // Couleur et pictogramme selon le type (mêmes teintes que le thème d'icônes).
    function style(e) {
        if (e.isDir) return { glyph: "folder", tint: "#2F6FD0" }
        const t = { odt: ["doc", "#2F6FD0"], doc: ["doc", "#2F6FD0"], docx: ["doc", "#2F6FD0"], txt: ["doc", "#8A8983"],
                    ods: ["table", "#2B8A3E"], xls: ["table", "#2B8A3E"], xlsx: ["table", "#2B8A3E"], csv: ["table", "#2B8A3E"],
                    odp: ["image", "#E8590C"], ppt: ["image", "#E8590C"], pptx: ["image", "#E8590C"], pdf: ["doc", "#E5484D"],
                    png: ["image", "#C2255C"], jpg: ["image", "#C2255C"], jpeg: ["image", "#C2255C"], webp: ["image", "#C2255C"],
                    zip: ["archive", "#D4A13C"], gz: ["archive", "#D4A13C"], xz: ["archive", "#D4A13C"] }[e.ext]
        return t ? { glyph: t[0], tint: t[1] } : { glyph: "doc", tint: "#8A8983" }
    }

    Grid {
        anchors { top: parent.top; right: parent.right; topMargin: 40 + Theme.space4; rightMargin: Theme.space4 }
        flow: Grid.TopToBottom
        layoutDirection: Qt.RightToLeft
        rows: Math.max(1, Math.floor((root.height - 40 - 140) / root.cell))
        rowSpacing: 4; columnSpacing: 4
        Repeater {
            model: root.entries
            Rectangle {
                id: tile
                required property var modelData
                width: root.cell - 4; height: root.cell - 4
                radius: Theme.radiusMd
                color: root.selected === modelData.path ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.28)
                     : mouse.containsMouse ? Qt.rgba(1, 1, 1, Theme.dark ? 0.08 : 0.45) : "transparent"
                AppIcon {
                    id: ico
                    anchors { horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 8 }
                    size: 44
                    iconName: root.style(tile.modelData).glyph
                    tint: root.style(tile.modelData).tint
                }
                Text {
                    anchors { top: ico.bottom; topMargin: 4; left: parent.left; right: parent.right; margins: 4 }
                    text: tile.modelData.name
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap; maximumLineCount: 2; elide: Text.ElideRight
                    font.family: Theme.fontFamily; font.pixelSize: Theme.fsXs
                    color: Theme.text
                }
                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.selected = tile.modelData.path
                    onDoubleClicked: { root.selected = ""; Session.launch("xdg-open", [tile.modelData.path]) }
                }
                Accessible.role: Accessible.Button
                Accessible.name: tile.modelData.name
            }
        }
    }
}
