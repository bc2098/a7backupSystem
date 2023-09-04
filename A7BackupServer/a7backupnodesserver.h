#ifndef A7BACKUPNODESSERVER_H
#define A7BACKUPNODESSERVER_H

#include <QObject>
#include "a7backupsystembase.h"
#include "a7remotecontrolserver.h"
#include <QUuid>
#include <QHostAddress>

struct RemoteAnswer {
    QJsonObject result;
    A7BaseBackup::Flags resultFlags;
};

class A7BackupNode;
class A7BackupUdpServer;
class A7BackupNode;
class A7BackupNodesServer : public QObject, public A7BaseBackup
{
    Q_OBJECT
protected:
    int m_timerId = -1;
    A7BackupUdpServer * m_udpServer = nullptr;
    QList<A7BackupNode*> m_nodes;
    QHash<QUuid, A7BackupNode* > m_hash;
    QQueue <int> m_empty;

    A7RemoteControlServer *m_remoteControlServer = nullptr;

    // to settings
    QString m_remoteLogin = "login";
    QString m_remotePassword = "password";

protected:
    A7BackupNode* searchNodeByUuid(const QUuid &uuid)const;
    QJsonObject nodeJson(const QUuid &uuid);
    A7BackupNode* searchNodeByIndex(int index)const;
    A7BackupNode* searchNodeByPacket(const NetworkPacketToServer &packet)const;
    int removeNode(A7BackupNode *node);
    int checkRemoteControlLogin(const QString &login, const QString &password);

public:
    RemoteAnswer remoteCommand(const int command, const QJsonObject & data);

public:
    explicit A7BackupNodesServer(QObject *parent = nullptr);
    virtual ~A7BackupNodesServer();
    int onReadPacket(const NetworkPacketToServer &packet, const QHostAddress&address, const quint16 port);
    A7BackupNode *addNode();
    int removeNodeByUuid(const QUuid &nodeUuid);
    int removeNodeByIndex(int nodeIndex);

    QJsonObject toJson() const;
    QJsonObject treeDump() const;
    QJsonObject nodeBackupList(const A7BackupNode *node) const;

    int loadFromJson(const QJsonObject &json);

    int saveToFile(const QString &fileName)const;
    int loadFromFile(const QString &fileName);

    A7BackupUdpServer *udpServer() const;

    A7RemoteControlServer *remoteControlServer() const;

signals:


    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};

#endif // A7BACKUPNODESSERVER_H
