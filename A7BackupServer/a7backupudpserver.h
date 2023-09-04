#ifndef A7BACKUPUDPSERVER_H
#define A7BACKUPUDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QUuid>
#include <QNetworkDatagram>
#include "a7backupsystembase.h"


class A7BackupNodesServer;
class A7BackupNode;
class A7BackupUdpServer : public QObject, public A7BaseBackup
{
    Q_OBJECT
protected:
    QUdpSocket * m_udp = nullptr; //listener?
    A7BackupNodesServer* m_nodes = nullptr;
protected:
    void onReadDatagramm(const QNetworkDatagram&datagramm);
    void onReadPacket(const NetworkPacketToServer &packet, const QHostAddress &address, const quint16 port);
    void errorAnswer(const NetworkPacketToServer &packet, int error, const QHostAddress &address, const quint16 port);
public:
    explicit A7BackupUdpServer(A7BackupNodesServer* nodes);
    virtual ~A7BackupUdpServer();
    void start();
    void stop();

    void loginResponseOk(const NetworkPacketTitle &title, const A7BackupNode*node);
    void sendPacket (NetworkPacketToClient &packet, A7BackupNode * node);

public slots:
    void onRead();

signals:

};

#endif // A7BACKUPUDPSERVER_H
