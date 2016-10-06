import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.2
import "qrc:/SupportComponents/"

// basic drop down menu
// DDMenu.qml contains most processing functions
// KeyboardProp1.qml is required for page focus items
// some functions and signals at the page level in "TesMultInputPage.qml" are required
// "dropdownScroll" enables scrollbar when width is small
ButtonProp1 {
    id: dropdownMenu
    color: "green" // "transparent"
    radius: 6
    butTxt: ""

    Layout.minimumWidth: 50
    Layout.preferredWidth: 100
    Layout.maximumWidth: 150
    Layout.minimumHeight: 20
    Layout.preferredHeight: 25
    Layout.maximumHeight: 30
    Layout.fillWidth: true

    property int menuID                                     // identifies the menu and it's data set(data_code) not the fieldID
    property variant items: ["dummy item1"]
    //property variant items

    property alias shownItem: shownItem
    property alias shownItemText: shownItem.butTxt          // current item in the "shownItem" button
    property color shownItemColor: "lightsteelblue"
    property alias curIndex: listView.currentIndex          // this value will be different than "index" if a selection is made before syncing them
    property color menuItemColor: Qt.darker("beige", 1.25)

    property alias listviewArray: listView.model
    property alias listview: listView

    property alias mousepropDDmenu: ddmousearea
    property alias kbpropDDmenu: keypropddmenu


    ButtonProp1 {
        id: shownItem
        width: parent.width
        height: parent.height
        color: shownItemColor
        butBrdrWidth: 1
        radius: 6
        butTxt: "intital item text"
        butTxtSz: 12

        anchors {
            left: parent.left
            right: parent.right
        }

        MouseProp1 {
            id: ddmousearea
            onEntered: shownItem.color = Qt.darker(shownItemColor, 1.25)
            onExited: shownItem.color = shownItemColor
            onClicked: {
                if (dropdownMenu.state === "dropDown") {
                    dropdownMenu.state = "";
                }
                else {
                    dropdownMenu.state = "dropDown"
                    listView.positionViewAtIndex(curIndex, ListView.Center);
                }
                keypropddmenu.forceActiveFocus();
            }
            //Component.onCompleted: { console.log("ddmousearea width-height " + ddmousearea.width + "-" + ddmousearea.height); }
        }
        //Component.onCompleted: { console.log("shownItem width-height " + shownItem.width + "-" + shownItem.height); }
    }
    ButtonProp1 { // may need to use the 2nd variant if text issues arise
        id: ddMenuItems
        width: dropdownMenu.width
        color: menuItemColor
        butBrdrWidth: 1
        opacity: 0.75
        butTxt: ""
        radius: 6
        clip: true // false shows max height
        anchors.top: shownItem.bottom

        ScrollView {
            id: dropdownScroll
            //width: dropdownMenu.width // add width property to get a scrollbar - wider menus seem to add scroll automatically
            height: dropdownMenu.height * 4

            ListView {
                id:listView
                model: dropdownMenu.items
                currentIndex: 0
                highlight: Rectangle { color: Qt.darker("steelblue", 0.8); radius: 6; border.color: butBrdrColor }

                delegate: Item {
                    width: shownItem.width
                    height: shownItem.height

                    Text {
                        id: ddmenuitemText
                        text: modelData
                        anchors.centerIn: parent
                        color: "black"
                    }
                    MouseProp1 {
                        onEntered: ddMenuItems.color = Qt.darker(menuItemColor, 1.25)
                        onExited:  ddMenuItems.color = menuItemColor
                        onClicked: {
                            //console.log("previous_itemID - " + listView.currentIndex) // keep this here for accurate log description
                            dropdownMenu.state = "";
                            shownItem.butTxt = modelData;
                            //console.log(shownItemText + " (chosen_item)(chosen_itemID) " + index)
                            curIndex = index;
                            getIndexLocation();
                        }
                    }
                }
            }
            //Component.onCompleted: { console.log("dropdownScroll width-height " + dropdownScroll.width + "-" + dropdownScroll.height); }
        }
        //Component.onCompleted: { console.log("ddMenuItems width-height " + ddMenuItems.width + "-" + ddMenuItems.height); }
    }
    //Component.onCompleted: { console.log("dropdownMenu width-height " + dropdownMenu.width + "-" + dropdownMenu.height); }

    states: [
        State {
            name: "dropDown"
            PropertyChanges { target: ddMenuItems; height:dropdownMenu.height*4; } //dropdownMenu.height*dropdownMenu.items.length;
        },
        State {
            name: "visible"
            PropertyChanges { target: dropdownMenu; visible: true; }
        }
    ]
    transitions: Transition {
        NumberAnimation { target: ddMenuItems; properties: "height"; easing.type: Easing.OutExpo; duration: 1000 }
    }

    KeyboardProp1 {
        id: keypropddmenu

        onFocusChanged: {
            //console.log("   onFocusChanged  menuID " + menuID)
            if (keypropddmenu.activeFocus === false) {
                dropdownMenu.state === ""
                shownItem.color = shownItemColor
            }
            else {
                shownItem.color = Qt.darker(shownItemColor, 1.25)
                keypropddmenu.forceActiveFocus()
            }
        }
        Keys.onReturnPressed: {
            if (dropdownMenu.state === "") {
                dropdownMenu.state = "dropDown";
                if (curIndex === 0) {
                    return;
                }
                dropdownMenu.listview.positionViewAtIndex(curIndex, ListView.Center)
            }
            else {
                dropdownMenu.state = "";
                if (autoItems.length < items.length && autoItems.length !== 0) {
                    shownItemText = autoItems[curIndex];
                }
                else {
                    shownItemText = items[curIndex]; // does the same as <shownItem.butTxt = modelData> in mouse area
                }
                getIndexLocation()
            }
            //console.log("onReturnPressed: item:index (" + shownItemText + ")(" + curIndex + ")");
        }
        Keys.onTabPressed: {
            navigateSideways(menuID)
        }
        Keys.onDownPressed: {
            if (dropdownMenu.state === "") {
                return;
            }
            if (autoItems.length < items.length && autoItems.length !== 0) {
                if (curIndex < autoItems.length - 1) {
                    curIndex = curIndex + 1;
                }
            }
            else {
                if (curIndex < items.length - 1) {
                    curIndex = curIndex + 1;
                }
            }
            //console.log("onDownPressed (items.length) " + items.length + "(selector item) " + curIndex);
        }
        Keys.onUpPressed: {
            if (dropdownMenu.state === "") {
                return;
            }
            if (curIndex > 0) {
                curIndex = curIndex - 1;
                //console.log("onUpPressed (selector item) " + curIndex);
            }
        }
    }
    Component.onCompleted: { dropdownMenu.state = ""; }
}
