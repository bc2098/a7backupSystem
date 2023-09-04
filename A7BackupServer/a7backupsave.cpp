#include "a7backupsave.h"
#include "a7backupnode.h"
#include <QJsonObject>


A7BackupSave::BackupType A7BackupSave::getBackupType() const
{
    return m_backupType;
}

void A7BackupSave::setBackupType(BackupType newBackupType)
{
    m_backupType = newBackupType;
}


qint64 A7BackupSave::getLastQueryTime() const
{
    return m_lastQueryTime;
}

void A7BackupSave::setLastQueryTime(qint64 newLastQueryTime)
{
    m_lastQueryTime = newLastQueryTime;
}

quint64 A7BackupSave::getLastQuerySeq() const
{
    return m_lastQuerySeq;
}

void A7BackupSave::setLastQuerySeq(quint64 newLastQuerySeq)
{
    m_lastQuerySeq = newLastQuerySeq;
}

A7BackupNode *A7BackupSave::parentNode() const
{
    return m_parentNode;
}

void A7BackupSave::prepareFileParts()
{
    if(m_fileName.isEmpty()){
        m_fileName = QString::number(m_backupTime)+"backup";
    }
    m_partsCompleted = QVector<bool>(m_partCount,false);
    m_lastPartDownloaded = -1;
    m_file=new QFile(m_fileName);
    m_file->open(QFile::WriteOnly);
    if(m_file->size() != m_backupSize){
        m_file->resize(m_backupSize);
    }


}

A7BackupSave::A7BackupSave(A7BackupNode *parentNode,qint64 backupTime, BackupStatus backupStatus)
    : A7BaseBackup()
    ,m_parentNode(parentNode)
    ,m_backupTime(backupTime)
    ,m_status(backupStatus)
{

}

A7BackupSave::A7BackupSave(A7BackupNode *parentNode, const QJsonObject &json)
    :A7BaseBackup()
    ,m_parentNode(parentNode)
{
    fromJson(json);
}

A7BackupSave::~A7BackupSave()
{
    if(m_file){
        m_file->deleteLater();
    }
}

int A7BackupSave::recievedPacket(const NetworkPacketToServer &packet)
{
    if(!packet.title.isResponse()){
        return -1;
    }
    switch (packet.networkCommand()) {
    case LoginCMD:
    case KeepaliveCMD:return -1;
    case FileHeaderCMD: return recievedFileHeaderPacket(packet);
    case FilePartCMD: return recievedFileHeaderPacket(packet);
    case BackupListCMD: return -1;
    case ActionCMD: return NoAnswer;
    }
}

int A7BackupSave::recievedFileHeaderPacket(const NetworkPacketToServer &packet)
{
    if(m_partSize != -1 ){
        return 0;
    }

    const NetworkPacketFileHeaderToServer & fileHeader = packet.data.fileHeader;
    m_backupSize=fileHeader.backupSize;
    m_partSize = fileHeader.partSize;
    m_partCount = fileHeader.partCount;
    prepareFileParts();
    sendQueryFileParts();
    return 0;

}

int A7BackupSave::recievedFilePartPacket(const NetworkPacketToServer &packet)
{
    const NetworkPacketFilePartToServer & filePart = packet.data.filePart;

    if(filePart.partNumber>=m_partsCompleted.count() && filePart.partNumber<0){
        return -1;
    }
    if(m_partsCompleted.value(filePart.partNumber)){
        return 0;
    }
    if(filePart.partSize>MAX_PART_SIZE){
        return -2;
    }
    m_partsCompleted.replace(filePart.partNumber,true);
    m_partsDownloaded ++;
    m_partCountQuery--;


    m_file->seek(filePart.partNumber*m_partSize);
    m_file->write(filePart.data,filePart.partSize);
    if(m_partsDownloaded == m_partCount){
        m_parentNode ->downloaded(this);
        return 0;
    }

    m_parentNode->downloading(m_partCount,m_partsDownloaded,this);
    if(m_partCountQuery == 0){
        sendQueryFileParts();
    }

    return 0;

}

void A7BackupSave::sendQueryFileHeader()
{
    m_lastQueryTime = QDateTime::currentMSecsSinceEpoch();
    NetworkPacketToClient query;
    query.title.command = FileHeaderCMD;
    query.title.time = m_lastQueryTime;
    query.data.fileHeader.backupTime = m_backupTime;

    m_parentNode->sendPacket(query, this);

}

void A7BackupSave::sendQueryFileParts()
{
    m_lastQueryTime = QDateTime::currentMSecsSinceEpoch();
    NetworkPacketToClient query;
    query.title.command = FilePartCMD;
    query.title.time = m_lastQueryTime;
    query.data.filePart.backupTime = m_backupTime;
    query.data.filePart.count = 0;

    for(qint64 i = m_lastPartDownloaded+1, isFirst = 1; i<m_partCount && query.data.filePart.count < FILE_PART_COUNT_MAX; i++){
        if(m_partsCompleted.at(i)){
            continue;
        }
        if(isFirst){
            m_lastPartDownloaded = i-1;
            isFirst = false;
        }

        query.data.filePart.partNumber[query.data.filePart.count] = i;
        query.data.filePart.count++;
    }

    if(query.data.filePart.count == 0){
        m_status = BackupStatus::Downloaded;
        m_parentNode ->downloaded(this);
        return;
    }
    m_partCountQuery = query.data.filePart.count;
    //query.data.filePart.partNumber =

    m_parentNode->sendPacket(query, this);

}

