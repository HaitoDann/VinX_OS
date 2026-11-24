#include "battery.h"
#include "dbusutil.h"

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDir>
#include <QFile>

namespace {
const QString kService = QStringLiteral("org.freedesktop.UPower");
const QString kPath = QStringLiteral("/org/freedesktop/UPower/devices/DisplayDevice");
const QString kIface = QStringLiteral("org.freedesktop.UPower.Device");
}

Battery::Battery(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::systemBus().connect(kService, kPath, QStringLiteral("org.freedesktop.DBus.Properties"),
                                         QStringLiteral("PropertiesChanged"), this, SLOT(refresh()));
    refresh();
}

void Battery::refresh()
{
    const QVariantMap p = VinxDBus::getAll(QDBusConnection::systemBus(), kService, kPath, kIface);
    // Type 2 = batterie ; State : 1 en charge, 4 pleine, 5 en attente de charge.
    m_present = p.value(QStringLiteral("IsPresent")).toBool() && p.value(QStringLiteral("Type")).toUInt() == 2;
    m_percentage = qRound(p.value(QStringLiteral("Percentage")).toDouble());
    const uint state = p.value(QStringLiteral("State")).toUInt();
    m_charging = state == 1 || state == 4 || state == 5;
    m_timeRemaining = p.value(state == 1 ? QStringLiteral("TimeToFull") : QStringLiteral("TimeToEmpty")).toLongLong();
    // Santé : lue sur la vraie batterie (le périphérique agrégé ne la donne pas).
    m_health = -1;
    QDBusInterface upower(kService, QStringLiteral("/org/freedesktop/UPower"), QStringLiteral("org.freedesktop.UPower"),
                          QDBusConnection::systemBus());
    const QDBusReply<QList<QDBusObjectPath>> devices = upower.isValid() ? upower.call(QStringLiteral("EnumerateDevices"))
                                                                         : QDBusReply<QList<QDBusObjectPath>>();
    for (const QDBusObjectPath &dev : devices.isValid() ? devices.value() : QList<QDBusObjectPath>()) {
        const QVariantMap d = VinxDBus::getAll(QDBusConnection::systemBus(), kService, dev.path(), kIface);
        if (d.value(QStringLiteral("Type")).toUInt() == 2 && d.value(QStringLiteral("PowerSupply")).toBool()) {
            const double cap = d.value(QStringLiteral("Capacity")).toDouble();
            if (cap > 0)
                m_health = qBound(0, qRound(cap), 100);
            break;
        }
    }
    Q_EMIT changed();
}

int Battery::chargeLimit() const
{
    const QDir ps(QStringLiteral("/sys/class/power_supply"));
    for (const QString &bat : ps.entryList({QStringLiteral("BAT*")}, QDir::Dirs | QDir::System)) {
        QFile f(ps.filePath(bat + QStringLiteral("/charge_control_end_threshold")));
        if (f.open(QIODevice::ReadOnly))
            return f.readAll().trimmed().toInt();
    }
    return -1;
}
