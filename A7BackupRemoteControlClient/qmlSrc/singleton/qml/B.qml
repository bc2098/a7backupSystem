pragma Singleton
import QtQuick 2.15
import BRCC 1.0

Item {
    property real dp: 1.0

    property real controlBaseHeight: dp * 60
    property real listDelegateHeight: dp * 75
    property real controlRadius: dp * 10

    property alias serverConnection: serverConnection

    BackupConnection { id: serverConnection }
}
