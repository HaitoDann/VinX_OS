import QtQuick
import Vinx.Theme
import Vinx.Controls

// Batterie dont le niveau se remplit selon le pourcentage ; éclair quand elle charge.
Item {
    id: root
    property int percentage: 100
    property bool charging: false
    property color color: Theme.text
    readonly property color levelColor: charging ? Theme.accent
        : percentage <= 10 ? Theme.danger : root.color
    implicitWidth: 22; implicitHeight: 16

    Rectangle {
        id: body
        x: 1; width: 18; height: 11; anchors.verticalCenter: parent.verticalCenter
        radius: 3; color: "transparent"
        border.width: 1.5; border.color: root.percentage <= 10 && !root.charging ? Theme.danger : root.color
        Rectangle {
            x: 2.5; y: 2.5; height: parent.height - 5; radius: 1.5
            width: Math.max(1.5, (parent.width - 5) * Math.max(0, Math.min(100, root.percentage)) / 100)
            color: root.levelColor
            Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
        }
    }
    // Borne.
    Rectangle { anchors { left: body.right; leftMargin: 1; verticalCenter: body.verticalCenter }
                width: 1.5; height: 5; radius: 1; color: body.border.color }
    Icon {
        visible: root.charging
        name: "bolt"; width: 11; height: 11; color: root.color
        anchors.centerIn: body
    }
}
