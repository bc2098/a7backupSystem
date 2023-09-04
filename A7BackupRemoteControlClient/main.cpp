#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "src/a7backupserverconnection.h"
#include "src/a7brccnodebackupitem.h"
#include "src/a7brccnodebackupmodel.h"
#include "src/a7brccnodemodel.h"
#include "src/a7brccnodeitem.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<A7BackupServerConnection>("BRCC", 1, 0, "BackupConnection");
    qmlRegisterUncreatableType<A7BRCCNodeItem>("BRCC", 1, 0, "NodeItem", "You can't create A7BRCCNodeItem itself!");
    qmlRegisterUncreatableType<A7BRCCNodeModel>("BRCC", 1, 0, "NodeModel", "You can't create A7BRCCNodeModel itself!");
    qmlRegisterUncreatableType<A7BRCCNodeBackupModel>("BRCC", 1, 0, "BackupModel", "You can't create A7BRCCNodeBackupModel itself!");
    qmlRegisterUncreatableType<A7BRCCNodeBackupItem>("BRCC", 1, 0, "BackupItem", "You can't create A7BRCCNodeBackupItem itself!");

    qmlRegisterSingletonType(QUrl("qrc:/qml/B.qml"), "BRCC", 1, 0, "B");

//    qRegisterMetaType<A7BRCCNodeBackupItem::BackupType>();
//    qRegisterMetaType<BackupStatus>();

//    qmlRegisterUncreatableType<A7BRCCNodeBackupItem::BackupType>("BRCC", 1, 0, "BackupType", "You can't create A7BRCCNodeBackupItem itself!");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
