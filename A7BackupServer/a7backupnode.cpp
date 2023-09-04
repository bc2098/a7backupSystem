#include "a7backupnode.h"
#include <QCryptographicHash>
#include "a7backupudpserver.h"
#include "a7backupnodesserver.h"

quint64 A7BackupNode::getLastBackupTime() const {
    return m_lastBackupTime;
}

void A7BackupNode::setLastBackupTime(quint64 newLastBackupTime) {
    m_lastBackupTime = newLastBackupTime;
}

const QUuid& A7BackupNode::getNodeUuid() const {
    return m_nodeUuid;
}

void A7BackupNode::setNodeUuid(const QUuid &newNodeUuid) {
    m_nodeUuid = newNodeUuid;
}

NodeStatus A7BackupNode::getNodeStatus() const {
    return m_nodeStatus;
}

void A7BackupNode::setNodeStatus(NodeStatus newNodeStatus) {
    m_nodeStatus = newNodeStatus;
}

const NetworkPin& A7BackupNode::getNetworkPin() const {
    return m_networkPin;
}

const QHostAddress &A7BackupNode::getRemoteIp() const {
    return m_remoteIp;
}

void A7BackupNode::setRemoteIp(const QHostAddress &newRemoteIp) {
    m_remoteIp = newRemoteIp;
}

quint16 A7BackupNode::getRemotePort() const {
    return m_remotePort;
}

void A7BackupNode::setRemotePort(quint16 newRemotePort) {
    m_remotePort = newRemotePort;
}

qint64 A7BackupNode::getLastUdpInTime() const {
    return m_lastUdpInTime;
}

void A7BackupNode::setLastUdpInTime(qint64 newLastUdpInTime) {
    m_lastUdpInTime = newLastUdpInTime;
}

qint64 A7BackupNode::getLastUdpOutTime() const {
    return m_lastUdpOutTime;
}

void A7BackupNode::setLastUdpOutTime(qint64 newLastUdpOutTime) {
    m_lastUdpOutTime = newLastUdpOutTime;
}

QString A7BackupNode::getNodeDescription() const {
    return m_nodeDescription;
}

void A7BackupNode::setNodeDescription(const QString &newNodeDescription) {
    m_nodeDescription = newNodeDescription;
}

A7BackupUdpServer *A7BackupNode::udpServer() const
{
    return m_nodeServer->udpServer();
}

A7RemoteControlServer *A7BackupNode::remoteControlServer() const
{
    return nodeServer()->remoteControlServer();
}

A7BackupNodesServer *A7BackupNode::nodeServer() const
{
    return m_nodeServer;
}

quint64 A7BackupNode::getNextSeq()
{
    return (++m_seq);
}

const A7BackupNode::RemoteNodeState &A7BackupNode::getRemoteNodeState() const
{
    return m_remoteNodeState;
}

A7BackupSave *A7BackupNode::addNewBackupSave(qint64 backup)
{
    A7BackupSave * backupSave = new A7BackupSave(this, backup);
    m_notCompleted.insert(backupSave);
    return backupSave;
}

A7BackupSave *A7BackupNode::at(qint64 backup) const
{
    return m_backupList.value(backup,nullptr);
}

int A7BackupNode::receiveLogin(const NetworkPacketToServer &packet)
{
    //const NetworkPacketLoginToServer & login = packet.data.login;

    if(!checkLogin(packet)) {
        return -1;
    }

    NetworkPacketToClient answer;
    answer.title.copy(packet.title);
    answer.title.flags = ResponseFLG;
    answer.data.login.loginStatus = NoError;
    answer.data.login.pin = m_networkPin;
    udpServer()->sendPacket(answer,this);

    return NoError;

}

int A7BackupNode::receiveKeepAlive(const NetworkPacketToServer &packet) {
   const NetworkPacketKeepAliveToServer &ka = packet.data.keepAlive;



   m_remoteNodeState.lastUpdate = packet.title.time;
   m_remoteNodeState.actionType =  static_cast<A7BackupNetworkAction>(ka.currentActionType);
   m_remoteNodeState.paercents = ka.actionPercent;
   m_remoteNodeState.actionTime = ka.currentActionTime;

   sendNodeEvent();


   if(ka.lastBackupTime==-1){
       return NoError;
   }
   if(ka.lastBackupTime == m_lastBackupTime){
       return NoError;
   }
   NetworkPacketToClient query;
   query.title.seq = getNextSeq();
   query.title.time = QDateTime::currentMSecsSinceEpoch();
   query.title.command = BackupListCMD;
   query.title.flags = NoFlagsFLG;
   query.data.backupList.fromBackupTime = m_lastBackupTime;
   udpServer()->sendPacket(query,this);
   return NoError;
   //need list:

}

