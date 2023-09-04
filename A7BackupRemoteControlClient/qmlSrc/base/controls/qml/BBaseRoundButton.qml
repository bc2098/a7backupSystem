import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

RoundButton { id: topItem
    font.pixelSize: B.dp * 22
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0
    font {
        family: "Noto Sans"
    }


    implicitHeight:  B.controlBaseHeight*0.8
    implicitWidth:  B.controlBaseHeight*0.8

    background: Rectangle {
        color: topItem.down ? "gray" : "white"
        radius: topItem.height / 2

    }

}
