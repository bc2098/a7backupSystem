import QtQuick 2.15
import BRCC 1.0
import "qrc:/qml"

BBaseListDelegate { id: topItem
    property NodeItem node: model.node

    BBaseLabel { id: lname
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        text: node ? node.name : ""
    }

    BBaseLabel { id: ldt
        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        text: node ? node.lastKeepAlive : ""
    }

    BBaseLabel { id: luuid
        anchors {
            right: parent.right
            top: parent.top
        }

        text: (node) ? node.uuid : ""
    }
}