int A7BackupNode::receiveBackupList(const NetworkPacketToServer &packet) {
    //
    // sta

    const NetworkPacketBackupListUpdateToServer & list = packet.data.backupList;

    for (int i =0; i<=list.count; i++){
        if(list.backups[i] <= m_lastBackupTime){
            continue;
        }
        addNewBackupSave(list.backups[i]);
    }
    return NoError;

        //starrtBackupDownload(int64);
}

int A7BackupNode::receiveBackupHeader(const NetworkPacketToServer &packet) {
    A7BackupSave*backupSave = at(  packet.data.fileHeader.backupTime );
    if(!backupSave){
        return -1;
    }
    return backupSave->recievedFileHeaderPacket(packet);
}

int A7BackupNode::receiveBackupPart(const NetworkPacketToServer &packet) {
    ///
    //nextBackupDownload(m_downloadingBackup);
    A7BackupSave*backupSave = at(  packet.data.fileHeader.backupTime );
    if(!backupSave){
        return -1;
    }
    return backupSave->recievedFilePartPacket(packet);

}

int A7BackupNode::receiveAction(const NetworkPacketToServer &packet)
{
    if(packet.data.action.actionTime < m_lastCommandCreate){
        return -1;
    }
    m_lastAction = NoAction;
    return NoError;
}


void A7BackupNode::sendKeepAlive() {
    NetworkPacketToClient command;
    command.title.time = QDateTime::currentMSecsSinceEpoch();
    command.title.seq = getNextSeq();
    command.title.command = KeepaliveCMD;
    udpServer()->sendPacket(command,this);

}

void A7BackupNode::sendCreateBackupSave()
{
    NetworkPacketToClient command;
    command.title.time = QDateTime::currentMSecsSinceEpoch();
    command.title.seq = getNextSeq();
    command.title.command = ActionCMD;
    command.data.action.actionTime = m_lastCommandCreate;
    command.data.action.actionType = CreateAction;
    udpServer()->sendPacket(command,this);

}

int A7BackupNode::sendNodeEvent()
{

    remoteControlServer()->sendNodeEvent(this);
    return NoError;

}

A7BackupNode::A7BackupNode(A7BackupNodesServer *nodeServer, const QUuid&uuid, qint32 index, qint64 time)
    :A7BaseBackup()
    ,m_nodeServer(nodeServer)
{
    m_nodeUuid = uuid;
    m_networkPin.index = index;
    m_networkPin.time = time;
    m_networkPin.random = QRandomGenerator64::global()->generate();
}

A7BackupNode::A7BackupNode(A7BackupNodesServer *nodeServer, const QJsonObject &json, qint32 index, qint64 time)
    :A7BaseBackup()
    ,m_nodeServer(nodeServer)
{
    fromJson(json);
    m_networkPin.index = index;
    m_networkPin.time = time;
    m_networkPin.random = QRandomGenerator64::global()->generate();
}

A7BackupNode::~A7BackupNode() {

}

int A7BackupNode::remove() {
    free();
    return NoError;
}

void A7BackupNode::free() {
    delete this;
}

int A7BackupNode::createBackupSave()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    if(m_lastCommandCreate + 60000 > time){
        return -1;
    }
    m_lastCommandCreate = time;
    m_lastAction = CreateAction;
    //m_nodeServer-
    sendCreateBackupSave();
    return NoError;
 }

void A7BackupNode::sendPacket(NetworkPacketToClient &packet, A7BackupSave *backupSave)
{
    backupSave->setLastQuerySeq(getNextSeq());
    udpServer()->sendPacket(packet,this);
}

void A7BackupNode::downloaded(A7BackupSave *backupSave)
{
    m_downloaded.insert(backupSave);
    //send event through backupNodeServer to all remote control clients
}

void A7BackupNode::downloading(qint64 partsCount, qint64 partsCompleted, A7BackupSave *backupSave)
{
    //send event through backupNodeServer to all remote control clients
}

int A7BackupNode::onReadPacket(const NetworkPacketToServer &packet)
{
    switch (packet.networkCommand()) {

    case LoginCMD:      return receiveLogin(packet);
    case KeepaliveCMD:  return receiveKeepAlive(packet);
    case FileHeaderCMD: return receiveBackupHeader(packet);
    case FilePartCMD:   return receiveBackupPart(packet);
    case BackupListCMD: return receiveBackupList(packet);
    case ActionCMD:     return NoAnswer;break;
    }
    return RequestError;
}

