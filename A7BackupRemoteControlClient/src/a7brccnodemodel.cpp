#include "a7brccnodemodel.h"

#include "src/a7backupserverconnection.h"
#include "src/a7brccnodebackupmodel.h"
#include "src/a7brccnodeitem.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

A7BRCCNodeModel::A7BRCCNodeModel(A7BackupServerConnection *serverConnection)
    : m_serverConnection{serverConnection}
{

}

A7BRCCNodeModel::~A7BRCCNodeModel() {

}

void A7BRCCNodeModel::append(A7BRCCNodeItem *node, bool needEvent) {
    if(needEvent) {beginInsertRows(QModelIndex(), rowCount(), rowCount());}
        node->setNodeIndex(rowCount());
        m_map_nodes.insert(node->uuid(), node);
        m_index_nodes.append(node);
    if(needEvent) {endInsertRows();}
}


void A7BRCCNodeModel::remove(A7BRCCNodeItem *node) {
    if(!node){
        return;
    }
    beginRemoveRows(QModelIndex(), node->getNodeIndex(), node->getNodeIndex());

        m_map_nodes.remove(node->uuid());
        m_index_nodes.removeAt(node->getNodeIndex());

    endRemoveRows();
    int i_removed=node->getNodeIndex();

    for(int i=i_removed;i<m_index_nodes.count();i++){
        A7BRCCNodeItem *node = m_index_nodes.at(i);
        node->setNodeIndex(i);
    }

    node->deleteLater();
}

void A7BRCCNodeModel::removeAll()
{
    if(!m_index_nodes.count()){
        return;
    }
    beginResetModel();
        m_index_nodes.clear();
        m_map_nodes.clear();
    endResetModel();
}

int A7BRCCNodeModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return 0;
    }
    //Q_UNUSED(parent)
    return m_index_nodes.count();
}

QVariant A7BRCCNodeModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return {};
    }
    if(index.row() < 0 || index.row() >= rowCount()){
        return {};
    }

    if(role != NodeRole) {
        return {};
    }

    A7BRCCNodeItem * node = m_index_nodes.at(index.row());//m_nodes.at(index.row());

    return QVariant::fromValue(node);
}

QHash<int, QByteArray> A7BRCCNodeModel::roleNames() const
{
    return {{NodeRole, "node"}};
}

void A7BRCCNodeModel::populateFromJsonArray(const QJsonArray &jarray) {
    if(jarray.isEmpty()) {
        return;
    }
    removeAll();
    beginResetModel();

    for(int c = 0; c < jarray.count(); c++) {
        A7BRCCNodeItem * item = new A7BRCCNodeItem(m_serverConnection);

//        item->setName(jarray.at(c).toObject().value("name").toString());
//        item->setUuid(jarray.at(c).toObject().value("uuid").toString());
//        item->setLastKeepAlive(QDateTime::currentDateTime());
        item->fromJson(jarray.at(c).toObject());
        append(item,false);
    }
    endResetModel();
}


void A7BRCCNodeModel::populateFromJson(const QJsonObject &jobject) {
    if(jobject.isEmpty()) {
        return;
    }

    QJsonArray nodeList = jobject.value("nodes").toArray();

    populateFromJsonArray(nodeList);
}

