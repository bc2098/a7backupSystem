import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Rectangle { id: topItem
    implicitWidth: B.dp * 150
    radius: B.controlRadius
    color: "lightgray"
    border.color: "darkgray"
    border.width: B.dp * 2
}
