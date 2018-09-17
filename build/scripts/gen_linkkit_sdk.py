#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys, os
import shutil

SDKNAME = "linkkit-sdk-c"

compdirs = [
    ['middleware/linkkit/sdk-c', ''],
    ['middleware/uagent/uota', 'src/services/uota'],
    ['utility/libc/compilers/armlibc', 'src/infra/stdc/armlibc'],
    ['utility/libc/compilers/iar', 'src/infra/stdc/iarlibc'],
]

# Example file list
examples = ['linkkitapp', 'linkkit_gateway', 'mqttapp', 'coapapp']
example_files = []

for example in examples:
    if example.startswith('linkkit'):
        dist_example_dir = "linkkit"
    elif example.startswith('mqtt'):
        dist_example_dir = "mqtt"
    elif example.startswith('coap'):
        dist_example_dir = "coap"  

    for filename in os.listdir('app/example/' + example):
        if (filename.find('example') != -1 and filename.endswith('.c')):
            src = os.path.join('app/example', example, filename)
            dest = os.path.join('examples', dist_example_dir, filename)
            example_files += [[src, dest]]
        elif filename == "newapi" or filename == "data":
	    for filename2 in os.listdir('app/example/' + example +"/" +filename):
                src = os.path.join('app/example', example, filename, filename2)
                dest = os.path.join('examples', dist_example_dir, filename, filename2)
                example_files += [[src, dest]]

def main(argv):
    source_dir = os.path.abspath(sys.argv[1])
    build_dir = os.path.abspath(sys.argv[2])

    sdk_dir = os.path.join(build_dir, SDKNAME)
    if os.path.isdir(sdk_dir):
        print("[INFO]: Removing existing %s" % sdk_dir)
        shutil.rmtree(sdk_dir)

    # Copy components to linkkit sdk dir
    for src, dest in compdirs + example_files:
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
