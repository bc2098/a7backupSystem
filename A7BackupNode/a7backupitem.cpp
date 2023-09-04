#include "a7backupitem.h"

qint64 A7BackupItem::getBackup() const
{
    return m_backup;
}

qint64 A7BackupItem::getLastQuery() const
{
    return m_lastQuery;
}

qint64 A7BackupItem::getFileSize() const
{
    return m_fileSize;
}

A7BackupItem::BackupType A7BackupItem::getBackupType() const
{
    return m_backupType;
}

qint64 A7BackupItem::partCount() const
{
    if(m_fileSize<=0){
        return 0;
    }
    return qint64(m_fileSize/MAX_PART_SIZE) + ((m_fileSize%MAX_PART_SIZE)?1:0);


}

A7BackupItem::A7BackupItem(A7BackupNodeMaster *master, qint64 backup)
    :A7BaseBackup()
    , m_master(master)
    ,m_backup(backup)
{

}

A7BackupItem::~A7BackupItem()
{
    m_file.close();
}

void A7BackupItem::free()
{
    delete this;
}

void A7BackupItem::close()
{
    m_file.close();
}

int A7BackupItem::getPart(int part, QByteArray &partData)
{
    m_lastQuery = QDateTime::currentMSecsSinceEpoch();
    if(!m_file.isOpen()){
        bool r = m_file.open(QFile::ReadOnly);
        if (!r) {
            return -3;
        }
    }
    int offset = part * MAX_PART_SIZE;
    if(offset>=m_file.size()){
        return -4;
    }
    m_file.seek(offset);
    int partSize = qMin<qint64>((MAX_PART_SIZE),(m_file.size()-offset));
    partData.append(m_file.read(partSize));
    return partSize;
}

int A7BackupItem::getPart(int part, char *partData)
{
    m_lastQuery = QDateTime::currentMSecsSinceEpoch();
    if(!m_file.isOpen()){
        bool r = m_file.open(QFile::ReadOnly);
        if (!r) {
            return -3;
        }
    }
    int offset = part * MAX_PART_SIZE;
    if(offset>=m_file.size()){
        return -4;
    }
    m_file.seek(offset);
    int partSizeRequest = qMin<qint64>((MAX_PART_SIZE),(m_file.size()-offset));
    return m_file.read(partData,partSizeRequest);
}
