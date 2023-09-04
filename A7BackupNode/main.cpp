#include "a7backupnodemaster.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    A7BackupNodeMaster master;
    master.start();

    return a.exec();
}
