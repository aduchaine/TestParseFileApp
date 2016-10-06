import QtQuick 2.7
import "qrc:/Pages/"

Item {
    id: interpagenav

    // this is for testing "onNavigateSideways" and could take "menu_id" as a variable if done differently
    function checkActiveFocus() {
        if (ddmenu1.kbpropDDmenu.activeFocus === true) { console.log("ActiveFocus   ddmenu1.kbpropDDmenu ") }
        else if (ddmenu2.kbpropDDmenu.activeFocus === true) { console.log("ActiveFocus   ddmenu2.kbpropDDmenu ") }
        else if (textmenu1.txtEdit.activeFocus === true) { console.log("ActiveFocus   textmenu1.txtEdit") }
        else if (textmenu2.txtEdit.activeFocus === true) { console.log("ActiveFocus   textmenu2.txtEdit") }
    }

    Connections {
        target: stackView

        // this is done for anything which accepts text input - WriteLineText, DropDownText,  SLText(212), ACTMenu(214)
        onSavepageInput: {
            //console.log(" onSavepageInput page - " + page_id);
            switch(page_id) {
			case 1:
				slt10pg1.saveInput();
			break; 
            }
        }

        // this could probably be better
        // TAB interpage navigation between fields - multiple fields on a page - not linked to a specifc component
        onNavigateSideways: {
            switch(field_id) {
            case 1:
                logpassword.txtLine.cursorVisible = true;
                logpassword.txtLine.forceActiveFocus();
                break;
            case 2:
                logname.txtLine.cursorVisible = true;
                logname.txtLine.forceActiveFocus();
                break;
            case 3:
                ddmenu1.shownItem.color = ddMonths.shownItemColor
                ddmenu2.state = "dropDown";
                ddmenu2.shownItem.color = Qt.darker(ddMonths.shownItemColor, 1.25)
                ddmenu2.kbpropDDmenu.forceActiveFocus();
                break;
            case 4:
                ddmenu2.shownItem.color = ddMonths.shownItemColor
                textmenu1.txtEdit.cursorVisible = true;
                textmenu1.txtEdit.forceActiveFocus();
                break;
            case 5:
                textmenu2.txtEdit.cursorVisible = true;
                textmenu2.txtEdit.forceActiveFocus();
                break;
            case 6:
                ddmenu1.state = "dropDown";
                ddmenu1.shownItem.color = Qt.darker(ddDays.shownItemColor, 1.25)
                ddmenu1.kbpropDDmenu.forceActiveFocus();
                break;
            }
        }
    }

    // menu controls - linked to dropdown style menus currently - DDMenu(213), ACTMenu(214)
    signal closeMenu(int field_id)
    onCloseMenu: {
        switch(field_id) {
			case 1:
			ddmenu5pg1.state = "";
			break; 
        case 2:
            ddmenu6pg1.state = "";
            break;
        case 3:
            ddmenu7pg1.state = "";
            break;
        }
    }
}
