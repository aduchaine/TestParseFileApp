import QtQuick 2.7 /*main*/
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import "qrc:/DefinedComponents/"
import "qrc:/StaticComponents/"
import "qrc:/SupportComponents/"
import "qrc:/Pages/"
import UIProcess 1.0


Window {
    property int initialWidth : 600
    property int initialHeight : 450

    id: mainWindow
    width: initialWidth
    height: initialHeight
    minimumWidth: 400
    minimumHeight: 300
    maximumWidth: 1000
    maximumHeight: 750
    visible: true
    title: qsTr("TestParseFileApp")

    property bool rebuild: false
    property int pageNum: -1  // this may cause problems with other processing - main_update.qml implementation
    property string nextPageURL
    property string prevPageURL

    function getPageURLstring(page_num) {
        var page_string;
        switch(page_num) {
        case 0:
            page_string = "Pages/Test_000.qml";
            break;
        case 1:
            page_string = "Pages/Test_001.qml";
            break;
        }
        return page_string;
    }

    Image {
        id: mainbkgrndTestParseFileApp
        visible: false
        fillMode: Image.Stretch
        anchors.fill: parent
        source: "Resources/background.png"
    }

    StackView {
        id: stackView
        anchors.fill: parent
        anchors.margins: 10
        Layout.fillWidth: true

        signal navigateSideways(int field_id);
        signal savepageInput (int page_id);

        delegate: Transitions {
            id: svDelegate
        }
        function getRandomNumber(scope, shift) {
            svDelegate.randnum = shift + (Math.random() * scope);
            return svDelegate.randnum;
        }
        function setSVDrannum() {
            svDelegate.ranDuration1 = getRandomNumber(200, 100);
            svDelegate.ranDuration2 = getRandomNumber(100, 100);
            svDelegate.ranX = getRandomNumber(mainWindow.width * 2, (-mainWindow.width));
            svDelegate.ranY = getRandomNumber(mainWindow.height * 2, (-mainWindow.height));
        }
        function lockInput(lock) {
            if(lock === true) {
                stackView.enabled = false;
                lockinputTimer.start();
            }
            else {
                stackView.enabled = true;
            }
        }
        Timer {
            id: lockinputTimer
            interval: 800
            onTriggered: {
                stackView.lockInput(false);
            }
        }

        StaticTransitions {
            id: statrans
        }
        function doNewPageProcess(page_id) {
            statrans.doPageStaticTransition();
            pageNum = page_id;
            console.log("doNewPageProcess");
            uiProcess.pageProcess(pageNum);

            // uiProcess.sendPromptData(pageNum);
            // uiProcess.sendMenuData(pageNum);
            // uiProcess.sendFieldData(pageNum);
            // uiProcess.getNextData(pageNum);
        }

        QuitButton {
            id: buttonQuit
            Layout.alignment : Qt.AlignTop
        }

        RowLayout /*main*/ {
            id: mainrlayout10
            anchors.fill: parent
            anchors.margins: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom;

            PreviousButton /*main*/ {
                id: buttonPrevious
                butTxt: "Previous"
                Layout.fillWidth: false;
                Layout.fillHeight: false;
                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom;
            }

            NextButton /*main*/ {
                id: buttonNext
                butTxt: "Continue"
                Layout.fillWidth: false;
                Layout.fillHeight: false;
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom;
                property alias nbutText: buttonNext.butTxt
                mouseNext.onClicked: { stackView.savepageInput(pageNum); }
            }

        } /*end layout*/

        ColumnLayout /*main*/ {
            id: mainclayout14
            anchors.fill: parent

            ReadOnlyText /*main*/ {
                id: mainheader
                txtText: "ParseFile"
                txtSize: 24
                txtColor: "white"
                Layout.alignment : Qt.AlignHCenter | Qt.AlignTop
                Layout.leftMargin: initialWidth * .1
                Layout.rightMargin: initialWidth * .1
                textID: -1
            }

        } /*end layout*/

        UIProcess {
            id: uiProcess

            onValidData: /*main*/ {
                nextPageURL = Qt.resolvedUrl(getPageURLstring(n_data));
                if (nextPageURL.length === 0) {
                    noData();
                }
                else {
                    stackView.lockInput(true);
                    stackView.replace(nextPageURL)
                    stackView.setSVDrannum();
                }
            }

            onPreviousPage: /*main*/ {
                prevPageURL = Qt.resolvedUrl(getPageURLstring(n_data))
                stackView.lockInput(true);
                stackView.replace(prevPageURL)
                stackView.setSVDrannum();
            }

            onInvalidData: /*main*/ {
                buttonNext.state = "on";
                buttonNext.toggleWarning();
            }

            onNoData: /*main*/ {
                buttonNext.state = "on";
                buttonNext.toggleWarning();
            }

            onProcessedData: /*main*/ {
                Qt.quit()
            }

            onSendFocusType: {
                keyinputStackView.setPageFocus(s_data);
            }

        } /*end uiprocess*/

        KeyboardProp1 /*main*/ {
            id: keyinputStackView
        }

        initialItem: SplashPage {
            Component.onCompleted: {
                lockinputTimer.interval = 100000;
                stackView.lockInput(true);
                keyinputStackView.forceActiveFocus()
            }
        }
    }
}
