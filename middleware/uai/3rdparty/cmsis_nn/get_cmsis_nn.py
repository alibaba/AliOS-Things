#!/usr/bin/env python

import os
import sys
import hashlib
import re
import json

def get_source_file(dirname):
    code_list = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".c") :
                idx = root.rfind('/')
                name = root[idx+1:]
                if (name+'.c') != file:
                    filename = os.path.join(root, file)
                    code_list.append(filename)

    return code_list

def get_header_file(dirname):
    header_list = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".h") :
                header_list.append(os.path.join(root, file))
    return header_list

def update_aosmk():
    code_list = []
    header_list = []
    aosmk = "aos.mk"

    with open(aosmk, "w") as f:
        f.write('NAME := cmsis_nn\r\n')
        f.write('$(NAME)_TYPE := third_party\r\n\r\n')
        f.write('GLOBAL_INCLUDES    +=   \\\r\n')
        f.write('                        CMSIS_5/CMSIS/Core/Include \\\r\n')
        f.write('                        CMSIS_5/CMSIS/DSP/Include \\\r\n')
        f.write('                        CMSIS_5/CMSIS/NN/Include \\\r\n')
        f.write('\r\n')
        f.write('$(NAME)_SOURCES    += \\\r\n')

    code_list = code_list + get_source_file('CMSIS_5/CMSIS/NN/Source')
    code_list = code_list + get_source_file('CMSIS_5/CMSIS/DSP/Source')

    sorted(code_list)

    with open(aosmk, "a") as f:
        for file in code_list:
            f.write('                        ' + file + '  \\\r\n')

def main():
    update_aosmk()

if __name__ == "__main__":
    main()
