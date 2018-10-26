#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os
import shutil
import replace

from copy_map import copy_map

SDKNAME = "linkkit-sdk-c"


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
    replace.replace_ip()
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
