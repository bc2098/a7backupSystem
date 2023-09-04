#include "a7backupnodesserver.h"
#include "a7backupnode.h"
#include "a7backupudpserver.h"

A7BackupUdpServer *A7BackupNodesServer::udpServer() const
{
    return m_udpServer;
}

A7RemoteControlServer *A7BackupNodesServer::remoteControlServer() const
{
    return m_remoteControlServer;
}

A7BackupNode *A7BackupNodesServer::searchNodeByUuid(const QUuid &uuid) const {
    return m_hash.value(uuid,nullptr);
}

QJsonObject A7BackupNodesServer::nodeJson(const QUuid &uuid) {
    A7BackupNode * node = searchNodeByUuid(uuid);
    if(node) {
        return node->toJson();
    }
    return {};
}

A7BackupNode *A7BackupNodesServer::searchNodeByIndex(int index) const {
    return m_nodes.value(index,nullptr);
}

A7BackupNode *A7BackupNodesServer::searchNodeByPacket(const NetworkPacketToServer &packet) const {
//    if(packetLen < int (sizeof(NetworkPacketTitleToServer))){
//        return nullptr;
//    }
    if(packet.networkCommand()==LoginCMD){
        return searchNodeByUuid( packet.title.toUuid() );
    }
    A7BackupNode* node = searchNodeByIndex(packet.title.pin.index);
    if(!node){
        return nullptr;
    }
    if(!node->checkPin(packet)){
        return nullptr;
    }
    return node;
}

int A7BackupNodesServer::removeNode(A7BackupNode *node) {
    if(!node){
        return 1;
    }
    m_empty.enqueue(node->getNetworkPin().index);
    m_nodes.replace(node->getNetworkPin().index,nullptr);
    m_hash.remove(node->getNodeUuid());
    return node->remove();

}

A7BackupNodesServer::A7BackupNodesServer(QObject *parent)
    : QObject{parent}, A7BaseBackup()
{
    loadFromFile("");

    m_udpServer = new A7BackupUdpServer(this);
    m_udpServer->start();

    m_remoteControlServer = new A7RemoteControlServer(this);
    m_remoteControlServer->start(12345);

    m_timerId = startTimer(1000);
}

A7BackupNodesServer::~A7BackupNodesServer()
{
    for(int i=0; i<m_nodes.count();i++){
        A7BackupNode* node = m_nodes.at(i);
        if(!node){
            continue;
        }
        node->free();
    }
    m_nodes.clear();
    m_hash.clear();
}

int A7BackupNodesServer::onReadPacket(const NetworkPacketToServer &packet, const QHostAddress &address, const quint16 port) {

    A7BackupNode * node = searchNodeByPacket(packet);
    if(!node){
        return A7BackupNetworkError::PinError;
    }

    node->setRemoteIp(address);
    node->setRemotePort(port);
    return node->onReadPacket(packet);


}



int A7BackupNodesServer::checkRemoteControlLogin(const QString &login, const QString &password) {
    if(login != m_remoteLogin || password != m_remotePassword) {
        return NoError;
    }

    return 1;
}

RemoteAnswer A7BackupNodesServer::remoteCommand(const int command, const QJsonObject &data) {
    //from remote clients (NOT nodes!)

    qDebug() << "A7BackupNodesServer::remoteCommand" << "0x"+QString::number(command, 16) << data;
    RemoteAnswer answer;
    answer.resultFlags = A7BaseBackup::Error;

    switch(command) {
    case A7BaseBackup::NodeList: {
        answer.result = toJson();
        answer.resultFlags = A7BaseBackup::Ok;
        return answer; }
    case A7BaseBackup::NodeInfo: {
        QString nodeUuid = data.value("node").toObject().value("uuid").toString();
        if(nodeUuid.isEmpty()) {
            return answer;
        }
        QJsonObject jnode = nodeJson(nodeUuid);
        if(!jnode.isEmpty()) {
            return answer;
        }
        answer.resultFlags = A7BaseBackup::Ok;
        answer.result = jnode;
        return answer; }
    case  A7BaseBackup::NodeAdd: {
        A7BackupNode *node = addNode();
        if(node) {
            answer.resultFlags = A7BaseBackup::Ok;
            m_remoteControlServer->sendToAll(A7BaseBackup::NodeAdd, node->toJson(), A7BaseBackup::Event);
        }
        return answer; }
    case  A7BaseBackup::NodeUpdate: {
        QJsonObject jdata = data.value("data").toObject();
        QJsonObject jnode = jdata.value("node").toObject();
        QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();
        A7BackupNode *node = searchNodeByUuid(nodeUuid);

        if(!node) {
            return answer; // with error by default
        }

        node->updateFromJson(jnode);

        m_remoteControlServer->sendToAll(A7BaseBackup::NodeInfo, node->toJson(), A7BaseBackup::Event);

        answer.resultFlags = A7BaseBackup::Ok;
        return answer;
    }
    case   A7BaseBackup::NodeRemove: {
        QJsonObject jdata = data.value("data").toObject();
        QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();
        A7BackupNode *node = searchNodeByUuid(nodeUuid);

        if(!node) {
            return answer; // with error by default
        }

        m_remoteControlServer->sendToAll(A7BaseBackup::NodeRemove, node->toJson(), A7BaseBackup::Event);

        if(removeNodeByUuid(nodeUuid) == 0) {
            answer.resultFlags = A7BaseBackup::Ok;
        }
        return answer; }
    case  A7BaseBackup::BackupList: {
        QJsonObject jdata = data.value("data").toObject();
        QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();
        if(nodeUuid.isEmpty()) {
            return answer;
        }
        A7BackupNode *node = searchNodeByUuid(nodeUuid);

        if(!node) {
            return answer; // with error by default
        }

        answer.resultFlags = A7BaseBackup::Ok;

        answer.result = node->allBackupsToJson();
        return answer;}
    case  A7BaseBackup::BackupInfo: break;
    case  A7BaseBackup::BackupAdd: {
        QJsonObject jdata = data.value("data").toObject();
        QJsonObject jnode = jdata.value("node").toObject();
        QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();
        A7BackupNode *node = searchNodeByUuid(nodeUuid);
        if(!node){
            return answer;
        }
        if(node->createBackupSave() == NoError){
            answer.resultFlags = A7BaseBackup::Ok;
        }else{
            answer.resultFlags = A7BaseBackup::Error;
        }
        return answer;
    }
        // позже
        // поставить ноде статус "делает бэкап"
        // нельзя запускать чаще чем раз в минуту
    case   A7BaseBackup::BackupRemove: break;
    case  A7BaseBackup::SystemLogin: {
        //check login
        QJsonObject jdata = data.value("data").toObject();
        QString login = jdata.value("login").toString();
        QString password = jdata.value("password").toString();

        if(checkRemoteControlLogin(login, password)) {
            answer.resultFlags = A7BaseBackup::Ok;
            return answer;
        }

        //disconnect and remove client
        // or
        //disconnect and move client to unauthorized clients list
        return answer;}
    }
    return answer;// return error
}

