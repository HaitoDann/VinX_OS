#pragma once

#include <QString>

// Applications GTK aux couleurs de VinX (ADR 0052), sans le module KDE kde-config-gtk-style :
// settings.ini (GTK 3 et 4), gtk.css (couleur d'accent) et réglages GNOME (gsettings, libadwaita).
namespace gtksync {

// Marque des fichiers écrits par VinX : un gtk.css personnel sans cette marque n'est jamais écrasé.
inline const char *marker() { return "/* Généré par VinX (Paramètres → Apparence) : ne pas modifier. */"; }

inline QString settingsIni(bool dark)
{
    return QStringLiteral("[Settings]\n"
                          "gtk-theme-name=Adwaita\n"
                          "gtk-application-prefer-dark-theme=%1\n"
                          "gtk-font-name=IBM Plex Sans 10\n"
                          "gtk-cursor-theme-name=vinx-cursors\n"
                          "gtk-cursor-theme-size=24\n"
                          "gtk-icon-theme-name=VinX\n"
                          "gtk-decoration-layout=:minimize,maximize,close\n")
        .arg(dark ? QStringLiteral("true") : QStringLiteral("false"));
}

// Couleur d'accent : sélection et boutons d'action (GTK 3 Adwaita et libadwaita).
inline QString css(const QString &fill, const QString &ink)
{
    return QString::fromUtf8(marker()) + QStringLiteral("\n"
        "@define-color accent_color %1;\n"
        "@define-color accent_bg_color %1;\n"
        "@define-color accent_fg_color %2;\n"
        "@define-color theme_selected_bg_color %1;\n"
        "@define-color theme_selected_fg_color %2;\n").arg(fill, ink);
}

// Accent nommé le plus proche, pour libadwaita (org.gnome.desktop.interface accent-color).
inline QString adwaitaAccent(const QString &vinxId)
{
    if (vinxId == QLatin1String("lagon")) return QStringLiteral("blue");
    if (vinxId == QLatin1String("turquoise")) return QStringLiteral("teal");
    if (vinxId == QLatin1String("citron")) return QStringLiteral("yellow");
    if (vinxId == QLatin1String("ambre")) return QStringLiteral("orange");
    if (vinxId == QLatin1String("corail")) return QStringLiteral("red");
    if (vinxId == QLatin1String("framboise")) return QStringLiteral("pink");
    if (vinxId == QLatin1String("graphite")) return QStringLiteral("slate");
    return QStringLiteral("green");
}

} // namespace gtksync
