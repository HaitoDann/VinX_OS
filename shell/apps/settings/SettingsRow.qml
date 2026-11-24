import QtQuick
import Vinx.Theme
import Vinx.Controls

// Ligne de réglage : une carte arrondie, libellé (et description) à gauche, contrôle à droite.
// Les lignes successives se suivent avec un petit écart (maquette 4 validée).
Item {
    id: row
    property string title
    property string description
    default property alias control: slot.data
    width: parent ? parent.width : 400
    height: Math.max(52, texts.implicitHeight + Theme.space3 * 2) + 6

    Rectangle {
        id: card
        anchors { left: parent.left; right: parent.right; top: parent.top; bottom: parent.bottom; bottomMargin: 6 }
        radius: Theme.radiusMd
        color: Theme.surfaceGlassStrong
        border.width: 1; border.color: Theme.border
    }
    Column {
        id: texts
        anchors { left: card.left; leftMargin: Theme.space4; verticalCenter: card.verticalCenter; right: slot.left; rightMargin: Theme.space4 }
        Label { text: row.title; width: parent.width }
        Label { visible: row.description !== ""; text: row.description; level: Label.Secondary; font.pixelSize: Theme.fsSm; width: parent.width; wrapMode: Text.WordWrap }
    }
    Item {
        id: slot
        anchors { right: card.right; rightMargin: Theme.space4; verticalCenter: card.verticalCenter }
        width: childrenRect.width; height: childrenRect.height
    }
}
