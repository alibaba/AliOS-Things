#!/usr/bin/env python

import os, shutil
import sys
import hashlib
import re
import json
from lib.code import get_md5sum, write_md5sum, compute_header_md5sum, get_depends_from_source, write_project_config
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
KERNEL_CONFIG_KEYWORD = "Kernel      Configuration"
COMPONENT_CONFIG_KEYWORD = "Components  Configuration"

def replace_one_config_in(text_config, line, line_repl, conds_line):
    """replace existed Config.in or insert into its corresponding menu"""
    p = "(if (.*)\n)?"+re.escape(line)+"(endif\n)?"
    match = re.search(p, text_config)
    if match:
        if not match.lastindex:
            # found mandatory depends
            return text_config
        else:
            # found optional depends
            line_repl = "if (%s || %s)\n" % (match.group(2), conds_line) + line + "endif\n"
            text_config = re.sub(p, line_repl, text_config, 1)
    else:
        # can not find the Config.in of this comp
        if "AOS_SDK_PATH/core/" in line:
            p = 'menu "%s"\n' % KERNEL_CONFIG_KEYWORD
            line_repl = p + line_repl
            text_config = re.sub(p, line_repl, text_config, 1)
        elif "AOS_SDK_PATH/components/" in line:
            p = 'menu "%s"\n' % COMPONENT_CONFIG_KEYWORD
            line_repl = p + line_repl
            text_config = re.sub(p, line_repl, text_config, 1)
    return text_config

def update_depends_config_in(text_config, mandatory_configs, optional_configs):
    """according to the depends, replace existed Config.in or insert into its 
    corresponding menu"""
    if mandatory_configs:
        mandatory_configs = sorted(list(set(mandatory_configs)))
        for config in mandatory_configs:
            line = 'source "$AOS_SDK_PATH/%s"\n' % config
            text_config = replace_one_config_in(text_config, line, line, "y")
        if optional_configs:
            for config in optional_configs:
                """ one dependency: comp_name, config_file, condition [[]] """
                line = 'source "$AOS_SDK_PATH/%s"\n' % config["config_file"]
                line_repl, conds_line = get_comp_optional_depends_text(config["condition"], config["config_file"])
                text_config = replace_one_config_in(text_config, line, line_repl, conds_line)
    return text_config

def update_depends_config(dirname, comps):
    """ according to the new added components, update source "xxx/Config.in" of 
    mandatory and optitional depends in application's Config.in file:
    1. remove the line of source "xxx/Config.in" if the comp is already existed 
    in Config.in( a copy of Config.in when project generated), 
    2. add the source "xxx/Config.in" for new added comps and their dependencies """

    with open(os.path.join(dirname, CONFIG_BAK_PATH, CONFIGIN_FILE), "r") as f:
        text_config = f.read()

    if not comps:
        with open(os.path.join(dirname, CONFIGIN_FILE), "w+") as f:
            f.write(text_config)
        return
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

    text_config = update_depends_config_in(text_config, mandatory_configs, optional_configs)
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
        return False, []

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
    p1 = re.compile(r'^\$\(NAME\)_COMPONENTS_CUSTOMIZED.*(:|\+)=\s*(.*)\s*')
    with open(aosmk, "r") as f:
        for line in f.readlines():
            if line.startswith("$(NAME)_COMPONENTS_CUSTOMIZED"):
                match = p1.match(line)
                if match:
                    if match.group(1) == ":":
                        # replace old str with new one if found $(NAME)_COMPONENTS_CUSTOMIZED :=
                        line = add_line
                        found_c_deps = True
                    else:
                        # found $(NAME)_COMPONENTS_CUSTOMIZED +=
                        if not found_c_deps:
                            # no $(NAME)_COMPONENTS_CUSTOMIZED := 
                            line = add_line + line
                            found_c_deps = True
            content.append(line)

    if not found_c_deps:
        append_text = """
# components added by include c header file in the source code. DO NOT EDIT!
%s

# add components name manually here if you want to import some components
$(NAME)_COMPONENTS_CUSTOMIZED +=

# tell build system to add components above. DO NOT EDIT!
$(NAME)_COMPONENTS += $($(NAME)_COMPONENTS_CUSTOMIZED)

""" % add_line
        content.append(append_text)

    with open(aosmk, "w") as f:
        for line in content:
            f.write(line)

    write_md5sum(config_file, new_md5sum)
    return True, depends

def check_user_added_comp(dirname):
    """ check user added comp by $(NAME)_COMPONENTS_CUSTOMIZED += whether updated """
    aosmk = os.path.join(dirname, AOS_MAKEFILE)
    config_file = os.path.join(dirname, DOT_AOS)
    old_comp_str = get_project_config(config_file, "USER_ADD_DEPS")

    user_added_comp = []
    p1 = re.compile(r'^\$\(NAME\)_COMPONENTS_CUSTOMIZED.*\+=\s*(.*)\s*')
    with open(aosmk, "r") as f:
        for line in f.readlines():
            if line.startswith("$(NAME)_COMPONENTS_CUSTOMIZED"):
                match = p1.match(line)
                if match:
                    user_added_comp += match.group(1).split()

    user_added_comp = sorted(list(set(user_added_comp)))
    new_comp_str = " ".join(user_added_comp)
    if new_comp_str == old_comp_str:
        return False, []

    config_data = {"USER_ADD_DEPS": new_comp_str}
    write_project_config(config_file, config_data)
    return True, user_added_comp


def main():
    if len(sys.argv) < 2:
        print("Usage: %s APPDIR" % sys.argv[0])
        return 1

    app_dir = sys.argv[1]

    # check components config in aos_config.h
    convert_aosconfig_config(os.path.join(app_dir, AOS_CONFIG_H_FILE), os.path.join(app_dir, DOT_CONFIG_FILE))
    # check dependencies in app source file
    src_updated, src_depends = update_aosmk(app_dir)
    # check dependencies in app aos.mk
    mk_updated, mk_depends = check_user_added_comp(app_dir)

    if src_updated or mk_updated:
        depends = src_depends + mk_depends
        depends = list(set(depends))
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
        update .config """
        gen_kconfig(app_dir, appname, boardname, True, False)

if __name__ == "__main__":
    main()
