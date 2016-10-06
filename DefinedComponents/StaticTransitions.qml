import QtQuick 2.7

Item {
    id: pagetransitions

    function doPageStaticTransition() {
        toggleTransition();
    }
    function toggleTransition() {
        if (buttonPrevious.state === "fadeIn") {
            buttonPrevious.state = "fadeOut";
            buttonNext.state = "fadeOut";
            buttonQuit.state = "fadeOut";
            fadeMainItemsTimer.start();
        }
        else {
            buttonPrevious.state = "fadeIn";
            buttonNext.state = "fadeIn";
            buttonQuit.state = "fadeIn";            
        }
    }
    Timer {
        id: fadeMainItemsTimer
        interval: 400
        onTriggered: {
            doPageStaticTransition()
        }
    }

 /*   states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: test_001; opacity: 1; color: butColor; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: test_001; opacity: .1; }
        }
    ]
    transitions: Transition {
        NumberAnimation { target: test_001; properties: "opacity"; easing.type: Easing.InCirc; duration: 400; }
    }

    function getPage() {

    }

    function doPageTransition(pageID) {
        togglePageTransition();
    }
    function togglePageTransition() {
        if (test_001.state === "fadeIn") {
            ddmenu.state = "fadeOut";
            fadePageItemsTimer.start();
        }
        else {
            ddmenu.state = "fadeIn";
            console.log("togglePageTransition: fadeIn");
        }
    }
    Timer {
        id: fadePageItemsTimer
        interval: 400
        onTriggered: {
            doPageTransition();
            console.log("fadePageItemsTimer:");
        }
    } */

}
