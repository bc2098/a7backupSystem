#ifndef A7REMOTECONTROLSERVER_H
#define A7REMOTECONTROLSERVER_H

#include "a7backupsystembase.h"
#include <QObject>
#include <QSet>
#include <QTcpServer>
class A7RemoteControlConnection;
class A7BackupNodesServer;
class A7BackupNode;
class A7BackupSave;
class A7RemoteControlServer : public QObject, public A7BaseBackup
{
    Q_OBJECT
    A7BackupNodesServer * m_nodeServer = nullptr;
    QTcpServer * m_server = nullptr;
    QSet<A7RemoteControlConnection*> m_connections;
protected:
    void closeAll();
    QByteArray packPacket(int command, const QJsonObject &jo, int flags)const;
public:
    explicit A7RemoteControlServer(A7BackupNodesServer*nodeServer);
    ~A7RemoteControlServer();
    void receivedPacket(A7RemoteControlConnection*connection,int command,const QJsonObject&data);

    void unregisterFromServer(A7RemoteControlConnection*connection);
    void sendToAll(int command, QJsonObject jo, int flags);
    void sendToAllNodeState(const QJsonObject &jdata);
    void sendToAllNodeBackupstate(const QJsonObject &jdata);
    bool start(quint16 port);
    void stop();

    void sendNodeEvent(A7BackupNode*node);
    void sendBackupSaveEvent(A7BackupSave*backupSave);
protected slots:
    void newConnection();
signals:

};

#endif // A7REMOTECONTROLSERVER_H
