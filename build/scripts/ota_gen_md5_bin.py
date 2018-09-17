#! /usr/bin/env python

import os, sys, re, struct, platform, getopt, subprocess
from sys import platform as _platform
import array,hashlib,struct


def crc16A(dat):
    crc=0xFFFF #65535
    index=0
    for j in dat:
        crc=crc^j
        print("crc=%x"%crc)
        for i in range(8):
            if (crc & 1)==1:
                crc =crc >> 1
                crc =crc ^ 0xA001 #40961
            else:
                crc=crc>>1
    return crc

def print_usage():
    print ""
    print "Usage: Merge a bin file into an exist bin file, create one if target is not exist"
    print sys.argv[0]
    print "Optional Usage:"
    print " [-o] <target binary file>"
    print " [-h | --help] Display usage"
    print " [<input binary file>]"
    sys.stdout.flush()

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'o:h')
    except getopt.GetoptError as err:
        print str(err)
        print_usage()
        sys.exit(2)

    OUTPUT_FILE = 0
    if not len(args) == 1:
        print_usage()
        sys.exit(2)
    else:
        INPUT_FILE = args[0]

    if not os.path.exists(INPUT_FILE):
        print "Please input a binary file"
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-o":
            OUTPUT_FILE = arg
        elif opt == "-h":
            print_usage()
            sys.exit()

    if OUTPUT_FILE == 0:
        OUTPUT_FILE = re.sub(r'.bin$', '.md5.bin', INPUT_FILE)

    fin = open(INPUT_FILE, 'rb')
    fout = open(OUTPUT_FILE, 'wb')
    data = fin.read()
    fout.write(data)
    magic = bytearray([0xef,0xef,0xef,0xef])
    size = os.path.getsize(INPUT_FILE)
    print size
    fout.write(magic)
    fout.write(struct.pack('<I', size))
    fout.write(hashlib.md5(data).digest())
    print(hashlib.md5(data).hexdigest())
    reserve = bytearray([0xFF,0xFF])
    fout.write(reserve)
    fout.write(reserve)
    fin.close()
    fout.close()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
