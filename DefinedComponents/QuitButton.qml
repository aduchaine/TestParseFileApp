import QtQuick 2.3
import "qrc:/StaticComponents/"

ButtonProp1 {
    id: quitbutton
    width: 50
    height: 30
    radius: 10
    butTxt: "Quit"
    butColor: "pink"
    butBrdrColor: "white"
    butHvrBrdrColor: "black"

    MouseProp1 {
        onEntered: doHover(3)
        onExited: doHover(4)
        onClicked: {
            //uiProcess.processQuit(); // if further processing is necessary, enable this and implement the processes
            console.log(butTxt + " clicked")
            Qt.quit()
        }
    }

    states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: buttonQuit; opacity: 1; color: butColor; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: buttonQuit; opacity: .1; }
        }
    ]

    transitions: Transition {
        NumberAnimation { target: buttonQuit; properties: "opacity"; easing.type: Easing.InCirc; duration:400; }
    }

}
