import QtQuick
import Vinx.Theme
import Vinx.Controls
import Vinx.Shell
import Vinx.Viewer

// Lecteur PDF de la Visionneuse VinX (ADR 0052) : miniatures à gauche, pages en défilement continu,
// zoom, recherche. Formulaires, annotations et impression : « Ouvrir dans Firefox ».
Window {
    id: win
    width: 1000; height: 760
    minimumWidth: 560; minimumHeight: 400
    visible: true
    title: Pdf.ready ? Pdf.name : qsTr("Lecteur PDF")
    color: Theme.surfaceWindow
    AppearanceSync {}

    property string startMode: ""        // options communes avec le visionneur (ignorées ici)
    property bool infoOpen: false
    property real zoom: 1.0              // 1 = page à la largeur de lecture confortable
    property int current: 0
    property string notice: ""
    readonly property real baseWidth: Math.min(pages.width - 48, 820)
    function goTo(i) { if (i >= 0 && i < Pdf.pageCount) { pages.positionViewAtIndex(i, ListView.Beginning); current = i } }
    function zoomBy(f) { zoom = Math.max(0.4, Math.min(4, zoom * f)) }
    function search(from) {
        const t = searchInput.text
        if (t.trim() === "") return
        const p = Pdf.find(t, from)
        if (p < 0) notice = qsTr("« %1 » introuvable dans ce document.").arg(t)
        else { notice = qsTr("« %1 » : page %2").arg(t).arg(p + 1); goTo(p) }
    }

    Shortcut { sequences: ["+", "Ctrl++", "Ctrl+="]; onActivated: win.zoomBy(1.2) }
    Shortcut { sequences: ["-", "Ctrl+-"]; onActivated: win.zoomBy(1 / 1.2) }
    Shortcut { sequences: ["0", "Ctrl+0"]; onActivated: win.zoom = 1 }
    Shortcut { sequences: ["PgDown", "Right"]; onActivated: win.goTo(win.current + 1) }
    Shortcut { sequences: ["PgUp", "Left"]; onActivated: win.goTo(win.current - 1) }
    Shortcut { sequence: "Home"; onActivated: win.goTo(0) }
    Shortcut { sequence: "End"; onActivated: win.goTo(Pdf.pageCount - 1) }
    Shortcut { sequences: ["Ctrl+F", "/"]; onActivated: searchInput.forceActiveFocus() }
    Shortcut { sequence: "F3"; onActivated: win.search(win.current + 1) }

    // ---------- Barre d'outils ----------
    Rectangle {
        id: bar
        anchors { left: parent.left; right: parent.right; top: parent.top }
        height: 52
        color: Theme.surfaceWindow
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }
        Row {
            anchors { left: parent.left; leftMargin: Theme.space4; verticalCenter: parent.verticalCenter }
            spacing: Theme.space3
            Label {
                objectName: "pageLabel"
                anchors.verticalCenter: parent.verticalCenter
                text: Pdf.ready ? qsTr("Page %1 / %2").arg(win.current + 1).arg(Pdf.pageCount) : ""
                font.weight: Font.DemiBold
            }
        }
        Row {
            anchors.centerIn: parent
            spacing: Theme.space1
            IconButton { flat: true; iconName: "minus"; toolTip: qsTr("Réduire (−)"); onClicked: win.zoomBy(1 / 1.2); anchors.verticalCenter: parent.verticalCenter }
            Label {
                width: 56; horizontalAlignment: Text.AlignHCenter; anchors.verticalCenter: parent.verticalCenter
                text: Math.round(win.zoom * 100) + " %"
                MouseArea { anchors.fill: parent; onClicked: win.zoom = 1 }
            }
            IconButton { flat: true; iconName: "plus"; toolTip: qsTr("Agrandir (+)"); onClicked: win.zoomBy(1.2); anchors.verticalCenter: parent.verticalCenter }
        }
        Row {
            anchors { right: parent.right; rightMargin: Theme.space4; verticalCenter: parent.verticalCenter }
            spacing: Theme.space2
            Rectangle {
                width: 220; height: 32; radius: Theme.radiusSm
                color: Theme.surfaceGlassStrong
                border.width: searchInput.activeFocus ? 2 : 1
                border.color: searchInput.activeFocus ? Theme.accent : Theme.border
                anchors.verticalCenter: parent.verticalCenter
                Icon { id: si; name: "search"; width: 14; height: 14; color: Theme.textTertiary; anchors { left: parent.left; leftMargin: Theme.space2; verticalCenter: parent.verticalCenter } }
                TextInput {
                    id: searchInput
                    anchors { left: si.right; leftMargin: Theme.space2; right: parent.right; rightMargin: Theme.space2; verticalCenter: parent.verticalCenter }
                    font.family: Theme.fontFamily; font.pixelSize: Theme.fsSm
                    color: Theme.text; clip: true; selectByMouse: true
                    Accessible.name: qsTr("Rechercher dans le document")
                    onAccepted: win.search(win.current)
                }
                Label { visible: searchInput.text === ""; text: qsTr("Rechercher"); level: Label.Tertiary; font.pixelSize: Theme.fsSm; anchors { left: searchInput.left; verticalCenter: parent.verticalCenter } }
            }
            Button { text: qsTr("Ouvrir dans Firefox"); onClicked: Pdf.openInBrowser(); anchors.verticalCenter: parent.verticalCenter }
        }
    }

    // ---------- Miniatures ----------
    Rectangle {
        id: side
        anchors { left: parent.left; top: bar.bottom; bottom: parent.bottom }
        width: 150
        color: Theme.surfaceSunken
        Rectangle { anchors.right: parent.right; height: parent.height; width: 1; color: Theme.border }
        ListView {
            id: thumbs
            anchors { fill: parent; margins: Theme.space3 }
            clip: true
            spacing: Theme.space3
            model: Pdf.ready ? Pdf.pageCount : 0
            currentIndex: win.current
            onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Contain)
            delegate: Column {
                required property int index
                width: ListView.view.width
                spacing: 4
                Rectangle {
                    readonly property size pt: Pdf.pageSize(parent.index)
                    width: parent.width - 16; anchors.horizontalCenter: parent.horizontalCenter
                    height: pt.width > 0 ? width * pt.height / pt.width : width * 1.41
                    color: "white"; radius: 3
                    border.width: win.current === parent.index ? 2 : 1
                    border.color: win.current === parent.index ? Theme.accent : Theme.border
                    Image {
                        anchors { fill: parent; margins: 2 }
                        source: "image://pdfpage/" + parent.parent.index
                        sourceSize.width: width * 2
                        asynchronous: true; cache: false
                    }
                    MouseArea { anchors.fill: parent; onClicked: win.goTo(parent.parent.index) }
                }
                Label { text: parent.index + 1; anchors.horizontalCenter: parent.horizontalCenter; level: Label.Tertiary; font.pixelSize: Theme.fsXs }
            }
        }
    }

    // ---------- Pages ----------
    ListView {
        id: pages
        anchors { left: side.right; right: parent.right; top: bar.bottom; bottom: parent.bottom }
        clip: true
        spacing: Theme.space4
        topMargin: Theme.space4; bottomMargin: Theme.space4
        model: Pdf.ready ? Pdf.pageCount : 0
        contentWidth: Math.max(width, win.baseWidth * win.zoom + 48)
        flickableDirection: Flickable.AutoFlickIfNeeded
        boundsBehavior: Flickable.StopAtBounds
        onContentYChanged: { const i = indexAt(contentX + width / 2, contentY + height / 3); if (i >= 0) win.current = i }
        delegate: Item {
            required property int index
            readonly property size pt: Pdf.pageSize(index)
            width: pages.contentWidth
            height: page.height
            Rectangle {
                id: page
                width: win.baseWidth * win.zoom
                height: parent.pt.width > 0 ? width * parent.pt.height / parent.pt.width : width * 1.41
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                border.width: 1; border.color: Theme.border
                Image {
                    anchors.fill: parent
                    source: "image://pdfpage/" + parent.parent.index
                    // Rendu net à l'écran : largeur réelle × densité de l'écran (zoom compris).
                    sourceSize.width: Math.round(page.width * Screen.devicePixelRatio)
                    asynchronous: true; cache: false
                }
            }
        }
    }

    // Erreur d'ouverture
    Column {
        anchors.centerIn: pages
        visible: !Pdf.ready
        spacing: Theme.space2
        Icon { name: "doc"; width: 40; height: 40; color: Theme.textTertiary; anchors.horizontalCenter: parent.horizontalCenter }
        Label { text: Pdf.error !== "" ? Pdf.error : qsTr("Aucun document."); level: Label.Secondary; anchors.horizontalCenter: parent.horizontalCenter }
    }

    // Bulle (recherche)
    Rectangle {
        visible: win.notice !== ""
        anchors { horizontalCenter: pages.horizontalCenter; bottom: parent.bottom; bottomMargin: Theme.space4 }
        width: noticeLabel.implicitWidth + Theme.space4 * 2; height: 36; radius: 18
        color: Theme.surfaceGlassStrong; border.width: 1; border.color: Theme.border
        Label { id: noticeLabel; anchors.centerIn: parent; text: win.notice }
        Timer { running: parent.visible; interval: 3500; onTriggered: win.notice = "" }
    }
}
