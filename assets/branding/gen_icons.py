#!/usr/bin/env python3
"""Generates the gpModeler icons/logos (original artwork) in place.

Usage: python3 assets/branding/gen_icons.py   (run from the repository root; requires Pillow)
Palette: Greenplum-like green -> Cloudberry-like blue.
"""
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageChops

GREEN_TOP = (122, 200, 64)
GREEN = (60, 168, 84)
BLUE = (24, 118, 210)
BLUE_DEEP = (16, 78, 158)
BOLD = "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
S = 1024  # working canvas size, downscaled at the end


def gradient(size, top, bottom):
    w, h = size
    img = Image.new("RGBA", size)
    px = img.load()
    for y in range(h):
        t = y / (h - 1)
        c = tuple(int(top[i] + (bottom[i] - top[i]) * t) for i in range(3)) + (255,)
        for x in range(w):
            px[x, y] = c
    return img


def cloud_mask(size=S):
    m = Image.new("L", (size, size), 0)
    d = ImageDraw.Draw(m)
    # three bumps + flat base, a stylised cloud
    d.ellipse((120, 400, 480, 760), fill=255)
    d.ellipse((290, 210, 740, 660), fill=255)
    d.ellipse((560, 330, 920, 690), fill=255)
    d.rounded_rectangle((190, 470, 850, 790), radius=160, fill=255)
    return m


def logo(badge=None):
    mask = cloud_mask()
    # soft shadow
    shadow = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    sh_mask = mask.filter(ImageFilter.GaussianBlur(22))
    shadow.paste((0, 40, 60, 110), (0, 14), sh_mask)

    body = gradient((S, S), GREEN_TOP, BLUE)
    # diagonal tint to blend green into blue (GP -> Cloudberry)
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    img = Image.alpha_composite(img, shadow)
    layer = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    layer.paste(body, (0, 0), mask)

    # rim: darker outline from the mask edge
    edge = ImageChops.subtract(mask, mask.filter(ImageFilter.MinFilter(25)))
    rim = Image.new("RGBA", (S, S), BLUE_DEEP + (255,))
    layer.paste(rim, (0, 0), edge.point(lambda v: int(v * 0.55)))
    img = Image.alpha_composite(img, layer)

    d = ImageDraw.Draw(img)
    f = ImageFont.truetype(BOLD, 430)
    text = "gp"
    bb = d.textbbox((0, 0), text, font=f)
    tw, th = bb[2] - bb[0], bb[3] - bb[1]
    tx, ty = (S - tw) // 2 - bb[0] + 10, 400 - bb[1] + 20
    d.text((tx + 6, ty + 8), text, font=f, fill=(10, 60, 110, 120))
    d.text((tx, ty), text, font=f, fill=(255, 255, 255, 255))

    if badge:
        add_badge(img, badge)
    return img


def add_badge(img, kind):
    d = ImageDraw.Draw(img)
    cx, cy, r = 800, 800, 170
    colors = {"ch": (214, 59, 59), "cli": (40, 44, 52), "se": (232, 150, 20)}
    glyph = {"ch": "!", "cli": ">_", "se": "{}"}
    d.ellipse((cx - r - 12, cy - r - 12, cx + r + 12, cy + r + 12), fill=(255, 255, 255, 255))
    d.ellipse((cx - r, cy - r, cx + r, cy + r), fill=colors[kind] + (255,))
    f = ImageFont.truetype(BOLD, 190 if kind != "cli" else 150)
    bb = d.textbbox((0, 0), glyph[kind], font=f)
    d.text((cx - (bb[0] + bb[2]) / 2, cy - (bb[1] + bb[3]) / 2), glyph[kind], font=f, fill=(255, 255, 255, 255))


