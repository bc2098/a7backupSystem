import QtQuick 2.15
import BRCC 1.0
import "qrc:/qml"

Item { id: topItem
    height: B.listDelegateHeight
    width: parent ? parent.width : 0

    property alias contentItem: contentItem
    default property alias contentData: contentItem.data

    signal leftButtonClicked()
    signal rightButtonClicked()

    property ListView parentList: ListView.view ? ListView.view : null

    property bool isCurrent: parentList ? parentList.currentIndex === index : false

    BBaseControlBackground { // back
        anchors.fill: parent
        border.color: (parent.isCurrent) ? "black" : "darkgray"
        border.width: (parent.isCurrent) ?  B.dp * 2 : B.dp * 1
    }

    Item { id: contentItem
        anchors {
            fill: parent
            margins: B.dp * 5
        }
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onTapped:{
            if(eventPoint.event.button == Qt.LeftButton) {
                leftButtonClicked();
            } else {
                rightButtonClicked();
            }
        }
    }

    onLeftButtonClicked: {
        if(!parentList) { return; }
        parentList.currentIndex = index;
    }
}
