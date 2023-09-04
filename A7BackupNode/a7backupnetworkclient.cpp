#include "a7backupnetworkclient.h"
#include "a7backupnodemaster.h"


A7BackupNetworkClient::A7BackupNetworkClient(A7BackupNodeMaster *master)
    : QObject{nullptr} , A7BaseBackup()
    ,m_master(master)
{
    m_pin.reset();
    m_udp = new QUdpSocket();
    connect(m_udp, &QUdpSocket::readyRead , this , &A7BackupNetworkClient::readyRead);
}

A7BackupNetworkClient::~A7BackupNetworkClient()
{
    if(m_udp){
        m_udp->close();
        m_udp->deleteLater();
    }
}

int A7BackupNetworkClient::start()
{
//    m_remoteAddress = remoteAddress;
//    m_remotePort = remotePort;
    bool b = m_udp->bind();
    if (!b) {
        qDebug()<<" error start udp"<<m_udp->errorString() ;
        return -1;
    }
    qDebug()<<"start udp"<<m_udp->localPort() ;

    return NoError;
}

void A7BackupNetworkClient::sendToServer(NetworkPacketToServer &data)
{
    if( data.title.command != LoginCMD ){
        data.title.pin = m_pin;
    }
    data.title.fromUuid(m_master->getUuid());
    int res = m_udp->writeDatagram(data.asConstChar(),data.networkPacketLen(),m_remoteAddress,m_remotePort);
    if(res!=data.networkPacketLen()){
        m_udp->close();
        m_udp->bind();
    }

}

void A7BackupNetworkClient::sendToServer(const QByteArray &data)
{
    int res = m_udp->writeDatagram(data,m_remoteAddress,m_remotePort);
    if(res!=data.length()){
        m_udp->close();
        m_udp->bind();
    }

}


/*
void A7BackupNetworkClient::onNewBackupAdded(A7BackupItem *item)
{

}

void A7BackupNetworkClient::onNewBackupCompleted(A7BackupItem *item)
{

}

void A7BackupNetworkClient::onNewBackupError(A7BackupItem *item)
{

}
*/

const NetworkPin &A7BackupNetworkClient::getPin() const
{
    return m_pin;
}

NetworkPin &A7BackupNetworkClient::getPin()
{
    return m_pin;
}

void A7BackupNetworkClient::receivedDatagram(const QNetworkDatagram &datagramm)
{
    const QByteArray ba = datagramm.data();
    const NetworkPacketToClient & packet = *reinterpret_cast<const NetworkPacketToClient*> (ba.constData());
    const int len = ba.length();
    if ( len < NetworkPacketToClient::titleLen() ){
        return;
    }
    if (len != packet.networkPacketLen() ){
        return;
    }
    A7BackupNetworkError res = static_cast<A7BackupNetworkError>( m_master->recievedPacket(packet));
    if(res== NoError || res==NoAnswer){
        return;
    }
    NetworkPacketToServer errorAnswer;
    errorAnswer.title.copy(packet.title);
    errorAnswer.title.flags |= ResponseFLG | ErrorFLG;
    errorAnswer.data.error.errorCode = res;
    errorAnswer.data.error.time = QDateTime::currentMSecsSinceEpoch();

    sendToServer(errorAnswer);



}

void A7BackupNetworkClient::readyRead()
{
    while (m_udp->hasPendingDatagrams()) {
        receivedDatagram( m_udp->receiveDatagram() );
    }
}
