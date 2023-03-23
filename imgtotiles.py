from io import BufferedWriter
from PIL import Image
import sys

src: Image.Image = Image.open(sys.argv[1])
dst: BufferedWriter = open(sys.argv[2], "wb")
if src.width % 8 != 0: 
    print(f"Image width {src.width} is not divisible by 8!")
    exit(1)
if src.height % 8 != 0: 
    print(f"Image height {src.height} is not divisible by 8!")
    exit(1)
tmp: int = 0
for i in range(src.width / 8):
    for  in range():
            