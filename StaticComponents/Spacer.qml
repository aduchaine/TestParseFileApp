import QtQuick 2.7
import QtQuick.Layouts 1.3

// Spacer { id: spacer1; boxHeight: buttonStart.height; opacity: 0; } -- basic horizontal spacer
Rectangle {
    id: spacer
    color: "red"
    opacity: .25
    width: boxWidth
    height: boxHeight

    Layout.fillWidth : true
    Layout.fillHeight : true

    property int boxWidth: spacer.width
    property int boxHeight: spacer.height
    property alias boxColor: spacer.color
    property alias boxProp: spacer
    property alias boxtxtVisible: spacerText.visible

    Text {
        id: spacerText
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        color: "black"
        font.pixelSize: 24
        font.bold: false
        text: spacer.width + " width-height " + spacer.height
    }
}
