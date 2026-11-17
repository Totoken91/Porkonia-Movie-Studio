# L'affiche des Ranga Boyz : rendu portrait du plan 50 (porkonia3d/po.sh) + halo des néons + titres
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageFilter
im = Image.open('/tmp/po.png').convert('RGB'); W, H = im.size
a = np.asarray(im, np.float32) / 255
hi = Image.fromarray((np.clip(a - 0.62, 0, 1) * 2.6 * 255).clip(0, 255).astype(np.uint8))
gl = np.asarray(hi.filter(ImageFilter.GaussianBlur(14)), np.float32) / 255 * 0.55 + np.asarray(hi.filter(ImageFilter.GaussianBlur(48)), np.float32) / 255 * 0.45
a = 1 - (1 - a) * (1 - np.clip(gl, 0, 1))
yy = np.linspace(0, 1, H)[:, None, None]; a *= 1 - 0.45 * np.clip(1 - yy / 0.30, 0, 1) ** 1.5       # le haut un peu assombri, pour le titre
im = Image.fromarray((a * 255).clip(0, 255).astype(np.uint8)).convert('RGBA')
B = '/usr/share/fonts/truetype/google-fonts/Poppins-Bold.ttf'; M = '/usr/share/fonts/truetype/google-fonts/Poppins-Medium.ttf'
def fit(txt, path, wmax, s0):
    s = s0
    while ImageDraw.Draw(im).textlength(txt, font=ImageFont.truetype(path, s)) > wmax: s -= 2
    return ImageFont.truetype(path, s)
def put(txt, font, y, fill, glow=None, track=0):
    L = Image.new('RGBA', (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L); w = d.textlength(txt, font=font) + track * (len(txt) - 1); x = (W - w) / 2
    for ch in txt: d.text((x, y), ch, font=font, fill=fill); x += d.textlength(ch, font=font) + track
    if glow:
        G = Image.new('RGBA', (W, H), (0, 0, 0, 0)); G.paste(Image.new('RGBA', (W, H), glow + (255,)), mask=L.split()[3]); im.alpha_composite(G.filter(ImageFilter.GaussianBlur(22))); im.alpha_composite(G.filter(ImageFilter.GaussianBlur(7)))
    sh = Image.new('RGBA', (W, H), (0, 0, 0, 0)); sh.paste(Image.new('RGBA', (W, H), (5, 0, 20, 200)), (5, 6), mask=L.split()[3]); im.alpha_composite(sh.filter(ImageFilter.GaussianBlur(3))); im.alpha_composite(L)
put('CANAL PORK PRÉSENTE', ImageFont.truetype(M, 30), 62, (255, 236, 200, 255), track=9)
put('RANGA', fit('RANGA', B, 900, 330), 80, (255, 255, 255, 255), glow=(255, 40, 140))
put('BOYZ', fit('RANGA', B, 900, 330), 330, (255, 255, 255, 255), glow=(40, 210, 255))
put('SOUS LA NEIGE DE DOUZI CITY', ImageFont.truetype(M, 34), 672, (255, 236, 200, 255), track=6)
im.convert('RGB').save('ranga-boyz-affiche.png')
