import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Flickable { id: topItem
    default property alias contentBodyData: contentBody.data

    contentHeight: contentBody.implicitHeight
    contentWidth: width

    ColumnLayout { id: contentBody
        width: parent.width
    }

    ScrollBar.vertical: ScrollBar{}
}
