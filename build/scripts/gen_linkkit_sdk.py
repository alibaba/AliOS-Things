#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os
import shutil
import re

from copy_map import copy_map

SDKNAME = "linkkit-sdk-c"

targetFilename= './out/export/linkkit-sdk-c/src/infra/system/guider.c'
new_ip='10.10.10.10'
replace_ip_rules = {
    '10.125.3.189': new_ip,
    '100.67.80.75': new_ip,
    '106.15.166.168': new_ip,
    '10.125.7.82': new_ip,
}

def replace(filename, replace_dict):
        tmp_str = ""
        with open(filename, 'r') as f:
            for line in f:
                for key in replace_dict:
                    value = replace_dict[key]
                    if re.search(key, line):
                        line=re.sub(key, value, line)
                tmp_str+=line

        with open(filename, 'w') as of:
            of.write(tmp_str)

def main(argv):
    source_dir = os.path.abspath(sys.argv[1])
    build_dir = os.path.abspath(sys.argv[2])

    sdk_dir = os.path.join(build_dir, SDKNAME)
    if os.path.isdir(sdk_dir):
        print("[INFO]: Removing existing %s" % sdk_dir)
        shutil.rmtree(sdk_dir)

    # Copy components to linkkit sdk dir
    for src, dest in copy_map:
        tmp_src = os.path.join(source_dir, src)
        if dest:
            tmp_dest = os.path.join(build_dir, SDKNAME, dest)
        else:
            tmp_dest = os.path.join(build_dir, SDKNAME)

        dest_dir = os.path.dirname(tmp_dest)
        if not os.path.isdir(dest_dir):
            os.makedirs(dest_dir)

        print("[INFO]: Copy %s to %s" % (tmp_src, tmp_dest))
        if os.path.isdir(tmp_src):
            shutil.copytree(tmp_src, tmp_dest)
        else:
            shutil.copyfile(tmp_src, tmp_dest)

    # replace ip address
    print("[INFO]: replace test information")
    replace(targetFilename, replace_ip_rules)

    # remove tests directory
    print("[INFO]: remove %s" % os.path.join(build_dir, SDKNAME, "tests"))
    shutil.rmtree(os.path.join(build_dir, SDKNAME, "tests"))

    # Generate tarball
    root_dir = build_dir
    base_dir = SDKNAME
    archive_name = sdk_dir
    archive_format = "gztar"

    print("[INFO]: Creating tarball ...")
    tarball = shutil.make_archive(archive_name, archive_format, root_dir, base_dir)
    print("[INFO]: Linkkit SDK exported at: %s" % tarball)

if __name__ == "__main__":
    main(sys.argv)
