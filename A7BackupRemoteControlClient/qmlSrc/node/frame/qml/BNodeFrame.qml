import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

BBaseFrame { id: topItem
    text: "Параметры ноды"
    property NodeItem node: null

    visible: (node)

    BLabeledTextField { id: nodeName
        title: "Название:"
        text: (node ? node.name : "не задано")
    }

    BLabeledTextField { id: nodeDescription
        title: "Описание"
        text: (node ? node.description : "")
    }

    BLabeledTextField { id: nodeUuid
        title: "UUID:"
        text: (node ? node.uuid : "не задано")
        readOnly: true
    }

    BBaseLabel {
        text: "Дата последнего бэкапа: "
    }
    BBaseLabel {
        text: "Статус: "
    }

    BBaseLabel {
        text: "Дата последнего keepalive: "
    }

    BBaseLabel {
        text: "host:port"
    }


    RowLayout {
        Layout.fillWidth: true
        BBaseRoundButton {
            text:"🗘"
            onClicked: update()
        }
        Item {Layout.fillWidth: true}
        BBaseRoundButton {
            text:"🗑"
            onClicked: remove()
        }
        BBaseRoundButton {
            text:"🖬"
            onClicked: save()
        }
    }

    function update() {
        B.serverConnection.requestNodeInfo(node.uuid);
    }

    function remove() {
        B.serverConnection.requestRemoveNode(node.uuid)
    }

    function save() {
        let nodeInfo = {};
        nodeInfo.name = nodeName.text;
        nodeInfo.description = nodeDescription.text;

        B.serverConnection.requestNodeUpdate(node.uuid, nodeInfo);
    }
}
