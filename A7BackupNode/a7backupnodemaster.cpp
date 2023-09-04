#include "a7backupnodemaster.h"
#include "a7backupitem.h"
#include "a7backupnetworkclient.h"
#include "a7backupprocess.h"

A7BackupNetworkClient *A7BackupNodeMaster::getClient() const
{
    return m_client;
}

quint64 A7BackupNodeMaster::getNextSeq()
{
    return (++m_seq);
}

QUuid A7BackupNodeMaster::getUuid() const
{
    return m_uuid;
}

void A7BackupNodeMaster::backupCompleted(qint64 backup, QString fileName)
{
    //A7BackupItem * item = at(backup);
    m_actionTime = 0;
    m_actionType = NoAction;
    m_actionPercent = -1;
    addNew(backup);

}

void A7BackupNodeMaster::backupPercent(qint64 backup, qint16 percent)
{
    Q_UNUSED(backup)
    m_actionPercent = percent;
    sendKeepAlive();
}

void A7BackupNodeMaster::backupError(qint64 backup, QString error)
{
    Q_UNUSED(backup)
    Q_UNUSED(error)
    //A7BackupItem * item = at(backup);
    m_actionTime = 0;
    m_actionType = NoAction;
    m_actionPercent = -1;
    sendKeepAlive();
}

void A7BackupNodeMaster::createNewBackup(qint64 backup)
{
    m_actionType = CreateAction;
    m_actionTime = backup;
    m_backupProcess->startBackup(backup,m_settings);
    //Start Process
}

A7BackupItem *A7BackupNodeMaster::at(qint64 backup) const
{
    return m_items.value(backup,nullptr);
}

A7BackupItem *A7BackupNodeMaster::addNew(qint64 backup)
{
    A7BackupItem * item = new A7BackupItem(this, backup);
    //
    m_items.insert(item->getBackup(),item);
    m_lastBackup = QDateTime::fromMSecsSinceEpoch(backup);
    sendKeepAlive();
    return item;
}

void A7BackupNodeMaster::clear()
{
    for(QMap<qint64,A7BackupItem*>::iterator i = m_items.begin(); i != m_items.end(); i++ ){
        i.value()->free();
    }
}

void A7BackupNodeMaster::saveSettings()
{
    QFile file("new_settings.json");
    file.open(QFile::WriteOnly);
    QJsonDocument doc(settingsToJson());
    file.write(doc.toJson());
    file.close();
    QDir dir;
    //проверка
    dir.remove("old_settings.json");
    dir.rename("settings.json","old_settings.json");
    dir.rename("new_settings.json","settings.json");
}

void A7BackupNodeMaster::checkBackup()
{
    QDateTime dt = QDateTime::currentDateTime();
    QTime t = dt.time();
    if(m_backupTime.hour() != t.hour()){
        return;
    }
    if(m_backupTime.minute() != t.minute()){
        return;
    }
    if(dt.toUTC().date() == m_lastBackup.toUTC().date()){
        return;
    }
    if(m_actionType != NoAction){
        return;
    }
    //dt.toUTC();
    //A7BackupItem * item = addNew(dt);

    createNewBackup(dt.toMSecsSinceEpoch());


    //m_backupProcess->startBackup(/*FROM SETTINGS*/);
}

void A7BackupNodeMaster::recievedAnswerLogin(const NetworkPacketToClient &packet)
{
    NetworkPin & np = m_client->getPin();

    if( np.time >= packet.data.login.pin.time ){
        return;
    }

    if(packet.title.hasError()){
        np.reset();
        return;
    }

    np.index = packet.data.login.pin.index;
    np.random = packet.data.login.pin.random;
    np.time = packet.data.login.pin.time;

    m_lastIncome = QDateTime::currentMSecsSinceEpoch();

}

