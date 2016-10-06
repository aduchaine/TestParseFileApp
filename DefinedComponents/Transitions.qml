import QtQuick 2.7
import QtQuick.Controls 1.4

StackViewDelegate {
     id: svDelegate

     property int counter: 0
     property int randnum: 0

     property int ranDuration1: 0
     property int ranDuration2: 0
     property int ranX: 0
     property int ranY: 0     

     function getTransition(properties) {        
         if (pageNum === 1 || pageNum === 7) {
             return cometogetherTransition;
         }
         //counter = 14; use for testing
         counter = Math.random() * 4;
         //console.log(" StackViewDelegate: getTransition -> counter = " + counter);
         switch (counter) {
         case 0:
             return getBasicTransition();
         case 1:
             return getFadeTransition();
         case 2:
             return getChaoticTransition();
         case 3: // very nice transition, smooth
             return comeupTransition;         
         case 4: // nice transition, fairly smooth
             return fallgetupTransition;
         case 5: // nice transition, smooth - page 1 and last page are anchored by this transition so, omitted from the rest
             return cometogetherTransition;
         }
     }
     function getBasicTransition() {
         counter = Math.random() * 5
         switch (counter) {
         case 0:
             return pushTransition;
         case 1:
             return popTransition;
         case 2:
             return verticalbottomTransition;
         case 3:
             return verticaltopTransition;
         case 4:
             return replaceTransition;
         }
     }
     function getFadeTransition() {
         counter = Math.random() * 2
         switch (counter) {
         case 0:
             return getAxisFadeTransition();
         case 1:
             return getDiagonalFadeTransition();
         }
     }
     function getAxisFadeTransition() { // the two vertical transitions are smoothest
         counter = Math.random() * 4
         switch (counter) {
         case 0:
             return expandNfadeTransition;
         case 1:
             return expandSfadeTransition;
         case 2:
             return expandEfadeTransition;
         case 3:
             return expandWfadeTransition;
         }
     }
     function getDiagonalFadeTransition() {
         counter = Math.random() * 4
         switch (counter) {
         case 0:
             return expandNWfadeTransition;
         case 1:
             return expandNEfadeTransition;
         case 2:
             return expandSEfadeTransition;
         case 3:
             return expandSWfadeTransition;
         }
     }
     function getChaoticTransition() {
         counter = Math.random() * 2
         switch (counter) {
         case 0:
             return pinballETransition;
         case 1:
             return pinballWTransition;
         }
     }

     function transitionFinished(properties) { // dunno what this does yet
         //console.log(" StackViewDelegate: transitionFinished");
         properties.exitItem.opacity = 1
        // properties.exitItem.x = 0
        // properties.exitItem.y = 0
     }     

     pushTransition: StackViewTransition { // from right
         PropertyAnimation { target: enterItem; property: "x"; from: target.width; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: target.width; duration: 400; }
     }
     popTransition: StackViewTransition { // from left
         PropertyAnimation { target: enterItem; property: "x"; from: -target.width; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: -target.width; duration: 400; }
     }
     replaceTransition: StackViewTransition { // fade
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
     }
     property Component verticalbottomTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "y"; from: target.height; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 400; }
     }
     property Component verticaltopTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "y"; from: -target.height; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: -target.height; duration: 400; }
     }
     property Component expandEfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "x"; from: target.width; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: target.width; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: -200 ; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width ; to: -200; duration: 800 }
     }
     property Component expandWfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         NumberAnimation { target: enterItem; properties: "width"; from: -200; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: -200; duration: 800 }
     }
     property Component expandSfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "y"; from: target.height; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: enterItem; properties: "height"; from: -200 ; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height ; to: -200; duration: 800 }
     }
     property Component expandNfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         NumberAnimation { target: enterItem; properties: "height"; from: -200; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: -200; duration: 800 }
     }
     property Component expandSWfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "y"; from: target.height; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800 }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800 }
     }
     property Component expandNWfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800 }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800 }
     }
     property Component expandNEfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "x"; from: target.width; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: target.width; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800 }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800 }
     }
     property Component expandSEfadeTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "x"; from: target.width; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: target.width; duration: 800; }
         PropertyAnimation { target: enterItem; property: "y"; from: target.height; to: 0; duration: 400; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 400 }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800 }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 400 }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800 }
     }
     property Component comeupTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "y"; from: target.height*2; to: 0; duration: 800; }
        // PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 400; }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 800 }
     }
     property Component fallgetupTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "y"; from: target.height*2; to: 0; duration: 800; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 400; }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 800 }
     }
     property Component cometogetherTransition: StackViewTransition {
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 400; }
         PropertyAnimation { target: enterItem; property: "y"; from: -target.height/2; to: 0; duration: 800; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: -target.height/2; duration: 800; }
         NumberAnimation { target: enterItem; properties: "height"; from: target.height*2; to: target.height; duration: 800; }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: target.height*2; duration: 800; }
     }
     property Component pinballETransition: StackViewTransition { // not really pinball but can make it better later
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 800; }
         PropertyAnimation { target: enterItem; property: "x"; from: svDelegate.ranX; to: 0; duration: svDelegate.ranDuration1; }
         PropertyAnimation { target: enterItem; property: "y"; from: svDelegate.ranY; to: 0; duration: svDelegate.ranDuration2; }
         PropertyAnimation { target: exitItem; property: "y"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 800; }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800; }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800; }
     }
     property Component pinballWTransition: StackViewTransition { // not really pinball but can make it better later
         PropertyAnimation { target: enterItem; property: "opacity"; from: 0; to: 1; duration: 800; }
         PropertyAnimation { target: exitItem; property: "opacity"; from: 1; to: 0; duration: 600; }
         PropertyAnimation { target: enterItem; property: "x"; from: (svDelegate.ranX);  to: 0; duration: svDelegate.ranDuration1; }
         PropertyAnimation { target: enterItem; property: "y"; from: (svDelegate.ranY); to: 0; duration: svDelegate.ranDuration2; }
         PropertyAnimation { target: exitItem; property: "x"; from: 0; to: target.width; duration: 800; }
         NumberAnimation { target: enterItem; properties: "width"; from: 0; to: target.width; duration: 800; }
         NumberAnimation { target: exitItem; properties: "width"; from: target.width; to: 0; duration: 800; }
         NumberAnimation { target: enterItem; properties: "height"; from: 0; to: target.height; duration: 800; }
         NumberAnimation { target: exitItem; properties: "height"; from: target.height; to: 0; duration: 800; }
     }
}
