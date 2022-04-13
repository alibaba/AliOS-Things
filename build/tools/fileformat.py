#!/usr/bin/env python

import os, sys, re, platform

def list_files(dir_path):
    file_list = [];

    for root, dirs, files in os.walk(dir_path):
        for f in files:
            if (os.path.splitext(f)[1] == ".h" or os.path.splitext(f)[1] == ".c" or os.path.splitext(f)[1] == ".mk"):
                file_list.append(os.path.join(root, f))
    return file_list

def main():

    if len(sys.argv) != 2:
        print "Dir args is empty, Enter the path to be processed!"
        os._exit(0)

    filedir = sys.argv[1]
    print filedir.strip()

    sys_version = platform.version()
    if "Ubuntu" in sys_version:
        os.environ['syscmd'] = str("fromdos")
    elif "CentOS" in sys_version:
        os.environ['syscmd'] = str("dos2unix")
    else:
        print "Not find the system version!"
        os._exit(0)

    file_list = list_files(filedir)
    for f in file_list:
        os.environ['file'] = str(f)
        os.system('$syscmd $file')

if __name__ == '__main__':
    main()

