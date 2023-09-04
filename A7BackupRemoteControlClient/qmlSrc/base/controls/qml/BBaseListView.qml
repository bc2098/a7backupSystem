import QtQuick 2.15
import QtQuick.Controls 2.15
import BRCC 1.0
import "qrc:/qml"

ListView { id: topItem
    signal addItem()
    property real margins: B.dp * 5

    clip: true
    spacing: B.dp * 5
    leftMargin: margins
    topMargin: margins
    rightMargin: margins
    bottomMargin: margins + addButton.height

    Button { id: addButton
        anchors{
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: topItem.margins
        }
        height: B.controlBaseHeight
        width: parent.width
        text: "+"
        background: Rectangle {
            color: addButton.down ? "gray" : "transparent"
            radius: B.controlRadius
        }
        font {
            pixelSize: B.dp * 48
        }

        onClicked: topItem.addItem()
        visible: (topItem.model)
    }

    ScrollBar.vertical: ScrollBar{}
}
