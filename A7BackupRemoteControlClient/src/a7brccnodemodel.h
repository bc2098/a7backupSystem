#ifndef A7BRCCNODEMODEL_H
#define A7BRCCNODEMODEL_H

#include <QAbstractListModel>

class A7BackupServerConnection;
class A7BRCCNodeItem;
class A7BRCCNodeModel : public QAbstractListModel
{
    Q_OBJECT

private:
    QMap <QUuid, A7BRCCNodeItem *> m_map_nodes;
    QList<A7BRCCNodeItem*> m_index_nodes;

    A7BackupServerConnection *m_serverConnection = nullptr;

public:
    enum ListRoles {
        NodeRole = Qt::UserRole + 1
    };

    explicit A7BRCCNodeModel(A7BackupServerConnection *serverConnection);
    ~A7BRCCNodeModel();

    void append(A7BRCCNodeItem *node, bool needEvent);
    void remove(A7BRCCNodeItem *node);
    void removeAll();

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    A7BackupServerConnection *serverConnection();

    void requestBackupListsForAllNodes();

public slots:
    void populateFromJsonArray(const QJsonArray &jarray);
    void populateFromJson(const QJsonObject &jobject);
    void populateFromJsonString(const QString &jstring);

    void nodeInfoChanged(const QJsonObject &jnodeInfo);
    void nodeBackupListChanged(const QJsonObject &nodeBackups);

    bool removeByUuid(const QString & uuidString);

    void nodeAdded(const QJsonObject &jdata);
    void nodeRemoved(const QJsonObject &jdata);

    void nodeBackupAdded(const QJsonObject &jdata);
    void nodeBackupRemoved(const QJsonObject &jdata);
    void backupInfoReceived(const QJsonObject &jdata);

signals:
    void serverConnectionChanged();
};

#endif // A7BRCCNODEMODEL_H
