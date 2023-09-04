#ifndef A7BRCCNODEBACKUPMODEL_H
#define A7BRCCNODEBACKUPMODEL_H


#include <QAbstractListModel>

class A7BRCCNodeItem;
class A7BRCCNodeBackupItem;
class A7BRCCNodeBackupModel : public QAbstractListModel
{
    Q_OBJECT

private:
    QMap<QString, A7BRCCNodeBackupItem*> m_map_backups;
    QList<A7BRCCNodeBackupItem*> m_index_backups;

    const A7BRCCNodeItem *m_nodeItem = nullptr;

public:
    enum ListRoles {
        BackupRole = Qt::UserRole + 1
    };

    explicit A7BRCCNodeBackupModel(const A7BRCCNodeItem *nodeItem);
    ~A7BRCCNodeBackupModel();

    void append(A7BRCCNodeBackupItem *item, bool needEvent);
    void remove(A7BRCCNodeBackupItem *item);
    void remove(int index);
    void removeAll();

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

public slots:
    void populateFromJsonArray(const QJsonArray &jarray);
    void populateFromJson(const QJsonObject &jobject);

    void backupAdded(const QJsonObject &jdata);
    void backupRemoved(const QJsonObject &jdata);
    void backupInfoChanged(const QJsonObject &jdata);
};

#endif // A7BRCCNODEBACKUPMODEL_H
