# Combine bootloader, partition table and application into a final binary.

import os, sys
import platform

cur_os = platform.system()
arch = platform.architecture()

max_size_data = 1024 * 128
littlefs_bin = 'littlefs.bin'

if cur_os == 'Linux':
    mklittlefs_tool = './mklittlefs_linux'
elif cur_os == 'Darwin':
    mklittlefs_tool = './mklittlefs_osx'
elif cur_os == 'Windows':
    mklittlefs_tool = './mklittlefs.exe'

target_path = os.path.join("./", "file-system")
cmd_str = "\"%s\" -c \"%s\" -d 5 -b 4096 -p 256 -s %d \"%s\"" % (mklittlefs_tool, target_path, max_size_data, littlefs_bin)
os.system(cmd_str)

