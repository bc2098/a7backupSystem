#include "a7backupudpserver.h"
#include "a7backupnode.h"
#include "a7backupnodesserver.h"

void A7BackupUdpServer::onReadDatagramm(const QNetworkDatagram &datagramm)
{
    const QByteArray data = datagramm.data();
    const int dataLen = data.length();
    if( dataLen<NetworkPacketToServer::titleLen() ){
        return;
    }
    const NetworkPacketToServer & packet= *reinterpret_cast<const NetworkPacketToServer*>(data.data());
    if(packet.networkPacketLen()!=dataLen){
        return;
    }
    onReadPacket(packet,datagramm.senderAddress(),datagramm.senderPort());


}

void A7BackupUdpServer::onReadPacket(const NetworkPacketToServer &packet, const QHostAddress&address, const quint16 port)
{
    int r = m_nodes->onReadPacket(packet,address,port);//Flags

    if(packet.title.isResponse()){
        return;
    }
    if(r!=0){
        errorAnswer(packet,r,address,port);
    }
}

void A7BackupUdpServer::errorAnswer(const NetworkPacketToServer &packet, int error, const QHostAddress &address, const quint16 port)
{
    NetworkPacketToClient answer;
    answer.title.copy(packet.title);
    //answer.title.command = ErrorCm
    answer.title.flags = ResponseFLG | ErrorFLG ;
    answer.data.error.errorCode = error;
    answer.data.error.time = packet.title.time;
    m_udp->writeDatagram(answer.asConstChar(),answer.networkPacketLen(),address,port);

}



A7BackupUdpServer::A7BackupUdpServer(A7BackupNodesServer *nodes)
    : QObject{nullptr}, A7BaseBackup()
    , m_nodes(nodes)
{
    m_udp = new QUdpSocket();
    connect(m_udp,&QUdpSocket::readyRead,this,&A7BackupUdpServer::onRead);
}

A7BackupUdpServer::~A7BackupUdpServer()
{
    if(m_udp){
        m_udp->deleteLater();
    }

}

void A7BackupUdpServer::start()
{
    if(!m_udp) {
        return;
    }
    m_udp->bind(3434);

    qDebug() << "Node server started on port" << m_udp->localPort();
}

void A7BackupUdpServer::stop()
{

}

void A7BackupUdpServer::loginResponseOk(const NetworkPacketTitle &title , const A7BackupNode *node)
{
    NetworkPacketToClient packet;
    packet.title.copy(title);
    packet.title.flags = A7BackupNetworkFlags::ResponseFLG;
    packet.data.login.loginStatus = A7BackupNetworkError::NoError;
    packet.data.login.size = sizeof(node->getNetworkPin() );
    packet.data.login.pin = node->getNetworkPin();

     m_udp->writeDatagram(packet.asConstChar(),packet.networkPacketLen(),node->getRemoteIp(),node->getRemotePort());

}

void A7BackupUdpServer::sendPacket(NetworkPacketToClient &packet, A7BackupNode *node)
{
    m_udp->writeDatagram(packet.asConstChar(),packet.networkPacketLen(),node->getRemoteIp(),node->getRemotePort());
}

void A7BackupUdpServer::onRead()
{
    while (m_udp->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udp->receiveDatagram();
        onReadDatagramm(datagram);
    }
}
