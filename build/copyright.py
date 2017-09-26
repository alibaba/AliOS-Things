#!/usr/bin/env python

import os
import re

rootdirs = ['./board/linuxhost',         \
            './board/mk108',             \
            './board/mk3060',            \
            './bootloader',              \
            './devices',                 \
            './example',                 \
            './framework',               \
            './include',                 \
            './kernel/hal',              \
            './kernel/modules/fs/kv',    \
            './kernel/protocols/mesh',   \
            './kernel/rhino',            \
            './kernel/syscall',          \
            './kernel/vcall',            \
            './kernel/vfs',              \
            './kernel/yloop',            \
            './platform/arch/arm',       \
            './platform/arch/csky',      \
            './platform/arch/include',   \
            './platform/arch/linux',     \
            './platform/arch/xtensa',    \
            './platform/mcu/beken/art',  \
            './platform/mcu/beken/hal',  \
            './platform/mcu/beken/aos',  \
            './platform/mcu/include',    \
            './platform/mcu/linux',      \
            './platform/mcu/template',   \
            './security/alicrypto/inc',  \
            './security/alicrypto/test', \
            './security/libid2/include', \
            './security/libkm/include',  \
            './security/prov/include',   \
            './security/tfs',            \
            './test',                    \
            './tools/cli',               \
            './tools/dda',               \
            './tools/testbed',           \
            './tools/trace',             \
            './tools/wifimonitor',       \
            './utility']

def list_files(dir_path):
    file_list = [];

    for root, dirs, files in os.walk(dir_path):
        for f in files:
            if (os.path.splitext(f)[1] == ".h" or os.path.splitext(f)[1] == ".c"):
                file_list.append(os.path.join(root, f))
    return file_list

def insert_copyright(f, regex, copyright):
    fo = open(f, 'r+')
    string = fo.read()
    if not re.search(regex, string, re.M):
        fo.seek(0, 0)
        fo.write(copyright)
        fo.write(string)

    fo.close()

def main():
    regex = r'Copyright'

    fo = open("./build/copyright", 'r')
    copyright = fo.read()
    fo.close()

    for rootdir in rootdirs:
        file_list = list_files(rootdir)
        for f in file_list:
            insert_copyright(f, regex, copyright)

if __name__ == '__main__':
    main()

