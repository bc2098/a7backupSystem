#ifndef A7BACKUPNODE_H
#define A7BACKUPNODE_H

#include "a7backupsave.h"
#include "a7backupsystembase.h"
#include <QObject>
#include <QUuid>
#include <QHostAddress>

enum class NodeStatus {
    Unknown = 0,
    Connecting = 1,
    Connected = 2,
    NoKeepalives = 3,
    Disconnected = 4,
    Error = 99
};

/*

*/

class A7BackupNodesServer;
class A7BackupUdpServer;
class A7RemoteControlServer;
class A7BackupNode: public A7BaseBackup
{
public:
    enum A7BackupNodeDownloadingMode{
        NoDownloadingDM = 0,
        DownloadingListDM = 1,
        DownloadingHeaderDM = 2,
        DownloadingPartsDM = 3,
    };
public:
    static const qint64 TIME_WAIT_RESPONSE = 1000;
    static const qint64 TIME_WAIT_RESPONSE_LIST = 1000;
    static const qint64 TIME_WAIT_RESPONSE_HEADER = 1000;
    static const qint64 TIME_WAIT_RESPONSE_PART = 1000;
    static const qint64 MAX_PARTS_IN_QUERY = 50;

protected:
    A7BackupNodesServer * m_nodeServer = nullptr;
    quint64 m_seq =0;



    NetworkPin m_networkPin;
    QUuid m_nodeUuid;
    //QString m_password;
    QHostAddress m_remoteIp;
    quint16 m_remotePort=0;
    qint64 m_lastUdpInTime=0;
    qint64 m_lastUdpOutTime=0;

    QString m_nodeName = "";
    QString m_nodeDescription = "";
    QString m_nodePassword;

protected:

    //qint64 m_lastDownloadedBackup = -1;
    //qint64 m_lastListBackup = -1;


    A7BackupNodeDownloadingMode m_downloadingMode = NoDownloadingDM;
    //qint64 m_downloadingBackup = -1;
    //qint64 m_downloadingPart = -1;
    qint64 m_lastQueryTime = 0;
    qint64 m_lastAnswerTime = 0;
    qint64 m_lastPartDownloaded = -1;
    qint64 m_lastFullPartDownloaded = -1;
    QVector <bool> m_partsDownloaded;
//    QVector <qint64> m_backupList;
    QMap<qint64, A7BackupSave*> m_backupList;

    //QSet<qint64>
    //QQueue <qint64> m_downloadQueue;
    QSet<A7BackupSave*> m_notCompleted;
    QSet<A7BackupSave*> m_downloaded;

    qint64 m_lastCommandCreate =0;
    qint16 m_lastAction =NoAction;

    struct RemoteNodeState
    {
        A7BackupNetworkAction actionType;
        qint64 actionTime;
        qint16 paercents;
        qint64 lastUpdate;
    }m_remoteNodeState;


protected:
    qint64 m_lastBackupTime = 0;
    //qint64 m_lastBackupIndex = 0;
    NodeStatus m_nodeStatus = NodeStatus::Unknown;

protected:
    A7BackupSave* addNewBackupSave(qint64 backup);
    A7BackupSave* at(qint64 backup)const;

    int receiveLogin(const NetworkPacketToServer &packet);
    int receiveKeepAlive(const NetworkPacketToServer &packet);
    int receiveBackupList(const NetworkPacketToServer &packet);
    int receiveBackupHeader(const NetworkPacketToServer &packet);
    int receiveBackupPart(const NetworkPacketToServer &packet);
    int receiveAction(const NetworkPacketToServer &packet);



    void sendKeepAlive();
    void sendCreateBackupSave();

    int sendNodeEvent();


public:
    explicit A7BackupNode(A7BackupNodesServer * nodeServer,const QUuid &uuid, qint32 index, qint64 time);
    explicit A7BackupNode(A7BackupNodesServer * nodeServer,const QJsonObject &json, qint32 index, qint64 time);
    ~A7BackupNode();
    int remove();
    void free();

    int createBackupSave();

    void sendPacket(NetworkPacketToClient&packet, A7BackupSave*backupSave);
    void downloaded (A7BackupSave*backupSave);
    void downloading (qint64 partsCount, qint64 partsCompleted, A7BackupSave*backupSave);
    int onReadPacket(const NetworkPacketToServer&packet);


    QJsonObject toJson(bool forSave=true) const;
    int fromJson(const QJsonObject &json);
    int updateFromJson(const QJsonObject &json);

    QJsonObject allBackupsToJson() const;
    QJsonObject backupToJson(qint64 backupId) const;

    bool checkLogin(const NetworkPacketToServer &packet)const;
    bool checkPin(const NetworkPacketToServer &packet)const;

    void pulse(qint64 time);
public:
    quint64 getLastBackupTime() const;
    void setLastBackupTime(quint64 newLastBackupTime);
    quint64 getLastBackupIndex() const;
    void setLastBackupIndex(quint64 newLastBackupIndex);
    const QUuid &getNodeUuid() const;
    void setNodeUuid(const QUuid &newNodeUuid);
    NodeStatus getNodeStatus() const;
    void setNodeStatus(NodeStatus newNodeStatus);
    const NetworkPin &getNetworkPin() const;
    const QHostAddress &getRemoteIp() const;
    void setRemoteIp(const QHostAddress &newRemoteIp);
    quint16 getRemotePort() const;
    void setRemotePort(quint16 newRemotePort);
    qint64 getLastUdpInTime() const;
    void setLastUdpInTime(qint64 newLastUdpInTime);
    qint64 getLastUdpOutTime() const;
    void setLastUdpOutTime(qint64 newLastUdpOutTime);
    QString getNodeDescription() const;
    void setNodeDescription(const QString &newNodeDescription);
    A7BackupNodesServer *nodeServer() const;
    A7BackupUdpServer *udpServer() const;
    A7RemoteControlServer *remoteControlServer() const;


    quint64 getNextSeq();
    const RemoteNodeState &getRemoteNodeState() const;
};

#endif // A7BACKUPNODE_H
