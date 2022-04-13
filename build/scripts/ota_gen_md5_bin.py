#! /usr/bin/env python3

import os, sys, re, struct, platform, getopt, subprocess
from sys import platform as _platform
import array,hashlib,struct

def update_crc16(crcin, byte):
    crc = crcin
    calculen = byte | 0x100
    crc <<= 1
    calculen <<= 1
    if calculen & 0x100 > 0:
        crc += 1
    if crc & 0x10000 > 0:
        crc ^= 0x1021
    while((calculen & 0x10000) == 0):
        crc <<= 1
        calculen <<= 1
        if calculen & 0x100 > 0:
            crc += 1
        if crc & 0x10000 > 0:
            crc ^= 0x1021

    return crc & 0xFFFF

def crc16_calculate(data, len):
    crc = 0
    for i in range(0, len):
        crc = update_crc16(crc, data[i])
    crc = update_crc16(crc, 0)
    crc = update_crc16(crc, 0)
    return crc & 0xFFFF

def hashcalculate(type, indata):
    if type == "md5":
        hashmethod = hashlib.md5()
    elif type == "sha256":
        hashmethod = hashlib.sha256()
    else:
        print("don't support other hash type")
        return 0
    hashmethod.update(indata)
    value = hashmethod.digest()
    return value

def get_image_magic(image_magic):
    if image_magic == '0xabababab':
        tmp_magic = 0xabababab
    elif image_magic == '0xcdcdcdcd':
        tmp_magic = 0xcdcdcdcd
    elif image_magic == '0xabcdabcd':
        tmp_magic = 0xabcdabcd
    elif image_magic == '0xefefefef':
        tmp_magic = 0xefefefef
    elif image_magic == '0xefcdefcd':
        tmp_magic = 0xefcdefcd
    else:
        tmp_magic = 0xefefefef
    return tmp_magic

def print_usage():
    print("")
    print("Usage: Merge a bin file into an exist bin file, create one if target is not exist")
    print(sys.argv[0])
    print("Optional Usage:")
    print(" [-i] <input binary file>")
    print(" [-o] <target binary file>")
    print(" [-m] <image magic type>")
    print(" [-h | --help] Display usage")
    print(" [<input binary file>]")
    sys.stdout.flush()

def main():
    try:
        opts,args = getopt.getopt(sys.argv[1:], "ho:m:i:")
    except getopt.GetoptError as err:
        print(str(err))
        print_usage()
        sys.exit(2)
    magic_str = ''
    if not len(opts) == 2:
        print_usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ["-o"]:
           OUTPUT_FILE = arg
        elif opt in ["-m"]:
            magic_str = arg
            #print('magic = ' + arg)
        elif opt in ["-i"]:
            INPUT_FILE = arg
            #print("input file = " + INPUT_FILE)
        elif opt in ["-h"]:
            print_usage()
            sys.exit()
    if not os.path.exists(INPUT_FILE):
        print("Please input a binary file")
        sys.exit(2)
    try:
        with open(INPUT_FILE, 'rb') as hfin:
            imagedata = hfin.read()
    except FileNotFoundError:
        print(INPUT_FILE + " is not exist!")
        sys.exit(2)
    except:
        print("read " + INPUT_FILE + " failed!")
        sys.exit(2)
    image_info_magic = get_image_magic(magic_str)
    image_valid_len = len(imagedata)
    image_md5 = hashcalculate("md5", imagedata)
    image_num = 0x01
    image_res = 0x00
    image_crc16 = crc16_calculate(bytearray(imagedata), image_valid_len)

    image_alignment = 0xffffffff
    newimagedata = imagedata[0:image_valid_len]
    newimagedata += struct.pack('<I', image_alignment)
    newimagedata += struct.pack('<I', image_info_magic)
    newimagedata += struct.pack('<I', image_valid_len)
    newimagedata += image_md5
    newimagedata += struct.pack('B', image_num)
    newimagedata += struct.pack('B', image_res)
    newimagedata += struct.pack('<H', image_crc16)

    OUTPUT_FILE = INPUT_FILE+".md5"
    try:
        with open(OUTPUT_FILE, 'wb') as imagefout:
            imagefout.write(newimagedata)
    except FileNotFoundError:
        print("output file path error!")
        sys.exit(2)
    except:
        print("output write error!")
        sys.exit(2)

    os.remove(INPUT_FILE)
    os.rename(OUTPUT_FILE,INPUT_FILE)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
