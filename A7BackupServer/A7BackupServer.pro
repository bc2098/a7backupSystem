QT = core network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/../A7BackupBase/a7backupsystembase.pri)

SOURCES += \
        a7backupnode.cpp \
        a7backupnodesserver.cpp \
        a7backupsave.cpp \
        a7backupudpserver.cpp \
        a7remotecontrolconnection.cpp \
        a7remotecontrolserver.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    a7backupnode.h \
    a7backupnodesserver.h \
    a7backupsave.h \
    a7backupudpserver.h \
    a7remotecontrolconnection.h \
    a7remotecontrolserver.h
