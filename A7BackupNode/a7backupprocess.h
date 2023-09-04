#ifndef A7BACKUPPROCESS_H
#define A7BACKUPPROCESS_H

#include <QThread>
#include <QtCore>

class A7BackupProcess : public QThread
{
    Q_OBJECT
protected:
    QStringList m_backupList;
    QString m_pathDst;
    qint64 m_backup=0;
    QString m_resultFileName;
    QProcess m_process;
public:
    explicit A7BackupProcess(QObject *parent = nullptr);
    qint64 startBackup(qint64 backup, const QJsonObject&settings);

signals:
    void completed(qint64 backup, QString fileName);
    void error(qint64 backup, QString error);
    void percent(qint64 backup, qint16 percent);


    // QThread interface
protected:
    virtual void run() Q_DECL_OVERRIDE;
};

#endif // A7BACKUPPROCESS_H
