import QtQuick 2.3
import "qrc:/StaticComponents/"

ButtonProp1 {
    id: buttonPrevious
    butTxt: "Previous"
    butColor: "lightblue"

    property alias mousePrev: mouseareaPrevious

    MouseProp1 {
        id: mouseareaPrevious
        onEntered: doHover(3)
        onExited: doHover(4)
        onClicked: {
            uiProcess.getPreviousPage(pageNum);
            //console.log("PreviousButton: MouseProp1 onClicked signal - > page# " + pageID);
        }
    }

    states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: buttonPrevious; opacity: 1; color: butColor; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: buttonPrevious; opacity: .1; }
        }
    ]
    transitions: Transition {
        NumberAnimation { target: buttonPrevious; properties: "opacity"; easing.type: Easing.InCirc; duration: 400; }
    }
}
