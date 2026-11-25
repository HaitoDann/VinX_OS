#pragma once

#include <QObject>
#include <functional>

#include "fileops_archive.h"
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

// Opérations sur les fichiers : copier, déplacer, corbeille, renommer, ouvrir, presse-papiers.
// Moteur KIO si disponible (conflits, progression, réseau, corbeille freedesktop), sinon
// opérations locales Qt.
class FileOps : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString task READ task NOTIFY busyChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY clipboardChanged)

public:
    explicit FileOps(QObject *parent = nullptr);
    bool busy() const { return m_jobs > 0; }
    QString task() const { return m_task; }
    int progress() const { return m_progress; }
    bool canPaste() const;

    // Conflits de noms : 0 = demander (KIO), 1 = remplacer, 2 = garder les deux, 3 = ignorer.
    enum Conflict { Ask = 0, Replace = 1, KeepBoth = 2, Skip = 3 };
    Q_ENUM(Conflict)
    Q_INVOKABLE void copy(const QList<QUrl> &urls, const QUrl &destDir, int conflict = KeepBoth);
    Q_INVOKABLE void move(const QList<QUrl> &urls, const QUrl &destDir, int conflict = KeepBoth);
    // Noms déjà présents dans destDir (dossiers locaux) ; les éléments déjà dans destDir sont ignorés.
    Q_INVOKABLE QStringList conflicts(const QList<QUrl> &urls, const QUrl &destDir) const;
    // Presse-papiers : éléments à coller et opération (couper = déplacer).
    Q_INVOKABLE QList<QUrl> clipboardUrls() const;
    Q_INVOKABLE bool clipboardIsCut() const;
    Q_INVOKABLE void trash(const QList<QUrl> &urls);
    Q_INVOKABLE void emptyTrash();
    // Corbeille : remet les éléments à leur emplacement d'origine, ou les supprime.
    Q_INVOKABLE void restore(const QList<QUrl> &trashUrls);
    Q_INVOKABLE void deletePermanently(const QList<QUrl> &urls);
    Q_INVOKABLE void rename(const QUrl &url, const QString &newName);
    // Crée « Nouveau dossier » (ou « Nouveau dossier (2) »…) et renvoie son nom.
    Q_INVOKABLE QString createFolder(const QUrl &dir, const QString &baseName);
    // Document vide (dossiers locaux) : « Nouveau document.txt », « .odt »… LibreOffice ouvre un
    // fichier vide comme un nouveau document du type de son extension. Renvoie le nom créé, ou "".
    Q_INVOKABLE QString createFile(const QUrl &dir, const QString &fileName);
    // Archives (ADR 0052), avec bsdtar (libarchive) : extraction dans un dossier du même nom, à côté
    // de l'archive (signal extracted), et compression en .zip. Dossiers locaux seulement.
    Q_INVOKABLE static bool isArchive(const QString &name) { return !archiveStem(name).isEmpty(); }
    Q_INVOKABLE QString extract(const QUrl &archive);
    // Appareils amovibles (ADR 0054) : monter « device:/dev/sdb1 » (signal mounted), éjecter un volume.
    Q_INVOKABLE void mount(const QUrl &device);
    Q_INVOKABLE void eject(const QUrl &mountPoint);
    Q_INVOKABLE QString compress(const QList<QUrl> &urls, const QUrl &dir);
    static QString archiveStem(const QString &name) { return archive::stem(name); }
    Q_INVOKABLE void open(const QUrl &url);
    Q_INVOKABLE void openTerminal(const QUrl &dir);

    // Presse-papiers système (compatible avec les autres gestionnaires de fichiers).
    Q_INVOKABLE void setClipboard(const QList<QUrl> &urls, bool cut);
    Q_INVOKABLE void paste(const QUrl &destDir, int conflict = KeepBoth);

    static bool validName(const QString &name);

Q_SIGNALS:
    void busyChanged();
    void progressChanged();
    void clipboardChanged();
    void failed(const QString &message);
    void done();
    void extracted(const QUrl &folder);
    void mounted(const QUrl &folder);
    void ejected(const QString &name);

private:
    // Travail long hors du fil de l'interface : renvoie un message d'erreur, ou "" si tout va bien.
    void runInThread(const std::function<QString(const std::function<void(int)> &)> &work);
    void begin(const QString &task);
    void finish(bool ok, const QString &error);

    int m_jobs = 0;
    QString m_task;
    int m_progress = -1;
};
