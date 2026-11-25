#pragma once

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

// Corbeille selon la norme freedesktop (ADR 0054), sans KIO : ~/.local/share/Trash/files (éléments)
// et Trash/info/<nom>.trashinfo (chemin d'origine, date). Même corbeille que KDE, GNOME et
// QFile::moveToTrash. La corbeille d'une clé USB (.Trash-UID) n'est pas encore gérée.
namespace trashspec {

struct Item {
    QString name;          // nom dans Trash/files (unique)
    QString filesPath;     // chemin dans la corbeille
    QString infoPath;      // fichier .trashinfo
    QString originalPath;  // emplacement d'origine
    QDateTime deleted;
};

inline QString homeTrash()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/Trash");
}

// Lit « Path= » et « DeletionDate= » (chemin encodé en pourcentages, comme une URL).
inline bool readInfo(const QString &infoPath, QString *original, QDateTime *deleted)
{
    QFile f(infoPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    bool header = false;
    while (!f.atEnd()) {
        const QString line = QString::fromUtf8(f.readLine()).trimmed();
        if (line == QLatin1String("[Trash Info]"))
            header = true;
        else if (header && line.startsWith(QLatin1String("Path=")))
            *original = QUrl::fromPercentEncoding(line.mid(5).toUtf8());
        else if (header && line.startsWith(QLatin1String("DeletionDate=")))
            *deleted = QDateTime::fromString(line.mid(13), Qt::ISODate);
    }
    return header && !original->isEmpty();
}

inline QList<Item> list(const QString &trash = homeTrash())
{
    QList<Item> out;
    const QDir files(trash + QStringLiteral("/files"));
    for (const QString &name : files.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System)) {
        Item it;
        it.name = name;
        it.filesPath = files.filePath(name);
        it.infoPath = trash + QStringLiteral("/info/") + name + QStringLiteral(".trashinfo");
        if (!readInfo(it.infoPath, &it.originalPath, &it.deleted))
            it.originalPath = QDir::homePath() + QLatin1Char('/') + name;   // info perdue : retour au dossier personnel
        out.append(it);
    }
    return out;
}

inline bool find(const QString &name, Item *item, const QString &trash = homeTrash())
{
    for (const Item &it : list(trash))
        if (it.name == name) {
            *item = it;
            return true;
        }
    return false;
}

inline bool removePath(const QString &path)
{
    const QFileInfo fi(path);
    if (fi.isDir() && !fi.isSymLink())
        return QDir(path).removeRecursively();
    return QFile::remove(path);
}

// Remet l'élément à sa place ; « nom (2) » si la place est prise. Renvoie le chemin restauré.
inline QString restore(const Item &it)
{
    const QFileInfo orig(it.originalPath);
    QDir parent = orig.dir();
    if (!parent.exists() && !QDir().mkpath(parent.absolutePath()))
        return {};
    QString target = orig.absoluteFilePath();
    for (int n = 2; QFileInfo::exists(target) || QFileInfo(target).isSymLink(); ++n)
        target = parent.filePath(orig.completeBaseName() + QStringLiteral(" (%1)").arg(n)
                                 + (orig.suffix().isEmpty() ? QString() : QLatin1Char('.') + orig.suffix()));
    if (!QFile::rename(it.filesPath, target))
        return {};
    QFile::remove(it.infoPath);
    return target;
}

inline bool erase(const Item &it)
{
    const bool ok = removePath(it.filesPath);
    if (ok)
        QFile::remove(it.infoPath);
    return ok;
}

inline bool empty(const QString &trash = homeTrash())
{
    bool ok = true;
    for (const Item &it : list(trash))
        ok = erase(it) && ok;
    return ok;
}

} // namespace trashspec
