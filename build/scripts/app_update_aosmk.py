#!/usr/bin/env python

import os, shutil
import sys
import hashlib
import re
import json
from lib.code import get_md5sum, write_md5sum, compute_header_md5sum, get_depends_from_source
from lib.config import get_project_config
from lib.comp import get_comp_mandatory_depends, get_comp_optional_depends, get_comp_optional_depends_text
from lib.comp import generate_default_header_file
from app_gen_kconfig import gen_kconfig
from aos_autoconf_convert import convert_aosconfig_config

DOT_AOS = ".aos"
COMP_INDEX = "aos_comp_index.json"
CONFIG_BAK_PATH = ".important.bak"
CONFIGIN_FILE = "Config.in"
DOT_CONFIG_FILE = ".config"
AOS_CONFIG_H_FILE = "aos_config.h"
APP_CONFIG_FILE = "app.config"
AOS_MAKEFILE = "aos.mk"

def update_depends_config(dirname, comps):
    """ according to the new added components, update source "xxx/Config.in" of 
    mandatory and optitional depends in application's Config.in file:
    1. remove the line of source "xxx/Config.in" if the comp is already existed 
    in .Config.in.bak( a copy of Config.in when project generated), 
    2. add the source "xxx/Config.in" for new added comps and their dependencies """
    if not comps:
        return False

    with open(os.path.join(dirname, CONFIG_BAK_PATH, CONFIGIN_FILE), "r") as f:
        text_config = f.read()

    """ Source Config.in of depends """
    mandatory_configs = []
    optional_configs = []
    comp_info = {}
    aos_sdk = os.environ.get("AOS_SDK_PATH")
    with open(os.path.join(aos_sdk, COMP_INDEX), "r") as f:
        comp_info = json.load(f)

    if comp_info:
        mandatory_deps = get_comp_mandatory_depends(comp_info, comps)
        mandatory_deps += comps
        for comp in mandatory_deps:
            if comp in comp_info:
                config = comp_info[comp]["config_file"]
                if config:
                    mandatory_configs.append(config)
        optional_deps = get_comp_optional_depends(comp_info, mandatory_deps)
        for comp in optional_deps:
            if comp["comp_name"] in comp_info:
                config = comp
                config["config_file"] = comp_info[comp["comp_name"]]["config_file"]
                # print("config is", config)
                optional_configs.append(config)

    if mandatory_configs:
        mandatory_configs = sorted(list(set(mandatory_configs)))
        for config in mandatory_configs:
            line = 'source "$AOS_SDK_PATH/%s"\n' % config
            text_config.replace(line, "")
            text_config += line
        if optional_configs:
            for config in optional_configs:
                """ one dependency: comp_name, config_file, condition [[]] """
                line = 'source "$AOS_SDK_PATH/%s"\n' % config["config_file"]
                text_config = text_config.replace(line, "")
                line = get_comp_optional_depends_text(config["condition"], config["config_file"])
                # print(line)
                text_config += line
    
    with open(os.path.join(dirname, CONFIGIN_FILE), "w+") as f:
        f.write(text_config)

def update_aosmk(dirname):
    """ Update App's aos.mk if checksum updated, put app dependencies into 
    $(NAME)_COMPONENTS_CUSTOMIZED. And update checksum """
    aosmk = os.path.join(dirname, AOS_MAKEFILE)
    config_file = os.path.join(dirname, DOT_AOS)

    old_md5sum = get_md5sum(config_file)
    (new_md5sum, include_list) = compute_header_md5sum(dirname)

    if old_md5sum == new_md5sum:
        return None

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
                # replace old str with new one if found $(NAME)_COMPONENTS_CUSTOMIZED
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
    return depends

def main():
    if len(sys.argv) < 2:
        print("Usage: %s APPDIR" % sys.argv[0])
        return 1

    app_dir = sys.argv[1]

    # check components config in aos_config.h
    convert_aosconfig_config(os.path.join(app_dir, AOS_CONFIG_H_FILE), os.path.join(app_dir, DOT_CONFIG_FILE))
    # check dependencies in app source file
    depends = update_aosmk(app_dir)
    
    if depends:
        # update app Config.in to involve new components
        update_depends_config(app_dir, depends)
        # update components header file of default configuration
        generate_default_header_file(os.path.join(app_dir, CONFIGIN_FILE))
        # get appname and boardname
        dot_config_file = os.path.join(app_dir, DOT_CONFIG_FILE)
        appname = get_project_config(dot_config_file, "AOS_BUILD_APP")
        boardname = get_project_config(dot_config_file, "AOS_BUILD_BOARD")
        os.rename(dot_config_file, os.path.join(app_dir, APP_CONFIG_FILE))
        """ run makefile to get default config for new added components,  and 
        update .config and aos_config.h """
        gen_kconfig(app_dir, appname, boardname)

if __name__ == "__main__":
    main()
