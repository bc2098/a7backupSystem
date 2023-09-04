#ifndef A7BACKUPSYSTEMBASE_H
#define A7BACKUPSYSTEMBASE_H
#include <QtCore>

enum A7BackupNetworkCommands {
    LoginCMD = 1,
    KeepaliveCMD = 2,
    FileHeaderCMD = 3,
    FilePartCMD = 4,
    //FilePartQueryCMD = 5,
    //FileHeaderQueryCMD = 6,
    BackupListCMD = 5,
    ActionCMD = 6,
    //ErrorCMD = 0x
};


enum A7BackupNetworkFlags {
    NoFlagsFLG = 0,
    ErrorFLG = 0x80,
    ResponseFLG = 0x01,
    NeedResponseFLG = 0x02,
};

enum A7BackupNetworkError {
    NoError         = 0 ,
    NoAnswer        = 1 ,
    LoginError      = 2 ,
    PinError        = 3 ,
    SizeError       = 4 ,
    RequestError    = 5 ,
    ActionBusy      = 6 ,
    BackupNotFound  = 7 ,
     //NodeError=5,

};

enum A7BackupNetworkAction {
    NoAction = 0,
    CreateAction = 1,
};
struct NetworkPin {
    qint32 index;//=-1;
    qint64 time;//=0;
    quint64 random;//=0;
    NetworkPin&reset();
}__attribute__ ((__packed__));

/*
///fixed size
struct NetworkPacketTitleToServer {
    quint32 version; //0
    quint8 uuid[16];
    NetworkPin pin;
    quint64 time;
    quint64 seq;
    quint16 command;
    quint16 flags;
    NetworkPacketTitleToServer();
    NetworkPacketTitleToServer(const NetworkPacketTitleToClient&src);
    NetworkPacketTitleToServer(const NetworkPacketTitleToServer&src);
    NetworkPacketTitleToServer&copy(const NetworkPacketTitleToClient&src);
    NetworkPacketTitleToServer&copy(const NetworkPacketTitleToServer&src);

}__attribute__ ((__packed__));
///fixed size

struct NetworkPacketTitleToClient {
    quint32 version; //0
    quint8 uuid[16];
    NetworkPin pin;
    quint64 time;
    quint64 seq;
    quint16 command;
    quint16 flags;
    NetworkPacketTitleToClient();
    NetworkPacketTitleToClient(const NetworkPacketTitleToClient&src);
    NetworkPacketTitleToClient(const NetworkPacketTitleToServer&src);
    NetworkPacketTitleToClient&copy(const NetworkPacketTitleToClient&src);
    NetworkPacketTitleToClient&copy(const NetworkPacketTitleToServer&src);
}__attribute__ ((__packed__));
*/
struct NetworkPacketTitle {
    quint32 version; //0
    quint8 uuid[16];
    NetworkPin pin;
    qint64 time;
    quint64 seq;
    quint16 command;
    quint16 flags;
    quint8 sign[16];//RESERVED
    NetworkPacketTitle();
    NetworkPacketTitle(const NetworkPacketTitle&src);
    NetworkPacketTitle(const NetworkPacketTitle*src);
    NetworkPacketTitle&copy(const NetworkPacketTitle&src);
    NetworkPacketTitle *copy(const NetworkPacketTitle*src);
    A7BackupNetworkCommands networkCommand()const;
    static int titleLen() ;
    bool isAnswer() const;
    bool isResponse() const;
    bool hasError() const;
    QUuid toUuid()const;
    void fromUuid(const QUuid&u);

}__attribute__ ((__packed__));

struct NetworkPacketKeepAliveToClient {
    qint64 reserved;
/*    qint32 count;
    qint64 firstBackupTime;
    qint64 firstBackupIndex;
    qint64 lastBackupTime;
    qint64 lastBackupIndex;
*/
}__attribute__ ((__packed__));

struct NetworkPacketKeepAliveToServer {
    qint32 count;
    qint64 firstBackupTime;
    //qint64 firstBackupIndex;
    qint64 lastBackupTime;
    qint64 currentActionType;
    qint64 currentActionTime;
    qint16 actionPercent;
    //qint64 lastBackupIndex;
}__attribute__ ((__packed__));

///fixed size
struct NetworkPacketLoginToServer {
    quint16 size;
    char md5[16]; //uuid+time+password
}__attribute__ ((__packed__));

///fixed size
struct NetworkPacketLoginToClient {
    quint16 loginStatus;
    quint16 size;
    NetworkPin pin; //uuid+time+password

}__attribute__ ((__packed__));

struct NetworkPacketBackupListUpdateToClient { //ask header
    qint64 fromBackupTime;
}__attribute__ ((__packed__));

struct NetworkPacketBackupListUpdateToServer { //ask header
    quint16 count;
    qint64 backups[0];
    int len()const;
}__attribute__ ((__packed__));


struct NetworkPacketFileHeaderToClient { //ask header
    qint64 backupTime;
    //quint64 backupIndex;
}__attribute__ ((__packed__));

