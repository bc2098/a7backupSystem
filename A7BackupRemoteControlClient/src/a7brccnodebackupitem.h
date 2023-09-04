#ifndef A7BRCCNODEBACKUPITEM_H
#define A7BRCCNODEBACKUPITEM_H

#include "a7backupsystembase.h"
#include <QObject>

class A7BRCCNodeBackupItem : public QObject, public A7BaseBackup
{
    Q_OBJECT
public:
    enum class QBackupType {
        UnknownType     = int(BackupType::Unknown),
        DailyType       = int(BackupType::Daily),
        WeeklyType      = int(BackupType::Weekly),
        MonthlyType     = int(BackupType::Monthly),
        YearlyType      = int(BackupType::Yearly)
    };
    Q_ENUM(QBackupType)

    enum class QBackupStatus {
        UnknownStatus           = int(BackupStatus::Unknown),
        ExistedStatus           = int(BackupStatus::Existed),
        HeaderDowloadingStatus  = int(BackupStatus::HeaderDowloading),
        PartsDowloadingStatus   = int(BackupStatus::PartsDowloading),
        DownloadedStatus        = int(BackupStatus::Downloaded),
        ErrorStatus             = int(BackupStatus::Error)
    };
    Q_ENUM(QBackupStatus)

protected:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged FINAL)
    Q_PROPERTY(int backupIndex READ getBackupIndex WRITE setBackupIndex NOTIFY backupIndexChanged FINAL)
    Q_PROPERTY(QBackupType backupType READ backupType NOTIFY backupTypeChanged FINAL)
    Q_PROPERTY(int size READ size  NOTIFY sizeChanged FINAL)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(QBackupStatus status READ status NOTIFY statusChanged FINAL)


public:
    explicit A7BRCCNodeBackupItem(QObject *parent = nullptr);
    ~A7BRCCNodeBackupItem();


    QString name() const;
    void setName(const QString &newName);

    QString id() const;
    void setId(const QString &newId);

    int getBackupIndex() const;
    void setBackupIndex(int newBackupIndex);

    QBackupType backupType() const;

    int size() const;

    QString fileName() const;

    A7BRCCNodeBackupItem::QBackupStatus status() const;

    void fromJson(const QJsonObject & jdata);



signals:

    void nameChanged();
    void idChanged();
    void backupIndexChanged();
    void backupTypeChanged();
    void sizeChanged();
    void fileNameChanged();
    void statusChanged();

private:
    QString m_name;
    QString m_id;
    int m_backupIndex = -1;
    QBackupType m_backupType;
    int m_size;
    QString m_fileName;
    QBackupStatus m_status;
};

//Q_DECLARE_METATYPE(A7BRCCNodeBackupItem::BackupType)
//Q_DECLARE_METATYPE(A7BRCCNodeBackupItem::BackupStatus)
#endif // A7BRCCNODEBACKUPITEM_H
