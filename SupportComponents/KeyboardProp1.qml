import QtQuick 2.7
import "qrc:/Pages/"

// this is the root keyboard focus at the page level - it does more of the page processes
// this will get trumped by TextEdit occasionally
// implemetation of ACTMenu.qml requires function "setPageFocus()" and how it's used here
Item {
    id: keyproperties

    property int maxPageNum // this is to be gained from a uiProcess signal/function and set once upon program startup for onRightPressed
    property alias processTimer: processDelayTimer

    // any page which uses this qml should be added to this function - currently, this means each page should have a case
    function setPageFocus(page_id) {
        switch(page_id) {
        case 0:
            keyproppg0.forceActiveFocus();
            break;
			case 1:
			keyproppg1.forceActiveFocus();
			break; 
        }
    }

    // probably need to alter what this does based on pageID or feildsID/dataCodeID/menuID != 0 when the situation arises
    Keys.onRightPressed: {
        //console.log(" Root KeyboardProp1: onRightPressed page - " + pageNum);
        if (pageNum < 7) {
            stackView.savepageInput(pageNum);
            processDelayTimer.start();
        }
    }
    Keys.onLeftPressed: {
        //console.log(" Root KeyboardProp1: onLeftPressed page - " + pageNum);
        uiProcess.getPreviousPage(pageNum);
    }
    Keys.onEscapePressed: {
        //console.log(" Root KeyboardProp1: onEscapePressed--> pageNum (" + pageID + ")")
        setPageFocus(pageID)
    }

    // before this timer triggers all the page data must be saved and ready to confirm
    Timer {
        id: processDelayTimer
        interval: 50
        onTriggered: {
            //console.log(" Root processDelayTimer: trigger signal - page = " + pageNum + " -> confirmData()")
            uiProcess.confirmData(pageNum);
        }
    }
}
