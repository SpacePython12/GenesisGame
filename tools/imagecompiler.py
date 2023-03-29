from io import BufferedWriter
from PIL import Image
import sys
import argparse
import math

def rgb8torgb3(val: tuple[int, int, int]) -> int:
    r, g, b = val
    ret: int = 0
    ret += round((r/255)*7)
    ret <<= 4
    ret += round((g/255)*7)
    ret <<= 4
    ret += round((b/255)*7)
    ret <<= 1
    return ret

def rgb8torgb5(val: tuple[int, int, int]) -> int:
    r, g, b = val
    ret: int = 0
    ret += round(r/255.0*31.0)
    ret <<= 5
    ret += round(g/255.0*31.0)
    ret <<= 5
    ret += round(b/255.0*31.0)
    ret <<= 1
    #print(f"R: {hex(r)} -> {hex(round(round(r*(31.0/255.0))*(255.0/31.0)))} | G: {hex(g)} -> {hex(round(round(g*(31.0/255.0))*(255.0/31.0)))} | B: {hex(b)} -> {hex(round(round(b*(31.0/255.0))*(255.0/31.0)))}\n")
    return ret

def rgb8torgb5clamp(val: tuple[int, int, int, int]) -> tuple[int, int, int, int]:
    (r, g, b, a) = val
    t = 0
    if a > 0: t = 255
    return (round(round(r*(31.0/255.0))*(255.0/31.0)), round(round(g*(31.0/255.0))*(255.0/31.0)), round(round(b*(31.0/255.0))*(255.0/31.0)), t)

def sortcolors(e):
    occurences, color = e
    r, g, b, a = color
    if a > 0:
        return occurences
    else:
        return 0

def imagetotileset(srcname: str, dstname: str):
    src: Image.Image = Image.open(srcname)
    dst: BufferedWriter = open(dstname, "wb")
    if src.width % 8 != 0: 
        print(f"Image width {src.width} is not divisible by 8!")
        exit(1)
    if src.height % 8 != 0: 
        print(f"Image height {src.height} is not divisible by 8!")
        exit(1)
    colors = src.getcolors()
    colors.sort(key=sortcolors, reverse=True)
    colors.insert(0, colors.pop())
    colors = [color for quant, color in colors][:255]
    for i in range(int(src.height / 8)):
        for j in range(int(src.width / 8)):
            for k in range(8):
                for l in range(8):
                    dst.write(colors.index((src.getpixel(((j*8)+l, (i*8)+k)))).to_bytes(length=1, byteorder="big"))
    dst.close()
    src.close()

def imagetopalette(srcname: str, dstname: str):
    src: Image.Image = Image.open(srcname)
    dst: BufferedWriter = open(dstname, "wb")
    colors = src.getcolors()
    colors.sort(key=sortcolors, reverse=True)
    colors.insert(0, colors.pop())
    for quant, color in colors[:255]:
        r, g, b, a = color
        dst.write(rgb8torgb5((r, g, b)).to_bytes(2, byteorder="little"))
    dst.close()
    src.close()

def clampimage(srcname: str, dstname: str):
    src: Image.Image = Image.open(srcname)
    dst: Image.Image = Image.new(src.mode, (src.width, src.height))
    for i in range(src.height):
        for j in range(src.width):
            dst.putpixel((j, i), rgb8torgb5clamp(src.getpixel((j, i))))
    src.close()
    dst.save(dstname)
    dst.close()

if (__name__ == "__main__"):
    argp = argparse.ArgumentParser(
        description="A tool to compile images to formats the Genesis can recognize.",    
    )
    argp.add_argument("action", choices=["palette", "tileset", "clamp"])
    argp.add_argument("image")
    argp.add_argument("-o", "--output")
    argp.add_argument("-p", "--palette")
    ns = argp.parse_args()
    if ns.action == "tileset":
        imagetotileset(ns.image, ns.output)
    elif ns.action == "palette":
        imagetopalette(ns.image, ns.output)
    elif ns.action == "clamp":
        clampimage(ns.image, ns.output)