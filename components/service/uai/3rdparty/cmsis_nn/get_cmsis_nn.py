#!/usr/bin/env python

import os
import sys
import hashlib
import re
import json
import shutil

def get_source_file(cmsis_path, dirname):
    code_list = []
    for root, dirs, files in os.walk(os.path.join(cmsis_path, dirname)):
        for file in files:
            if file.endswith(".c") :
                idx = root.rfind('/')
                name = root[idx+1:]
                if (name+'.c') != file:
                    filename = os.path.join(root, file)
                    filename = filename[len(cmsis_path) + 1:]
                    code_list.append(filename)

    return code_list

def get_header_file(dirname):
    header_list = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".h") :
                header_list.append(os.path.join(root, file))
    return header_list

def update_aosmk(cmsis_path):
    code_list = []
    header_list = []
    aosmk = os.path.join(cmsis_path, "source.mk")

    with open(aosmk, "w") as f:
        f.write('\r\n')
        f.write('$(NAME)_SOURCES    += \\\r\n')

    code_list = code_list + get_source_file(cmsis_path, 'CMSIS_5/CMSIS/NN/Source')
    code_list = code_list + get_source_file(cmsis_path, 'CMSIS_5/CMSIS/DSP/Source')

    sorted(code_list)

    with open(aosmk, "a") as f:
        num = 0
        for file in code_list:
            num = num + 1
            if num == len(code_list):
                f.write('                        ' + file + '  \r\n')
            else:
                f.write('                        ' + file + '  \\\r\n')

def download_source(cmsis_path):
    add_source = os.path.join(cmsis_path, "CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_pool_q7_HWC_nonsquare.c")
    if not os.path.exists(add_source):
        shutil.copy(os.path.join(cmsis_path, "arm_pool_q7_HWC_nonsquare.c"), add_source)

def main():
    cmsis_path = os.path.dirname(sys.argv[0])
    if len(sys.argv) > 1:
        cmsis_path = sys.argv[1]

    download_source(cmsis_path)
    update_aosmk(cmsis_path)

if __name__ == "__main__":
    main()
