#include "a7backupnodesserver.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    A7BackupNodesServer server;

    return a.exec();
}
