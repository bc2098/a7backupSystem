#include "a7brccnodebackupmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include "src/a7brccnodebackupitem.h"

A7BRCCNodeBackupModel::A7BRCCNodeBackupModel(const A7BRCCNodeItem *nodeItem)
    : m_nodeItem{nodeItem}
{}

A7BRCCNodeBackupModel::~A7BRCCNodeBackupModel() {
    qDebug() << "backup model destructor";
    removeAll();
}

void A7BRCCNodeBackupModel::append(A7BRCCNodeBackupItem *item, bool needEvent) {
    if(needEvent) { beginInsertRows(QModelIndex(), rowCount(), rowCount()); }
        item->setBackupIndex(rowCount());
        m_map_backups.insert(item->id(), item);
        m_index_backups.append(item);
    if(needEvent) { endInsertRows(); }

}

void A7BRCCNodeBackupModel::remove(A7BRCCNodeBackupItem *item) {
    qDebug() << "rebove()";
    if(!item) {
        qDebug() << "no item!";
        return;
    }

    beginRemoveRows(QModelIndex(), item->getBackupIndex(), item->getBackupIndex());
        m_map_backups.remove(item->id());
        m_index_backups.removeAt(item->getBackupIndex());
    endRemoveRows();

    int i_removed = item->getBackupIndex();

    for(int i = i_removed; i < m_index_backups.count(); i++){
        A7BRCCNodeBackupItem *item = m_index_backups.at(i);
        item->setBackupIndex(i);
    }
    item->deleteLater();
}

void A7BRCCNodeBackupModel::removeAll() {
    if(!m_index_backups.count()){
        return;
    }
    beginResetModel();
        m_index_backups.clear();
        m_map_backups.clear();
    endResetModel();
}

int A7BRCCNodeBackupModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return 0;
    }

    return m_index_backups.count();
}

QVariant A7BRCCNodeBackupModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return {};
    }

    if(index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    if(role != BackupRole) {
        return {};
    }

    A7BRCCNodeBackupItem * item = m_index_backups.at(index.row());

    return QVariant::fromValue(item);
}

QHash<int, QByteArray> A7BRCCNodeBackupModel::roleNames() const {
    return {{BackupRole, "backup"}};
}

void A7BRCCNodeBackupModel::populateFromJsonArray(const QJsonArray &jarray) {
    if(jarray.isEmpty()) {
        return;
    }

    beginResetModel();
    removeAll();

    for(int c = 0; c < jarray.count(); c++) {
        A7BRCCNodeBackupItem * item = new A7BRCCNodeBackupItem(this);
        //connect to backup list chaged / backup changed
        item->setName(jarray.at(c).toObject().value("name").toString());
        item->setId(jarray.at(c).toObject().value("id").toString());
        append(item, false);
    }
    endResetModel();
}

void A7BRCCNodeBackupModel::populateFromJson(const QJsonObject &jobject) {
    if(jobject.isEmpty()) {
        return;
    }

    QJsonArray backupList = jobject.value("backups").toArray();

    populateFromJsonArray(backupList);
}

void A7BRCCNodeBackupModel::backupAdded(const QJsonObject &jdata) {
    if(jdata.isEmpty()) {
        return;
    }
    qDebug() << "backupAdded" << jdata;
    QJsonObject jbackup = jdata.value("node").toObject().value("backup").toObject();

    A7BRCCNodeBackupItem *item = m_map_backups.value(jbackup.value("id").toString());

    if(item) {
        // backup info changed (e.g. become weekly from daily)
        return;
    }

    item = new A7BRCCNodeBackupItem(this);

//    item->setId(jbackup.value("id").toString());
    item->fromJson(jbackup);
    ///...
    append(item, true);
}

void A7BRCCNodeBackupModel::backupRemoved(const QJsonObject &jdata) {
    if(jdata.isEmpty()) {
        return;
    }
    qDebug() << "backupRemoved" << jdata;
    QJsonObject jbackup = jdata.value("node").toObject().value("backup").toObject();

    A7BRCCNodeBackupItem *item = m_map_backups.value(jbackup.value("id").toString());

    if(!item) {
        qDebug() << "no backup item!";
        return;
    }

    remove(item);
}

void A7BRCCNodeBackupModel::backupInfoChanged(const QJsonObject &jdata) {
    if(jdata.isEmpty()) {
        return;
    }
    qDebug() << "backupRemoved" << jdata;
    QJsonObject jbackup = jdata.value("node").toObject().value("backup").toObject();

    A7BRCCNodeBackupItem *item = m_map_backups.value(jbackup.value("id").toString());

    if(!item) {
        qDebug() << "no backup item!";
        return;
    }

    item->setId(jbackup.value("id").toString());
}
