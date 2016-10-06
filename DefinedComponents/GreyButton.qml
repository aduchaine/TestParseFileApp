import QtQuick 2.3
import QtQuick.Layouts 1.3
import "qrc:/StaticComponents/"

ButtonProp1 {
    id: greyButton
    butTxtColor: "white"
    butTxtBold: true

    property int responseID     // responseID for what the user input

    property bool on: false

    Connections {
        target: uiProcess

        onSendToField: {
            //console.log("onSendToField = field:resID:response " + n_data + ":" + s_data + ":" + str_data);
            getFieldData(n_data, s_data, str_data);
        }
        onChangeState: {
            propertyChange(s_data, f_switch)
        }
    }
  //  void sendFieldData(int n_data, QString str_data);   // sends button text labels/responses as a string

    function toggle() {
        if (greyButton.state == "on") {
            greyButton.state = "off";
            uiProcess.pi_data2 = fieldID;
            uiProcess.pi_data1 = responseID;
            //console.log("off processClick PgFldRes " + pageID + "-" + fieldID + "-" + responseID);
            uiProcess.processClick();
        }
        else {
            greyButton.state = "on";
            uiProcess.pi_data2 = fieldID;
            uiProcess.pi_data1 = responseID;
            //console.log("on processClick PgFldRes " + pageID + "-" + fieldID + "-" + responseID);
            uiProcess.processClick();
        }
    }
    // for button UI states after going next/previous and switching buttons on the same fieldID
    // this will need some work
    function propertyChange(pc_data, switch_on) {
        if (pc_data === fieldID) {
            if (switch_on === false) {
                //console.log("onChangeState: invisible field " + s_data)
                greyButton.visible = false;
            }
        }

        if (switch_on === true) {
            if (pc_data === responseID) {
                greyButton.state = "on";
                //console.log("button #" + pc_data + " turned on");
            }
        }
        else { // for button pages where only one answer is accepted
            if (pc_data === responseID) {
                greyButton.state = "off";
                //console.log("button #" + pc_data + " turned off");
            }
        }
    }
    // probably need to define text size based on char#
    function getFieldData(field_id, response_id, string_data) {
        if (field_id === fieldID) {
            if (response_id === responseID) {
                butTxt = string_data;
            }
        }
    }

    states: [
        State {
            name: "on"
            PropertyChanges { target: greyButton; on: true; color: Qt.darker(butColor, 2) }
        },
        State {
            name: "off"
            PropertyChanges { target: greyButton; on: false; color: butColor }
        }
    ]
    MouseProp1 {
        onEntered: doHover(3)
        onExited: doHover(4)
        onClicked: {
            toggle()
        }
    }
}
