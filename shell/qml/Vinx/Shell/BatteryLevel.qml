import QtQuick
import Vinx.Theme

// Batterie dessinée (proportions du SVG branding/icons/battery.svg, tracé 24×24) :
// le niveau intérieur se remplit et se vide avec le pourcentage, sans changer d'icône.
// En charge : le pictogramme éclair est affiché à côté par l'appelant.
Item {
    id: root

    property color color: Theme.text
    property real percentage: 0
    implicitWidth: 16
    implicitHeight: 16

    // Espace de dessin 24×24 mis à l'échelle de la taille finale.
    Item {
        width: 24
        height: 24
        transform: Scale { xScale: root.width / 24; yScale: root.height / 24 }

        Rectangle {
            x: 2.5; y: 7
            width: 17; height: 10
            radius: 2.5
            color: "transparent"
            border.width: 1.8
            border.color: root.color
        }

        Rectangle {
            readonly property real pct: Math.max(0, Math.min(100, root.percentage))
            readonly property real full: 13.4
            x: 4.3; y: 8.9
            height: 6.2
            radius: 1
            width: full * pct / 100
            Behavior on width { NumberAnimation { duration: Theme.durFast } }
        }

        Rectangle {
            x: 21.1; y: 10.5
            width: 1.8; height: 3
            radius: 0.9
            color: root.color
        }
    }
}
