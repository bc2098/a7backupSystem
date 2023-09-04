#ifndef A7BRCCNODEITEM_H
#define A7BRCCNODEITEM_H

#include <QObject>
#include <QDateTime>
#include <QUuid>

class A7BackupServerConnection;
class A7BRCCNodeBackupModel;
class A7BRCCNodeItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(qint16 status READ status WRITE setStatus NOTIFY statusChanged FINAL)
    Q_PROPERTY(QDateTime lastKeepAlive READ lastKeepAlive WRITE setLastKeepAlive NOTIFY lastKeepAliveChanged FINAL)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged FINAL)
    Q_PROPERTY(quint16 port READ port WRITE setport NOTIFY portChanged FINAL)
    Q_PROPERTY(QUuid uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
    Q_PROPERTY(A7BRCCNodeBackupModel * backupModel READ backupModel NOTIFY backupModelChanged FINAL)
    Q_PROPERTY(int nodeIndex READ getNodeIndex WRITE setNodeIndex NOTIFY nodeIndexChanged FINAL)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged FINAL)

    Q_PROPERTY(QString actionTime READ actionTime NOTIFY actionTimeChanged FINAL)
    Q_PROPERTY(QString lastUpdateTime READ lastUpdateTime NOTIFY lastUpdateTimeChanged FINAL)
    Q_PROPERTY(int actionType READ actionType NOTIFY actionTypeChanged FINAL)
    Q_PROPERTY(int actionPercents READ actionPercents NOTIFY actionPercentsChanged FINAL)

private:
    quint16 m_port;
    QString m_host;
    QDateTime m_lastKeepAlive;
    qint16 m_status;
    QString m_name;
    QUuid m_uuid;
    int _nodeIndex = -1;

    const A7BackupServerConnection *m_serverConnection = nullptr;
    A7BRCCNodeBackupModel *m_backupModel = nullptr;

    QString m_description;

    qint64 m_actionTime = 0;
    qint64 m_lastUpdateTime = 0;
    int m_actionType = -1;
    int m_actionPercents = -1;
protected:
    QString intTimeToISOStr(qint64 time) const;

public:
    explicit A7BRCCNodeItem(const A7BackupServerConnection *serverConnection);
    ~A7BRCCNodeItem();

    quint16 port() const;
    void setport(quint16 newPort);

    QString host() const;
    void setHost(const QString &newHost);

    QDateTime lastKeepAlive() const;
    void setLastKeepAlive(const QDateTime &newLastKeepAlive);

    qint16 status() const;
    void setStatus(qint16 newStatus);

    QString name() const;
    void setName(const QString &newName);

    QUuid uuid() const;
    void setUuid(const QUuid &newUuid);

    const A7BackupServerConnection *serverConnection() const;

    A7BRCCNodeBackupModel *backupModel() const;

    int getNodeIndex() const;
    void setNodeIndex(int newNodeIndex);

    void backupAdded(const QJsonObject &jdata);
    void backupRemoved(const QJsonObject &jdata);
    void backupInfoChanged(const QJsonObject &jdata);


    QString description() const;
    void setDescription(const QString &newDescription);
    void fromJson(const QJsonObject &jdata);

    QString actionTime() const;
    void setActionTime(qint64 newActionTime);

    QString lastUpdateTime() const;
    void setLastUpdateTime(qint64 newLastUpdateTime);

    int actionType() const;
    void setActionType(int newActionType);

    int actionPercents() const;
    void setActionPercents(int newActionPercents);

signals:

    void portChanged();
    void hostChanged();
    void lastKeepAliveChanged();
    void statusChanged();
    void nameChanged();
    void uuidChanged();
    void serverConnectionChanged();
    void backupModelChanged();
    void nodeIndexChanged();
    void descriptionChanged();
    void actionTimeChanged();
    void lastUpdateTimeChanged();
    void actionTypeChanged();
    void actionPercentsChanged();
};

#endif // A7BRCCNODEITEM_H
