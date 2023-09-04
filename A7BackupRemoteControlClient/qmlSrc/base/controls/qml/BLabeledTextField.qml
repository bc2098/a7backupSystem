import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Item { id: topItem
    property string title: "Title:"
    Layout.fillWidth: true
    implicitHeight: titleItem.height + textFieldItem.height + B.dp * 5

    property alias textField: textFieldItem
    property alias text: textFieldItem.text
    property alias readOnly: textFieldItem.readOnly

    BBaseLabel { id: titleItem
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: B.dp * 5
            right: parent.right
        }
        text: topItem.title
    }

    BBaseTextField { id: textFieldItem
        anchors {
            left: parent.left
            right: parent.right
            top: titleItem.bottom
            topMargin: B.dp * 5
        }

        width: parent.width
        TapHandler {
            acceptedButtons: Qt.RightButton
            onSingleTapped: {
                let selectStart = textFieldItem.selectionStart;
                let selectEnd = textFieldItem.selectionEnd;
                let curPos = textFieldItem.cursorPosition;

                context.x = eventPoint.position.x
                context.y = eventPoint.position.y+textFieldItem.height

                context.open(textFieldItem);

                textFieldItem.cursorPosition = curPos;
                textFieldItem.select(selectStart,selectEnd);
            }
        }
    }

    Menu { id: context
        MenuItem {
            text: "Вырезать"
            enabled: !topItem.readOnly
            onTriggered: textFieldItem.cut()
        }

        MenuItem {
            text: "Копировать"
            onTriggered: textFieldItem.copy()
        }
        MenuItem {
            text: "Вставить"
            enabled: !topItem.readOnly
            onTriggered: textFieldItem.paste()
        }
    }
}

