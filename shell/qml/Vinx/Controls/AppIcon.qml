import QtQuick
import Vinx.Theme
import Vinx.Controls  // types C++ du module (Icon) — import explicite requis avant Qt 6.5

// Icône d'application VinX (ADR 0018) : tuile neutre arrondie, pictogramme à l'encre au trait,
// un point de couleur qui identifie l'application. Même dessin que le thème d'icônes « VinX ».
Item {
    id: root
    property string iconName
    property color tint: "#8A8983"     // couleur du point
    property real size: Theme.dockIcon
    // Variante pleine (dock, ADR 0047) : la tuile prend la couleur, sans bordure ni point.
    property bool filled: false

    // Pictogramme posé sur une tuile pleine : encre claire sur couleur foncée, encre foncée
    // sinon (le vert clair de l'identité s'écrit en encre foncée, ADR 0018).
    readonly property color tileGlyph: {
        const c = root.tint
        const l = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b
        return l > 0.5 ? "#141414" : "#F8F7F4"
    }

    implicitWidth: size; implicitHeight: size

    Rectangle {
        anchors.fill: parent
        radius: width * 0.24
        color: root.filled ? root.tint : (Theme.dark ? "#2A2D2B" : "#E3E1DB")
        border.width: root.filled ? 0 : 1
        border.color: Theme.border
    }
    Icon {
        anchors.centerIn: parent
        width: parent.width * 0.5; height: width
        name: root.iconName
        color: root.filled ? root.tileGlyph : Theme.text
    }
    Rectangle {
        visible: !root.filled
        width: root.width * 0.15; height: width; radius: width / 2
        x: root.width * 0.80 - width / 2; y: root.height * 0.20 - height / 2
        color: root.tint
        border.width: Theme.dark && Qt.colorEqual(root.tint, "#141414") ? 1 : 0
        border.color: Theme.text
    }
}
