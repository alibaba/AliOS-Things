#!/usr/bin/env python

import os, sys
import re

def find_comp_mkfile(dirname):
    """ Find component makefiles from dirname """
    mklist = []
    for root, dirs, files in os.walk(dirname):
        if 'out' in root or 'build' in root or 'publish' in root:
            continue

        if 'aos.mk' in files:
            mklist += ["%s/aos.mk" % root]
            continue

    return mklist

def get_comp_name(mkfile):
    """ Get comp name from mkfile """
    name = None
    patten = re.compile(r'^NAME.*=\s*(.*)\s*')
    with open(mkfile, 'r') as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                 name = match.group(1)

    return name

def get_comp_optname(compname, mkfile):
    """ Return config option name of comp """
    optname = None
    compname = compname.upper().replace("-", "_")
    if "application/" in mkfile:
        optname = "AOS_APP_%s" % compname.replace("APP_", "")
    elif "board/" in mkfile:
        optname = "AOS_BOARD_%s" % compname.replace("BOARD_", "")
    elif "mcu/" in mkfile:
        optname = "AOS_MCU_%s" % compname.replace("MCU_", "")
    elif "arch/" in mkfile:
        optname = "AOS_ARCH_%s" % compname.replace("ARCH_", "")
    else:
        optname = "AOS_COMP_%s" % compname

    return optname

def get_app_name(mkfile):
    """ Return app dir name as app name """
    name = None
    patten = re.compile(r".*(application/example/|application/profile/|test/develop/)(.*)/aos.mk")
    match = patten.match(mkfile)
    if match:
        name = match.group(2).replace("/", ".")

    if not name:
        patten = re.compile(r".*/application/(\w*/)?(.*)/aos.mk")
        match = patten.match(mkfile)
        if match:
            name = match.group(2).replace("/", ".")

    if not name:
        name = get_comp_name(mkfile)

    return name

def get_board_name(mkfile):
    """ Return board dir name as board name """
    name = None
    patten = re.compile(r".*(board/)(.*)/aos.mk")
    match = patten.match(mkfile)
    if match:
        name = match.group(2).replace("/", ".")

    return name

def write_config_file(source_root, config_file, mklist):
    """ Parse comps and write data to total config file """
    # contents will be wrote to config file
    conf_dict = {}
    # sort as keys
    config_keys = []
    # comp name defined by NAME
    real_names = []

    for mkfile in mklist:
        comptype = "normal"
        aliasname = ""
        mkfile = mkfile.replace("\\", "/")

        name = get_comp_name(mkfile)
        if not name:
            continue

        real_names += [name]

        if "application/" in mkfile or "test/develop/" in mkfile:
            comptype = "app"
            aliasname = get_app_name(mkfile)
        elif "board/" in mkfile:
            comptype = "board"
            aliasname = get_board_name(mkfile)
        else:
            pass

        if not name in conf_dict:
            conf_dict[name] = { "mkfile": mkfile, "aliasname": aliasname, "type": comptype }
            config_keys += [name]
        else:
            print("[ERROR]: Duplicate component found: %s" % name)
            print("- %s: %s" % (name, conf_dict[name]))
            print("- %s: %s" % (name, mkfile))
            return 1

    with open (config_file, 'w') as f:
        f.write("REAL_COMPONENTS := %s\n" % " ".join(real_names))
        for compname in real_names:
            optname = get_comp_optname(compname, conf_dict[compname]["mkfile"])
            if conf_dict[compname]["type"] == "app" or conf_dict[compname]["type"] == "board":
                f.write("OBJ-$(%s) += %s\n" % (optname, conf_dict[compname]["aliasname"]))
            else:
                f.write("OBJ-$(%s) += %s\n" % (optname, compname))

        for key in config_keys:
            mkfile = conf_dict[key]["mkfile"]
            location = os.path.dirname(mkfile)
            aliasname = conf_dict[key]["aliasname"]
            f.write("%s_MAKEFILE := %s\n" % (key, mkfile))
            f.write("%s_LOCATION := %s\n" % (key, location))
            if aliasname and aliasname != key:
                f.write("%s_MAKEFILE := %s\n" % (aliasname, mkfile))
                f.write("%s_LOCATION := %s\n" % (aliasname, location))

def main(argv):
    if len(argv) < 3:
        print("Usage: %s SOURCE_ROOT CONFIG_FILE" % argv[0])
        return 1

    source_root = argv[1]
    config_file = argv[2]
    print("Parsing all components ...")
    mklist = find_comp_mkfile(source_root)

    appdir = os.environ.get("APPDIR")
    if appdir:
        mklist += find_comp_mkfile(appdir)

    ret = write_config_file(source_root, config_file, mklist)
    if ret:
        print("Failed to create %s ...\n" % config_file)

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
