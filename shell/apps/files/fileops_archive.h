#pragma once

#include <QString>
#include <QStringList>

// Archives reconnues par Fichiers (ADR 0052), extraites avec bsdtar.
namespace archive {

// « photos.tar.gz » → « photos » ; "" si ce n'est pas une archive connue.
inline QString stem(const QString &name)
{
    static const QStringList exts = {QStringLiteral(".tar.gz"), QStringLiteral(".tar.xz"), QStringLiteral(".tar.bz2"),
                                     QStringLiteral(".tar.zst"), QStringLiteral(".tgz"), QStringLiteral(".zip"),
                                     QStringLiteral(".tar"), QStringLiteral(".7z"), QStringLiteral(".rar")};
    for (const QString &e : exts)
        if (name.endsWith(e, Qt::CaseInsensitive) && name.size() > e.size())
            return name.left(name.size() - e.size());
    return {};
}

} // namespace archive
