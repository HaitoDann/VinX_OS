#!/usr/bin/env python3
"""Génère le thème d'icônes « VinX » (freedesktop) à partir des pictogrammes de branding/icons.

Style (ADR 0018) : tuile neutre arrondie, pictogramme à l'encre au trait, un point de couleur.
Les applications tierces gardent leur logo officiel (hérité de Breeze / hicolor).

Usage : gen_icons.py DOSSIER_SORTIE
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
GLYPHS = os.path.join(HERE, "..", "icons")

# Applications : tuile pleine colorée (ADR 0046). Emplacements et types de fichiers : tuile neutre (ADR 0018).
TILE = "#E3E1DB"
TILE_LINE = "#CFCCC4"
INK = "#141414"

# nom d'icône → (pictogramme, couleur du point, contexte). Pas d'icône VinX pour les applications
# tierces (Firefox, LibreOffice, Thunderbird…) : leur logo officiel est conservé (thème hérité).
ICONS = {
    # Applications VinX et outils système
    "org.vinx.files": ("folder", "#2F6FD0", "apps"),
    "system-file-manager": ("folder", "#2F6FD0", "apps"),
    "org.vinx.store": ("bag", "#10DD8C", "apps"),
    "org.vinx.settings": ("sliders", "#8A8983", "apps"),
    "systemsettings": ("sliders", "#8A8983", "apps"),
    "preferences-system": ("sliders", "#8A8983", "apps"),
    "org.vinx.welcome": ("info", "#10DD8C", "apps"),
    "org.vinx.monitor": ("chip", "#0B7285", "apps"),
    "org.vinx.images": ("image", "#E8590C", "apps"),
    "org.vinx.pdf": ("doc", "#E5484D", "apps"),
    "org.vinx.calculator": ("calculator", "#E8590C", "apps"),
    "org.vinx.screenshot": ("fullscreen", "#C2255C", "apps"),
    "utilities-terminal": ("terminal", INK, "apps"),
    "org.kde.konsole": ("terminal", INK, "apps"),
    "help-browser": ("info", "#8A8983", "apps"),
    "preferences-desktop-notification": ("bell", "#8A8983", "apps"),
    "org.vinx.updates": ("update", "#10DD8C", "apps"),
    "system-software-update": ("update", "#10DD8C", "apps"),
    # Utilitaires KDE et Debian installés par défaut (ADR 0043) : tuile VinX plutôt que le style Breeze.
    # Les logiciels tiers à marque forte (Firefox, Thunderbird, LibreOffice, VLC) gardent leur logo.
    "ark": ("archive", "#D4A13C", "apps"),
    "org.kde.ark": ("archive", "#D4A13C", "apps"),
    "accessories-calculator": ("calculator", "#E8590C", "apps"),
    "org.kde.kcalc": ("calculator", "#E8590C", "apps"),
    "okular": ("doc", "#E5484D", "apps"),
    "org.kde.okular": ("doc", "#E5484D", "apps"),
    "timeshift": ("history", "#0B7285", "apps"),
    "install-debian": ("install", "#10DD8C", "apps"),
    "calamares": ("install", "#10DD8C", "apps"),
    "system-installer": ("install", "#10DD8C", "apps"),
    "printer": ("printer", "#2F6FD0", "apps"),
    "preferences-devices-printer": ("printer", "#2F6FD0", "apps"),
    "org.kde.kwalletmanager": ("lock", "#8A8983", "apps"),
    "kwalletmanager": ("lock", "#8A8983", "apps"),
    "org.kde.skanlite": ("scanner", "#2F6FD0", "apps"),
    "skanlite": ("scanner", "#2F6FD0", "apps"),
    "org.gnome.SimpleScan": ("scanner", "#2F6FD0", "apps"),
    "simple-scan": ("scanner", "#2F6FD0", "apps"),
    # Emplacements
    "folder": ("folder", None, "places"),
    "inode-directory": ("folder", None, "mimetypes"),
    "user-home": ("home", "#10DD8C", "places"),
    "user-desktop": ("grid", None, "places"),
    "folder-documents": ("doc", "#2F6FD0", "places"),
    "folder-download": ("download", "#10DD8C", "places"),
    "folder-downloads": ("download", "#10DD8C", "places"),
    "folder-pictures": ("image", "#E8590C", "places"),
    "folder-images": ("image", "#E8590C", "places"),
    "folder-music": ("music", "#C2255C", "places"),
    "folder-videos": ("video", "#C2255C", "places"),
    "folder-templates": ("note", None, "places"),
    "folder-publicshare": ("globe", None, "places"),
    "folder-remote": ("globe", "#0B7285", "places"),
    "network-workgroup": ("globe", "#0B7285", "places"),
    "user-trash": ("trash", None, "places"),
    "user-trash-full": ("trash", "#E5484D", "places"),
    "drive-harddisk": ("drive", None, "devices"),
    "drive-removable-media": ("drive", "#2F6FD0", "devices"),
    "drive-removable-media-usb": ("drive", "#2F6FD0", "devices"),
    "media-removable": ("drive", "#2F6FD0", "devices"),
    # Types de fichiers
    "text-x-generic": ("note", None, "mimetypes"),
    "text-plain": ("note", None, "mimetypes"),
    "application-pdf": ("doc", "#E5484D", "mimetypes"),
    "image-x-generic": ("image", "#E8590C", "mimetypes"),
    "audio-x-generic": ("music", "#C2255C", "mimetypes"),
    "video-x-generic": ("video", "#C2255C", "mimetypes"),
    "application-x-executable": ("terminal", INK, "mimetypes"),
    "text-x-script": ("terminal", INK, "mimetypes"),
    "application-x-shellscript": ("terminal", INK, "mimetypes"),
    "package-x-generic": ("bag", "#D4A13C", "mimetypes"),
    "application-zip": ("bag", "#D4A13C", "mimetypes"),
    "application-x-compressed-tar": ("bag", "#D4A13C", "mimetypes"),
    "application-vnd.oasis.opendocument.text": ("doc", "#2F6FD0", "mimetypes"),
    "application-vnd.openxmlformats-officedocument.wordprocessingml.document": ("doc", "#2F6FD0", "mimetypes"),
    "x-office-document": ("doc", "#2F6FD0", "mimetypes"),
    "application-vnd.oasis.opendocument.spreadsheet": ("table", "#10DD8C", "mimetypes"),
    "application-vnd.openxmlformats-officedocument.spreadsheetml.sheet": ("table", "#10DD8C", "mimetypes"),
    "x-office-spreadsheet": ("table", "#10DD8C", "mimetypes"),
    "application-vnd.oasis.opendocument.presentation": ("image", "#E8590C", "mimetypes"),
    "x-office-presentation": ("image", "#E8590C", "mimetypes"),
    "text-html": ("globe", "#E8590C", "mimetypes"),
    "application-x-generic": ("doc", None, "mimetypes"),
    "unknown": ("doc", None, "mimetypes"),
}


def glyph_body(name):
    """Contenu (éléments) d'un pictogramme 24 × 24 au trait."""
    with open(os.path.join(GLYPHS, name + ".svg"), encoding="utf-8") as f:
        svg = f.read()
    return re.sub(r"^.*?<svg[^>]*>|</svg>\s*$", "", svg, flags=re.S)


