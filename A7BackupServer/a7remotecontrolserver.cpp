#include "a7remotecontrolserver.h"
#include "a7backupnodesserver.h"
#include "a7backupsystembase.h"
#include "a7remotecontrolconnection.h"
#include "a7backupnode.h"
#include "a7backupsave.h"
#include <QJsonDocument>
void A7RemoteControlServer::closeAll()
{
    for(QSet<A7RemoteControlConnection*>::iterator i=m_connections.begin();
         i != m_connections.end(); i++){
        (*i)->free();
    }
    m_connections.clear();

}

QByteArray A7RemoteControlServer::packPacket(int command, const QJsonObject &jo, int flags) const {
    QJsonObject jc;
    jc.insert("command", command);
    jc.insert("dt", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    jc.insert("flags", flags);
    jc.insert("data", jo);
    qDebug() << "pack" << command << flags << jc;
    QByteArray data = QJsonDocument (jc).toJson(QJsonDocument::Compact);
    data.append('\0');
    return data;
}

A7RemoteControlServer::A7RemoteControlServer(A7BackupNodesServer *nodeServer)
    : QObject{nullptr}, A7BaseBackup()
    , m_nodeServer{nodeServer}
{
    m_server = new QTcpServer();
    connect(m_server,&QTcpServer::newConnection,this,&A7RemoteControlServer::newConnection);
}

A7RemoteControlServer::~A7RemoteControlServer() {
    stop();
    closeAll();
    m_server->deleteLater();
}

void A7RemoteControlServer::receivedPacket(A7RemoteControlConnection *connection, int command, const QJsonObject &data) {
    RemoteAnswer answer = m_nodeServer->remoteCommand(command, data);
    int flags = answer.resultFlags | A7BaseBackup::Flags::Response;

//    if(answer.isEmpty()){
//        flags |= A7BaseBackup::Flags::Error;
//    }
    //command+=FlagResponse;


    qDebug() << "send answer cmd:" << "0x"+QString::number(command, 16) << "flags:" << "0x"+QString::number(flags, 16) << answer.result;
    QByteArray adata = packPacket(command, answer.result, flags);
    connection->sendToClient(adata);

    if(command == A7BaseBackup::SystemLogin && (answer.resultFlags & A7BaseBackup::Error)) {
        //disconnect and kill this client
        connection->unregisterServer();
        return;
    }
}

void A7RemoteControlServer::unregisterFromServer(A7RemoteControlConnection *connection) {
    m_connections.remove(connection);
}

void A7RemoteControlServer::sendToAll(int command, QJsonObject jo, int flags) { // for events
    QByteArray data = packPacket(command, jo, flags);
    for(QSet<A7RemoteControlConnection*>::iterator i = m_connections.begin();
         i != m_connections.end(); i++){
        (*i)->sendToClient(data);
    }
}

void A7RemoteControlServer::sendToAllNodeState(const QJsonObject &jdata)
{
    sendToAll(NodeInfo, jdata, Event);
}

void A7RemoteControlServer::sendToAllNodeBackupstate(const QJsonObject &jdata)
{
    sendToAll(BackupInfo, jdata, Event);
}

bool A7RemoteControlServer::start(quint16 port) {
    if(!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Error start listening: " << m_server->errorString();
        return false;
    }
    qDebug() << "remoteControlServer started..." << m_server->serverAddress() << m_server->serverPort();
    return true;
}

void A7RemoteControlServer::stop() {
    if(!m_server->isListening()) {
        return;
    }
    m_server->close();
    qDebug() << "remoteControlServer stopped.";
}

void A7RemoteControlServer::sendNodeEvent(A7BackupNode *node)
{
    QJsonObject nodeInfo;// = node;
    nodeInfo.insert("uuid", node->getNodeUuid().toString());
    nodeInfo.insert("lastUpdate", QString::number(node->getRemoteNodeState().lastUpdate));
    nodeInfo.insert("actionTime", QString::number(node->getRemoteNodeState().actionTime));
    nodeInfo.insert("actionType", node->getRemoteNodeState().actionType);
    nodeInfo.insert("actionPercent", node->getRemoteNodeState().paercents);


    sendToAllNodeState(nodeInfo);


}
//for percents, state and all
void A7RemoteControlServer::sendBackupSaveEvent(A7BackupSave *backupSave)
{
    QJsonObject nodeInfo;// = node;
    A7BackupNode* node = backupSave->parentNode();

    nodeInfo.insert("uuid", node->getNodeUuid().toString());
    nodeInfo.insert("backupTime", QString::number( backupSave->getBackupTime() ) );

    QJsonObject backupInfo;
    backupInfo.insert("status", QString::number(qint64(backupSave->getStatus())));

    nodeInfo.insert("backup", backupInfo);
    sendToAllNodeBackupstate(nodeInfo);

}

void A7RemoteControlServer::newConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    A7RemoteControlConnection * connection = new A7RemoteControlConnection(this,socket);
    m_connections.insert(connection);
}
