import QtQuick
import Vinx.Theme
import Vinx.Controls  // types C++ du module (Icon) — import explicite requis avant Qt 6.5

// Icône d'application VinX (ADR 0046, remplace le style « tuile neutre + point » de l'ADR 0018) :
// tuile pleine, sans bordure, dégradé doux dans la couleur de l'application, pictogramme blanc
// (ou à l'encre sur les couleurs claires). Fichiers, Paramètres et Store ont un dessin propre (art).
// Même dessin que le thème d'icônes « VinX » (branding/icon-theme/gen_icons.py).
Item {
    id: root
    property string iconName
    property color tint: "#8A8983"     // couleur de la tuile
    property string art: ""            // « files », « settings », « store » : dessin complet
    property real size: Theme.dockIcon

    // Encre claire sauf sur les couleurs vives et claires (vert du logo, jaune…).
    readonly property bool lightTile: (0.299 * tint.r + 0.587 * tint.g + 0.114 * tint.b) > 0.62
    implicitWidth: size; implicitHeight: size

    Rectangle {
        anchors { fill: parent; margins: root.width * 0.03 }
        visible: root.art === ""
        radius: width * 0.25
        gradient: Gradient {
            GradientStop { position: 0; color: Qt.lighter(root.tint, root.lightTile ? 1.12 : 1.35) }
            GradientStop { position: 1; color: root.tint }
        }
    }
    Icon {
        anchors.centerIn: parent
        visible: root.art === ""
        width: parent.width * 0.52; height: width
        name: root.iconName
        // Terminal (tuile presque noire) : pictogramme vert du logo.
        color: root.lightTile ? "#06130D" : (root.tint.hslLightness < 0.12 ? "#10DD8C" : "#FFFFFF")
    }
    Image {
        anchors.fill: parent
        visible: root.art !== ""
        source: root.art !== "" ? "qrc:/qt/qml/Vinx/Controls/appicons/" + root.art + ".svg" : ""
        sourceSize: Qt.size(root.width * 2, root.height * 2)
        smooth: true
    }
}
