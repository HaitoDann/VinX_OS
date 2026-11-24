#!/usr/bin/env python3
"""Thème de curseurs « VinX » (ADR 0047).

Dessins propres à VinX : flèche sans queue aux coins arrondis (encre, liseré blanc), main remplacée
par la même flèche en vert du logo, curseur texte arrondi, attente = grille 2 × 2 qui s'allume dans le
sens des aiguilles d'une montre (comme la grille de points du fond d'écran). Les autres formes
(redimensionnement, interdit…) sont héritées de Breeze.

Usage : gen_cursors.py SORTIE   (demande ImageMagick « convert » pour rendre les SVG)
Le thème généré est versionné dans branding/cursors/VinX : la construction du paquet n'en a pas besoin.
"""
import os
import struct
import subprocess
import sys
import tempfile

INK, WHITE, GREEN, GREEN_INK = "#1B1C1F", "#FFFFFF", "#10DD8C", "#06130D"
SIZES = (24, 32, 48, 64)            # tailles nominales ; dessins sur une grille de 32
ARROW = "M5 3.2 C5 2.4 5.9 2 6.5 2.5 L22.6 15.6 C23.2 16.1 22.9 17.1 22.1 17.1 L14.4 17.6 C14 17.6 13.6 17.8 13.4 18.1 L7.6 25.3 C7.1 25.9 6.1 25.6 6.1 24.8 Z"
ARROW_HOT = (5, 2.6)


def arrow(fill, line):
    return (f'<path d="{ARROW}" fill="none" stroke="{WHITE}" stroke-width="3.2" stroke-linejoin="round"/>'
            f'<path d="{ARROW}" fill="{fill}" stroke="{line}" stroke-width="1" stroke-linejoin="round"/>')


def grid(cx, cy, cell, phase, frames=8):
    """Grille 2 × 2 ; la case allumée tourne (haut-gauche → haut-droite → bas-droite → bas-gauche)."""
    order = [(0, 0), (1, 0), (1, 1), (0, 1)]
    out = []
    gap = cell * 0.28
    x0, y0 = cx - cell - gap / 2, cy - cell - gap / 2
    pos = phase / frames * 4
    for i, (gx, gy) in enumerate(order):
        d = (pos - i) % 4                  # 0 = allumée, puis s'éteint
        alpha = max(0.22, 1 - d * 0.42)
        x, y = x0 + gx * (cell + gap), y0 + gy * (cell + gap)
        out.append(f'<rect x="{x:.2f}" y="{y:.2f}" width="{cell}" height="{cell}" rx="{cell * 0.3:.2f}" '
                   f'fill="{GREEN}" fill-opacity="{alpha:.2f}" stroke="{WHITE}" stroke-width="1.1" stroke-opacity="{min(1, alpha + 0.3):.2f}"/>')
    return "".join(out)


def svg(body):
    return f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 32 32">{body}</svg>'


IBEAM = ("M12 5.5 C13.6 5.5 15 6.2 16 7.2 C17 6.2 18.4 5.5 20 5.5 M16 7.2 V24.8 M12 26.5 C13.6 26.5 15 25.8 16 24.8 "
         "C17 25.8 18.4 26.5 20 26.5 M13.5 16 H18.5")
SHAPES = {
    # nom : (fonction(phase) → svg, point chaud sur 32, nombre d'images)
    "default": (lambda p: svg(arrow(INK, INK)), ARROW_HOT, 1),
    "pointer": (lambda p: svg(arrow(GREEN, GREEN_INK)), ARROW_HOT, 1),
    "text": (lambda p: svg(f'<path d="{IBEAM}" fill="none" stroke="{WHITE}" stroke-width="4" stroke-linecap="round"/>'
                           f'<path d="{IBEAM}" fill="none" stroke="{INK}" stroke-width="1.8" stroke-linecap="round"/>'),
             (16, 16), 1),
    "wait": (lambda p: svg(grid(16, 16, 6.4, p)), (16, 16), 8),
    "progress": (lambda p: svg(arrow(INK, INK) + grid(24, 24, 5, p)), ARROW_HOT, 8),
}
ALIASES = {
    "default": ["left_ptr", "arrow", "top_left_arrow", "left_arrow"],
    "pointer": ["hand1", "hand2", "pointing_hand", "e29285e634086352946a0e7090d73106", "9d800788f1b08800ae810202380a0822"],
    "text": ["xterm", "ibeam"],
    "wait": ["watch"],
    "progress": ["left_ptr_watch", "half-busy", "3ecb610c1bf2410f44200f48c40d3599", "08e8e1c95fe2fc01f976f1e063a24ccd"],
}
DELAY = 110  # ms par image


def render(svg_text, size, tmp):
    src, png = os.path.join(tmp, "c.svg"), os.path.join(tmp, "c.rgba")
    with open(src, "w") as f:
        f.write(svg_text)
    subprocess.run(["convert", "-background", "none", "-density", str(72 * size / 32 * 4), src,
                    "-resize", f"{size}x{size}", "-depth", "8", f"RGBA:{png}"], check=True)
    with open(png, "rb") as f:
        data = f.read()
    pixels = []
    for i in range(0, len(data), 4):
        r, g, b, a = data[i:i + 4]
        pixels.append((a << 24) | ((r * a // 255) << 16) | ((g * a // 255) << 8) | (b * a // 255))
    return pixels


def xcursor(images):
    """images : liste de (taille, xhot, yhot, délai, pixels). Format Xcursor (libXcursor)."""
    ntoc = len(images)
    header = struct.pack("<4sIII", b"Xcur", 16, 0x10000, ntoc)
    pos = 16 + ntoc * 12
    toc, chunks = b"", b""
    for size, xh, yh, delay, px in images:
        toc += struct.pack("<III", 0xFFFD0002, size, pos)
        chunk = struct.pack("<IIIIIIIII", 36, 0xFFFD0002, size, 1, size, size, xh, yh, delay)
        chunk += struct.pack(f"<{len(px)}I", *px)
        chunks += chunk
        pos += len(chunk)
    return header + toc + chunks


def main(out):
    cdir = os.path.join(out, "cursors")
    os.makedirs(os.path.join(os.path.dirname(os.path.abspath(out)), "preview"), exist_ok=True)
    os.makedirs(cdir, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        for name, (draw, (hx, hy), frames) in SHAPES.items():
            images = []
            for size in SIZES:
                for p in range(frames):
                    images.append((size, round(hx * size / 32), round(hy * size / 32),
                                   DELAY if frames > 1 else 0, render(draw(p), size, tmp)))
            with open(os.path.join(cdir, name), "wb") as f:
                f.write(xcursor(images))
            for alias in ALIASES.get(name, []):
                link = os.path.join(cdir, alias)
                if os.path.lexists(link):
                    os.remove(link)
                os.symlink(name, link)
            # Aperçu pour la documentation et la revue.
            with open(os.path.join(tmp, "c.svg"), "w") as f:
                f.write(draw(0))
            subprocess.run(["convert", "-background", "none", "-density", "288", os.path.join(tmp, "c.svg"),
                            os.path.join(os.path.dirname(os.path.abspath(out)), "preview", f"{name}.png")], check=True)
    with open(os.path.join(out, "index.theme"), "w") as f:
        f.write("[Icon Theme]\nName=VinX\nComment=Curseurs de VinX OS\nInherits=breeze_cursors\n")
    print(f"Thème de curseurs généré dans {out}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "VinX")
