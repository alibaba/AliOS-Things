#! /usr/bin/env python3
import os
import platform
import argparse
import sys
import shutil

cur_os = platform.system()
arch = platform.architecture()
path = ''
magic = '0xefefefef'
if cur_os == 'Linux':
    if '64bit' in arch:
        path = 'linux64'
    else:
        path = 'linux32'
elif cur_os == 'Darwin':
    path = 'osx'
elif cur_os == 'Windows':
    path = 'win32'
if path:
    path = os.path.join("tools", path, "xz")
bin_path = os.getcwd()
print("curr_path:%s\r\n" % bin_path)
shutil.copy(os.path.join(bin_path, "aos.bin"), os.path.join(bin_path, "ota_rtos_ota_all.bin"))
shutil.copy(os.path.join(bin_path, "aos.bin"), os.path.join(bin_path, "ota_rtos_ota_xz.bin"))

cmd_str = "\"%s\"  -f --lzma2=dict=32KiB --check=crc32 -k %s" % (os.path.abspath(path), os.path.join(bin_path, "ota_rtos_ota_xz.bin"))
os.system(cmd_str)
cmd_str = "python ota_gen_md5_bin.py \"%s\" -m %s" % (os.path.join(bin_path, "ota_rtos_ota_all.bin"), magic)
os.system(cmd_str)

cmd_str = "python ota_gen_md5_bin.py \"%s\" -m %s" % (os.path.join(bin_path, "ota_rtos_ota_xz.bin.xz"), magic)
os.system(cmd_str)
print("run external script success")
