#include "a7brccnodebackupitem.h"
#include <QDebug>

A7BRCCNodeBackupItem::A7BRCCNodeBackupItem(QObject *parent)
    : QObject{parent}
{
}

A7BRCCNodeBackupItem::~A7BRCCNodeBackupItem() {
    qDebug() << "backup destructor ";
}

QString A7BRCCNodeBackupItem::name() const {
    return m_name;
}

void A7BRCCNodeBackupItem::setName(const QString &newName) {
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

QString A7BRCCNodeBackupItem::id() const {
    return m_id;
}

void A7BRCCNodeBackupItem::setId(const QString &newId) {
    if (m_id == newId)
        return;
    m_id = newId;
    emit idChanged();
}

int A7BRCCNodeBackupItem::getBackupIndex() const {
    return m_backupIndex;
}

void A7BRCCNodeBackupItem::setBackupIndex(int newBackupIndex) {
    if (m_backupIndex == newBackupIndex)
        return;
    m_backupIndex = newBackupIndex;
    emit backupIndexChanged();
}

A7BRCCNodeBackupItem::QBackupType A7BRCCNodeBackupItem::backupType() const {
    return m_backupType;
}

int A7BRCCNodeBackupItem::size() const {
    return m_size;
}

QString A7BRCCNodeBackupItem::fileName() const {
    return m_fileName;
}

A7BRCCNodeBackupItem::QBackupStatus A7BRCCNodeBackupItem::status() const {
    return m_status;
}

void A7BRCCNodeBackupItem::fromJson(const QJsonObject &jdata) {
    //{"filename":"filename.1","id":"0","size":"0","status":0,"type":0}
    m_id = jdata.value("id").toString();
    m_backupType = static_cast<QBackupType>(jdata.value("type").toInt());
    m_fileName = jdata.value("filename").toString();
    m_size = jdata.value("size").toString().toULongLong();
    m_status = static_cast<QBackupStatus>(jdata.value("status").toInt());


    idChanged();
    backupTypeChanged();
    fileNameChanged();
    sizeChanged();
    statusChanged();

}
