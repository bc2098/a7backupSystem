import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import BRCC 1.0
import "qrc:/qml"

Window { id: topItem
    width: B.dp * 800
    height: B.dp * 480
    visible: true
    title: qsTr("Hello World")

    SplitView {
        anchors.fill: parent
        BBaseSplitViewContainer { id: nodeListContainer
            BBaseListView { id: lvnodes
                anchors {
                    fill: parent
                }
                model: B.serverConnection.nodeModel

                delegate: BNodeDelegate {}
            }
        }

        BBaseSplitViewContainer { id: nodeBackupListContainer
            property NodeItem parentNode: lvnodes.currentItem ? lvnodes.currentItem.node : null

            BBaseListView { id: lvbackups
                anchors {
                    fill: parent
                }
                model: parent.parentNode ? parent.parentNode.backupModel : null

                delegate: BBackupDelegate {}
            }
        }

        BBaseSplitViewContainer {
            BBaseFlickable {
                anchors.fill: parent
                anchors.margins: B.dp * 5
                BNodeFrame {
                    node: nodeBackupListContainer.parentNode
                }

                BBackupFrame {
                    backup: lvbackups.currentItem ? lvbackups.currentItem.backup : null
                }
            }
        }
    }


    BRequestsLogic { id: rlogic
        connection: B.serverConnection
    }

    //****************************************** TEST FUNCTIONS ******************************************


    BDebugWindow {
        connection: B.serverConnection
        currentNode: lvnodes.currentItem ? lvnodes.currentItem.node : null
        currentBackup: lvbackups.currentItem ? lvbackups.currentItem.backup : null
        x: topItem.x + topItem.width
        y: topItem.y
    }

    Component.onCompleted: {
//        serverConnection.testRead(testNodeList);
//        serverConnection.testRead(testNodeBackupList);
//        testNodeUpdateTimer.start();
        B.serverConnection.connectToServer("127.0.0.1", 12345, "login", "password");
    }

    Timer { id: testNodeUpdateTimer
        interval: 1000
        repeat: true
        running: false
        onTriggered: serverConnection.testRead(testNodeUpdate);
    }

    function removeNode(uuid, model) {
        model.removeByUuid(uuid);
    }

    function testNodeRemovedEvent() {
        B.serverConnection.testRead(testNodeRemoved);
    }

    function testNodeAddedEvent() {
        B.serverConnection.testRead(testNodeAdded);
    }

    function testNodeBackupAddedEvent(){
        B.serverConnection.testRead(testNodeBackupAdded);
    }

    function testNodeBackupRemovedEvent() {
        B.serverConnection.testRead(testNodeBackupRemoved);
    }

    property string testNodeList: '
{
    "command": 33,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "nodes": [
            { "id":"b774aece-9525-4ecf-96a2-88e6b03af65e", "name":"name1"},
            { "id":"d581b638-87df-45ea-83bd-7cd7503db6da", "name":"name2"},
            { "id":"d304a09d-a665-4d07-9b12-da884275ba30", "name":"name3"}
        ]
    }
}'

        property string testNodeUpdate: '
{
    "command": 2,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"d581b638-87df-45ea-83bd-7cd7503db6da",
            "name":"name1"
        }
    }
}'

        property string testNodeBackupList: '
{
    "command": 3,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"d581b638-87df-45ea-83bd-7cd7503db6da",
            "backups": [
                { "id": "backup1id", "dt": "backup1dt", "size":10 },
                { "id": "backup2id", "dt": "backup2dt", "size":11 },
                { "id": "backup3id", "dt": "backup3dt", "size":12 },
                { "id": "backup4id", "dt": "backup4dt", "size":13 },
                { "id": "backup5id", "dt": "backup5dt", "size":14 }
            ]
        }
    }
}'

        property string testNodeBackupAdded: '
{
    "command": 4,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"dbfb4724-8e79-4439-9eb1-87af74cc18d6",
            "backup": { "id": "backup6id", "dt": "backup6dt", "size":16 }
        }
    }
}'

        property string testNodeBackupRemoved: '
{
    "command": 5,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"dbfb4724-8e79-4439-9eb1-87af74cc18d6",
            "backup": { "id": "backup6id", "dt": "backup6dt", "size":16 }
        }
    }
}'


        property string testNodeAdded: '
{
    "command": 65,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"dbfb4724-8e79-4439-9eb1-87af74cc18d6",
            "name":"name4"
        }
    }
}'

        property string testNodeRemoved: '

{
    "command": 7,
    "dt": "17-08-2023T12:34:56Z",
    "data": {
        "node": {
            "id":"dbfb4724-8e79-4439-9eb1-87af74cc18d6",
            "name":"name4"
        }
    }
}'
}
