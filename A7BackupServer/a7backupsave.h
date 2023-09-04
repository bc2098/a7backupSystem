#ifndef A7BACKUPSAVE_H
#define A7BACKUPSAVE_H

#include "a7backupsystembase.h"
#include <QObject>

//#include "a7backupnode.h"

class A7BackupNode;
class QFile;
class A7BackupSave :  public A7BaseBackup
{
public:
    static const qint64 TIME_QUERY = 1000;
    static const qint64 FILE_PART_COUNT_MAX = 20;
protected:

    A7BackupNode *m_parentNode=nullptr;
    qint64 m_backupTime = 0;
    qint64 m_backupSize = -1;
    qint64 m_partSize = -1;

    qint16 m_partCount = -1;
    qint64 m_lastPartDownloaded = -1;
    QVector<bool> m_partsCompleted;
    qint16 m_partCountQuery = -1;
    qint64 m_partsDownloaded =0;

    QString m_fileName = "";
    QFile *m_file = nullptr;

    BackupType m_backupType = BackupType::Unknown;
    BackupStatus m_status = BackupStatus::Unknown;

    qint64 m_lastQueryTime = 0;
    quint64 m_lastQuerySeq = 0;
protected:
    void prepareFileParts();
public:
    explicit A7BackupSave(A7BackupNode *parentNode, qint64 backupTime, BackupStatus backupStatus = BackupStatus::Existed);
    explicit A7BackupSave(A7BackupNode *parentNode, const QJsonObject&json);
    ~A7BackupSave();

    int recievedPacket(const NetworkPacketToServer&packet);
    int recievedFileHeaderPacket(const NetworkPacketToServer&packet);
    int recievedFilePartPacket(const NetworkPacketToServer&packet);
    void sendQueryFileHeader();
    void sendQueryFileParts();

    bool isDownloaded() const;
    int pulse( qint64 time);

 public:

    BackupStatus getStatus() const;
    void setStatus(BackupStatus newStatus);
    qint64 getBackupTime() const;
    void setBackupTime(qint64 newBackupTime);
    //quint64 getBackupIndex() const;
    //void setBackupIndex(quint64 newBackupIndex);
    quint64 getBackupSize() const;
    void setBackupSize(quint64 newBackupSize);
    quint64 getPartSize() const;
    void setPartSize(quint64 newPartSize);
    quint64 getPartCount() const;
    void setPartCount(quint64 newPartCount);
    QString getFileName() const;
    void setFileName(const QString &newFileName);


    bool createFile(qint64 fileSize);
    //bool openFile();
    bool receivePart(quint64 partIndex, quint64 partOffset, const QByteArray &data);

    QJsonObject toJson();
    int fromJson(const QJsonObject &jbackup);

    BackupType getBackupType() const;
    void setBackupType(BackupType newBackupType);
    qint64 getLastQuery() const;
    void setLastQuery(qint64 newLastQuery);
    qint64 getLastQueryTime() const;
    void setLastQueryTime(qint64 newLastQueryTime);
    quint64 getLastQuerySeq() const;
    void setLastQuerySeq(quint64 newLastQuerySeq);
    A7BackupNode *parentNode() const;
};

#endif // A7BACKUPSAVE_H
