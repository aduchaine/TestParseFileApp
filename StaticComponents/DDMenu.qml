import QtQuick 2.7

// DDMenu = drop down menu
// DropDownMenu.qml contains layout, list and menu focus items and functions
// KeyboardProp1.qml is required for page focus items
// some functions and signals at the page level in "TesMultInputPage.qml" are required
DropDownMenu {
    id: ddmenu

    property int fieldID: shownItem.fieldID
    property int responseID                                 // responce ID for what the user input

    property variant autoItems // test

    property int word_count: 0                              //  used to verify menu population completion
    property int word_count_incr: 0                         //  used to verify menu population completion

    property int transitionID
    property string dynamicData           // this may become necessary when communicating with C++
    property bool dynamicField: false

    Connections {
        target: uiProcess

        onSendToField: {
            if (n_data === fieldID) {
                //console.log("onSendToField = field:resID:response " + n_data + ":" + s_data + ":" + str_data);
                if (s_data == 1) {
                    dynamicData = str_data;
                }
                else {
                    showMenuSelection(n_data, str_data);
                }
            }
        }
        onChangeState: {
            if (s_data === fieldID) {
                if (f_switch === false) {
                    //console.log("onChangeState: invisible field " + s_data)
                    ddmenu.visible = false;
                }
            }
        }
        onFieldState: {
            if (n_data === fieldID) {
                //console.log("onFieldState: fadeOut field: " + n_data + " transitionID: " + s_data)
                ddmenu.state = "fadeOut";
                transitionID = s_data;
                fadeMenuTimer.start();
            }
        }
    }

    signal processSelection(int p_index);

    // backend processing
    onProcessSelection: {
        responseID = p_index;
        //console.log("pageID-fieldID-responseID:  " + pageID + "-" + fieldID + "-" + responseID)
        uiProcess.pi_data2 = fieldID;
        uiProcess.pi_data1 = responseID;
        uiProcess.pi_data3 = menuID;
        uiProcess.processClick();
    }

    // these two are for menu data populating from c++ (memory or external files)
    function saveWordCount(field_id, w_count) {
        //console.log("saveWordCount: question-w_count = " + field_id + "-" + w_count);
        autoItems = new Array();        
        word_count_incr = 0;
        word_count = w_count;
        fieldID = field_id;
        if (w_count === 0) {
            ddmenu.state = ""
        }
    }
    function saveWords(clicked, string_data) {
        if (clicked === false) {
            //console.log("saveWords: items - string_data = " + string_data);
            items[word_count_incr] = string_data;
            ++word_count_incr;
            if (word_count_incr === word_count && word_count != 0) {
                listviewArray = items;
            }
        }
        else {
            dynamicField = true;
            //console.log("saveWords: autoItems - string_data = " + string_data);
            autoItems[word_count_incr] = string_data;
            ++word_count_incr;
            if (word_count_incr === word_count && word_count != 0) {
                listviewArray = autoItems;
            }
        }
    }
    function menuPropertyChange(field_id, menu_data) {
        if (field_id === fieldID) { // for menu state
            //console.log(field_id + " fieldID:menu_item " + menu_data + "-" + autoItems[menu_data]);
            shownItemText = items[menu_data]
            curIndex = menu_data
        }
    }
    function getIndexLocation() {
        if (dynamicField === false) {
            for (var i = 0; i < items.length; i++) {
                if (items[i] === shownItemText) {
                    //console.log(shownItemText + " (item)chosen(ID) " + i);
                    processSelection(i);
                    break;
                }
            }
        }
        else {
            for (var i = 0; i < autoItems.length; i++) {
                if (autoItems[i] === shownItemText) {
                    //console.log(shownItemText + " (item)chosen(ID) " + i);
                    processSelection(i);
                    break;
                }
            }
        }
    }
    function showMenuSelection(field_id, string_data) {
        if (field_id === fieldID) {
            shownItemText = string_data;
        }
    }
    function doMenuTransition(type) {
        switch(type) {
        case 1:
            if (dynamicData.length > 0) {
                shownItemText = dynamicData;
                dynamicData = "";
                curIndex = 0;
            }
            ddmenu.visible = true;
            ddmenu.state = "fadeIn";
            break;
        case 2:
            ddmenu.visible = false;
            break;
        }
    }
    states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: ddmenu; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: ddmenu; }
        }
    ]
    transitions: [
        Transition {
            from: "fadeIn"; to: "fadeOut";
            PropertyAnimation { target: ddmenu; property: "opacity"; from: 1; to: 0; easing.type: Easing.InCirc; duration: 400; }
        },
        Transition {
            from: "fadeOut"; to: "fadeIn";
            PropertyAnimation { target: ddmenu; property: "opacity"; from: 0; to: 1; easing.type: Easing.InCirc; duration: 400; }
        }
    ]
    Timer {
        id: fadeMenuTimer
        interval: 400
        onTriggered: {
            doMenuTransition(transitionID);
        }
    }
}
