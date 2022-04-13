#!/usr/bin/env python

import os, sys
import re

def get_depend_app_name(mkfile):
    """ Get depend app name from mkfile """
    app_name = []
    with open(mkfile, "r") as f:
       for line in f:
            line = line.strip()
            while line.endswith('\\'):
                line = line[:-1] + next(f).rstrip('\n')

            if not line or "#" in line:
                continue

            patten = re.compile(r"^(DEPEND_APPS)\s?.*=\s?(.*)")
            if "DEPEND_APPS" in line:
                match = patten.match(line)
                if match:
                    app_name += match.group(2).split()

    return app_name

def get_depend_apps_name(apps_name, processed_apps, all_makefiles):
    """ Get all depend apps name recursively from mkfile """
    depends = []
    patten = re.compile(r"(.*)_MAKEFILE\s?.*=\s?(.*)")
    for app in apps_name:
        app_mkfile = app + "_MAKEFILE"
        for line in all_makefiles:
            if app_mkfile in line:
                match = patten.match(line)
                if match:
                    mkfile = match.group(2)
                    depends += get_depend_app_name(mkfile)

    processed_apps += apps_name
    processed_apps = list(set(processed_apps))
    depends = list(set(depends).difference(set(processed_apps)))
    if depends:
        processed_apps = get_depend_apps_name(depends, processed_apps, all_makefiles)
    return processed_apps

def write_config_file(app_name, comp_file, deps_file):
    """ Save all depend apps name to file """
    new_added_deps = [app_name]
    processed_deps = []
    all_makefiles = []
    with open(comp_file, "r") as f:
       for line in f:
            line = line.strip()
            if not line or "#" in line:
                continue
            if "_MAKEFILE" in line:
                all_makefiles.append(line)

    processed_deps = get_depend_apps_name(new_added_deps, processed_deps, all_makefiles)
    processed_deps = list(set(processed_deps).difference(set(new_added_deps)))
    print("depends are ", processed_deps)
    with open (deps_file, 'w') as f:
        for deps in processed_deps:
            f.write("%s\n" % (deps))

def main(argv):
    if len(argv) < 4:
        print("Usage: %s APP COMPONENTS_FILE DEPEND_APPS" % argv[0])
        return 1

    app_name = argv[1]
    comp_file = argv[2]
    deps_file = argv[3]

    print("Parsing all apps ...")   
    write_config_file(app_name, comp_file, deps_file)
    

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
