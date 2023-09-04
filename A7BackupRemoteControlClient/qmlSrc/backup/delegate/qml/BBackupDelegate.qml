import QtQuick 2.15
import BRCC 1.0
import "qrc:/qml"

BBaseListDelegate { id: topItem
    property BackupItem backup: model ? model.backup ? model.backup: null : null

    BBaseLabel { id: lid
        anchors {
            left: parent.left
            top: parent.top
        }

        text: backup ? backup.id : ""
    }

}
