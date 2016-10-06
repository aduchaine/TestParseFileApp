import QtQuick 2.7
import "qrc:/SupportComponents/"

// some functions and signals at the page level in "InputDataStringPage.qml" are required
// WriteLineText.qml is required for text input function and focus items
WriteLineText {
    id: sltext
    txlBoxColor: Qt.darker("tan", .9)
    txlBoxBrdrColor: "white"
    maxLineLen: 20

    inputStatus: 0                      // 0 = standard line text editing, 1 - drop down autocomplete

    property int responseID             // responce ID for what the user input - signifies which text field is processed
    property string textSelected        // selected user text input

    Connections {
        target: uiProcess

     //   onSendToField: {
     //       if (n_data === fieldID) {
     //           //console.log("onSendToField = field:resID:response " + n_data + ":" + s_data + ":" + str_data);
     //           if (s_data == 1) {
     //               dynamicData = str_data;
     //           }
     //           else {
     //               showMenuSelection(n_data, str_data);
     //           }
     //       }
     //   }
        onChangeState: {
            if (s_data === fieldID) {
                //console.log("onChangeState: invisible field " + s_data)
                if (f_switch === false) {
                    //console.log("onChangeState: invisible field " + s_data)
                    sltext.visible = false;
                }
            }
        }
    //    onFieldState: {
    //        if (n_data === fieldID) {
    //            //console.log("onFieldState: fadeOut field: " + n_data + " transitionID: " + s_data)
    //            ddmenu.state = "fadeOut";
    //            transitionID = s_data;
    //            fadeMenuTimer.start();
    //        }
    //    }
    }

    signal saveField(bool navigate);
    onSaveField: {
        saveInput();
        if (navigate === true) {
            navigateSideways(fieldID)
        }
    }
    function saveInput() {
        txtLine.selectAll();
        textSelected = txtLine.selectedText;
        txtLine.deselect();
        txtLine.cursorVisible = false;
        uiProcess.pi_data2 = fieldID;
        uiProcess.ps_data1 = textSelected;
        uiProcess.checkStringInput();
        //console.log("textInputString = " + textSelected);
    }
}
