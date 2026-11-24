#include "preferences.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
#include <QFileInfo>

Preferences::Preferences(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("vinx"), QStringLiteral("shell"))
{
    QDir().mkpath(QFileInfo(m_settings.fileName()).absolutePath());
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &Preferences::reload);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &Preferences::reload);
    watch();
}

void Preferences::watch()
{
    const QString file = m_settings.fileName();
    const QString dir = QFileInfo(file).absolutePath();
    if (!m_watcher.directories().contains(dir))
        m_watcher.addPath(dir);
    if (QFileInfo::exists(file) && !m_watcher.files().contains(file))
        m_watcher.addPath(file);   // QSettings remplace le fichier : on le surveille à nouveau
}

void Preferences::reload()
{
    m_settings.sync();
    watch();
    ++m_revision;
    Q_EMIT changed();
}

QVariant Preferences::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings.value(key, defaultValue);
}

void Preferences::setValue(const QString &key, const QVariant &value)
{
    if (m_settings.value(key) == value)
        return;
    m_settings.setValue(key, value);
    m_settings.sync();
    watch();
    ++m_revision;
    Q_EMIT changed();
    // Le shell surveille le fichier, mais la surveillance peut manquer un remplacement atomique :
    // on le prévient aussi directement (fond d'écran, dock…) quand le réglage vient d'une autre application.
    if (qEnvironmentVariableIntValue("VINX_SHELL_PROCESS") != 1) {
        const QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.vinx.Shell"), QStringLiteral("/Shell"),
                                                                QStringLiteral("org.vinx.Shell"), QStringLiteral("reloadPreferences"));
        QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
    }
}
