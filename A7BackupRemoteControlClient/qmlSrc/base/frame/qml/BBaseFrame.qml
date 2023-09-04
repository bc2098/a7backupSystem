import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Pane { id: topItem
    Layout.fillWidth: true
    implicitHeight: ltitle.height + (isCollapsed ? 0 : contentItem.implicitHeight) + padding * 2.5
    property string text: "default title"

    property bool isCollapsed: false

    background: BBaseControlBackground {}

    contentItem: ColumnLayout { id: contentBody
        spacing: B.dp * 5
        clip: true
    }

    RowLayout { id: titleRow
        width: parent.width
        TapHandler {
            onTapped: topItem.isCollapsed = !topItem.isCollapsed
        }

        BBaseLabel {
            width: parent.width
            text: topItem.text
            font.bold: true
        }

        Item {Layout.fillWidth: true}

        BBaseLabel { id: ltitle
            text: " 〉"
            rotation: topItem.isCollapsed ? 0 : 90
            Layout.preferredHeight: height
            transformOrigin: Item.Center
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter
        }
    }


    padding: B.dp * 10
}
