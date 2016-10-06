import QtQuick 2.3
import QtQuick.Layouts 1.3

// basic text button with solid color, rounded edges, a thin border and hover state change settings
Rectangle {
    id: button1
    color: butColor
    radius: 16
    border.width: butBrdrWidth
    border.color: butBrdrColor

    Layout.minimumWidth: 50
    Layout.preferredWidth: 100
    Layout.maximumWidth: 150
    Layout.minimumHeight: 30
    Layout.preferredHeight: 40
    Layout.maximumHeight: 50
    Layout.fillWidth: true
    Layout.fillHeight: true

    property color butColor: "grey"
    property color butHvrColor: Qt.darker("grey", 1.5)
    property int butBrdrWidth: 2
    property color butBrdrColor: "black"
    property color butHvrBrdrColor: "gold" // Qt.darker("beige" , 1.5)
    property alias butTxtFld: button1Text
    property alias butTxt: button1Text.text
    property alias butTxtColor: button1Text.color
    property alias butTxtSz: button1Text.font.pixelSize
    property alias butTxtBold: button1Text.font.bold

    property int fieldID

    Text {
        id: button1Text
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        color: "black"
        font.pixelSize: 16
        font.bold: false
        text: "button1"
    }
}
