#include <QtTest>

#include "fileops_archive.h"
#include "gtksync.h"

// Réglages GTK écrits par VinX et noms d'archives de Fichiers (ADR 0052).
class TestGtkSync : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void settings()
    {
        const QString light = gtksync::settingsIni(false);
        QVERIFY(light.startsWith(QLatin1String("[Settings]\n")));
        QVERIFY(light.contains(QLatin1String("gtk-application-prefer-dark-theme=false")));
        QVERIFY(light.contains(QLatin1String("gtk-cursor-theme-name=vinx-cursors")));
        QVERIFY(gtksync::settingsIni(true).contains(QLatin1String("prefer-dark-theme=true")));
    }
    void css()
    {
        const QString c = gtksync::css(QStringLiteral("#10DD8C"), QStringLiteral("#06130D"));
        QVERIFY(c.startsWith(QString::fromUtf8(gtksync::marker())));
        QVERIFY(c.contains(QLatin1String("@define-color accent_bg_color #10DD8C;")));
        QVERIFY(c.contains(QLatin1String("@define-color accent_fg_color #06130D;")));
    }
    void accent()
    {
        QCOMPARE(gtksync::adwaitaAccent(QStringLiteral("vert")), QStringLiteral("green"));
        QCOMPARE(gtksync::adwaitaAccent(QStringLiteral("lagon")), QStringLiteral("blue"));
        QCOMPARE(gtksync::adwaitaAccent(QStringLiteral("inconnu")), QStringLiteral("green"));
    }
    void archives()
    {
        QCOMPARE(archive::stem(QStringLiteral("photos.zip")), QStringLiteral("photos"));
        QCOMPARE(archive::stem(QStringLiteral("Sauvegarde.TAR.GZ")), QStringLiteral("Sauvegarde"));
        QCOMPARE(archive::stem(QStringLiteral("a.tar.xz")), QStringLiteral("a"));
        QCOMPARE(archive::stem(QStringLiteral("notes.txt")), QString());
        QCOMPARE(archive::stem(QStringLiteral(".zip")), QString());
    }
};

QTEST_GUILESS_MAIN(TestGtkSync)
#include "tst_gtksync.moc"
