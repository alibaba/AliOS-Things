# -*- coding: utf-8 -*-
"""Utility to convert images to MONO_HMSB format."""

from PIL import Image
from os import path
import sys

def error(msg):
    """Display error and exit."""
    print (msg)
    sys.exit(-1)

def set_bit(value, index, high=True):
    """Set the index:th bit of value.

    Args:
        value (int): Number that will have bit modified.
        index (int): Index of bit to set.
        high (bool): True (default) = set bit high, False = set bit low
    """
    mask = 1 << index
    value &= ~mask
    if high:
        value |= mask
    return value

def write_bin(f, pixel_list, width):
    """Save image in MONO_HMSB format."""
    index = 0
    list_bytes = []
    image_byte = 0
    windex = 0
    for pix in pixel_list:
        image_byte = set_bit(image_byte, index, pix > 0)
        index += 1
        windex += 1
        if index > 7 or windex >= width:
            list_bytes.append(image_byte)
            image_byte = 0
            index = 0
            if windex >= width:
                windex = 0
    f.write(bytearray(list_bytes))

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        error('Please specify input file: ./img2monohmsb.py test.png')
    in_path = args[1]
    if not path.exists(in_path):
        error('File Not Found: ' + in_path)

    filename, ext = path.splitext(in_path)
    out_path = filename + '.mono'
    img = Image.open(in_path).convert('1')  # Open and covert to monochrome
    pixels = list(img.getdata())
    with open(out_path, 'wb') as f:
        write_bin(f, pixels, img.width)
    print('Saved: ' + out_path)