void A7BRCCNodeModel::populateFromJsonString(const QString &jstring) {
    if(jstring.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument jo = QJsonDocument::fromJson(jstring.toUtf8(), &error);

    if(error.error != QJsonParseError::NoError) {
        qDebug() << "error parse JSON data!" << error.errorString();
        return;
    }

    QJsonArray data = jo.object().value("data").toObject().value("nodes").toArray();
    qDebug() << data;

    populateFromJsonArray(data);
}

void A7BRCCNodeModel::nodeInfoChanged(const QJsonObject &jnodeInfo) {
    if(jnodeInfo.isEmpty()) {
        return;
    }

//    QJsonObject jnode = jnodeInfo.value("node").toObject();

    A7BRCCNodeItem * node = m_map_nodes.value(jnodeInfo.value("uuid").toString());

    if(!node) {
        qDebug() << "NODE NOT FOUND!!! UUid" << jnodeInfo.value("uuid").toString();
        return;
    }

//    node->setLastKeepAlive(QDateTime::currentDateTime());
    node->fromJson(jnodeInfo);
}

void A7BRCCNodeModel::nodeBackupListChanged(const QJsonObject &nodeBackups) {
    A7BRCCNodeItem * node = m_map_nodes.value(nodeBackups.value("uuid").toString());
    if(!node) {
        qDebug() << "A7BRCCNodeModel::nodeBackupListChanged() node NOT FOUND!";
        return;
    }

    QJsonArray jbackupArray = nodeBackups.value("backups").toArray();
    node->backupModel()->populateFromJsonArray(jbackupArray);
}

bool A7BRCCNodeModel::removeByUuid(const QString &uuidString)
{
    A7BRCCNodeItem * node = m_map_nodes.value(uuidString);
    if(!node){
        return false;
    }
    remove(node);
    return true;
}

void A7BRCCNodeModel::nodeAdded(const QJsonObject &jdata) {
    qDebug() << "A7BRCCNodeModel::nodeAdded" << jdata;
    if(jdata.isEmpty()) {
        return;
    }

    //QJsonObject jnode = jdata.value("node").toObject();

    A7BRCCNodeItem * node = m_map_nodes.value(jdata.value("uuid").toString());

    if(node) {
        nodeInfoChanged(jdata);
        return;
    }

    A7BRCCNodeItem * item = new A7BRCCNodeItem(m_serverConnection);
    //connect to backup list chaged / backup changed
//    item->setName(jdata.value("name").toString());
//    item->setUuid(jdata.value("uuid").toString());
//    item->setLastKeepAlive(QDateTime::currentDateTime());
    item->fromJson(jdata);
    append(item, true);
}

void A7BRCCNodeModel::nodeRemoved(const QJsonObject &jdata) {
    qDebug() << "node removed" << jdata;
    if(jdata.isEmpty()) {
        return;
    }

//    QJsonObject jnode = jdata.value("node").toObject();

    A7BRCCNodeItem * node = m_map_nodes.value(jdata.value("uuid").toString());

    if(!node) {
        return;
    }

    remove(node);
}

void A7BRCCNodeModel::nodeBackupAdded(const QJsonObject &jdata) {
    QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();

    A7BRCCNodeItem * node = m_map_nodes.value(nodeUuid);

    if(!node) {
        qDebug() << "Node with given uuid is not exist! " << nodeUuid;
        return;
    }

    node->backupAdded(jdata);
}

void A7BRCCNodeModel::nodeBackupRemoved(const QJsonObject &jdata) {
    QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();

    A7BRCCNodeItem * node = m_map_nodes.value(nodeUuid);

    if(!node) {
        qDebug() << "Node with given uuid is not exist! " << nodeUuid;
        return;
    }

    node->backupRemoved(jdata);
}

void A7BRCCNodeModel::backupInfoReceived(const QJsonObject &jdata) {
    QString nodeUuid = jdata.value("node").toObject().value("uuid").toString();

    A7BRCCNodeItem * node = m_map_nodes.value(nodeUuid);

    if(!node) {
        qDebug() << "Node with given uuid is not exist! " << nodeUuid;
        return;
    }

    node->backupInfoChanged(jdata);
}


A7BackupServerConnection *A7BRCCNodeModel::serverConnection() {
    return m_serverConnection;
}

void A7BRCCNodeModel::requestBackupListsForAllNodes() {
    for(int i = 0; i < m_index_nodes.count(); i++) {
        QUuid nodeUuid = m_index_nodes.at(i)->uuid();
        serverConnection()->requestNodeBackupList(nodeUuid.toString());
    }
}
