#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os, re

def gen_readme_file(bin_output_path, version):
    readmename = os.path.join(bin_output_path, "readme.txt")
    with open(readmename, "w") as f:
        f.write("version : " + version + " \r\n")

def find_version(config_file):
    version = None
    with open(config_file, "r") as f:
        config_mk_str = f.read()
        find_str = re.search( r'DSYSINFO_APP_VERSION=.*', config_mk_str, re.M|re.I)
        if find_str:
            version = find_str.group()
            version = version.replace('DSYSINFO_APP_VERSION=', '')
            version = version.replace('\\"', '')
            version = version.split()[0]

    return version

def main():
    bin_output_path = sys.argv[1]
    config_file     = sys.argv[2]

    version = find_version(config_file)
    if version:
        gen_readme_file(bin_output_path, version)

if __name__ == "__main__":
    main()

