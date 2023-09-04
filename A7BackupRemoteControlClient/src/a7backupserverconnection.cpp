#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "a7backupserverconnection.h"

void A7BackupServerConnection::extract(){
    while(extractPacket() > 0){}
}

int A7BackupServerConnection::extractPacket() {
    int i = m_bufferIn.indexOf('\0');
    if(i < 0){
        return 0;
    }

    extractPacket( m_bufferIn.left(i));
    m_bufferIn.remove(0, i+1);
    return i;
}

void A7BackupServerConnection::extractPacket(const QByteArray &jdata) {
    QJsonDocument jd = QJsonDocument::fromJson(jdata);
    extractPacket(jd.object());
}

void A7BackupServerConnection::extractPacket(const QJsonObject &json) {
    qDebug() << "FROM SERVER: A7BackupServerConnection::extractPacket(json)" << json;

    processJsonPacket(json);
}

QByteArray A7BackupServerConnection::packRequest(const QJsonObject &jdata, Commands command, Flags flags) {
    if(m_socket->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Socket is not connected!";
        return {};
    }

    QJsonObject jmsg;

    jmsg.insert("command", command);
    jmsg.insert("flags", flags);
    jmsg.insert("dt",QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    jmsg.insert("data", jdata);

    QByteArray msg = QJsonDocument(jmsg).toJson(QJsonDocument::Compact);
    msg.append('\0');

    return msg;
}

void A7BackupServerConnection::sendRequest(const QByteArray &request) {
    if(request.isEmpty() || request.isNull()){
        qDebug() << "No data!";
        return;
    }

    m_socket->write(request);
}

A7BackupServerConnection::A7BackupServerConnection(QObject *parent)
    : QObject{parent}, A7BaseBackup()
{
    m_nodeModel = new A7BRCCNodeModel(this);

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &A7BackupServerConnection::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &A7BackupServerConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &A7BackupServerConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &A7BackupServerConnection::onConnectionError);

}

A7BackupServerConnection::~A7BackupServerConnection() {

}

QHostAddress A7BackupServerConnection::serverAddress() const {
    return m_serverAddress;
}

void A7BackupServerConnection::setServerAddress(const QHostAddress &newServerAddress) {
    if (m_serverAddress == newServerAddress)
        return;
    m_serverAddress = newServerAddress;
    emit serverAddressChanged();
}

quint16 A7BackupServerConnection::serverPort() const {
    return m_serverPort;
}

void A7BackupServerConnection::setServerPort(quint16 newServerPort) {
    if (m_serverPort == newServerPort)
        return;
    m_serverPort = newServerPort;
    emit serverPortChanged();
}

bool A7BackupServerConnection::isConnected() const {
    return m_isConnected;
}

void A7BackupServerConnection::setIsConnected(bool newIsConnected) {
    if (m_isConnected == newIsConnected)
        return;
    m_isConnected = newIsConnected;
    emit isConnectedChanged();
}

void A7BackupServerConnection::onReadyRead() {
    qDebug() << "readyRead";
    m_bufferIn.append(m_socket->readAll());
    extract();
}

void A7BackupServerConnection::onConnected() {
    qDebug() << "connected";
    setIsConnected(true);

    if(m_checkConnectTimer > 0) {
        killTimer(m_checkConnectTimer);
    }

    m_isStart = true;

    m_checkConnectTimer = startTimer(1000);
    // send init requests
    // login
    // node list
    // backup list for nodes?
    // etc.
    requestLogin();
}

void A7BackupServerConnection::onDisconnected() {
    qDebug() << "disconnected";
    setIsConnected(false);

    if(!m_isWasConnected) { // we doesnt conected before
        return;
    }
}

void A7BackupServerConnection::onConnectionError() {
    qDebug() << "connection error" << m_socket->errorString() << m_socket->peerAddress() << m_socket->peerPort()
             << m_serverAddress << m_serverPort;
    setIsConnected(false);
    if(!m_isWasConnected) {
        return;
    }

//    reconnect();
}

void A7BackupServerConnection::connectToServer() {
    m_socket->connectToHost(m_serverAddress, m_serverPort);
    m_isWasConnected = true;
}

void A7BackupServerConnection::requestLogin() {
    qDebug() << "requestLogin" << login() << password();
    QJsonObject jdata;

    jdata.insert("login",  login());
    jdata.insert("password", password());

    QByteArray req = packRequest(jdata, SystemLogin, Request);
    sendRequest(req);
}

void A7BackupServerConnection::connectToServer(const QString &address, const quint16 port, const QString &login, const QString &password) {
    if (address.isNull() || port == 0) {
        qDebug() << "connectToServer wrong parameters" << address << port;
        return;
    }

    if(login.isEmpty() || password.isEmpty()) {
        qDebug() << "empty login or password!";
        return;
    }

    setServerAddress(QHostAddress(address));
    setServerPort(port);
    setLogin(login);
    setPassword(password);

    connectToServer();
}

