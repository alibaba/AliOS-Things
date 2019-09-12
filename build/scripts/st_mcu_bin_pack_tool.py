#! /usr/bin/env python

# this script is used for combining the bootloader .bin file and application .bin file for developerkit board
# the usage please saw below help()

import os, sys, re, struct, platform, getopt, subprocess, shutil
from sys import platform as _platform
import array,hashlib,struct

def help():
    print("Usage:")
    print("<bank_size> <app_offset> <boot_bin> <app_bin> [none_app2].")
    print("[none_app2] is optional, set 1 to generate bin without app in bank2.")

def pack_bin_ready(boot_bin, app_bin):
    npos = app_bin.index(".bin")
    tmp_name = app_bin[0:npos]
    tmp_name = tmp_name + "_all.bin"
    if os.path.exists(tmp_name) == True:
        try:
            os.remove(tmp_name)
        except:
            print("deleted file error!")
            return "" 
    try:
        shutil.copyfile(boot_bin, tmp_name)
    except:
        print("copy file failed!")
        return "" 
    return tmp_name

def fill_file_blank_addr(target_bin, want_size):
    if target_bin == "" or want_size <=0:
        print("fill file blank addr parameters error")
        return False
    target_bin_size = os.path.getsize(target_bin)
    if target_bin_size > want_size:
        print("target bin size > want size")
        return False
    write_len = want_size - target_bin_size
    tmp_buf = [0xff]*write_len
    with open(target_bin, 'ab+') as f:
        for data in tmp_buf:
            dat = struct.pack('B',data)
            f.write(dat)
    #print "write_len = %#d"%write_len
    return True

def combine_files(target_bin, src_bin):
    if target_bin == "" or src_bin == "":
        return False
    with open(src_bin, 'rb') as sf:
        all_the_data = sf.read()
        with open(target_bin, 'ab+') as tf:
            tf.write(all_the_data)

def main():
    bank_size = 0x00
    app_offset = 0x00
    none_app2 = False
    output_file = ""
    if (len(sys.argv) != 5 and len(sys.argv) != 6):
        help()
        return
    else:
        if (sys.argv[1].find('0x') == 0) or (sys.argv[1].find('0X') == 0):
            bank_size = int(sys.argv[1], 16)
            #print "bank_size = %#x"%bank_size
        else:
            bank_size = int(sys.argv[1],10)
            #print "bank_size = %#d"%bank_size

        if (sys.argv[2].find('0x') == 0) or (sys.argv[2].find('0X') == 0):
            app_offset = int(sys.argv[2], 16)
            #print "app_offset = %#x"%app_offset
        else:
            app_offset = int(sys.argv[2],10)
            #print "app_offset = %#d"%app_offset

        if len(sys.argv) == 6 and sys.argv[5] == "1":
            print("none app2 mode")
            none_app2 = True
        else:
            print("dual boot and app mode.")

        output_file = pack_bin_ready(sys.argv[3], sys.argv[4])
        if output_file == "":
            return

        if fill_file_blank_addr(output_file, app_offset) == False:
            print("fill file1 blank addr failed!")
            return

        if combine_files(output_file, sys.argv[4]) == False:
            print("combine file1 failed!")
            return
        if fill_file_blank_addr(output_file, bank_size) == False:
            print("fill file2 blank addr failed!")
            return
        if combine_files(output_file, sys.argv[3]) == False:
            print("combine file2 failed!")
            return
        if none_app2 == False:
            if fill_file_blank_addr(output_file, bank_size + app_offset) == False:
                print("fill file3 blank addr failed!")
                return
            if combine_files(output_file, sys.argv[4]) == False:
                print("combine file3 failed!")
                return
        print("pack completed!")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
