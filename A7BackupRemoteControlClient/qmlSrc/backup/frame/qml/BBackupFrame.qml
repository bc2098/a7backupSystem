import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

BBaseFrame { id: topItem
    text: "Параметры бэкапа"
    property BackupItem backup: null
    visible: (backup)

    BBaseLabel {
        text: "id: "+backup.id
    }

    BBaseLabel {
        text: "Время: "+Date(backup.id)
    }

    BBaseLabel {
        text: "Тип: "+backup.backupType
    }

    BBaseLabel {
        text: "Файл: "+backup.fileName
    }

    BBaseLabel {
        text: "Размер: "+backup.size
    }

    BBaseLabel {
        text: "Статус: "+backup.status
    }

    //button row

/*
* update info
* remove -> dialog
* move (change type)
*/
}
