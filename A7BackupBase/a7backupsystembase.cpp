#include "a7backupsystembase.h"




A7BaseBackup::A7BaseBackup()
{

}

A7BaseBackup::~A7BaseBackup()
{

}


NetworkPacketTitle::NetworkPacketTitle()
{
    memset(this,0,sizeof(*this));
}

NetworkPacketTitle::NetworkPacketTitle(const NetworkPacketTitle &src)
{
    copy(src);
}

NetworkPacketTitle::NetworkPacketTitle(const NetworkPacketTitle *src)
{
    copy(src);
}

NetworkPacketTitle &NetworkPacketTitle::copy(const NetworkPacketTitle &src)
{
    return *copy(&src);
}

NetworkPacketTitle *NetworkPacketTitle::copy(const NetworkPacketTitle *src)
{
    memcpy(this,src,sizeof(*src));
    return this;
}

A7BackupNetworkCommands NetworkPacketTitle::networkCommand() const
{
    return static_cast<A7BackupNetworkCommands> (command);
}

int NetworkPacketTitle::titleLen()
{
    return sizeof(NetworkPacketTitle);
}

bool NetworkPacketTitle::isAnswer() const
{
    return flags & ResponseFLG;
}

bool NetworkPacketTitle::isResponse() const
{
    return flags & ResponseFLG;
}

bool NetworkPacketTitle::hasError() const
{
    return flags & ErrorFLG;
}

QUuid NetworkPacketTitle::toUuid() const
{
    return QUuid::fromRfc4122(QByteArray(reinterpret_cast<const char*>(uuid),16));
}

void NetworkPacketTitle::fromUuid(const QUuid &u)
{
    memcpy(uuid,u.toRfc4122().constData(),16);
}

NetworkPacketToClient::NetworkPacketToClient()
{
    memset(this,0,sizeof(*this));
}

const char *NetworkPacketToClient::asConstChar() const
{
    return reinterpret_cast<const char* >(this);
}

char *NetworkPacketToClient::asChar()
{
    return reinterpret_cast<char* >(this);
}

int NetworkPacketToClient::networkPacketLen() const
{
    int len = sizeof (NetworkPacketTitle);
    if(title.hasError()){
        return len + sizeof(NetworkPacketError);
    }
    switch (networkCommand()) {
    case    LoginCMD : len += sizeof ( NetworkPacketLoginToClient); break;
    case    KeepaliveCMD : len += sizeof ( NetworkPacketKeepAliveToClient); break;
    case    BackupListCMD : len += sizeof (NetworkPacketBackupListUpdateToClient);break;
    case    FileHeaderCMD : len += sizeof ( NetworkPacketFileHeaderToClient); break;
    case    FilePartCMD : len += this->data.filePart.len(); break;
    case    ActionCMD : len += sizeof ( NetworkPacketActionToClient); break;
     }
    return len;

}

int NetworkPacketToServer::networkPacketLen() const
{
    int len = sizeof (NetworkPacketTitle);
    if(title.hasError()){
        return len + sizeof(NetworkPacketError);
    }
     switch (networkCommand()) {
     case    LoginCMD : len += sizeof ( NetworkPacketLoginToServer); break;
     case    KeepaliveCMD : len += sizeof ( NetworkPacketKeepAliveToServer); break;
     case    FileHeaderCMD : len += sizeof ( NetworkPacketFileHeaderToServer); break;
     case    FilePartCMD : len +=  this->data.filePart.len(); break;
     case    BackupListCMD: len += this->data.backupList.len(); break;
     case    ActionCMD: len += sizeof ( NetworkPacketActionToServer); break;

     }
     return len;

}


A7BackupNetworkCommands NetworkPacketToClient::networkCommand() const
{
    return title.networkCommand();
}

int NetworkPacketToClient::titleLen()
{
    return NetworkPacketTitle::titleLen();
}

NetworkPacketToServer::NetworkPacketToServer()
{
    memset(this,0,sizeof(*this));
}

const char *NetworkPacketToServer::asConstChar() const
{
    return reinterpret_cast<const char* >(this);
}

char *NetworkPacketToServer::asChar()
{
    return reinterpret_cast<char*>(this);
}


A7BackupNetworkCommands NetworkPacketToServer::networkCommand() const
{
    return title.networkCommand();
}

int NetworkPacketToServer::titleLen()
{
    return NetworkPacketTitle::titleLen();
}

NetworkPin &NetworkPin::reset()
{
    memset(this,0,sizeof(*this));
    index = -1;
    return *this;

}

int NetworkPacketBackupListUpdateToServer::len() const
{
    return sizeof(*this) + count * sizeof (qint64);
}


int NetworkPacketFilePartToClient::len() const
{
    return sizeof(*this) + count * sizeof(qint64);
}

int NetworkPacketFilePartToServer::len() const
{
    return sizeof(*this)+partSize;
}
