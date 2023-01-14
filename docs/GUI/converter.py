#!/usr/bin/env python

# vim: set ai et ts=4 sw=4:

from PIL import Image
import sys
import os

if len(sys.argv) < 2:
    print("Usage: {} <image-file>".format(sys.argv[0]))
    sys.exit(1)

fname = sys.argv[1]

img = Image.open(fname)
if img.width > 220 or img.height > 220:
    print("Error: Image too big. It sould be 220x220");
    sys.exit(2)

fname = fname[:fname.rfind('.')]
print("const uint16_t {}[{}][{}] = ".format(str(fname), str(img.height), str(img.width)), end='')
print("{")
for y in range(0, img.height):
    s = "{"
    for x in range(0, img.width):
        (r, g, b) = img.getpixel( (x, y) )
        color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        # for right endiness, so ILI9341_DrawImage would work
        color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8)
        s += "0x{:04X},".format(color565)
    s += "},"
    print(s)

print("};")
