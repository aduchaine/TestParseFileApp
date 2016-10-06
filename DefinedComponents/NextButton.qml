import QtQuick 2.3
import "qrc:/StaticComponents/"

ButtonProp1 {
    id: buttonNext
    butTxt: "Next"
    butColor: Qt.darker("green", .8)

    property alias mouseNext: mouseareaNext

    MouseProp1 {
        id: mouseareaNext
        onEntered: doHover(3)
        onExited: doHover(4)
        onClicked: {
            keyinputStackView.processTimer.start();
            //console.log("NextButton: MouseProp1 onClicked signal")
        }
    }
    function changeText(string) {
        butTxtSz = 16;
        butTxt = string;        
        butTxtBold = true;
        butTxtColor = "white"
    }
    function toggleWarning() {
        if (buttonNext.state == "on") {
            buttonNext.state = "off";
            buttonNext.changeText("Stop");
            warningTimer.start();
        }
        else {
            buttonNext.state = "on";
            console.log("on toggle ");
        }
    }

    property bool on: false

    states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: buttonNext; opacity: 1; color: butColor; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: buttonNext; opacity: .1; }
        },
        State {
            name: "on";
            PropertyChanges { target: buttonNext; on: true; }
        },
        State {
            name: "off";
            PropertyChanges { target: buttonNext; on: false; }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation { target: buttonNext; properties: "opacity"; easing.type: Easing.InCirc; duration: 400; }
        },
        Transition {
            from: "off"; to: "on";
            ColorAnimation { target: buttonNext; from: "green"; to: "red"; properties: "color"; duration: 500; }
        },
        Transition {
            from: "on"; to: "off";
            ColorAnimation { target: buttonNext; from: "red"; to: "green"; properties: "color"; duration: 2500; }
        }
    ]
    Timer {
        id: warningTimer
        interval: 3000
        onTriggered: {
            buttonNext.changeText("Continue");
            butTxtBold = false;
            butTxtColor= "black"
            //toggleWarning();
        }
    }
}
