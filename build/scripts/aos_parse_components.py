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
    name = None
    patten = re.compile(r'^NAME.*=\s*(.*)\s*')
    with open(mkfile, 'r') as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                 name = match.group(1)

    return name

def write_config_file(source_root, config_file, mklist):
    config_contents = {}
    config_keys = []
    aliasname = ""
    patten = re.compile(source_root + "(/*|\\\\*)(.*)(/|\\\\.*mk)")

    for mkfile in mklist:
        name = get_comp_name(mkfile)

        if not name:
            continue

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
        for key in config_keys:
            f.write("%s_MAKEFILE := %s\n" % (key, config_contents[key]))

def main(argv):
    if len(argv) < 3:
        print("Usage: %s SOURCE_ROOT CONFIG_FILE" % argv[0])
        return 1

    source_root = argv[1]
    config_file = argv[2]
    mklist = find_comp_mkfile(source_root)
    write_config_file(source_root, config_file, mklist)

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