void A7BackupServerConnection::disconnectFromServer() {
    m_isWasConnected = false;
    m_socket->disconnectFromHost();
    setIsConnected(false);

    if(m_checkConnectTimer > 0) {
        killTimer(m_checkConnectTimer);
    }
}

void A7BackupServerConnection::reconnect() {
    qDebug() << "socket state:" << m_socket->state();

    if(m_socket->state() == QTcpSocket::ConnectingState) {
        m_socket->abort();
    }

    m_socket->disconnectFromHost();

    connectToServer();
}

void A7BackupServerConnection::checkConnect() {
    if(m_socket->state() == QTcpSocket::ConnectedState) {
        return;
    }
    reconnect();
}

void A7BackupServerConnection::testRead(const QString &testString) { // onReadyRead
    QJsonObject jobject = parseJson(testString);
    processJsonPacket(jobject);
}

void A7BackupServerConnection::loginResultReceived(const QJsonObject &jdata, Flags flags) {
    Q_UNUSED(jdata)
    qDebug() << "A7BackupServerConnection::loginResultReceived; flags:" << "0x"+QString::number(flags, 16);
    if(flags & Error) {
        qDebug() << "Error login";
        disconnectFromServer();
        return;
    }

    requestNodeList();
}

void A7BackupServerConnection::nodeListReceived(const QJsonObject &jdata) {
    m_nodeModel->populateFromJson(jdata);

    if(!m_isStart) {
        return;
    }

    // request backup lists for all nodes
    m_nodeModel->requestBackupListsForAllNodes();
    m_isStart = false;

}

void A7BackupServerConnection::nodeInfoChanged(const QJsonObject &jdata) {
    qDebug() << "A7BackupServerConnection::nodeInfoChanged" << jdata;
    m_nodeModel->nodeInfoChanged(jdata);
}

void A7BackupServerConnection::backupListReceived(const QJsonObject &jdata) {
    m_nodeModel->nodeBackupListChanged(jdata);
}

void A7BackupServerConnection::backupInfoReceived(const QJsonObject &jdata) {
    m_nodeModel->backupInfoReceived(jdata);
}

void A7BackupServerConnection::nodeBackupAdded(const QJsonObject &jdata) {
    m_nodeModel->nodeBackupAdded(jdata);
}

void A7BackupServerConnection::nodeBackupRemoved(const QJsonObject &jdata) {
    m_nodeModel->nodeBackupRemoved(jdata);
}

void A7BackupServerConnection::nodeAdded(const QJsonObject &jdata) {
    m_nodeModel->nodeAdded(jdata);
}

void A7BackupServerConnection::nodeRemoved(const QJsonObject &jdata) {
    m_nodeModel->nodeRemoved(jdata);
}

A7BRCCNodeModel *A7BackupServerConnection::nodeModel() const {
    return m_nodeModel;
}

QJsonObject A7BackupServerConnection::parseJson(const QString &jstring) {
    if(jstring.isEmpty()) {
        return {};
    }

    QJsonParseError parseError;
    QJsonDocument jd = QJsonDocument::fromJson(jstring.toUtf8(), &parseError);

    if(parseError.error != QJsonParseError::NoError) {
        qDebug() << "Error parse JSON data! Error:" << parseError.errorString();
        qDebug() << jstring;
        return {};
    }

    return jd.object();
}

int A7BackupServerConnection::processJsonPacket(const QJsonObject &jobject) {
    //  Кому |  Что
    // {Node,Backup}  | {Info, Added, Removed, List}
    // Flags {Response, Error, Command, Event} // NeedResponse, NotResponseIfError}
    const quint64 cmd = static_cast<Commands>(jobject.value("command").toInt());
    QString dt = jobject.value("dt").toString();
    Flags flags = static_cast<Flags>(jobject.value("flags").toInt());

    QJsonObject jdata = jobject.value("data").toObject();

    qDebug() << "FLAGS:" << "0x"+QString::number(flags, 16)<< "CMD:" << "0x"+QString::number(cmd, 16);

    if(flags & Event) {
        switch( cmd ) {
        case NodeAdd:       nodeAdded(jdata); break;
        case NodeRemove:    nodeRemoved(jdata); break;
        case NodeInfo:      nodeInfoChanged(jdata);break;
        case BackupList:    backupListReceived(jdata);break;
        case BackupInfo:    backupInfoReceived(jdata); break;
        case BackupAdd:     break; //FIXME:<<!!!!!!!!
        case BackupRemove:  break;
        case NodeUpdate:    break;
        case NodeList:      break;
        case SystemLogin:   break;
        }
        return 0;
    } else if(flags & Response) {
        if(flags & Error) {
            qDebug() << "SERVER RETURN ERROR!";
        }

        // RESPONSES

        switch(cmd) {
        case SystemLogin: // login result
            loginResultReceived(jobject, flags);
            break;
        case NodeInfo: //нода поменялась
            nodeInfoChanged(jdata);
            break;
        case NodeList: //список нод
            nodeListReceived(jdata);
            break;
        case NodeAdd: // node added
            //nodeAdded(jdata);
            // this is response
            break;
        case NodeUpdate:

            break;
        case NodeRemove: // node removed
            //nodeRemoved(jdata);
            //this is response
            break;
        case BackupList: //для ноды пришел список бэкапов
            backupListReceived(jdata);
            break;
        case BackupInfo: //для ноды пришел список бэкапов
            backupListReceived(jdata);
            break;
        case BackupAdd: // node backup added
            nodeBackupAdded(jdata);
            break;
        case BackupRemove: // node backup removed
            nodeBackupRemoved(jdata);
            break;
        default:
            qDebug() << "unknown response command";
            break;
        }

    } else if(flags & Request) {
        qDebug() << "SERVER REQUESTS SOMETHING o_O";
    }

    return 0;
}

