import QtQuick 2.3

MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true

    property int hoverStatus: 0

    // this has so many applications
    // not calling "doHover()" is the same as "doHover(0)"
    // hoverstat: 0 = off; odd # for entered or hovered; even # for exited
    function doHover(hoverstat) {
        if (hoverstat === 0) {
            console.log("no hover effect")
            return;
        }
        else if (hoverstat === 1) {
            parent.color = butHvrColor
        }
        else if (hoverstat === 2) {
            parent.color = butColor
        }
        else if (hoverstat === 3) {
            parent.border.color = butHvrBrdrColor
        }
        else if (hoverstat === 4) {
            parent.border.color = butBrdrColor
        }
    }

    signal buttonClick() // unsure if needed - possibly a placeholder signal
}

