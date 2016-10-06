import QtQuick 2.3
import QtQuick.Layouts 1.3
import "qrc:/StaticComponents/"


// when main.qml is created the splash could be an option and the file dependency populated according to it's presence after "initialItem: "
ButtonProp1 {
    id: pagesplash
    width: 250
    height: 100
    radius: 10
    butColor: "lightgrey"
    butBrdrWidth: 5
    butHvrBrdrColor: "green"
    butTxtBold: false
    butTxtSz: 24
    butTxtColor: "red"
    butTxtFld.width: mainWindow.width - 20
    butTxtFld.wrapMode: Text.WordWrap
    anchors {
        centerIn: parent
        left: mainWindow.left;
        right: mainWindow.right;
    }

    property int msgCounter: 0
    property bool on: true
    property bool update: false

    Connections {
        target: uiProcess

        onBeginApp: {
            if (n_data < 3 || n_data === 5) {
                //console.log("SplashPage: onBeginApp: " + n_data + " rebuild " + rebuild);
                switch (n_data) {
                case 0:
                    update = false;
                    beginTimer.start();
                    changeText(0, "Parse File");
                    break;
                case 1:
                    changeText(0, "Startup Error\n");
                    uiProcess.processDirectiveResult(1, 0, false);
                    changeText(1, "\nUnable to complete startup routine.\nCheck \"error_logs~.txt\" for error code.\n\nClick to close.");
                    stackView.lockInput(false);
                    break;                    
                // if we need to update again, rebuild should be false - may need some work in UIProcess
                case 2:
                    update = true;
                    rebuild = false;
                    beginTimer.start();                    
                    changeText(0, "Updates Required");
                    break;
                case 5:
                    update = true;
                    rebuild = true;
                    beginTimer.start();
                    changeText(0, "Parse File");
                    break;
                }
            }
        }

        onDynamicText: {
            //console.log("onDynamicText: " + str_data);
            ++msgCounter;
            if (msgCounter > 12) {
                changeText(0, "");
                butTxtFld.font.pixelSize = 16;
                return;
            }
            changeText(d_data, str_data);
        }
    }

    function changeText(how, string) {
        //console.log("changeText: " + string);
        if (msgCounter <= 12) {
            if (string.length > 60) {
                butTxtFld.font.pixelSize = 13;
            }
            else if (string.length > 40) {
                butTxtFld.font.pixelSize = 14;
            }
            else if (string.length > 30) {
                butTxtFld.font.pixelSize = 16;
            }
        }

        switch (how) {
        case 0: // replace
            butTxt = string;
            break;
        case 1: // append under - newline
            butTxt += "\n" + string;
            break;
        case 2: // append
            butTxt += string;
            break;
        case 3: // append above - newline
            butTxt = string + "\n" + butTxt;
            break;
        }
    }

    // parse in "mainbkgrnd + arg1" which is app name/folder
    // parse in first page "filename" = filename with pageID = 0 -> also need a relative path but this could remain standard
    // do a transition - like shrink in the middle and fade to nothing
    function removeSplash() {
        //console.log("removeSplash: " + update);
        if (update === false) {
            pagesplash.state = "off";
            mainbkgrndTestParseFileApp.visible = true;
            stackView.replace(Qt.resolvedUrl("/Pages/Test_000.qml"));
            lockinputTimer.interval = 800;
            stackView.lockInput(false);
            buttonNext.visible = true;
            buttonPrevious.visible = true;
        }
        else {
            pagesplash.state = "off";
            mainbkgrndTestParseFileApp.visible = true;
            stackView.replace(Qt.resolvedUrl("/main_update.qml"));
            lockinputTimer.interval = 800;
            stackView.lockInput(false);
        }
    }

    states: [
        State {
            name: "on"
            PropertyChanges { target: pagesplash; on: true; visible: true; z:1000 }
        },
        State {
            name: "off"
            PropertyChanges { target: pagesplash; on: false; visible: false }
        }
    ]
    Timer {
        id: splashTimer
        interval: 500
        onTriggered: {            
            uiProcess.parseSelf(false, false);
        }
    }
    Timer {
        id: beginTimer
        interval: 1500
        onTriggered: {
            removeSplash();
        }
    }
    // stackView.lockInput(true); possibly not needed
    Component.onCompleted: {
        stackView.lockInput(true);
        pagesplash.state = "on";
        splashTimer.start();
        changeText(0, "Preparing data...");
        buttonPrevious.state = "fadeOut";
        buttonNext.state = "fadeOut";
        buttonQuit.state = "fadeOut";
    }
}
