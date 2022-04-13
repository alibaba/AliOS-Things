#! /usr/bin/env python
import os
import platform
import argparse
import sys
import shutil


print(sys.argv)
parser = argparse.ArgumentParser()
parser.add_argument('--target', dest='target', action='store')
args = parser.parse_args()

mypath = os.path.dirname(sys.argv[0])
os.chdir(mypath)
print(os.getcwd())
target = args.target
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

hw_module = 0
cmd_str = "python haas1000_genbin.py %d \"%s\"" % (hw_module, target)
os.system(cmd_str)

bin_path = os.path.join("..", "write_flash_gui", "ota_bin")
shutil.copy(os.path.join(bin_path, "ota_rtos.bin"), os.path.join(bin_path, "ota_rtos_ota.bin"))

cmd_str = "\"%s\"  -f --lzma2=dict=32KiB --check=crc32 -k %s" % (os.path.abspath(path), os.path.join(bin_path, "ota_rtos_ota.bin"))
os.system(cmd_str)

cmd_str = "python ota_gen_md5_bin.py \"%s\" -m %s" % (os.path.join(bin_path, "ota_rtos_ota.bin"), magic)
os.system(cmd_str)

cmd_str = "python ota_gen_md5_bin.py \"%s\" -m %s" % (os.path.join(bin_path, "ota_rtos_ota.bin.xz"), magic)
os.system(cmd_str)

print("run external script success")