void A7BackupServerConnection::requestNodeList() {
    qDebug() << "requestNodeList";
    QByteArray req = packRequest({}, NodeList, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestNodeBackupList(const QString &nodeUuid) {
    qDebug() << "requestNodeBackupList" << nodeUuid;
    QJsonObject jdata;
    QJsonObject jnode;
    jnode.insert("uuid", nodeUuid);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, BackupList, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestAddNode(const QString &nodeName, const QString &nodeUuid) {
    qDebug() << "requestAddNode" << nodeName << nodeUuid;
    QJsonObject jdata;
    QJsonObject jnode;
    jnode.insert("uuid", nodeUuid);
    jnode.insert("name", nodeName);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, NodeAdd, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestRemoveNode(const QString &nodeUuid) {
    qDebug() << "requestRemoveNode" << nodeUuid;
    QJsonObject jdata;
    QJsonObject jnode;
    jnode.insert("uuid", nodeUuid);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, NodeRemove, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestNodeInfo(const QString &nodeUuid) {
    qDebug() << "requestNodeInfo" << nodeUuid;
    QJsonObject jdata;
    QJsonObject jnode;
    jnode.insert("uuid", nodeUuid);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, NodeInfo, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestBackupInfo(const QString &nodeUuid, const QString &backupId) {
    qDebug() << "requestBackupInfo" << nodeUuid << backupId;
    QJsonObject jdata;
    QJsonObject jnode;
    QJsonObject jbackup;

    jbackup.insert("id", backupId);

    jnode.insert("uuid", nodeUuid);
    jnode.insert("backup", jbackup);

    jdata.insert("node", jnode);
    QByteArray req = packRequest(jdata, BackupInfo, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestCreateBackup(const QString &nodeUuid) {
    qDebug() << "requestCreateBackup" << nodeUuid;
    QJsonObject jdata;
    QJsonObject jnode;
    jnode.insert("uuid", nodeUuid);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, BackupAdd, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestRemoveBackup(const QString &nodeUuid, const QString &backupId) {
    qDebug() << "requestRemoveBackup" << nodeUuid << backupId;
    QJsonObject jdata;
    QJsonObject jnode;
    QJsonObject jbackup;

    jbackup.insert("id", backupId);

    jnode.insert("uuid", nodeUuid);
    jnode.insert("backup", jbackup);

    jdata.insert("node", jnode);
    QByteArray req = packRequest(jdata, BackupRemove, Request);
    sendRequest(req);
}

void A7BackupServerConnection::requestMoveBackup(const QString &nodeUuid, const QString &backupId, BackupType targetType) {
    //????
}

void A7BackupServerConnection::requestNodeUpdate(const QString &nodeUuid, const QJsonObject &nodeData) {
    qDebug() << "A7BackupServerConnection::requestNodeUpdate" << nodeUuid << nodeData;

    QJsonObject jdata;
    QJsonObject jnode(nodeData);
    jnode.insert("uuid", nodeUuid);

    jdata.insert("node", jnode);

    QByteArray req = packRequest(jdata, NodeUpdate, Request);
    sendRequest(req);
}



/***
 * requests (send)
 * - get node list
 * - get node backup list
 * - add node
 * - remove node
 * - get node info
 * - get backup info
 * - create backup (add)
 * - remove backup
 * - move backup
 *
 * answers (received)
 * - node list
 * - node backup list
 * - node info
 * - backup info
 * - success/error add node
 * - success/error remove node
 * - success/error move node
 *
 * events (received)
 * - node added
 * - node removed
 * - node info changed
 * - backup added
 * - backup removed
 * - node backup info changed
 *
 ***/


void A7BackupServerConnection::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)
    checkConnect();

}

QString A7BackupServerConnection::login() const
{
    return m_login;
}

void A7BackupServerConnection::setLogin(const QString &newLogin)
{
    if (m_login == newLogin)
        return;
    m_login = newLogin;
    emit loginChanged();
}

QString A7BackupServerConnection::password() const
{
    return m_password;
}

void A7BackupServerConnection::setPassword(const QString &newPassword)
{
    if (m_password == newPassword)
        return;
    m_password = newPassword;
    emit passwordChanged();
}
