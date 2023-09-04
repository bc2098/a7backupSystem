import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

TextField { id: topItem
    Layout.fillWidth: true
    background: BBaseControlBackground {
        border {
            color: topItem.activeFocus ? "black" : "darkgray"
            width: B.dp * 1
        }
    }
    selectByMouse: true
    selectedTextColor: "white"
    selectionColor: "#555555"
}
