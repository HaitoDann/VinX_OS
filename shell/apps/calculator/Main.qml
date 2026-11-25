import QtQuick
import Vinx.Theme
import Vinx.Controls
import Vinx.Shell
import "qrc:/qt/qml/Vinx/Shell/Calc.js" as Calc

// Calculatrice VinX (ADR 0052) : grandes touches, dernier calcul au-dessus du résultat, virgule
// française, pavé numérique. Moteur partagé avec la recherche (Calc.js, sans eval).
Window {
    id: win
    width: 340; height: 520
    minimumWidth: 300; minimumHeight: 460
    visible: true
    title: qsTr("Calculatrice")
    color: Theme.surfaceWindow
    AppearanceSync {}

    property string expr: ""            // saisie en cours
    property string last: ""            // dernier calcul, « 12 × 3 = »
    property bool error: false
    property var history: []            // [{ expr, result }], le plus récent d'abord

    // Affichage : opérateurs typographiques et virgule.
    function pretty(s) { return s.replace(/\*/g, " × ").replace(/\//g, " ÷ ").replace(/\+/g, " + ").replace(/(\d|\))-/g, "$1 − ").replace(/\./g, ",") }
    // Pourcentage comme sur une calculatrice : « 200+10% » = 220, « 50*20% » = 10.
    function percent(s) {
        const m = /^(.*[^+\-*/(])([+\-])(\d+(?:[.,]\d+)?)%$/.exec(s)
        if (m) {
            const base = value(m[1])
            if (base !== null) return "(" + m[1] + ")" + m[2] + "(" + base + "*" + m[3] + "/100)"
        }
        return s.replace(/(\d+(?:[.,]\d+)?)%/g, "($1/100)")
    }
    function value(s) {
        if (s.indexOf("%") >= 0) s = percent(s)
        const v = Calc.evaluate(s)
        if (v !== null) return v
        const n = Number(s.replace(/,/g, "."))       // nombre seul
        return s !== "" && isFinite(n) ? n : null
    }
    function press(k) {
        error = false
        if (k === "C") { expr = ""; last = ""; return }
        if (k === "⌫") { expr = expr.slice(0, -1); return }
        if (k === "±") { expr = expr.startsWith("-(") && expr.endsWith(")") ? expr.slice(2, -1) : (expr === "" ? "" : "-(" + expr + ")"); return }
        if (k === "( )") {
            const open = (expr.match(/\(/g) || []).length - (expr.match(/\)/g) || []).length
            expr += open > 0 && /[\d)]$/.test(expr) ? ")" : "("
            return
        }
        if (k === "=") { compute(); return }
        const map = { "×": "*", "÷": "/", "−": "-", ",": "." }
        const c = map[k] || k
        // Après un résultat, un chiffre recommence ; un opérateur continue le calcul.
        if (last !== "" && expr !== "" && /^[0-9.]$/.test(c) && expr === resultText) expr = ""
        if (/[*/+\-%]$/.test(expr) && /^[*/+%]$/.test(c)) expr = expr.slice(0, -1)
        expr += c
    }
    property string resultText: ""
    function compute() {
        if (expr === "") return
        const v = value(expr)
        if (v === null || !isFinite(v)) { error = true; return }
        const r = Calc.format(v)
        last = pretty(expr) + " ="
        history = [{ expr: pretty(expr), result: r }].concat(history).slice(0, 20)
        resultText = String(v)
        expr = resultText
    }
    // Aperçu du résultat pendant la saisie.
    readonly property string preview: { const v = value(expr); return v === null || !isFinite(v) ? "" : Calc.format(v) }

    Item {
        anchors.fill: parent
        focus: true
        Keys.onPressed: (e) => {
            const t = e.text
            if (e.key === Qt.Key_Return || e.key === Qt.Key_Enter || t === "=") win.press("=")
            else if (e.key === Qt.Key_Backspace) win.press("⌫")
            else if (e.key === Qt.Key_Escape || e.key === Qt.Key_Delete) win.press("C")
            else if (/^[0-9]$/.test(t)) win.press(t)
            else if (t === "," || t === ".") win.press(",")
            else if (t === "+" || t === "%" || t === "(" || t === ")") { win.error = false; win.expr += t }
            else if (t === "-") win.press("−")
            else if (t === "*" || t === "x") win.press("×")
            else if (t === "/") win.press("÷")
            else if (e.matches(StandardKey.Copy)) { copyHelper.text = win.preview !== "" ? win.preview : Calc.format(Number(win.expr) || 0); copyHelper.selectAll(); copyHelper.copy() }
            else return
            e.accepted = true
        }
    }
    TextEdit { id: copyHelper; visible: false }

    Column {
        anchors { fill: parent; margins: Theme.space4 }
        spacing: Theme.space3

        // Écran
        Rectangle {
            width: parent.width; height: 120
            radius: Theme.radiusMd
            color: Theme.surfaceGlassStrong
            border.width: 1; border.color: Theme.border
            Column {
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; margins: Theme.space4 }
                spacing: 2
                Label {
                    objectName: "lastLine"
                    width: parent.width; horizontalAlignment: Text.AlignRight; elide: Text.ElideLeft
                    text: win.last !== "" && win.expr === win.resultText ? win.last : (win.expr !== "" ? win.pretty(win.expr) : " ")
                    level: Label.Secondary; font.pixelSize: Theme.fsSm
                }
                Label {
                    objectName: "result"
                    width: parent.width; horizontalAlignment: Text.AlignRight; elide: Text.ElideLeft
                    text: win.error ? qsTr("Calcul impossible") : (win.preview !== "" ? win.preview : (win.expr === "" ? "0" : win.pretty(win.expr)))
                    color: win.error ? Theme.danger : Theme.text
                    font.pixelSize: win.error ? 22 : 38; font.weight: Font.DemiBold
                }
            }
        }

        // Touches
        Grid {
            id: keys
            columns: 4
            spacing: Theme.space2
            width: parent.width
            readonly property real cell: (width - spacing * 3) / 4
            Repeater {
                model: ["C", "( )", "%", "÷", "7", "8", "9", "×", "4", "5", "6", "−", "1", "2", "3", "+", "±", "0", ",", "="]
                Rectangle {
                    id: key
                    required property string modelData
                    readonly property bool op: ["÷", "×", "−", "+"].indexOf(modelData) >= 0
                    readonly property bool eq: modelData === "="
                    width: keys.cell; height: Math.min(keys.cell * 0.78, (win.height - 120 - Theme.space4 * 3 - Theme.space2 * 4) / 5)
                    radius: Theme.radiusMd
                    color: eq ? (keyMouse.pressed ? Qt.darker(Theme.accent, 1.1) : Theme.accent)
                         : op ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, keyMouse.pressed ? 0.38 : 0.22)
                         : keyMouse.pressed ? Theme.surfaceActive : keyMouse.containsMouse ? Theme.surfaceHover : Theme.surfaceSunken
                    Label {
                        anchors.centerIn: parent
                        text: key.modelData
                        font.pixelSize: 20; font.weight: Font.Medium
                        color: key.eq ? Theme.accentForeground : Theme.text
                    }
                    MouseArea { id: keyMouse; anchors.fill: parent; hoverEnabled: true; onClicked: win.press(key.modelData) }
                    Accessible.role: Accessible.Button
                    Accessible.name: key.modelData
                }
            }
        }
    }
}
