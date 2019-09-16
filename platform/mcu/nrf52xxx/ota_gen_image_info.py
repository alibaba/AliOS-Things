#! /usr/bin/env python

import os, sys, re, struct, platform, getopt, subprocess
from sys import platform as _platform
import array,hashlib,struct

def print_usage():
    print("")
    print("discription: generate a bin file summary info")
    print(sys.argv[0])
    print("Optional Usage:")
    print(" [-o] <target binary file>")
    print(" [-m] <iamge magic type>")
    print(" [-h | --help] Display usage")
    print(" [<input binary file>]")
    sys.stdout.flush()

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'o:m:h')
    except getopt.GetoptError as err:
        print(str(err))
        print_usage()
        sys.exit(2)

    OUTPUT_FILE = 0
    if not len(args) == 1:
        print_usage()
        sys.exit(2)
    else:
        INPUT_FILE = args[0]
    if not os.path.exists(INPUT_FILE):
        print("Please input a binary file")
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-o":
            OUTPUT_FILE = arg 
        elif opt == "-m":
            magic_str = arg
        elif opt == "-h":
            print_usage()
            sys.exit()

    if OUTPUT_FILE == 0:
        OUTPUT_FILE = re.sub(r'.bin$', '.info.bin', INPUT_FILE)

    fin = open(INPUT_FILE, 'rb')
    fout = open(OUTPUT_FILE, 'wb')
    data = fin.read()
    magic = int(magic_str, 16)
    size = os.path.getsize(INPUT_FILE)
    print(size)
    fout.write(struct.pack('<I', magic))
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
