#ifndef A7REMOTECONTROLCONNECTION_H
#define A7REMOTECONTROLCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include "a7backupsystembase.h"


class A7RemoteControlServer;
class A7RemoteControlConnection : public QObject, public A7BaseBackup
{
    Q_OBJECT
protected:
    A7RemoteControlServer* m_server=nullptr;
    QTcpSocket* m_socket=nullptr;
    QByteArray m_bufferIn;
protected:
    void unregisterFromServer();
    void extract();
    int extractPacket();
    void extractPacket(const QByteArray & jdata);
    void extractPacket(const QJsonObject & json);
    void receivedPacket(int command,const QJsonObject&data);
public:
    explicit A7RemoteControlConnection(A7RemoteControlServer * server, QTcpSocket* socket);
    ~A7RemoteControlConnection();
    void free();
    void unregisterServer();
    void sendToClient(const QByteArray&data);
protected slots:
    void onReadyRead();
    void onDisconnected();
signals:

};

#endif // A7REMOTECONTROLCONNECTION_H
