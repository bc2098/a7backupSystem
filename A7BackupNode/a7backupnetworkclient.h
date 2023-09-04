#ifndef A7BACKUPNETWORKCLIENT_H
#define A7BACKUPNETWORKCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

#include "a7backupsystembase.h"


class A7BackupNodeMaster;
class A7BackupItem;
class A7BackupNetworkClient : public QObject, public A7BaseBackup
{
    Q_OBJECT



protected:
    A7BackupNodeMaster*  m_master = nullptr;
    QUdpSocket * m_udp = nullptr;
    QHostAddress m_remoteAddress = QHostAddress("127.0.0.1");
    quint16 m_remotePort =3434;
    NetworkPin m_pin;
    //quint16 m_loacalPort =0;
protected:
    void sendEvent();
    void receivedDatagram( const  QNetworkDatagram & datagramm );
public:
    explicit A7BackupNetworkClient(A7BackupNodeMaster * master);
    virtual ~A7BackupNetworkClient();
    int start();


public:
    void sendToServer(NetworkPacketToServer &data);
    void sendToServer(const QByteArray& data);
    //void onNewBackupAdded(A7BackupItem*item);
    //void onNewBackupCompleted(A7BackupItem*item);
    //void onNewBackupError(A7BackupItem*item);


    const NetworkPin &getPin() const;
    NetworkPin &getPin();

protected slots:
    void readyRead();

signals:

};

#endif // A7BACKUPNETWORKCLIENT_H
