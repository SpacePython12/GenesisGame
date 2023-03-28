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
    ret += round((r/255)*7)
    ret <<= 4
    ret += round((r/255)*7)
    ret <<= 1
    return ret

def imagetotileset(srcname: str, dstname: str):
    img: Image.Image = Image.open(srcname)
    src = img.quantize(16, dither=Image.FLOYDSTEINBERG)
    dst: BufferedWriter = open(dstname, "wb")
    if src.width % 8 != 0: 
        print(f"Image width {src.width} is not divisible by 8!")
        exit(1)
    if src.height % 8 != 0: 
        print(f"Image height {src.height} is not divisible by 8!")
        exit(1)
    tmp: int = 0
    for i in range(int(src.height / 8)):
        for j in range(int(src.width / 8)):
            for k in range(8):
                for l in range(8):
                    tmp += (src.getpixel(((j*8)+l, (i*8)+k))) % 16
                    tmp <<= 4
    dst.write(tmp.to_bytes(length=int(math.ceil(tmp.bit_length()/8)), byteorder="big"))
    dst.close()
    img.close()

def imagetopalette(srcname: str, dstname: str):
    img: Image.Image = Image.open(srcname)
    src = img.quantize(16, dither=Image.FLOYDSTEINBERG)
    dst: BufferedWriter = open(dstname, "wb")
    for color in [(src.getpalette()[(i*3)], src.getpalette()[(i*3)+1], src.getpalette()[(i*3)+2]) for i in range(int(len(src.getpalette())/3))][:15]:
        dst.write(rgb8torgb3(color).to_bytes(2, "big"))
    dst.close()
    src.close()

if (__name__ == "__main__"):
    argp = argparse.ArgumentParser(
        description="A tool to compile images to formats the Genesis can recognize.",    
    )
    argp.add_argument("action", choices=["palette", "tileset"])
    argp.add_argument("image")
    argp.add_argument("-o", "--output")
    argp.add_argument("-p", "--palette")
    ns = argp.parse_args()
    if ns.action == "tileset":
        imagetotileset(ns.image, ns.output)
    elif ns.action == "palette":
        imagetopalette(ns.image, ns.output)