ART = os.path.join(HERE, "..", "appicons")
# Dessins complets (ADR 0046) : Fichiers, Paramètres, Store.
ART_FOR = {"org.vinx.files": "files", "system-file-manager": "files", "org.vinx.store": "store",
           "org.vinx.settings": "settings", "systemsettings": "settings", "preferences-system": "settings"}


def _mix(hexc, f):
    """Éclaircit une couleur (f > 1) vers le blanc."""
    r, g, b = (int(hexc[i:i + 2], 16) for i in (1, 3, 5))
    t = min(1.0, f - 1.0)
    return "#%02X%02X%02X" % tuple(round(c + (255 - c) * t) for c in (r, g, b))


def app_svg(glyph, tint):
    """Tuile pleine en dégradé, pictogramme blanc (encre sur les couleurs claires), sans bordure."""
    r, g, b = (int(tint[i:i + 2], 16) / 255 for i in (1, 3, 5))
    light = 0.299 * r + 0.587 * g + 0.114 * b > 0.62
    ink = "#06130D" if light else ("#10DD8C" if max(r, g, b) < 0.12 else "#FFFFFF")
    top = _mix(tint, 1.12 if light else 1.35)
    return ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64"><defs><linearGradient id="g" x1="0" y1="0" '
            f'x2=".35" y2="1"><stop offset="0" stop-color="{top}"/><stop offset="1" stop-color="{tint}"/>'
            '</linearGradient></defs><rect x="4" y="4" width="56" height="56" rx="14" fill="url(#g)"/>'
            f'<g transform="translate(15.5 15.5) scale(1.375)" fill="none" stroke="{ink}" stroke-width="1.8" '
            f'stroke-linecap="round" stroke-linejoin="round" color="{ink}">{glyph_body(glyph)}</g></svg>')


def icon_svg(glyph, dot):
    parts = [
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">',
        f'<rect x="4.5" y="4.5" width="55" height="55" rx="13.5" fill="{TILE}" stroke="{TILE_LINE}"/>',
        f'<g transform="translate(17 17) scale(1.25)" fill="none" stroke="{INK}" stroke-width="1.7" '
        f'stroke-linecap="round" stroke-linejoin="round" color="{INK}">{glyph_body(glyph)}</g>',
    ]
    if dot:
        parts.append(f'<circle cx="49" cy="15" r="4.6" fill="{dot}"/>')
    parts.append("</svg>")
    return "".join(parts)


INDEX = """[Icon Theme]
Name=VinX
Comment=Icônes de VinX OS : tuile neutre, pictogramme au trait, point de couleur
Inherits=breeze,hicolor
Directories=scalable/apps,scalable/places,scalable/mimetypes,scalable/devices

[scalable/apps]
Size=64
MinSize=16
MaxSize=512
Type=Scalable
Context=Applications

[scalable/places]
Size=64
MinSize=16
MaxSize=512
Type=Scalable
Context=Places

[scalable/mimetypes]
Size=64
MinSize=16
MaxSize=512
Type=Scalable
Context=MimeTypes

[scalable/devices]
Size=64
MinSize=16
MaxSize=512
Type=Scalable
Context=Devices
"""


def main(out):
    for ctx in ("apps", "places", "mimetypes", "devices"):
        os.makedirs(os.path.join(out, "scalable", ctx), exist_ok=True)
    with open(os.path.join(out, "index.theme"), "w", encoding="utf-8") as f:
        f.write(INDEX)
    for name, (glyph, dot, ctx) in ICONS.items():
        if name in ART_FOR:
            with open(os.path.join(ART, ART_FOR[name] + ".svg"), encoding="utf-8") as f:
                svg = f.read()
        elif ctx == "apps" and dot:
            svg = app_svg(glyph, dot)
        else:
            svg = icon_svg(glyph, dot)
        with open(os.path.join(out, "scalable", ctx, name + ".svg"), "w", encoding="utf-8") as f:
            f.write(svg)
    print(f"{len(ICONS)} icônes générées dans {out}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "VinX")