def doc_icon(label):
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    x0, y0, x1, y1, fold = 170, 90, 854, 934, 190
    d.polygon([(x0, y0 + 40), (x0 + 40, y0), (x1 - fold, y0), (x1, y0 + fold), (x1, y1 - 40), (x1 - 40, y1), (x0 + 40, y1), (x0, y1 - 40)], fill=(244, 247, 250, 255), outline=(150, 165, 180, 255))
    d.polygon([(x1 - fold, y0), (x1, y0 + fold), (x1 - fold, y0 + fold)], fill=(208, 218, 228, 255))
    band = gradient((x1 - x0, 230), GREEN, BLUE)
    img.paste(band, (x0, 640), band.split()[3])
    small = logo().resize((420, 420), Image.LANCZOS)
    img.alpha_composite(small, (302, 170))
    f = ImageFont.truetype(BOLD, 170)
    bb = d.textbbox((0, 0), label, font=f)
    d.text(((S - (bb[0] + bb[2])) / 2, 760 - bb[1] + 10), label, font=f, fill=(255, 255, 255, 255))
    return img


def wordmark(width=558, height=119):
    img = Image.new("RGBA", (width * 4, height * 4), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    f = ImageFont.truetype(BOLD, int(height * 4 * 0.78))
    d.text((0, 0), "gpModeler", font=f, fill=(236, 240, 244, 255))
    bb = d.textbbox((0, 0), "gpModeler", font=f)
    img = img.crop((bb[0], bb[1] - 10, bb[2], bb[3] + 10))
    img.thumbnail((width * 4, height * 4), Image.LANCZOS)
    out = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    img = img.resize((int(img.width / 4), int(img.height / 4)), Image.LANCZOS)
    out.alpha_composite(img, ((width - img.width) // 2, (height - img.height) // 2))
    return out


def splash(size=599):
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    lg = logo().resize((int(size * 0.8), int(size * 0.8)), Image.LANCZOS)
    img.alpha_composite(lg, ((size - lg.width) // 2, -int(size * 0.04)))
    d = ImageDraw.Draw(img)
    f = ImageFont.truetype(BOLD, 96)
    bb = d.textbbox((0, 0), "gpModeler", font=f)
    d.text(((size - (bb[2] - bb[0])) / 2 - bb[0], 400), "gpModeler", font=f, fill=(236, 240, 244, 255), stroke_width=3, stroke_fill=(20, 60, 110, 255))
    f2 = ImageFont.truetype(BOLD, 26)
    sub = "for Greenplum & Apache Cloudberry"
    bb = d.textbbox((0, 0), sub, font=f2)
    d.text(((size - (bb[2] - bb[0])) / 2 - bb[0], 520), sub, font=f2, fill=(236, 240, 244, 255), stroke_width=2, stroke_fill=(20, 60, 110, 255))
    return img


def save_png(img, path, size):
    img.resize((size, size), Image.LANCZOS).save(path)


def save_ico(img, path):
    img.resize((256, 256), Image.LANCZOS).save(path, sizes=[(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)])


def save_icns(img, path):
    img.resize((512, 512), Image.LANCZOS).save(path, format="ICNS")


if __name__ == "__main__":
    app, dbm, sch = logo(), doc_icon("DBM"), doc_icon("SCH")

    save_png(app, "assets/conf/pgmodeler_logo.png", 256)
    save_png(dbm, "assets/conf/pgmodeler_dbm.png", 256)
    save_png(sch, "assets/conf/pgmodeler_sch.png", 256)
    save_png(app, "libs/libgui/res/icons/pgmodeler_logo.png", 128)
    wordmark().save("libs/libgui/res/images/pgmodeler_name.png")
    splash().save("libs/libgui/res/images/pgmodeler_splash.png")

    save_ico(app, "apps/pgmodeler/res/pgmodeler.ico")
    save_ico(dbm, "apps/pgmodeler/res/pgmodeler_dbm.ico")
    save_ico(logo("ch"), "apps/pgmodeler-ch/res/pgmodeler_ch.ico")
    save_ico(logo("cli"), "apps/pgmodeler-cli/res/pgmodeler_cli.ico")
    save_ico(logo("se"), "apps/pgmodeler-se/res/pgmodeler_se.ico")
    save_icns(app, "apps/pgmodeler/res/Resources/pgmodeler.icns")
    save_icns(dbm, "apps/pgmodeler/res/Resources/pgmodeler_dbm.icns")
    print("done")
