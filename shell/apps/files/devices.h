#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

// Clés USB et disques amovibles (ADR 0054), sans KDE : `lsblk -J` pour la liste, `udisksctl` pour
// monter et démonter (autorisé sans mot de passe pour la session active, par la règle polkit d'udisks).
namespace devices {

// Partitions amovibles avec un système de fichiers : [{ path, label, mountpoint, size }].
inline QVariantList parseLsblk(const QByteArray &json)
{
    QVariantList out;
    std::function<void(const QJsonArray &, bool)> walk = [&](const QJsonArray &nodes, bool removableParent) {
        for (const QJsonValue &v : nodes) {
            const QJsonObject o = v.toObject();
            const bool removable = removableParent || o.value(QStringLiteral("rm")).toBool() || o.value(QStringLiteral("hotplug")).toBool();
            const QString fstype = o.value(QStringLiteral("fstype")).toString();
            const QString type = o.value(QStringLiteral("type")).toString();
            if (removable && !fstype.isEmpty() && fstype != QLatin1String("swap") && (type == QLatin1String("part") || type == QLatin1String("disk")))
                out.append(QVariantMap{{QStringLiteral("path"), o.value(QStringLiteral("path")).toString()},
                                       {QStringLiteral("label"), o.value(QStringLiteral("label")).toString()},
                                       {QStringLiteral("mountpoint"), o.value(QStringLiteral("mountpoint")).toString()},
                                       {QStringLiteral("size"), o.value(QStringLiteral("size")).toString()}});
            walk(o.value(QStringLiteral("children")).toArray(), removable);
        }
    };
    walk(QJsonDocument::fromJson(json).object().value(QStringLiteral("blockdevices")).toArray(), false);
    return out;
}

// « Mounted /dev/sdb1 at /media/vincent/CLE. » → « /media/vincent/CLE »
inline QString parseMountedAt(const QString &out)
{
    const int at = out.indexOf(QLatin1String(" at "));
    if (at < 0)
        return {};
    QString path = out.mid(at + 4).trimmed();
    if (path.endsWith(QLatin1Char('.')))
        path.chop(1);
    return path;
}

} // namespace devices
