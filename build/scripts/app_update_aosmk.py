#!/usr/bin/env python

import os
import sys
import hashlib
import re
import json
from lib.code import get_md5sum, write_md5sum, compute_header_md5sum, get_depends_from_source
from lib.config import get_project_config

CONFIG_FILE = ".aos"
COMP_INDEX = "aos_comp_index.json"


def update_aosmk(dirname):
    """ Update App's aos.mk """
    aosmk = os.path.join(dirname, "aos.mk")
    config_file = os.path.join(dirname, CONFIG_FILE)

    old_md5sum = get_md5sum(config_file)
    (new_md5sum, include_list) = compute_header_md5sum(dirname)

    if old_md5sum == new_md5sum:
        return True

    comp_info = {}
    with open(COMP_INDEX, "r") as f:
        comp_info = json.load(f)

    source_depends = []
    if comp_info:
        source_depends = get_depends_from_source(comp_info, include_list)

    origin_depends = []
    tmp = get_project_config(config_file, "DEPENDENCIES")
    if tmp:
        origin_depends = tmp.split()

    depends = list(set(source_depends).difference(set(origin_depends)))

    content = []
    found_c_deps = False
    add_line = "$(NAME)_COMPONENTS_CUSTOMIZED := %s\n" % " ".join(depends)
    with open(aosmk, "r") as f:
        for line in f.readlines():
            if line.startswith("$(NAME)_COMPONENTS_CUSTOMIZED"):
                line = add_line
                found_c_deps = True

            content.append(line)

    if not found_c_deps:
        content.append(add_line)
        content.append("$(NAME)_COMPONENTS += $($(NAME)_COMPONENTS_CUSTOMIZED)\n")

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