A7BackupNode* A7BackupNodesServer::addNode() {
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    QUuid uuid = QUuid::createUuid();
    int index = -1;
    A7BackupNode * node = nullptr;

    if(!m_empty.isEmpty()) {
         index = m_empty.dequeue();
    } else {
         index = m_nodes.count();
         m_nodes.append(nullptr);
    }

    node = new A7BackupNode(this,uuid, index, time);
    m_nodes.replace(index, node);
    m_hash.insert(uuid, node);

    m_remoteControlServer->sendToAll(A7BaseBackup::NodeAdd, node->toJson(), A7BaseBackup::Event);

    return node;
}

int A7BackupNodesServer::removeNodeByUuid(const QUuid &nodeUuid) {
    return removeNode( searchNodeByUuid(nodeUuid) );
}

int A7BackupNodesServer::removeNodeByIndex(int nodeIndex) {
    return removeNode( searchNodeByIndex(nodeIndex) );
}

QJsonObject A7BackupNodesServer::toJson() const {
    QJsonObject jo;
    QJsonArray ja;

    for(int i = 0; i < m_nodes.count(); i++){
         A7BackupNode *node = m_nodes.at(i);
         if(!node){
             continue;
         }
         ja.append(node->toJson());
    }
    jo.insert("nodes", ja);
    jo.insert("timeLocal", QDateTime::currentDateTime().toString(Qt::ISODate));
    jo.insert("timeUtc", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    return jo;
}

QJsonObject A7BackupNodesServer::treeDump() const {
    QJsonObject jo;
    QJsonArray nodeArray;

    for(int i = 0; i < m_nodes.count(); i++){
         A7BackupNode *node = m_nodes.at(i);
         if(!node){
             continue;
         }

         QJsonObject jnode = node->allBackupsToJson(); // with backup array

         nodeArray.append(jnode);
    }
    jo.insert("nodes", nodeArray);
    jo.insert("timeLocal", QDateTime::currentDateTime().toString(Qt::ISODate));
    jo.insert("timeUtc", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    return jo;
}

QJsonObject A7BackupNodesServer::nodeBackupList(const A7BackupNode * node) const {
    QJsonObject jnodeBackups = node->allBackupsToJson();

    return jnodeBackups;
}


int A7BackupNodesServer::loadFromJson(const QJsonObject &json) {
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    QJsonArray ja = json.value("nodes").toArray();

    for(int i = 0; i < ja.count(); i++){
         A7BackupNode *node = new A7BackupNode(this,ja.at(i).toObject(), i, time);
//         node->fromJson(ja.at(i).toObject());

         m_hash.insert(node->getNodeUuid(), node);
         m_nodes.append(node);         
    }
    return NoError;
}

int A7BackupNodesServer::saveToFile(const QString &fileName) const {
    QString fn = (fileName.isEmpty()) ? "nodes.json" : fileName;
    QFile file(fn);
    if(!file.open(QFile::WriteOnly)){

         return 1;
    }

//    QJsonDocument jd(toJson());
    QJsonDocument jd(treeDump());
    file.write(jd.toJson());
    file.close();
    return NoError;
}

int A7BackupNodesServer::loadFromFile(const QString &fileName) {
    qDebug() << "node server load from file";
    QString fn = (fileName.isEmpty()) ? "nodes.json" : fileName;
    QFile file(fn);

    if(!file.open(QFile::ReadOnly)){
         return 1;
    }

    QJsonDocument jd = QJsonDocument::fromJson(file.readAll());
    file.close();
    loadFromJson(jd.object());
    return NoError;
}



void A7BackupNodesServer::timerEvent(QTimerEvent *event) {
    saveToFile("");
    qint64 ms= QDateTime::currentMSecsSinceEpoch();
    for(int i = 0; i < m_nodes.count(); i++){
        A7BackupNode*node=m_nodes.at(i);
        if(!node){
            continue;
        }
        node->pulse(ms);
    }
}