int A7BackupNodeMaster::recievedAnswer(const NetworkPacketToClient &packet)
{
    switch (packet.title.networkCommand()) {
    case LoginCMD :     recievedAnswerLogin(packet); break;
    case KeepaliveCMD:  break;
    case FileHeaderCMD: break;
    case FilePartCMD:   break;

    case BackupListCMD: break;
    case ActionCMD:     break;

    }

    return NoAnswer;
}

int A7BackupNodeMaster::recievedKeepAlive(const NetworkPacketToClient &packet)
{
    Q_UNUSED(packet);
    m_lastIncome = QDateTime::currentMSecsSinceEpoch();
    return NoAnswer;
}

int A7BackupNodeMaster::recievedRequest(const NetworkPacketToClient &packet)
{
    switch (packet.title.networkCommand()) {
    case LoginCMD : return RequestError;
    case KeepaliveCMD: return recievedKeepAlive(packet);

    case FileHeaderCMD: return recievedRequestFileHeader(packet);
    case FilePartCMD:   return recievedRequestFilePart(packet);
    case BackupListCMD: return recievedRequestBackupList(packet);
    case ActionCMD:     return recievedRequestAction(packet);
    }
}

int A7BackupNodeMaster::recievedRequestFilePart(const NetworkPacketToClient &packet)
{

    A7BackupItem* item = at(packet.data.filePart.backupTime);
    if(!item){
        return BackupNotFound;
    }
    if( packet.data.filePart.count > MAX_LIST_COUNT ){
        return RequestError;
    }

    qint64 pc = item->partCount();

    for(int i=0;  i< packet.data.filePart.count; i++){
        qint64 partNumber = packet.data.filePart.partNumber[i];
        if(partNumber < 0 || partNumber >=pc ){
            return RequestError;
        }
        NetworkPacketToServer answer;

        answer.title.copy(packet.title);
        answer.title.flags = ResponseFLG;
        answer.data.filePart.partNumber = partNumber;
        answer.data.filePart.backupOffset = partNumber * MAX_PART_SIZE;
        answer.data.filePart.backupTime = packet.data.filePart.backupTime;

        int res = item->getPart(partNumber,answer.data.filePart.data);
        if(res!=0){
            return RequestError;
        }
        answer.data.filePart.partSize = res;
        m_client->sendToServer(answer);
    }
    return NoAnswer;

}

int A7BackupNodeMaster::recievedRequestFileHeader(const NetworkPacketToClient &packet)
{

    A7BackupItem* item = at(packet.data.fileHeader.backupTime);
    if(!item){
        return BackupNotFound;
    }

    NetworkPacketToServer answer;

    answer.title.copy(packet.title);
    answer.title.flags = ResponseFLG;
    answer.data.fileHeader.backupTime = item->getBackup();
    answer.data.fileHeader.backupSize = item->getFileSize();
    answer.data.fileHeader.partSize = MAX_PART_SIZE;
    answer.data.fileHeader.partCount = item->partCount();

    m_client->sendToServer(answer);
    return NoAnswer;

}

int A7BackupNodeMaster::recievedRequestBackupList(const NetworkPacketToClient &packet)
{
    NetworkPacketToServer answer;

    answer.title.copy(packet.title);
    answer.title.flags = ResponseFLG;

    QMap<qint64, A7BackupItem*>::iterator i = m_items.find(packet.data.backupList.fromBackupTime);

    if(i == m_items.end() ){
        i = m_items.begin();
    }else{
        ++i;
    }

    for(answer.data.backupList.count =0 ; i != m_items.end() && answer.data.backupList.count < MAX_LIST_COUNT; i++ ){

        answer.data.backupList.backups[answer.data.backupList.count] = i.key();
        answer.data.backupList.count++;
    }

    m_client->sendToServer(answer);


     return NoAnswer;

}

