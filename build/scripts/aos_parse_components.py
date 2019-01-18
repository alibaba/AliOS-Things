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

        oldmk = os.path.basename(root) + '.mk'
        if oldmk in files:
            mklist += ["%s/%s" % (root, oldmk)]
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
    compname = compname.upper()
    if "app/" in mkfile:
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

def write_config_file(source_root, config_file, mklist):
    """ Parse comps and write data to total config file """
    # contents will be wrote to config file
    config_contents = {}
    # sort as keys
    config_keys = []
    # comp name defined by NAME
    real_names = []
    # old style with xx.yy.zz
    aliasname = ""
    patten = re.compile(source_root + "(/*|\\\\*)(.*)(/|\\\\.*mk)")

    for mkfile in mklist:
        mkfile = mkfile.replace("\\", "/")
        name = get_comp_name(mkfile)

        if not name:
            continue

        real_names += [name]
        match = patten.match(mkfile)
        if match:
            aliasname = match.group(2)
            aliasname = aliasname.replace("/", ".").replace("\\", ".")

        if not name in config_contents:
            config_contents[name] = mkfile
            config_keys += [name]
        else:
            print "[ERROR]: Duplicate component found: %s" % name
            print "- %s: %s" % (name, config_contents[name])
            print "- %s: %s" % (name, mkfile)
            return 1

        if name == aliasname:
            continue

        if not aliasname in config_contents:
            config_contents[aliasname] = mkfile
            config_keys += [aliasname]
        else:
            print "[ERROR]: Duplicate component found: %s" % aliasname
            print "- %s: %s" % (aliasname, config_contents[aliasname])
            print "- %s: %s" % (aliasname, mkfile)
            return 1

    with open (config_file, 'w') as f:
        f.write("REAL_COMPONENTS := %s\n" % " ".join(real_names))
        for compname in real_names:
            optname = get_comp_optname(compname, config_contents[compname])
            f.write("OBJ-$(%s) += %s\n" % (optname, compname))

        for key in config_keys:
            f.write("%s_MAKEFILE := %s\n" % (key, config_contents[key]))
            f.write("%s_LOCATION := %s\n" % (key, os.path.dirname(config_contents[key])))

def main(argv):
    if len(argv) < 3:
        print("Usage: %s SOURCE_ROOT CONFIG_FILE" % argv[0])
        return 1

    source_root = argv[1]
    config_file = argv[2]
    print "Parsing all components ..."
    mklist = find_comp_mkfile(source_root)
    ret = write_config_file(source_root, config_file, mklist)
    if ret:
        print "Failed to create %s ...\n" % config_file

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
