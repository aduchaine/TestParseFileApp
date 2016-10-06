import QtQuick 2.7
import QtQuick.Layouts 1.3
import "qrc:/DefinedComponents/"
import "qrc:/StaticComponents/"
import "qrc:/SupportComponents/"
import UIProcess 1.0

ColumnLayout { /*page begin*/
    id: test_000

    property int pageID
    property int responseID

    pageID: 0

    Spacer { id: spacer1pg0; /*boxHeight: someitem.height * 1;*/ opacity: .0; }

    ReadOnlyText {
        id: rot2pg0
        txtText: "rot2pg0"
        txtSize: 16
        txtColor: "black"
        textID: 1
    }

    ReadOnlyText {
        id: rot3pg0
        txtText: "rot4pg0"
        txtSize: 16
        txtColor: "black"
        textID: 2
    }

    Spacer { id: spacer4pg0; /*boxHeight: someitem.height * 1;*/ opacity: .0; }

    Connections /*uiProcess*/ {
        target: uiProcess

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

    } /*end connections*/

    KeyboardProp1 {
        id: keyproppg0
    }

    Component.onCompleted: {
        stackView.doNewPageProcess(pageID);
        keyproppg0.forceActiveFocus();
    }
}
