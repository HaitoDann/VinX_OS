#include <QtTest>

#include "devices.h"
#include "trashspec.h"

// Fichiers sans KIO (ADR 0054) : corbeille freedesktop et appareils amovibles.
class TestFilesBackend : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void trash()
    {
        QTemporaryDir root;
        const QString trash = root.filePath(QStringLiteral("Trash"));
        QDir().mkpath(trash + QStringLiteral("/files"));
        QDir().mkpath(trash + QStringLiteral("/info"));
        QDir().mkpath(root.filePath(QStringLiteral("Documents")));
        // Élément « Mon CV.odt » supprimé de Documents (chemin encodé, comme l'écrivent KDE et GNOME).
        QFile f(trash + QStringLiteral("/files/Mon CV.odt"));
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("cv");
        f.close();
        QFile info(trash + QStringLiteral("/info/Mon CV.odt.trashinfo"));
        QVERIFY(info.open(QIODevice::WriteOnly));
        info.write("[Trash Info]\nPath=" + QUrl::toPercentEncoding(root.filePath(QStringLiteral("Documents/Mon CV.odt")), "/")
                   + "\nDeletionDate=2026-11-25T10:00:00\n");
        info.close();

        const auto items = trashspec::list(trash);
        QCOMPARE(items.size(), 1);
        QCOMPARE(items.first().originalPath, root.filePath(QStringLiteral("Documents/Mon CV.odt")));
        QCOMPARE(items.first().deleted.date(), QDate(2026, 11, 25));

        // La place est prise : restauré en « Mon CV (2).odt ».
        QFile taken(root.filePath(QStringLiteral("Documents/Mon CV.odt")));
        QVERIFY(taken.open(QIODevice::WriteOnly));
        taken.close();
        QCOMPARE(trashspec::restore(items.first()), root.filePath(QStringLiteral("Documents/Mon CV (2).odt")));
        QVERIFY(trashspec::list(trash).isEmpty());
        QVERIFY(!QFile::exists(trash + QStringLiteral("/info/Mon CV.odt.trashinfo")));
    }
    void emptyTrash()
    {
        QTemporaryDir root;
        const QString trash = root.filePath(QStringLiteral("Trash"));
        QDir().mkpath(trash + QStringLiteral("/files/dossier/sous"));
        QDir().mkpath(trash + QStringLiteral("/info"));
        QVERIFY(trashspec::empty(trash));
        QVERIFY(trashspec::list(trash).isEmpty());
    }
    void lsblk()
    {
        const QByteArray json = R"({"blockdevices":[
          {"path":"/dev/nvme0n1","label":null,"mountpoint":null,"rm":false,"hotplug":false,"type":"disk","fstype":null,"size":"238G",
           "children":[{"path":"/dev/nvme0n1p2","label":null,"mountpoint":"/","rm":false,"hotplug":false,"type":"part","fstype":"btrfs","size":"237G"}]},
          {"path":"/dev/sdb","label":null,"mountpoint":null,"rm":true,"hotplug":true,"type":"disk","fstype":null,"size":"29G",
           "children":[{"path":"/dev/sdb1","label":"CLE","mountpoint":null,"rm":true,"hotplug":true,"type":"part","fstype":"vfat","size":"29G"}]}]})";
        const QVariantList d = devices::parseLsblk(json);
        QCOMPARE(d.size(), 1);
        QCOMPARE(d.first().toMap().value(QStringLiteral("path")).toString(), QStringLiteral("/dev/sdb1"));
        QCOMPARE(d.first().toMap().value(QStringLiteral("label")).toString(), QStringLiteral("CLE"));
        QCOMPARE(devices::parseMountedAt(QStringLiteral("Mounted /dev/sdb1 at /media/vincent/CLE.\n")), QStringLiteral("/media/vincent/CLE"));
        QCOMPARE(devices::parseMountedAt(QStringLiteral("Error")), QString());
    }
};

QTEST_GUILESS_MAIN(TestFilesBackend)
#include "tst_filesbackend.moc"