QJsonObject A7BackupNode::toJson(bool forSave) const { // only node
    QJsonObject json;

    json.insert("uuid", m_nodeUuid.toString());
    json.insert("name", m_nodeName);
    json.insert("lastBackup", QString::number(m_lastBackupTime));
    json.insert("status", static_cast<int>(m_nodeStatus));
    json.insert("description", m_nodeDescription);
    json.insert("password", m_nodePassword);

    if(forSave){
        return json;
    }



    return json;
}

int A7BackupNode::fromJson(const QJsonObject &json) {
    m_nodeUuid   = ( json.value("uuid").toString() );
    m_lastBackupTime = json.value("lastBackup").toString().toLongLong();
    m_nodeStatus = static_cast<NodeStatus>(json.value("status").toInt());
    m_nodeName = json.value("name").toString();
    m_nodeDescription = json.value("description").toString();
    m_nodePassword = json.value("password").toString();

    QJsonArray jbackups = json.value("backups").toArray();

    for(int i = 0; i < jbackups.count(); i++) {
        QJsonObject jo = jbackups.at(i).toObject();
        A7BackupSave * backup = new A7BackupSave(this, jo);
        if(backup->getStatus() != BackupStatus::Downloaded){
            m_notCompleted.insert(backup);
        }
        m_backupList.insert(backup->getBackupTime(), backup);

//        qDebug() << "LOAD BACKUP" << jbackups.at(i).toObject() << backup->toJson();
    }

    qDebug() << "BACKUPS LAODED" << m_backupList.count();

    return NoError;
}

int A7BackupNode::updateFromJson(const QJsonObject &json) {
    qDebug() << "UPDATE FROM JSON" << json;
    m_nodeName = json.value("name").toString();
    m_nodeDescription = json.value("description").toString();

    return NoError;
}

QJsonObject A7BackupNode::allBackupsToJson() const {
    QJsonObject json = toJson();

//    json.insert("uuid", m_nodeUuid.toString());
//    json.insert("name", m_nodeName);
//    json.insert("description", m_nodeDescription);
//    json.insert("lastBackup", QString::number(m_lastBackupTime));
//    json.insert("status", static_cast<int>(m_nodeStatus));

    QJsonArray jbackups;

//    for(QMapIterator<qint64, A7BackupSave*> i(m_backupList); i.hasNext(); i.next()) {
//        jbackups.append(i.value()->toJson());
//        qDebug() << "SAVE BACKUP" << i.value()->toJson();
//    }

    foreach (A7BackupSave * v,  m_backupList) {
        jbackups.append(v->toJson());
//        qDebug() << "SAVE BACKUP" << v->toJson();
    }

    json.insert("backups", jbackups);

    return json;
}

QJsonObject A7BackupNode::backupToJson(qint64 backupId) const {
    A7BackupSave * backup = m_backupList.value(backupId);

    return backup->toJson();
}

bool A7BackupNode::checkLogin(const NetworkPacketToServer &packet) const {
    QByteArray sign;
    sign += QByteArray::number(packet.title.time);
    sign += QByteArray::number(packet.title.seq);
    sign += m_nodePassword.toUtf8();

    qDebug()<<"SIGN: "<<sign;

    QByteArray md5 = QCryptographicHash::hash(sign, QCryptographicHash::Md5);
    qDebug()<<"MD5: "<<md5.toHex();

    QByteArray r_md5( packet.data.login.md5 , 16);
    qDebug()<<"CHECK: "<<r_md5.toHex();

    if(md5 != r_md5) {
        qDebug() << "PASSWORD INCORRECT";
        return false;
    }
    qDebug() << "PASSWORD OK";
    return true;

}

bool A7BackupNode::checkPin(const NetworkPacketToServer &packet) const {
    if(packet.title.pin.random != m_networkPin.random){
        return false;
    }
    if(packet.title.pin.time != m_networkPin.time){
        return false;
    }
    return true;
}

void A7BackupNode::pulse(qint64 time) {

    //TODO:
    if(m_remotePort!=0){
        sendKeepAlive();
    }

    if(m_lastAction){
        sendCreateBackupSave();
    }

    if(!m_downloaded.count() && !m_notCompleted.count()){
        return;
    }

    for(QSet<A7BackupSave*>::iterator i = m_downloaded.begin();i != m_downloaded.end();i++){
        m_notCompleted.remove((*i));
    }

    for(QSet<A7BackupSave*>::iterator i = m_notCompleted.begin();i != m_notCompleted.end();i++){
        (*i)->pulse(time);
    }


}
