QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/a7backupserverconnection.cpp \
        src/a7brccnodebackupitem.cpp \
        src/a7brccnodebackupmodel.cpp \
        src/a7brccnodeitem.cpp \
        src/a7brccnodemodel.cpp

RESOURCES += qml.qrc

include($$PWD/qmlSrc/qmlsources.pri)
include($$PWD/../A7BackupBase/a7backupsystembase.pri)

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/a7backupserverconnection.h \
    src/a7brccnodebackupitem.h \
    src/a7brccnodebackupmodel.h \
    src/a7brccnodeitem.h \
    src/a7brccnodemodel.h

DISTFILES += \
    JsonMessages
