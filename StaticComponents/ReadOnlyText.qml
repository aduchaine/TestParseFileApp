import QtQuick 2.7
import QtQuick.Layouts 1.3

Rectangle {
    id: rotextBox
    color: "transparent"

    Layout.leftMargin: 30
    Layout.rightMargin: 30
    Layout.fillWidth : true
    Layout.fillHeight : true

    property alias txtBox: rotextBox
    property alias txtBoxColor: rotextBox.color
    property alias txtProp: justText    
    property alias txtText: justText.text
    property alias txtSize: justText.font.pixelSize
    property alias txtBold: justText.font.bold
    property alias txtColor: justText.color
    property alias txtWrap: justText.wrapMode

    property int textID                   // text field ID - possibly the question ID
    property string dynamicData           // this may become necessary when communicating with C++

    Connections {
        target: uiProcess

        onSendToPrompt: {
            getPromptData(n_data, str_data);            
        }

        onDynamicText: { // may need to pass or use d_data as condition for how this works
            if (n_data === textID) {
                //console.log("onDynamicText: textID " + n_data + " string: " + str_data)
                if (d_data === 0) {
                    dynamicData = str_data;
                    doTextTransition(str_data);
                }
                else if (d_data === 1) {
                    changeText(d_data, str_data);
                }
            }
        }
    }

    // probably need to define text size based on char#
    function getPromptData(prompt_id, string_data) {
        if (prompt_id === textID) {
            txtText = string_data;
        }
    }
    function changeText(how, string) {
        //console.log("changeText: " + string);
        if (string.length > 60) {
            justText.font.pixelSize = 13;
        }
        else if (string.length > 40) {
            justText.font.pixelSize = 14;
        }
        else if (string.length > 30) {
            justText.font.pixelSize = 16;
        }

        switch (how) {
        case 0: // replace
            txtText = string;
            break;
        case 1: // append under - newline
            txtText += "\n" + string;
            break;
        case 2: // append
            txtText += string;
            break;
        case 3: // append above - newline
            txtText = string + "\n" + butTxt;
            break;
        }
    }
    function doTextTransition(string) {
        toggleTransition(string);
    }
    function toggleTransition(string) {
        if (rotextBox.state === "fadeIn") {
            rotextBox.state = "fadeOut";
            fadeTextTimer.start();
        }
        else {
            rotextBox.state = "fadeIn";
            changeText(0, dynamicData);
        }
    }
    states: [
        State {
            name: "fadeIn"
            PropertyChanges { target: rotextBox; opacity: 1; }
        },
        State {
            name: "fadeOut"
            PropertyChanges { target: rotextBox; opacity: .0; }
        }
    ]
    transitions: Transition {
        NumberAnimation { target: rotextBox; properties: "opacity"; easing.type: Easing.InCirc; duration: 400; }
    }
    Timer {
        id: fadeTextTimer
        interval: 400
        onTriggered: {
            doTextTransition(txtText);
        }
    }
    Text {        
        id: justText
        horizontalAlignment: Text.AlignHCenter
        anchors {
            left: parent.left
            right: parent.right
        }        
        opacity: 1
        font.bold: false
        font.pixelSize: 20
        wrapMode: Text.WordWrap        
        color: "black"
        text: "justText"
    }
    Component.onCompleted: {
        rotextBox.state = "fadeIn";
    }
}
