# -*- coding: UTF-8 -*-

import sys, os

def gen_pad_file(bin_output_file, pad_num, pad_out_put_file):
    outfile = file(pad_out_put_file, "wb")
    infile  = file(bin_output_file, "rb")

    paddata = bytearray(pad_num)
    data    = infile.read()

    outfile.write(paddata)
    outfile.write(data)

    infile.close()
    outfile.close()

def main():
    bin_output_file  = sys.argv[1]
    pad_num          = int(sys.argv[2])
    pad_out_put_file = sys.argv[3]
    gen_pad_file(bin_output_file, pad_num, pad_out_put_file)

if __name__ == "__main__":
    main()

