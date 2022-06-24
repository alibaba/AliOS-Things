from os import path
import sys
import binascii

if __name__ == '__main__':

    args = sys.argv
    if len(args) != 2:
        error('Please specify input file: ./img2monohmsb.py test.png')
    in_path = args[1]
    if not path.exists(in_path):
        error('File Not Found: ' + in_path)

    array_size = 85*85
    with open(in_path, "rb") as f:
        buf = bytearray(f.read(array_size))

        zztest = binascii.hexlify(buf)
        print(zztest)
