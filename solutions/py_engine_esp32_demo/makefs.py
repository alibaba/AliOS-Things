# Combine bootloader, partition table and application into a final binary.

import os, sys
import platform

sys.path.append(os.getenv("IDF_PATH") + "/components/partition_table")

import gen_esp32part

def load_partition_table(filename):
    with open(filename, "rb") as f:
        return gen_esp32part.PartitionTable.from_binary(f.read())

cur_os = platform.system()
arch = platform.architecture()

arg_partitions_bin = sys.argv[1]
# Load the partition table.
partition_table = load_partition_table(arg_partitions_bin)
max_size_data = 0
offset_data = 0

littlefs_bin = 'fs.bin'
# Inspect the partition table to find offsets and maximum sizes.
for part in partition_table:
    if part.type == gen_esp32part.DATA_TYPE and part.subtype == 0x81:
        offset_data = part.offset
        max_size_data = part.size
print("max_size_data:%d" % max_size_data)
if cur_os == 'Linux':
    mklittlefs_tool = './mklittlefs_linux'
elif cur_os == 'Darwin':
    mklittlefs_tool = './mklittlefs_osx'
elif cur_os == 'Windows':
    mklittlefs_tool = './mklittlefs.exe'
target_path = os.path.join("./", "fs")
cmd_str = "\"%s\" -c \"%s\" -d 5 -b 4096 -p 256 -s %d \"%s\"" % (mklittlefs_tool, target_path, max_size_data, littlefs_bin)
os.system(cmd_str)

