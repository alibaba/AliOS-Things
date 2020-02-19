#!/usr/bin/env python

import os
import sys
import re
import json
from lib.comp import find_comp_mkfile, get_comp_name, get_comp_deps
from lib.code import get_include_file, get_configin_file

INCLUDE_FOLDER = "include"
COMPONENT_FOLDER = ["application", "test", "core", "platform", "components"]

def get_comp_include_info(comp_name, mkfile, common_include_dir):
    """ Get include infos from mkfile by searching lines started with GLOBAL_INCLUDES:
    return data: {"include_dirs": [], "include_files": []}
    include_dirs = $(GLOBAL_INCLUDES) + $(AOS_SDK_PATH)/include
    include_files = $(GLOBAL_INCLUDES)/*.h 
        + $(AOS_SDK_PATH)/include/aos/<component_name>.h
        + $(AOS_SDK_PATH)/include/aos/<component_name>/*.h if core component
    """
    include_info = {}

    if "/platform/" in mkfile:
        return include_info

    dirs = []
    with open(mkfile, "r") as f:
       for line in f:
            line = line.strip()
            while line.endswith('\\'):
                line = line[:-1] + next(f).rstrip('\n')

            if not line or "#" in line:
                continue

            patten = re.compile(r"^(GLOBAL_INCLUDES)\s?.*=\s?(.*)")
            if "GLOBAL_INCLUDES" in line:
                match = patten.match(line)
                if match:
                    dirs += match.group(2).split()

    base_dir = os.path.dirname(mkfile)
    include_info["include_dirs"] = []
    include_info["include_files"] = []
    if dirs:
        dirs = list(set(dirs))
        include_info["include_dirs"] = [os.path.abspath(os.path.join(base_dir, dir)) for dir in dirs]
    
    ret = []
    for dir in include_info["include_dirs"]:
        ret = get_include_file(dir) + ret

    """ component in ./core, and its header files are in
        ./include/aos/<component_name>.h
        ./include/aos/<component_name>/*.h
    """
    if "/core/" in mkfile:
        tmp_dir = os.path.abspath(os.path.join(common_include_dir, "aos"))
        tmp_file = os.path.join(tmp_dir, "%s.h" % comp_name)
        if os.path.isfile(tmp_file):
            ret.insert(0, tmp_file)
        else:
            tmp_dir = os.path.join(tmp_dir, comp_name)
            if os.path.isdir(tmp_dir):
                ret = get_include_file(tmp_dir) + ret
    """ component in ./components, and its header files are in
        ./include/*/<component_name>/*h
        the dir should be put into GLOBAL_INCLUDES
    """

    include_info["include_dirs"].insert(0, common_include_dir)

    if ret:
        include_info["include_files"] = ret

    return include_info


def get_comp_info(source_root, mklist, common_include_dir):
    """ Get comp infos from makefile(aos.mk) list that includes: comp_name, comp_dir, 
    include_dir, include_files, Config.in, mandatory and optional dependencies
    """
    comp_info = {}
    source_root = os.path.abspath(source_root).replace("\\", "/")

    for mkfile in mklist:
        mkfile = mkfile.replace("\\", "/")
        comp_name = get_comp_name(mkfile)
        if comp_name:
            if comp_name == "system_include" or comp_name == "buildsystem" or comp_name == "buildconfigs":
                continue

            comp_dir = os.path.dirname(mkfile)
            include_info = get_comp_include_info(comp_name, mkfile, common_include_dir)
            config_file = get_configin_file(comp_dir)
            mandatory_deps, optional_deps = get_comp_deps(mkfile)

            # Replace "\\" as "/"
            cleaned_include_info = {}
            for key in include_info:
                cleaned_include_info[key] = []
                for tmp in include_info[key]:
                    cleaned_include_info[key].append(tmp.replace("\\", "/"))

            config_file = config_file.replace("\\", "/")
            comp_info[comp_name] = cleaned_include_info
            comp_info[comp_name]["location"] = comp_dir.replace(source_root + "/", "")
            comp_info[comp_name]["config_file"] = config_file.replace(source_root + "/", "")
            comp_info[comp_name]["dependencies"] = mandatory_deps
            comp_info[comp_name]["optional_dependencies"] = optional_deps

            # Keep a copy for board with dirname
            if comp_name.startswith("board_"):
                board_name = mkfile.split("/")[-2]
                comp_info[board_name] = comp_info[comp_name]

    return comp_info


def main():
    """ analysis SDK code and generate comp info file, involves comp_name, comp_dir, 
    include_dir, include_files, Config.in, mandatory and optional dependencies """
    if len(sys.argv) < 3:
        print("Usage: %s SOURCE_ROOT OUTPUT_FILE" % sys.argv[0])
        return 1

    source_root = sys.argv[1]
    output_file = sys.argv[2]
    mklist = []
    for comp_dir in COMPONENT_FOLDER:
        mklist += find_comp_mkfile(os.path.join(source_root, comp_dir))
    common_include_dir = os.path.abspath(os.path.join(source_root, INCLUDE_FOLDER))

    appdir = os.environ.get("APPDIR")
    if appdir:
        mklist += find_comp_mkfile(appdir)

    includes = get_comp_info(source_root, mklist, common_include_dir)
    with open(output_file, "w") as f:
        json.dump(includes, f, sort_keys=True, indent=4)

if __name__ == "__main__":
    main()
