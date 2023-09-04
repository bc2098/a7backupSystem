#ifndef A7BACKUPSERVERCONNECTION_H
#define A7BACKUPSERVERCONNECTION_H

#include "src/a7brccnodemodel.h"
#include "a7backupsystembase.h"
#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>

class A7BackupServerConnection : public QObject, public A7BaseBackup
{
    Q_OBJECT

    Q_PROPERTY(QHostAddress serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged FINAL)
    Q_PROPERTY(quint16 serverPort READ serverPort WRITE setServerPort NOTIFY serverPortChanged FINAL)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged FINAL)
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged FINAL)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged FINAL)

    Q_PROPERTY(A7BRCCNodeModel* nodeModel READ nodeModel NOTIFY nodeModelChanged FINAL)

protected:
    void extract();
    int extractPacket();
    void extractPacket(const QByteArray &jdata);
    void extractPacket(const QJsonObject &json);

private:
    QTcpSocket * m_socket = nullptr;
    QHostAddress m_serverAddress;
    quint16 m_serverPort = 0;
    bool m_isConnected=false;
    A7BRCCNodeModel *m_nodeModel = nullptr;
    QByteArray m_bufferIn;
    bool m_isWasConnected = false;
    int m_checkConnectTimer = -1;
    QString m_login = "";
    QString m_password = "";
    bool m_isStart = false;

private:
    QByteArray packRequest(const QJsonObject & jdata, Commands command, Flags flags);
    void sendRequest(const QByteArray & request);
    void connectToServer();
    void requestLogin();


public:
    explicit A7BackupServerConnection(QObject *parent = nullptr);
    virtual ~A7BackupServerConnection();

    QHostAddress serverAddress() const;
    void setServerAddress(const QHostAddress &newServerAddress);

    quint16 serverPort() const;
    void setServerPort(quint16 newServerPort);

    bool isConnected() const;
    void setIsConnected(bool newIsConnected);

    A7BRCCNodeModel *nodeModel() const;

    QJsonObject parseJson(const QString & jstring);
    int processJsonPacket(const QJsonObject & jobject);

signals:
    void serverAddressChanged();
    void serverPortChanged();
    void isConnectedChanged();

    void nodeModelChanged();
    void loginChanged();
    void passwordChanged();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onConnectionError();

public slots:
    void connectToServer(const QString &address, const quint16 port, const QString &login, const QString &password);
    void disconnectFromServer();
    void reconnect();
    void checkConnect();

    void testRead(const QString &testString);

    void loginResultReceived(const QJsonObject & jdata, Flags flags);

    void nodeListReceived(const QJsonObject &jdata);
    void nodeInfoChanged(const QJsonObject &jdata);
    void backupListReceived(const QJsonObject &jdata);
    void backupInfoReceived(const QJsonObject &jdata);
    void nodeBackupAdded(const QJsonObject &jdata);
    void nodeBackupRemoved(const QJsonObject &jdata);
    void nodeAdded(const QJsonObject &jdata);
    void nodeRemoved(const QJsonObject &jdata);

public: // for qml
    Q_INVOKABLE void requestNodeList();
    Q_INVOKABLE void requestNodeBackupList(const QString &nodeUuid);
    Q_INVOKABLE void requestAddNode(const QString &nodeName, const QString &nodeUuid);
    Q_INVOKABLE void requestRemoveNode(const QString &nodeUuid);
    Q_INVOKABLE void requestNodeInfo(const QString &nodeUuid);
    Q_INVOKABLE void requestBackupInfo(const QString &nodeUuid, const QString &backupId);
    Q_INVOKABLE void requestCreateBackup(const QString &nodeUuid);
    Q_INVOKABLE void requestRemoveBackup(const QString &nodeUuid, const QString &backupId);
    Q_INVOKABLE void requestMoveBackup(const QString &nodeUuid, const QString &backupId, BackupType targetType);
    Q_INVOKABLE void requestNodeUpdate(const QString &nodeUuid, const QJsonObject &nodeData);


    QString login() const;
    void setLogin(const QString &newLogin);

    QString password() const;
    void setPassword(const QString &newPassword);

protected:
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};
#endif // A7BACKUPSERVERCONNECTION_H
