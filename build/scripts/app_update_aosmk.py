#!/usr/bin/env python

import os
import sys
import hashlib
import re
import json

CONFIG_FILE = ".aos"
COMP_INCLUDES = "aos_comp_includes.json"

def get_comp_depends(include_list):
    includes = []
    depends = []
    with open(COMP_INCLUDES, "r") as f:
        includes = json.load(f)

    for include_file in include_list:
        found = False
        for key in includes:
            dirs = includes[key]["include_dirs"]
            files = includes[key]["include_files"]
            for dir in dirs:
                if os.path.abspath(os.path.join(dir, include_file)) in files:
                    depends.append(key)
                    found = True
                    break
            if found:
                break

    return depends

def get_md5sum(config_file):
    ret = None
    with open(config_file, "r") as f:
        for line in f.readlines():
            line = line.strip()
            if "MD5SUM_HEADER" in line:
                ret = line.replace("MD5SUM_HEADER=", "")

    return ret

def write_md5sum(config_file, md5sum_value):
    content = []
    with open(config_file, "r") as f:
        for line in f.readlines():
            if "MD5SUM_HEADER" in line:
                continue
            else:
                content.append(line)

    content.append("MD5SUM_HEADER=%s\n" % md5sum_value)
    with open(config_file, "w") as f:
        for line in content:
            f.write(line)

def md5sum(string):
    return hashlib.md5(string).hexdigest()

def get_include_file(code_file):
    include_list = []

    patten = re.compile(r'#include\s+(<|")(.*)(>|")')
    with open(code_file, "r") as f:
        for line in f.readlines():
            if line.startswith("#include"):
                line = line.strip()
                match = patten.match(line)
                if match:
                    include_list.append(match.group(2))

    return include_list

def update_aosmk(dirname):
    code_list = []
    aosmk = os.path.join(dirname, "aos.mk")
    config_file = os.path.join(dirname, CONFIG_FILE)

    old_md5sum = get_md5sum(config_file)
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".c") or file.endswith(".h") or file.endswith(".cpp"):
                code_list.append(os.path.join(root, file))

    include_list = []
    for code_file in code_list:
        include_list += get_include_file(code_file)

    sorted(include_list)
    new_md5sum = md5sum(" ".join(include_list))

    if old_md5sum == new_md5sum:
        return True

    content = []
    with open(aosmk, "r") as f:
        for line in f.readlines():
            if "$(NAME)_COMPONENTS" in line:
                continue

            content.append(line)

    depends = get_comp_depends(include_list)
    for dep in depends:
        content.append("$(NAME)_COMPONENTS += %s\n" % dep)

    with open(aosmk, "w") as f:
        for line in content:
            f.write(line)

    write_md5sum(config_file, new_md5sum)

def main():
    if len(sys.argv) < 2:
        print("Usage: %s APPDIR" % sys.argv[0])
        return 1

    app_dir = sys.argv[1]
    update_aosmk(app_dir)

if __name__ == "__main__":
    main()
