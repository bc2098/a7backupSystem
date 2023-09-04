#include "a7remotecontrolconnection.h"
#include "a7remotecontrolserver.h"
#include <QJsonDocument>
void A7RemoteControlConnection::unregisterFromServer()
{
    if(!m_server){
        return;
    }
    m_server->unregisterFromServer(this);
    m_server=nullptr;
    //free(); ///???
}

void A7RemoteControlConnection::extract(){
    while (extractPacket()>0) {

    }
}

int A7RemoteControlConnection::extractPacket() {
    int i = m_bufferIn.indexOf('\0');
    if(i < 0){
        return 0;
    }

    extractPacket( m_bufferIn.left(i));
    m_bufferIn.remove(0,i+1);
    return i;
}

void A7RemoteControlConnection::extractPacket(const QByteArray &jdata) {
    QJsonDocument jd = QJsonDocument::fromJson(jdata);
    extractPacket(jd.object());
}

void A7RemoteControlConnection::extractPacket(const QJsonObject &json) {
    Commands command = static_cast<Commands>(json.value("command").toInt());
    Flags flag = static_cast<Flags>(json.value("flags").toInt());
    QJsonObject data = json.value("data").toObject();

//    qDebug() << json;
    qDebug() << "A7RemoteControlConnection::extractPacket; Received request from control client" << command << flag << json;

    if(flag != Request) {
        return;
    }

    receivedPacket(command, json);

    /// <---
    /// get list
    /// add node
    /// remove node
    /// ?get all
    /// get node info
    /// get backup info
    /// -->
    /// added
    /// removed
    /// listChanged
    /// nodeChanged
    /// backup added
    /// ...
}

void A7RemoteControlConnection::receivedPacket(int command, const QJsonObject &data)
{
    if(!m_server){
        return;
    }
    m_server->receivedPacket(this, command, data);
}

A7RemoteControlConnection::A7RemoteControlConnection(A7RemoteControlServer *server, QTcpSocket *socket)
    : QObject{nullptr}, A7BaseBackup()
    , m_server{server}
    , m_socket{socket}
{

    connect(socket, &QTcpSocket::readyRead, this, &A7RemoteControlConnection::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &A7RemoteControlConnection::onDisconnected);
}

A7RemoteControlConnection::~A7RemoteControlConnection() {

}

void A7RemoteControlConnection::free() {
    this->deleteLater();
}

void A7RemoteControlConnection::unregisterServer()
{
    m_server = nullptr;
    m_socket->disconnectFromHost();
    free();
}

void A7RemoteControlConnection::sendToClient(const QByteArray &data) {
    m_socket->write(data);
}

void A7RemoteControlConnection::onReadyRead() {
    m_bufferIn.append(m_socket->readAll());
    extract();
}

void A7RemoteControlConnection::onDisconnected() {
    qDebug() << "Control client disconnected" << static_cast<QTcpSocket*>(QObject::sender())->peerAddress();
    unregisterFromServer();
    deleteLater();
}
