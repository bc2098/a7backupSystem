import QtQuick 2.15
import BRCC 1.0

Item { id: topItem
    property BackupConnection connection: null

    function getNodeList() {
        connection.requestNodeList();
    }

    function getNodeBackupList(nodeUuid) {
        connection.requestNodeBackupList(nodeUuid);
    }

    function addNode(name, uuid) {
        connection.requestAddNode(name, uuid);
    }

    function removeNode(nodeUuid) {
        connection.requestRemoveNode(nodeUuid);
    }

    function getNodeInfo(nodeUuid) {
        connection.requestNodeInfo(nodeUuid);
    }

    function getBackupInfo(nodeUuid, backupId) {
        connection.requestBackupInfo(nodeUuid, backupId);
    }

    function createBackup(nodeUuid) {
        connection.requestCreateBackup(nodeUuid);
    }

    function removeBackup(nodeUuid, backupId) {
        connection.requestRemoveBackup(nodeUuid, backupId);
    }

    function moveBackup(nodeUuid, backupId, targetState) { //targetState суточный, недельный, месячный; только вперед
        connection.requestMoveBackup(nodeUuid, backupId, targetState);
    }
}
