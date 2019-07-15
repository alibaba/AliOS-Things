#!/usr/bin/env python

import os
import sys
import re
import json
from aos_parse_components import find_comp_mkfile, get_comp_name


def find_include_file(dirname):
    include_files = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".h"):
                include_files += [os.path.abspath("%s/%s" % (root, file))]

    return include_files


def get_comp_include_info(comp_name, mkfile, common_include_dir):
    """ Get include dirs from mkfile """
    include_info = {}
    dirs = []
    with open(mkfile, "r") as f:
       for line in f:
            line = line.strip()
            while line.endswith('\\'):
                line = line[:-1] + next(f).rstrip('\n')

            if not line or "#" in line:
                continue

#            patten = re.compile(r"^(\$\(NAME\)|GLOBAL)_INCLUDES\s?.*=\s?(.*)")
            patten = re.compile(r"^(GLOBAL_INCLUDES)\s?.*=\s?(.*)")
            if "$(NAME)_INCLUDES" in line or "GLOBAL_INCLUDES" in line:
                match = patten.match(line)
                if match:
                    dirs += match.group(2).split()

    base_dir = os.path.dirname(mkfile)
    include_info["include_dirs"] = []
    include_info["include_files"] = []
    if dirs:
        dirs = list(set(dirs))
        include_info["include_dirs"] = [os.path.abspath(os.path.join(base_dir, dir)) for dir in dirs]

    ret = find_include_file(base_dir)
    extra_dir = os.path.join(common_include_dir, comp_name)
    if os.path.isdir(extra_dir):
        ret = find_include_file(extra_dir) + ret
        include_info["include_dirs"].insert(0, extra_dir)

        
    if "/kernel/" in mkfile:
        tmp_dir = os.path.join(os.path.join(common_include_dir, "aos"))
        tmp_file = os.path.join(tmp_dir, "%s.h" % comp_name)
        if os.path.isfile(tmp_file):
            include_info["include_dirs"].insert(0, common_include_dir)
            ret.insert(0, tmp_file)

    if ret:
        include_info["include_files"] = ret

    return include_info


def get_inlcudes(mklist, common_include_dir):
    """ Get component include files """
    comp_includes = {}
    for mkfile in mklist:
        if "/platform/" in mkfile:
            continue

        comp_name = get_comp_name(mkfile)
        if comp_name:
            include_info = get_comp_include_info(comp_name, mkfile, common_include_dir)
            comp_includes[comp_name] = include_info

    return comp_includes


def main():
    if len(sys.argv) < 3:
        print("Usage: %s SOURCE_ROOT OUTPUT_FILE" % sys.argv[0])
        return 1

    source_root = sys.argv[1]
    output_file = sys.argv[2]
    mklist = find_comp_mkfile(source_root)
    common_include_dir = os.path.abspath(os.path.join(source_root, "include"))

    appdir = os.environ.get("APPDIR")
    if appdir:
        mklist += find_comp_mkfile(appdir)

    includes = get_inlcudes(mklist, common_include_dir)
    with open(output_file, "w") as f:
        json.dump(includes, f, sort_keys=True, indent=4)
    #ret = write_config_file(source_root, output_file, mklist)
    #if ret:
    #    print "Failed to create %s ...\n" % config_file

if __name__ == "__main__":
    main()
