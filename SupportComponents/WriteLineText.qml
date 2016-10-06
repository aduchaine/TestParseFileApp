import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

// text and text field focus portion of auto complete drop down menu - this is also used for other line text editing
// ACTMenu.qml contains most processing functions
// DropDownText.qml contains layout and list functions
// KeyboardProp1.qml is required for focus items
// some functions and signals at the page level in "TesMultInputPage.qml" are required
Rectangle {
    id: textBox
    color: txlBoxColor
    radius: 10
    border.width: txlBoxBrdrWidth
    border.color: txlBoxBrdrColor

    Layout.minimumWidth: 150
    Layout.preferredWidth: 200
    Layout.maximumWidth: 250
    Layout.minimumHeight: 20
    Layout.preferredHeight: 30
    Layout.maximumHeight: 40
    Layout.fillWidth: true
    Layout.fillHeight: true

    property color txlBoxColor: "white"
    property color txlBoxBrdrColor: "black"
    property int txlBoxBrdrWidth: 2

    property alias txtLine: textlineedit
    property alias txlTxt: textlineedit.text
    property alias txlTxtColor: textlineedit.color
    property alias txlTxtSz: textlineedit.font.pixelSize
    property alias txlTxtBold: textlineedit.font.bold
    property alias txlTxtAnchor: textlineedit.anchors

    property alias txlLabel: txtlinelabel
    property alias txlLabelTxt: txtlinelabel.text
    property alias txlLabelSz: txtlinelabel.font.pixelSize
    property alias txlLabelAnchor: txtlinelabel.anchors

    property string txtLineInput                        // user text input
    property int maxLineLen: 15                         // max char for input - should use as a function variable

    property int fieldID                                // specific field whose value will change with the program
    property int inputStatus                            // sets the input status for AutoComplete - will likely change the name


    // any page which uses this qml through a component should be added to this function - DropDownText,  SLText(212), ACTMenu(214)
    function setPageFocus(page_id) {
        switch(page_id) {
			case 1:
			keyproppg1.forceActiveFocus();
			break; 
        }
    }

    // this function is directly linked to "ACTMenu.qml" and "checkAutoComplete()" C++ function
    function checkString() {
        if (inputStatus === 1) {
            var cursor_pos = txtLine.cursorPosition
            //console.log(" checkStrlen ")
            if (autocompleteOn === true) {
                return;
            }
            //console.log("(" + shownItemText + ") chosen-len-pos " + txlTxt.length + "-" + cursor_pos) // shownItemText or txlTxt
            if (txlTxt.length > 2) {
                word_count = 0
                word_count_incr = 0
                autoItems = new Array();
                //console.log("(" + shownItemText + ")<-chosen-len-cursor_pos " + txlTxt.length + "-" + cursor_pos)

                // these variables will be passed/sent/signalled to c++
                uiProcess.ps_data1 = shownItemText;
                uiProcess.pi_data3 = menuID;
                uiProcess.checkAutoComplete() // c++ function - could send a code if the function is not known
            }
            else if (txlTxt.length < 3) {
                dropdownText.state = "";
            }
        }
        else if (txtLine.length > maxLineLen) {
            console.log(" Too Long!  " + txtLine.length)
            txtLine.clear();
        }
    }
    function textChange(string_data, field_id) {
        if (field_id === fieldID) {
            txlTxt = string_data;
            //console.log("textChange: str-pos: " + string_data + "-" + field_id);
        }
    }

    // WIP - this may become standalone
    // VKboard write to text edit field - simple but does the job - pos = ending cursor pos - handle cursor manipulation
    // some of the VKboard functions intertwined with UI functions should be separated somehow (save functions, etc)
    function doVKfill(field_id, pos, key) {
        if (key === "ENTER") {
            if (field_id === 1 || field_id === 2) {
                saveField(false)
            }
            else if (field_id === 5 || field_id === 6) { // disabled VKboard on this page until process is refined
                var buf_len = txtLine.length;
                stackView.sendBuflen(field_id, buf_len); // this would probably be better than messing with the save issue and multiple apparent signals
                saveInput();
            }
        }
        else if (key === "BACKSPACE") {
            txtLine.remove(pos, pos + 1);
            txtLine.cursorPosition = pos;
        }
        else if (key === "LEFT") {
            txtLine.cursorPosition = pos;
        }
        else if (key === "RIGHT") {
            txtLine.cursorPosition = pos;
        }
        else {
            txtLine.insert(pos - 1, key);
            txtLine.cursorPosition = pos;
        }
    }
    TextEdit {
        id: textlineedit
        font.pixelSize: 16

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            leftMargin: 20
        }
        Label {
            id: txtlinelabel
            font.pixelSize: 12
            font.italic: true
            opacity: 0.5
            anchors {
                right: parent.right
                rightMargin: 10
            }
            text: "text edit label"
        }

        // these are needed for auto complete
        onTextChanged: {
            checkString()
            if (inputStatus === 1) {
                //    console.log("WriteLineText: (len-text)selectedText(pos)shownItemText(text-len):(" + selectedText.length + "-"
                //                + selectedText + "-" + txtLine.cursorPosition + "-" + shownItemText + "-" + shownItemText.length + ")")
                if (selectedText.length <= 0) {
                    txtlinelabel.text = "text edit label";
                }
                else {
                    txtlinelabel.text = "";
                }
            }
        }
        Keys.onPressed: {
            //console.log(" Root WriteLineText: onPressed--> pageNum " + pageID)
            if (event.key === Qt.Key_Enter) {
                event.accepted = true;
                return;
            }
            if (inputStatus === 1) {
                cursorVisible = true;
                if (event.key === Qt.Key_Backspace) {
                    autocompleteOn = true;
                    if (autocomSelectOn === true) {
                        acCursorLeft(true)
                        event.accepted = true;
                    }
                    return; // to allow refills of ac text while deleting, omit/move return and/or change the logic
                }
                autocomSelectOn = false
                autocompleteOn = false
            }
        }
        Keys.onEscapePressed: {
            setPageFocus(pageID)
        }
        Keys.onReturnPressed: {
            if (inputStatus === 1) {
                if (actmenu.state === "dropDown") {
                    shownItemText = autoItems[curIndex];;
                }
                saveField(false)
                cursorVisible = false;
                setPageFocus(pageID)
            }
            else {
                saveField(false)
                cursorVisible = false;
                setPageFocus(pageID)
            }
        }
        Keys.onTabPressed: {
            if (inputStatus === 1) {
                cursorVisible = false;
                saveField(true)
            }
            else {
                cursorVisible = false;
                saveField(true)
            }
        }
        Keys.onLeftPressed: {
            if (inputStatus === 1) {
                cursorVisible = true
                autocompleteOn = true
                if (autocomSelectOn === true) {
                    acCursorLeft(false)
                }
            }
            cursorPosition = cursorPosition - 1
        }
        // specifically for auto complete menu - would not need to be here otherwise
        Keys.onDownPressed: {
            if (inputStatus === 1) {
                if (dropdownText.state === "") {
                    return;
                }
                if (curIndex < actmenu.autoItems.length - 1) {
                    curIndex = curIndex + 1;
                }
            }
        }
        Keys.onUpPressed: {
            if (inputStatus === 1) {
                if (dropdownText.state === "") {
                    return;
                }
                if (curIndex > 0) {
                    curIndex = curIndex - 1;
                }
            }
        }
        Component.onCompleted: {
            cursorVisible = false;
        }
    }
}
