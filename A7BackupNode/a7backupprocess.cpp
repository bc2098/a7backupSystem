#include "a7backupprocess.h"
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

A7BackupProcess::A7BackupProcess(QObject *parent)
    : QThread{parent}
{

}

qint64 A7BackupProcess::startBackup(qint64 backup, const QJsonObject &settings)
{
    if(isRunning()){
        return -1;
    }
    //m_backupList = settings;
    //m_pathDst = pathDst;
    m_backup = backup;
    //m_resultFileName = pathDst+"/"+QString::number(m_backup)+".bckup";
    start();
    return 0;
    //run();
}


void A7BackupProcess::run()
{
    for(int i =0 ;i<10; i++){
        QThread::msleep(1000);
        emit percent(m_backup,i*1000);
    }
    emit completed(m_backup, m_resultFileName);
    return ;
/*
    m_process.execute("");
    bool r = m_process.waitForFinished(-1);
    if (r) {
        emit completed(m_backup, m_resultFileName);
    }else{
        emit error(m_backup,m_process.errorString());
    }
*/
    //QFileInfo("").i


}
