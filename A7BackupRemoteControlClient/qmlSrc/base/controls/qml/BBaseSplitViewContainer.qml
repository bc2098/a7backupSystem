import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Item { id: topItem
    height: parent.height
    SplitView.preferredWidth: parent.width / SplitView.view.count
}
