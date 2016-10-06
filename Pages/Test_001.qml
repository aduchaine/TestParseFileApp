import QtQuick 2.7
import QtQuick.Layouts 1.3
import "qrc:/DefinedComponents/"
import "qrc:/StaticComponents/"
import "qrc:/SupportComponents/"
import UIProcess 1.0

ColumnLayout { /*page begin*/
    id: test_001

    property int pageID
    property int responseID
    property alias test_001: test_001

    pageID: 1

    Spacer { id: spacer1pg1; boxHeight: ddmenu5pg1.height * 1.75; opacity: .0; }

    ReadOnlyText {
        id: rot2pg1
        txtText: "rot2pg1"
        txtSize: 16
        txtColor: "black"
        textID: 3
    }

    Spacer { id: spacer3pg1; boxHeight: ddmenu5pg1.height * .5; opacity: .0; }

    RowLayout {
        id: rlayout4pg1
        spacing: 20
        anchors { } // bottom: someitem.top; top: someitem.bottom; right: someitem.left; left: someitem.right;
        Layout.alignment: Qt.AlignHCenter // | Qt.AlignBottom, Qt.AlignTop, Qt.AlignVCenter

        DDMenu {
            id: ddmenu5pg1
            shownItemText: "ddmenu5pg1"
            Layout.fillHeight: false
            fieldID: 1
            menuID: 1
            responseID: 3
            kbpropDDmenu.onFocusChanged: {
                if (kbpropDDmenu.activeFocus === false) {
                    interpageprocspg1.closeMenu(fieldID);
                }
            }
        }

        DDMenu {
            id: ddmenu6pg1
            shownItemText: "ddmenupg1"
            Layout.fillHeight: false
            fieldID: 2
            menuID: 2
            responseID: 3
            kbpropDDmenu.onFocusChanged: {
                if (kbpropDDmenu.activeFocus === false) {
                    interpageprocspg1.closeMenu(fieldID);
                }
            }
        }

        DDMenu {
            id: ddmenu7pg1
            shownItemText: "ddmenupg1"
            Layout.fillHeight: false
            fieldID: 3
            menuID: 3
            responseID: 3
            kbpropDDmenu.onFocusChanged: {
                if (kbpropDDmenu.activeFocus === false) {
                    interpageprocspg1.closeMenu(fieldID);
                }
            }
        }

    } /*end layout*/

    Spacer { id: spacer8pg1; boxHeight: ddmenu5pg1.height * .75; opacity: .0; }

    RowLayout {
        id: rlayout9pg1
        spacing: 20
        anchors { } // bottom: someitem.top; top: someitem.bottom; right: someitem.left; left: someitem.right;
        Layout.alignment: Qt.AlignHCenter // | Qt.AlignBottom, Qt.AlignTop, Qt.AlignVCenter

        SLText {
            id: slt10pg1
            //visible: true
            txlTxtSz: 14
            txlLabelTxt: "sltpg"
            maxLineLen: 25
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            fieldID: 4
            responseID: 2
            inputStatus: 0            
        }

        GreyButton {
            id: greybut11pg1
            //visible: true
            butTxt: "greybutpg"
            /*Layout.alignment: Qt.AlignHCenter;*/
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            fieldID: 5
            responseID: 4
        }

        GreyButton {
            id: greybut12pg1
            //visible: true
            butTxt: "greybutpg"
            /*Layout.alignment: Qt.AlignHCenter;*/
            Layout.fillWidth: false;
            Layout.fillHeight: false;
            fieldID: 6
            responseID: 5
        }

    } /*end layout*/

    Spacer { id: spacer13pg1; boxHeight: ddmenu5pg1.height * 1; opacity: .0; }

    Connections /*uiProcess*/ {
        target: uiProcess

        onStringData: {
            switch(str_pos) { // str_pos = fieldID
			case 4:
			slt10pg1.textChange(str_data, str_pos);
			break; 
            }
        }

        onNoData: {
            buttonNext.color = Qt.darker("green", .8);
        }

        onInvalidData: {
            buttonNext.color = Qt.darker("green", .8);
        }

        onValidData: {
            buttonNext.color = Qt.darker("tan", 1.2);
        }

        onPreviousPage: {
            buttonPrevious.color = Qt.darker("yellow", .7);
        }

        onSendWordCount: {
            switch (n_data) { // fieldID
            case 1:
                ddmenu5pg1.saveWordCount(n_data, s_data);
                break;
            case 2:
                ddmenu6pg1.saveWordCount(n_data, s_data);
                break;
            case 3:
                ddmenu7pg1.saveWordCount(n_data, s_data);
                break;
            }
        }

        onSendWords: {
            switch (d_data) { // fieldID
            case 1:
                ddmenu5pg1.saveWords(f_switch, str_data);
                break;
            case 2:
                ddmenu6pg1.saveWords(f_switch, str_data);
                break;
            case 3:
                ddmenu7pg1.saveWords(f_switch, str_data);
                break;
            }
        }

        onDataCode: {

            switch(s_data) { // fieldID
            case 1:
                ddmenu5pg1.menuPropertyChange(s_data, n_data);
                break;
            case 2:
                ddmenu6pg1.menuPropertyChange(s_data, n_data);
                break;
            case 3:
                ddmenu7pg1.menuPropertyChange(s_data, n_data);
                break;
            }
        }

    } /*end connections*/

    KeyboardProp1 {
        id: keyproppg1
    }

    InterpageProcesses {
        id: interpageprocspg1
    }

    Component.onCompleted: {
        stackView.doNewPageProcess(pageID);
        keyproppg1.forceActiveFocus();
    }
}
