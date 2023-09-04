#ifndef A7BACKUPITEM_H
#define A7BACKUPITEM_H
#include <QtCore>
#include "a7backupsystembase.h"
#include <QFile>

class A7BackupNodeMaster;

class A7BackupItem: public A7BaseBackup
{
protected:
    A7BackupNodeMaster * m_master=nullptr;

    qint64 m_backup = -1;// == time create
    qint64 m_fileSize = -1;
    BackupType m_backupType =  BackupType::Unknown;

    qint64 m_lastQuery = 0;
    QFile  m_file;

//in memory
protected:
    BackupStatus m_backupStatus = BackupStatus::Unknown;



public:
    A7BackupItem(A7BackupNodeMaster * master, qint64 backup);
    ~A7BackupItem();
    void free();
    void close();
    bool creataBackup();
    int getPart(int part, QByteArray & partData);
    int getPart(int part, char* partData);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject&json);
    qint64 getBackup() const;
    qint64 getLastQuery() const;
    qint64 getFileSize() const;
    BackupType getBackupType() const;
    qint64 partCount() const;
};

#endif // A7BACKUPITEM_H
