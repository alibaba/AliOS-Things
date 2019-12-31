#! /usr/bin/env python

import os, sys, re, struct, getopt
from sys import platform
import array,hashlib,struct

def print_usage():
	print("")
	print("Usage:" + sys.argv[0])
	print(sys.argv[0] + " main_bin mcu_bin offset magic")
	print(" [-h | --help] Display usage")
	sys.stdout.flush()

def write_file(file_name, gap_szie, data):
        if file_name is None:
                print 'file_name cannot be none\n'
                sys.exit(0)
        fp = open(file_name,'ab')
        if fp:
		fp.write(data)
                if gap_szie > 0:
                        fp.write('\xFF'*gap_szie)
                fp.close()
        else:
                print '%s write fail\n'%(file_name)

def comp_file_md5(input_file, magic_str):
	magic = int(magic_str, 16)
	output_file = input_file+".ota"
	fin = open(input_file, 'rb')
	data = fin.read()
	fout = open(output_file, 'wb')
	fout.write(data)
	size = os.path.getsize(input_file)
	fout.write(struct.pack('<I', magic))
	fout.write(struct.pack('<I', size))
	fout.write(hashlib.md5(data).digest())
	print("magic:0x%x size:%d md5:%s" %(magic, size, hashlib.md5(data).hexdigest()))
	reserve = bytearray([0xFF,0xFF])
	fout.write(reserve)
	fout.write(reserve)
	fin.close()
	fout.close()
	os.remove(input_file)
	os.rename(output_file, input_file)

def main():
	try:
        	opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
    	except getopt.GetoptError as err:
		print(str(err))
		print_usage()
		sys.exit(2)
    	magic = "0xefcdefcd"
	if len(args) < 3:
		print_usage()
		sys.exit(2)
	else:
		main_file = args[0]
		mcu_file = args[1]
		offset = int(args[2], 16)

	if len(args) > 3:
		magic =  args[3]

	if not os.path.exists(main_file):
		print("Input a main file.")
		sys.exit(2)

	output_file = main_file+"_ota.bin"
	if os.path.exists(output_file):
		os.remove(output_file)

	gap_szie = offset - os.path.getsize(main_file)
        fmain = open(main_file, 'rb')
        main_data = fmain.read()
	fp = open(output_file,'ab')
	if fp:
		fp.write(main_data)

        fmcu = open(mcu_file, 'rb')
        mcu_data = fmcu.read()
	print("file main:0x%x  mcu:0x%x  off:0x%x " %(os.path.getsize(main_file), os.path.getsize(mcu_file), offset))
	if gap_szie > 0:
		fp.write('\xFF'*gap_szie)
        if fp:
                fp.write(mcu_data)
	fp.close()
	fmcu.close()
	fmain.close()
	comp_file_md5(output_file, magic)

if __name__ == "__main__":
	try:
		main()
	except KeyboardInterrupt:
		pass
