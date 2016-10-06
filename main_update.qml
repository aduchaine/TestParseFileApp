import QtQuick 2.7
import QtQuick.Layouts 1.3
import "qrc:/DefinedComponents/"
import "qrc:/StaticComponents/"
import "qrc:/SupportComponents/"
import UIProcess 1.0

ColumnLayout {
    id: mainUpdate

    property int msgCounter: 0    

    ColumnLayout /*update*/ {
        id: updateclayout
        anchors.fill: parent

        ReadOnlyText {
            id: updateheader
            txtText: "ParseFile"
            txtSize: 24
            txtColor: "black"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            anchors {
                left: parent.left
                right: parent.right
            }
            textID: -1
        }

        ReadOnlyText {
            id: updatetext
            txtSize: 20
            txtColor: "black"
            Layout.alignment: Qt.AlignHCenter
            anchors {
                left: parent.left
                right: parent.right
            }
            textID: -1
        }

    } /*end layout*/

    Spacer { id: spacer1pgupdate; boxHeight: updatetext.height * .65; opacity: .0; }

    RowLayout /*update*/ {
        id: updaterlayout
        spacing: 40
        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom;
        anchors {
            top: spacer1pgupdate.top
        }

        NextButton /*update*/ {
            id: buttonUpdate1
            butTxt: "Update"
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            mouseNext.onClicked: { processUpdate(1); }
        }

        NextButton /*update*/ {
            id: buttonUpdate2
            butTxt: "Update"
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            mouseNext.onClicked: { processUpdate(2); }
        }

    } /*end layout*/

    Connections  /*uiProcess*/ {
        target: uiProcess

        onBeginApp: {
            if (n_data < 3) {
                return;
            }
            //console.log("main_update: onBeginApp: " + n_data + " rebuild: " + rebuild);
            switch (n_data) {
            case 3:
                reparseTimer.start();
                changeText(0, "\nParse File");
                break;
            case 4:
                //rebuild = true;
                mainbkgrndTestParseFileApp.visible = false;
                uiProcess.parseSelf(true, false);
                stackView.replace(Qt.resolvedUrl("/DefinedComponents/SplashPage.qml"));
                break;
            case 6:
                changeText(0, "Parse Error");
                //uiProcess.processDirectiveResult(1, 0, false);
                changeText(1, "\nUnable to complete parse.\nCheck \"error_logs~.txt\" for error code.\n\nClick \"Quit\"to close.");
                stackView.lockInput(false);
                break;
            }
        }

        onDynamicText: { // d_data - change type(how to change)
            ++msgCounter;
            if (msgCounter > 12) {
                //console.log("onDynamicText: " + rebuild);
                updatetext.txtSize = 16;
                return;
            }
            changeText(d_data, str_data);
        }

        onChangeState: {
            fadeTimer.start();
            buttonUpdate1.state = "fadeOut";
            buttonUpdate2.state = "fadeOut";
        }

        onProcessedData: {
            //console.log("main_update: onProcessedData: Quit");
            Qt.quit()
        }

    } /*end connections*/

    // we need to recv a value for a variable which corresponds the the action to be taken or the value should be saved and accessed on an "update" click
    function processUpdate(button) {
        rebuild = true;
        //console.log("processUpdate: rebuild " + rebuild + " update? ");
        uiProcess.pi_data1 = button;
        uiProcess.processDirectiveResult(1, button, true);
    }

    // much of the sizing could be done within a general function .qml or the base component (not as much flexibility there)
    function checkStringLength(type, string) {
        var new_size;
        switch (type) {
        case 1: // RoT fields
            new_size = 20;
            if (string.length > 60) {
                new_size = 13;
            }
            else if (string.length > 40) {
                new_size = 14;
            }
            else if (string.length > 30) {
                new_size = 16;
            }
            break;
        case 2: // button fields
            new_size = 16;
            if (string.length > 14) {
                new_size = 11;
            }
            else if (string.length > 12) {
                new_size = 12;
            }
            else if (string.length > 10) {
                new_size = 14;
            }
            break;
        }
        //console.log("checkStringLength: new_size " + new_size);
        return new_size;
    }

    function changeText(how, string) {
        //console.log("changeText: " + string);
        var new_len;
        if (msgCounter <= 12) {
            updatetext.txtSize = checkStringLength(1, string);
        }

        switch (how) {
        case 0: // replace
            //updatetext.doTextTransition(string);
            //console.log("checkStringLength(1, string): " + checkStringLength(1, string));
            updatetext.txtSize = checkStringLength(1, string);
            updatetext.txtText = string;
            break;
        case 1: // append under - newline
            updatetext.txtText += "\n" + string;
            break;
        case 2: // append
            updatetext.txtText += string;
            break;
        case 3: // append above - newline
            updatetext.txtText = string + "\n" + updatetext.txtText;
            break;
        case 4: // buttonUpdate1            
            buttonUpdate1.butTxtSz = checkStringLength(2, string);
            buttonUpdate1.state = "fadeIn";
            buttonUpdate1.visible = true;
            buttonUpdate1.butTxt = string;
            break;
        case 5: // buttonUpdate2
            buttonUpdate2.butTxtSz = checkStringLength(2, string);
            buttonUpdate2.state = "fadeIn";
            buttonUpdate2.visible = true;
            buttonUpdate2.butTxt = string;
            break;
        }
    }

    KeyboardProp1 {
        id: keyproppgupdate
    }

    Timer {
        id: reparseTimer
        interval: 800
        onTriggered: {           
            uiProcess.parseSelf(true, true);
        }
    }

    Timer {
        id: fadeTimer
        interval: 400
        onTriggered: {
            buttonUpdate1.opacity = 0;
            buttonUpdate2.opacity = 0;
            processTimer.start();
        }
    }

    Timer {
        id: processTimer
        interval: 400
        onTriggered: {
            uiProcess.beginApp(3);
        }
    }

    Timer {
        id: beginTimer
        interval: 1500
        onTriggered: {            
            //console.log("beginTimer: " + rebuild);
            if (rebuild === false) {
                uiProcess.processDirectiveResult(1, 0, false);
                if (msgCounter > 12) { // this should not happen
                    changeText(0, "\nUnable to complete startup routine.\nCheck \"error_logs~.txt\" for error code.\n\nClick \"Quit\" to close or \"Update\" to process changes.");
                }
                else {
                    changeText(3, "Unable to complete startup routine.\n");
                    changeText(1, "\nClick \"Quit\" to close or the button below to perform the appropriate action.");
                }
            }
            else {
                buttonUpdate1.visible = true;
                buttonUpdate1.state = "fadeIn";
                updatetext.txtSize = 16;
                changeText(0, "The build procedure must be completed for the application.\n\nNot rebuilding could produce undesired results.");
                changeText(4, "Close");
                rebuild = false;                
            }
        }
    }
    Component.onCompleted: {
        //console.log("onCompleted: rebuild " + rebuild);
        buttonUpdate1.state = "fadeOut";
        buttonUpdate2.state = "fadeOut";
        buttonUpdate1.visible = false;
        buttonUpdate2.visible = false;
        buttonNext.visible = false;
        buttonPrevious.visible = false;
        buttonQuit.state = "fadeIn";
        if (rebuild === false) {
            beginTimer.start();
            changeText(0, "\nupdates...");
        }
        else {
            beginTimer.start();
            changeText(0, "");
        }
    }
}

