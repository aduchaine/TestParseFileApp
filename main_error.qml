import QtQuick 2.7 /*main*/
import QtQuick.Window 2.2
import UIProcess 1.0

Window {
    property int initialWidth : 600
    property int initialHeight : 450

    id: mainWindow
    width: initialWidth
    height: initialHeight
    minimumWidth: 400
    minimumHeight: 300
    maximumWidth: 1000
    maximumHeight: 750
    visible: true
    title: qsTr("ERROR")

    property int msgCounter: 0

    UIProcess {
        id: uiProcess

    } /*end uiprocess*/

    Connections  /*uiProcess*/ {
        target: uiProcess

        onDynamicText: { // d_data - change type(how to change)
            ++msgCounter;
            if (msgCounter > 12) {
                errortext.font.pixelSize = 16;
                return;
            }
            changeText(d_data, str_data);
        }

    } /*end connections*/


    function changeText(how, string) {
        //console.log("changeText: " + string);
        if (msgCounter <= 12) {
            if (string.length > 60) {
                errortext.font.pixelSize = 13;
            }
            else if (string.length > 40) {
                errortext.font.pixelSize = 14;
            }
            else if (string.length > 30) {
                errortext.font.pixelSize = 16;
            }
        }

        switch (how) {
        case 0: // replace
            errortext.text = string;
            break;
        case 1: // append under - newline
            errortext.text += "\n" + string;
            break;
        case 2: // append
            errortext.text += string;
            break;
        case 3: // append above - newline
            errortext.text = string + "\n" + errortext.text;
            break;
        }
    }

    Text {
        id: errortext
        width: mainWindow.width - 20
        font.pixelSize: 20
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors {
            left: parent.left
            right: parent.right
            centerIn: parent
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Timer {
        id: beginTimer
        interval: 1500
        onTriggered: {
            uiProcess.processDirectiveResult(1, 0, false);
            if (msgCounter > 12) {
                changeText(0, "\nUnable to complete startup routine.\nCheck \"error_logs~.txt\" for error code.\n\nClick to close.");
            }
            else {
                changeText(1, "\nUnable to complete startup routine.\nCheck \"error_logs~.txt\" for error code.\n\nClick to close.");
            }
        }
    }

    Component.onCompleted: { beginTimer.start();  changeText(0, "Startup Error\n"); }
}

