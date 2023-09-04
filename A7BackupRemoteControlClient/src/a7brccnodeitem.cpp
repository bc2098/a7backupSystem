#include "a7brccnodeitem.h"
#include <QJsonObject>
#include "src/a7brccnodebackupmodel.h"
#include <QDebug>
#include <QDateTime>
void A7BRCCNodeItem::fromJson(const QJsonObject &jdata) {

    if(jdata.contains("name")){
        setName(jdata.value("name").toString());
    }
    if(jdata.contains("description")){
        setDescription(jdata.value("description").toString());
    }
    if(jdata.contains("uuid")){
        setUuid(jdata.value("uuid").toString());
    }
    if(jdata.contains("status")){
        setStatus(jdata.value("status").toInt());
    }
    if(jdata.contains("host")){
        setHost(jdata.value("host").toString());
    }
    if(jdata.contains("port")){
        setport(jdata.value("port").toInt());
    }

    if(jdata.contains("lastUpdate")) {
        QString val = jdata.value("lastUpdate").toString();
        setLastUpdateTime(val.toLongLong());
    }
    if(jdata.contains("actionTime")) {
        QString val = jdata.value("actionTime").toString();
        setActionTime(val.toLongLong());
    }
    if(jdata.contains("actionType")) {
        setActionType(jdata.value("actionType").toInt());
    }
    if(jdata.contains("actionPercent")) {
        setActionPercents(jdata.value("actionPercent").toInt());
    }

}

QString A7BRCCNodeItem::intTimeToISOStr(qint64 time) const
{
    return QDateTime::fromMSecsSinceEpoch(time).toString(Qt::ISODate);
}

A7BRCCNodeItem::A7BRCCNodeItem(const A7BackupServerConnection *serverConnection)
    : m_serverConnection{serverConnection}
{
    m_backupModel = new A7BRCCNodeBackupModel(const_cast<A7BRCCNodeItem*>(this));
}

A7BRCCNodeItem::~A7BRCCNodeItem() {
    qDebug() << "node destructor";
    if(m_backupModel) {
        m_backupModel->deleteLater();
    }
}

int A7BRCCNodeItem::getNodeIndex() const
{
    return _nodeIndex;
}

void A7BRCCNodeItem::setNodeIndex(int newNodeIndex)
{
    if (_nodeIndex == newNodeIndex)
        return;
    _nodeIndex = newNodeIndex;
    emit nodeIndexChanged();
}

void A7BRCCNodeItem::backupAdded(const QJsonObject &jdata) {
    m_backupModel->backupAdded(jdata);
}

void A7BRCCNodeItem::backupRemoved(const QJsonObject &jdata) {
    m_backupModel->backupRemoved(jdata);

}

void A7BRCCNodeItem::backupInfoChanged(const QJsonObject &jdata) {
    m_backupModel->backupInfoChanged(jdata);
}

quint16 A7BRCCNodeItem::port() const {
    return m_port;
}

void A7BRCCNodeItem::setport(quint16 newPort) {
    if (m_port == newPort)
        return;
    m_port = newPort;
    emit portChanged();
}

QString A7BRCCNodeItem::host() const {
    return m_host;
}

void A7BRCCNodeItem::setHost(const QString &newHost) {
    if (m_host == newHost)
        return;
    m_host = newHost;
    emit hostChanged();
}

QDateTime A7BRCCNodeItem::lastKeepAlive() const {
    return m_lastKeepAlive;
}

void A7BRCCNodeItem::setLastKeepAlive(const QDateTime &newLastKeepAlive) {
    if (m_lastKeepAlive == newLastKeepAlive)
        return;
    m_lastKeepAlive = newLastKeepAlive;
    emit lastKeepAliveChanged();
}

qint16 A7BRCCNodeItem::status() const {
    return m_status;
}

void A7BRCCNodeItem::setStatus(qint16 newStatus) {
    if (m_status == newStatus)
        return;
    m_status = newStatus;
    emit statusChanged();
}

QString A7BRCCNodeItem::name() const {
    return m_name;
}

void A7BRCCNodeItem::setName(const QString &newName) {
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

QUuid A7BRCCNodeItem::uuid() const {
    return m_uuid;
}

void A7BRCCNodeItem::setUuid(const QUuid &newUuid) {
    if (m_uuid == newUuid)
        return;
    m_uuid = newUuid;
    emit uuidChanged();
}

const A7BackupServerConnection *A7BRCCNodeItem::serverConnection() const {
    return m_serverConnection;
}

A7BRCCNodeBackupModel *A7BRCCNodeItem::backupModel() const {
    return m_backupModel;
}

QString A7BRCCNodeItem::description() const
{
    return m_description;
}

void A7BRCCNodeItem::setDescription(const QString &newDescription)
{
    if (m_description == newDescription)
        return;
    m_description = newDescription;
    emit descriptionChanged();
}

QString A7BRCCNodeItem::actionTime() const {
    return intTimeToISOStr(m_actionTime);
}

void A7BRCCNodeItem::setActionTime(qint64 newActionTime)
{
    if (m_actionTime == newActionTime)
        return;
    m_actionTime = newActionTime;
    emit actionTimeChanged();
}

QString A7BRCCNodeItem::lastUpdateTime() const {
    return intTimeToISOStr(m_lastUpdateTime);
}

void A7BRCCNodeItem::setLastUpdateTime(qint64 newLastUpdateTime)
{
    if (m_lastUpdateTime == newLastUpdateTime)
        return;
    m_lastUpdateTime = newLastUpdateTime;
    emit lastUpdateTimeChanged();
}

int A7BRCCNodeItem::actionType() const
{
    return m_actionType;
}

void A7BRCCNodeItem::setActionType(int newActionType)
{
    if (m_actionType == newActionType)
        return;
    m_actionType = newActionType;
    emit actionTypeChanged();
}

int A7BRCCNodeItem::actionPercents() const
{
    return m_actionPercents;
}

void A7BRCCNodeItem::setActionPercents(int newActionPercents)
{
    if (m_actionPercents == newActionPercents)
        return;
    m_actionPercents = newActionPercents;
    emit actionPercentsChanged();
}