struct NetworkPacketActionToServer { //ask header
    qint64 actionTime;
    qint64 actionType;
    //quint64 backupIndex;
}__attribute__ ((__packed__));

struct NetworkPacketActionToClient { //ask header
    qint64 actionTime;
    qint64 actionType;
    //quint64 backupIndex;
}__attribute__ ((__packed__));

/*
struct NetworkPacketBackupCount {
    qint32 count;
    qint64 firstBackupTime;
    //qint64 firstBackupIndex;
    qint64 lastBackupTime;
    //qint64 lastBackupIndex;
}__attribute__ ((__packed__));
*/
struct NetworkPacketBackupProcess {
    qint64 backupTime;
    //qint64 backupIndex;
    qint32 processState;
    qint32 percents;
}__attribute__ ((__packed__));


struct NetworkPacketFileHeaderToServer { //response header
    qint64 backupTime;
    //quint64 backupIndex;
    qint64 backupSize;
    qint64 partSize; //count is size/partSize  + 1/0
    qint64 partCount;
}__attribute__ ((__packed__));

struct NetworkPacketFilePartToClient {
    qint64 backupTime;
    qint64 count;
    qint64 partNumber[0];
    int len() const;
}__attribute__ ((__packed__));

struct NetworkPacketFilePartToServer {
    qint64 backupTime;
    qint64 backupOffset;
    qint64 partNumber;
    qint16 partSize;
    char data[0];
    int len() const;
}__attribute__ ((__packed__));


struct NetworkPacketError {
    qint16 errorCode;
    qint64 time;
}__attribute__ ((__packed__));


union NetworkPacketDataToClient {
    NetworkPacketKeepAliveToClient keepAlive;
    NetworkPacketLoginToClient login;
    NetworkPacketBackupListUpdateToClient backupList;
    NetworkPacketActionToClient action;
    NetworkPacketFileHeaderToClient fileHeader;
    NetworkPacketFilePartToClient filePart;
    NetworkPacketError error;

    quint8 data[512];

}__attribute__ ((__packed__));

union NetworkPacketDataToServer {
    NetworkPacketKeepAliveToServer keepAlive;
    NetworkPacketLoginToServer login;
    NetworkPacketBackupListUpdateToServer backupList;
    NetworkPacketActionToServer action;
    NetworkPacketFileHeaderToServer fileHeader;

    NetworkPacketFilePartToServer filePart;
    NetworkPacketError error;

    quint8 data[512];
}__attribute__ ((__packed__));


///flexible size: for reciever only!!!
struct NetworkPacketToClient {
    NetworkPacketTitle title;
    NetworkPacketDataToClient data;
    NetworkPacketToClient();
    const char* asConstChar() const;
    char* asChar();
    int networkPacketLen()const ;
    A7BackupNetworkCommands networkCommand()const;
    static int titleLen();



} __attribute__ ((__packed__));

///flexible size: for reciever only!!!
struct NetworkPacketToServer {
    NetworkPacketTitle title;
    NetworkPacketDataToServer data;
    NetworkPacketToServer();
    const char* asConstChar() const;
    char* asChar();
    int networkPacketLen()const ;
    A7BackupNetworkCommands networkCommand()const;
    static int titleLen();
}  __attribute__ ((__packed__));





class A7BaseBackup{
public:
    static const int MAX_PART_SIZE = 400;
    static const int MAX_LIST_COUNT = 50;
public:
    enum Flags {
        Event       = 0x01,
        Response    = 0x02,
        Request     = 0x04,
        Error       = 0x08,
        Ok          = 0x10
    };

    enum EntityTypes {
        System      = 0x01,
        Node        = 0x02,
        Backup      = 0x03
    };

    enum CommandTypes {
        Info        = 0x10,
        List        = 0x20,
        Add         = 0x30,
        Remove      = 0x40,
        Update      = 0x50,
        Login       = 0x60
        ///Create      = 0x70
    };

    enum Commands {
        NodeList        = Node | List,
        NodeInfo        = Node | Info,
        NodeAdd         = Node | Add,
        NodeRemove      = Node | Remove,
        NodeUpdate      = Node | Update,

        BackupList      = Backup | List,
        BackupInfo      = Backup | Info,
        BackupAdd       = Backup | Add,
        BackupRemove    = Backup | Remove,

        SystemLogin     = System | Login
    };

    enum class BackupType {
        Unknown     = 0,
        Daily       = 1,
        Weekly      = 2,
        Monthly     = 3,
        Yearly      = 4
    };

    enum class BackupStatus {
        Unknown = 0,
        Existed = 1,
        HeaderDowloading = 2,
        PartsDowloading = 3,
        Downloaded = 4,
        Error = -1
    };
public:
    A7BaseBackup ();
    virtual ~A7BaseBackup();
    //static int networkPacketToServerLen(A7BackupNetworkCommands networkCommand);
    //static int networkPacketToClientLen(A7BackupNetworkCommands networkCommand);


};

#endif // A7BACKUPSYSTEMBASE_H