int A7BackupNodeMaster::recievedRequestAction(const NetworkPacketToClient &packet)
{
    if(packet.data.action.actionTime <= m_actionTime ){
        return NoAnswer;
    }
    if(m_actionType != NoAction){
        return ActionBusy;
    }
    m_actionType = packet.data.action.actionType;
    m_actionTime = packet.data.action.actionTime;
    createNewBackup(m_actionTime);


    //
    NetworkPacketToServer answer;
    answer.title.copy(packet.title);
    answer.title.flags = ResponseFLG;
    answer.data.action.actionTime = m_actionTime;
    answer.data.action.actionType = m_actionType;
    m_client->sendToServer(answer);

    return NoAnswer;
}

void A7BackupNodeMaster::sendKeepAlive()
{
    NetworkPacketToServer packet;
    packet.title.command= KeepaliveCMD;
    packet.title.time = QDateTime::currentMSecsSinceEpoch();
    packet.title.seq = getNextSeq();
    packet.title.flags = NoFlagsFLG;
    packet.data.keepAlive.count = m_items.count();
    packet.data.keepAlive.currentActionType = m_actionType;
    packet.data.keepAlive.currentActionTime = m_actionTime;
    packet.data.keepAlive.firstBackupTime = (m_items.count()) ? m_items.firstKey(): -1;
    packet.data.keepAlive.lastBackupTime =  (m_items.count()) ? m_items.lastKey() : -1;
    packet.data.keepAlive.actionPercent = m_actionPercent;
    m_client->sendToServer(packet);

}

void A7BackupNodeMaster::sendLogin()
{
    NetworkPacketToServer packet;
    packet.title.command= LoginCMD;
    packet.title.time = QDateTime::currentMSecsSinceEpoch();
    packet.title.seq = getNextSeq();
    packet.title.flags = NoFlagsFLG;
    packet.data.login.size = 16;
    QByteArray sign;
    sign+=QByteArray::number(packet.title.time);
    sign+=QByteArray::number(packet.title.seq);
    sign+=m_password.toUtf8();
    qDebug()<<"SIGN: "<<sign;

    QByteArray md5 = QCryptographicHash::hash(sign, QCryptographicHash::Algorithm::Md5);
    qDebug()<<"MD5: "<<md5.toHex();

    memcpy(packet.data.login.md5,md5.constData(),16);
    m_client->sendToServer(packet);

}


A7BackupNodeMaster::A7BackupNodeMaster(QObject *parent)
    : QObject{parent}, A7BaseBackup()
{
    m_client = new A7BackupNetworkClient(this);
    m_backupProcess = new A7BackupProcess(this);
    connect(m_backupProcess,&A7BackupProcess::completed,this,&A7BackupNodeMaster::backupCompleted);
    connect(m_backupProcess,&A7BackupProcess::error,this,&A7BackupNodeMaster::backupError);
    connect(m_backupProcess,&A7BackupProcess::percent,this,&A7BackupNodeMaster::backupPercent);

}

A7BackupNodeMaster::~A7BackupNodeMaster()
{
    if(m_client){
        m_client->deleteLater();
    }
    if(m_backupProcess){
        m_backupProcess->deleteLater();
    }
    clear();
}

bool A7BackupNodeMaster::start()
{
    int r = m_client->start();
    if (r != NoError) {
        return false;
    }
    startTimer(1000);
    return true;
}

int A7BackupNodeMaster::recievedPacket(const NetworkPacketToClient &packet)
{
    if(packet.title.flags & A7BackupNetworkFlags::ResponseFLG){
        //answer

        return recievedAnswer(packet);
    }else{
        return recievedRequest(packet);
    }





}

const QJsonObject &A7BackupNodeMaster::settingsToJson() const
{
    return m_settings;
}


void A7BackupNodeMaster::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    checkBackup();
    if(m_client->getPin().index == -1){
        sendLogin();
        return;
    }
    if(m_lastIncome + 5000 < time){
        m_client->getPin().reset();
        sendLogin();
        return;
    }
    sendKeepAlive();


}