bool A7BackupSave::isDownloaded() const
{
    return (BackupStatus::Downloaded == m_status);
}

int A7BackupSave::pulse(qint64 time)
{
    if(isDownloaded()){
        return 1;
    }
    if (time  <= m_lastQueryTime + TIME_QUERY){
        return 2;
    }
    switch (m_status) {
    case BackupStatus::Existed:
    case BackupStatus::HeaderDowloading: sendQueryFileHeader(); return 0;
    case BackupStatus::PartsDowloading: sendQueryFileParts(); return 0;
    case BackupStatus::Unknown: return -1;
    case BackupStatus::Downloaded: return 1;
    case BackupStatus::Error: return -2;
    }
    return 3;
}

A7BackupSave::BackupStatus A7BackupSave::getStatus() const {
    return m_status;
}

void A7BackupSave::setStatus(BackupStatus newStatus) {
    m_status = newStatus;
}

qint64 A7BackupSave::getBackupTime() const {
    return m_backupTime;
}

void A7BackupSave::setBackupTime(qint64 newBackupTime) {
    m_backupTime = newBackupTime;
}

//quint64 A7BackupSave::getBackupIndex() const {
//    return m_backupIndex;
//}

//void A7BackupSave::setBackupIndex(quint64 newBackupIndex) {
//    m_backupIndex = newBackupIndex;
//}

quint64 A7BackupSave::getBackupSize() const {
    return m_backupSize;
}

void A7BackupSave::setBackupSize(quint64 newBackupSize) {
    m_backupSize = newBackupSize;
}

quint64 A7BackupSave::getPartSize() const {
    return m_partSize;
}

void A7BackupSave::setPartSize(quint64 newPartSize) {
    m_partSize = newPartSize;
}

quint64 A7BackupSave::getPartCount() const
{
    return m_partCount;
}

void A7BackupSave::setPartCount(quint64 newPartCount)
{
    m_partCount = newPartCount;
}

QString A7BackupSave::getFileName() const
{
    return m_fileName;
}

void A7BackupSave::setFileName(const QString &newFileName)
{
    m_fileName = newFileName;
}

bool A7BackupSave::createFile(qint64 fileSize) {
    if (m_fileName.isEmpty()) {
        setStatus(BackupStatus::Error);
        return false;
    }
    QFile file(m_fileName);

    if (fileSize <= 0) {
        setStatus(BackupStatus::Error);
        return false;
    }

    if (!file.open(QFile::WriteOnly)) {
        setStatus(BackupStatus::Error);
        return false;
    }

    if (!file.resize(fileSize)) { // reserve space on disk
        setStatus(BackupStatus::Error);
        return false;
    }

    file.close();

    return true;
}


bool A7BackupSave::receivePart(quint64 partIndex, quint64 partOffset, const QByteArray &data) {
    if (m_fileName.isEmpty()) {
        setStatus(BackupStatus::Error);
        return false;
    }

    QFile file(m_fileName);

    if (file.size() != m_backupSize) {
        setStatus(BackupStatus::Error);
        return false;
    }

    if (!file.open(QFile::WriteOnly)) {
        setStatus(BackupStatus::Error);
        return false;
    }

    if(partIndex * MAX_PART_SIZE != partOffset){
        return false;
    }

    file.seek(partOffset);
    file.write(data);
    file.close();

    return true;
}

QJsonObject A7BackupSave::toJson() {
    QJsonObject res;

    res.insert("id", QString::number(getBackupTime()));
    res.insert("filename", getFileName());
    res.insert("status", static_cast<int>(m_status));
    res.insert("size", QString::number(getBackupSize()));
    res.insert("type", static_cast<int>(getBackupType()));
    res.insert("partSize",m_partSize);
    res.insert("partCount",m_partCount);
    if(m_status!=BackupStatus::PartsDowloading){
        return res;
    }
    QJsonArray ja;
    for(int i =0; i<m_partsCompleted.count();i++){
        QJsonObject jo;
        jo.insert("i",i);
        jo.insert("got",m_partsCompleted.at(i));
        ja.append(jo);
    }
    res.insert("parts",ja);


    return res;
}

int A7BackupSave::fromJson(const QJsonObject &jbackup) {
    setBackupTime(jbackup.value("id").toString().toLongLong());
    setFileName(jbackup.value("filename").toString());
    setStatus(static_cast<BackupStatus>(jbackup.value("status").toInt(int(BackupStatus::Unknown))));
    setBackupSize(jbackup.value("size").toInt(-1));
    setBackupType(static_cast<BackupType>(jbackup.value("type").toInt(int(BackupType::Unknown))));
    setPartSize(jbackup.value("partSize").toInt(-1));
    setPartCount(jbackup.value("partCount").toInt(-1));

    switch(m_status){
    case BackupStatus::Existed: prepareFileParts();return 0;
    case BackupStatus::Unknown: return 0;
    case BackupStatus::HeaderDowloading: return 0;
    case BackupStatus::PartsDowloading: break;
    case BackupStatus::Downloaded: return 0;
    case BackupStatus::Error: return 0;
        break;
    }

    prepareFileParts();

    QJsonArray ja = jbackup.value("parts").toArray();

    for(int i =0; i<m_partsCompleted.count();i++){
        QJsonObject jo = ja.at(i).toObject();
        m_partsCompleted.replace(jo.value("i").toInt(),jo.value("got").toBool());
    }
    return 0;
}
