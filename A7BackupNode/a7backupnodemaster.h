#ifndef A7BACKUPNODEMASTER_H
#define A7BACKUPNODEMASTER_H

#include <QObject>
#include "a7backupsystembase.h"

class A7BackupNetworkClient;
class A7BackupProcess;
class A7BackupItem;
class A7BackupNodeMaster : public QObject, public A7BaseBackup
{
    Q_OBJECT


protected:
    A7BackupNetworkClient * m_client=nullptr;
    A7BackupProcess * m_backupProcess = nullptr;
    QDateTime m_lastBackup;
    QTime m_backupTime;
    QMap<qint64, A7BackupItem*> m_items;
    qint64 m_actionType = NoAction;
    qint16 m_actionPercent =-1;
    qint64 m_actionTime = 0;
    quint64 m_seq =0;
    QUuid m_uuid = "{25fd0c37-10c3-436c-a2c4-958d802cbd55}";


    QString m_password = "password";
    qint64 m_lastIncome = 0;


    QJsonObject m_settings;
protected slots:
    void backupCompleted(qint64 backup, QString fileName);
    void backupPercent(qint64 backup, qint16 percent);
     void backupError(qint64 backup, QString error);
    void createNewBackup(qint64 backup);

protected:
    A7BackupItem * at (qint64 backup)const;
    A7BackupItem * addNew(qint64 backup);
    void clear();
    void loadSettings();
    void saveSettings();
    void checkBackup();
    void recievedAnswerLogin(const NetworkPacketToClient &packet);
    int recievedAnswer(const NetworkPacketToClient &packet);
    int recievedKeepAlive(const NetworkPacketToClient &packet);
    int recievedRequest(const NetworkPacketToClient &packet);
    int recievedRequestFilePart(const NetworkPacketToClient &packet);
    int recievedRequestFileHeader(const NetworkPacketToClient &packet);
    int recievedRequestBackupList(const NetworkPacketToClient &packet);
    int recievedRequestAction(const NetworkPacketToClient &packet);
    void sendKeepAlive();
    void sendLogin();

public:
    explicit A7BackupNodeMaster(QObject *parent = nullptr);
    virtual ~A7BackupNodeMaster();
    bool start();
    int recievedPacket ( const NetworkPacketToClient& packet);
    const QJsonObject &settingsToJson()const ;

    A7BackupNetworkClient *getClient() const;

    quint64 getNextSeq() ;

    QUuid getUuid() const;

signals:


    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};

#endif // A7BACKUPNODEMASTER_H
