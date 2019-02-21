#!/usr/bin/env python

import os, sys, re
import shutil

from aos_parse_components import get_comp_name

# Global definitions
templatedir = "build/scripts/kconfig_template"
top_config_in = "build/Config.in"
board_config_in = "board/Config.in"
example_config_in = "app/example/Config.in"
profile_config_in = "app/profile/Config.in"

autogen_start = "--- Generated Automatically ---"
autogen_end = "--- End ---"

def update_config_in(config_file, config_list):
    """ Update Config.in file that copied from template """
    patten = re.compile(r'\s*source (.*)')
    config_contents = []
    with open (config_file, "r") as cf:
        for line in cf.readlines():
            prefix = ""
            match = patten.match(line)
            if match:
                config_tmp = match.group(1)
                if not re.sub(r'"', "", config_tmp) in config_list:
                    continue

            config_contents += [line]

    if config_contents:
        with open (config_file, "w") as cf:
            for line in config_contents:
                cf.write(line)

def get_opt_config(config_file, keyword, sdir):
    patten = re.compile(r"\s+select\s+(%s.*?)(\s+if.*)?$" % keyword)
    opt_name = ""
    opt_config = ""
    with open(config_file, "r") as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                opt_name = match.group(1)

    if opt_name:
        patten = re.compile(r"^(menuconfig|config)\s+(%s)$" % opt_name)
        for root, dirs, files in os.walk(sdir):
            if 'Config.in' in files:
                if root == sdir:
                    continue

                with open("%s/Config.in" % root, "r") as f:
                    line = f.readline().strip()
                    match = patten.match(line)
                    if match:
                        opt_config = "%s/Config.in" % root
                if opt_config:
                    break
    if not opt_name:
        print("Warning: Can't found %s from %s ..." % (keyword, config_file))

    if opt_name and not opt_config:
        print("Warning: The option is not defined %s ..." % (opt_name))

    return (opt_name, opt_config)

def create_board_config_in(config_file, config_list):
    """ Create board/Config.in """

    with open (config_file, "w") as cf:
        cf.write("# %s\n" % autogen_start)
        cf.write("menu \"BSP         Configuration\"\n")
        cf.write("choice\n")
        cf.write("    prompt \"Select board\"\n")
        cf.write("\n")

        for config in config_list:
            patten = re.compile(r"^(menuconfig|config)\s+(AOS_BOARD_.*)$")
            config_name = ""
            with open(config, "r") as f:
                for line in f.readlines():
                    match = patten.match(line.strip())
                    if match:
                        config_name = match.group(2)
                        break
                    else:
                        print("Error: boardname empty!")
                        return 1

            patten = re.compile(r".*board/(.*)/Config.in")
            match = patten.match(config)
            if match:
                boarddir = match.group(1).replace("/", ".")
            else:
                print("Error: can't get board directory")

            cf.write("source \"%s\"\n" % config)
            cf.write("if %s\n" % config_name)
            cf.write("    config AOS_BUILD_BOARD\n")
            cf.write("        default \"%s\"\n" % boarddir)
            cf.write("\n")

            (mcu_name, mcu_config) = get_opt_config(config, "AOS_MCU_", "platform/mcu")
            if mcu_config:
                cf.write("    source \"%s\"\n" % mcu_config)

                (arch_name, arch_config) = get_opt_config(mcu_config, "AOS_ARCH_", "platform/arch")
                if arch_config:
                    cf.write("    source \"%s\"\n" % arch_config)

            cf.write("endif\n")
            cf.write("\n")

        cf.write("endchoice\n")
        cf.write("\n")

        cf.write("endmenu\n")
        cf.write("# %s\n" % autogen_end)

def create_app_config_in(config_file, config_list):
    """ Create app/*/Config.in files """
    with open (config_file, "w") as cf:
        cf.write("# %s\n" % autogen_start)
        if "example/" in config_file:
            cf.write("config AOS_APP_EXAMPLE\n")
            cf.write("    bool \"Builtin Examples\"\n")
            cf.write("\n")
            cf.write("if AOS_APP_EXAMPLE\n")
            cf.write("choice\n")
            cf.write("    prompt \"Select example\"\n")

        if "profile/" in config_file:
            cf.write("config AOS_APP_PROFILE\n")
            cf.write("    bool \"Builtin Profiles\"\n")
            cf.write("\n")
            cf.write("if AOS_APP_PROFILE\n")
            cf.write("choice\n")
            cf.write("    prompt \"Select profile\"\n")

        cf.write("\n")

        for config in config_list:
            mkfile = re.sub(r"Config.in", r"aos.mk", config)
            appname = get_comp_name(mkfile)
            if not appname:
                print("Error: Can't get app name from %s" % mkfile)
                return 1

            patten = re.compile(r"app/(example|profile)/(.*)/Config.in")
            match = patten.match(config)
            if match:
                appdir = match.group(2).replace("/", ".")
            else:
                print("Error: can't get app directory")

            cf.write("source \"%s\"\n" % config)
            cf.write("if AOS_APP_%s\n" % appname.upper())
            cf.write("    config AOS_BUILD_APP\n")
            cf.write("        default \"%s\"\n" % appdir)
            cf.write("endif\n")
            cf.write("\n")

        cf.write("endchoice\n")
        cf.write("endif\n")
        cf.write("# %s\n" % autogen_end)

def update_top_config_in(top_config_in):
    """ Extra updates for build/Config.in """
    contents = []
    patten = re.compile(r".*(NULL|Null).*")
    with open (top_config_in, "r") as cf:
        for line in cf.readlines():
            match = patten.match(line)
            if match:
                continue
            contents += [line]

    with open (top_config_in, "w") as cf:
        for line in contents:
            cf.write(line)

def main():
    if not os.path.isfile("build/Makefile"):
        print("Error: %s must be run in Sources Root dir!\n" % sys.argv[0])
        return 1

    config_list = []
    source_root = "./"
    for root, dirs, files in os.walk(source_root):
        if 'Config.in' in files:
            config_file = "%s/Config.in" % root.replace(source_root, "")
            config_list += [config_file]

    templates = os.listdir(templatedir)
    for template in templates:
        destdir = re.sub(r"\.Config\.in", "", template)
        destdir = re.sub(r"\.", "/", destdir)
        sourcefile = os.path.join(templatedir, template)
        destfile = os.path.join(destdir, "Config.in")

        if os.path.isdir(destdir):
            if "linkkit/Config.in" in destfile:
                if not os.path.isfile(re.sub(r"Config.in", "sdk-c/aos.mk", destfile)):
                    continue

            shutil.copyfile(sourcefile, destfile)
            config_list += [destfile]

    # Update config files according to installed comps
    for config_file in config_list:
        update_config_in(config_file, config_list)

    # Create board/Config.in
    board_config_list = []
    for config_file in config_list:
        if config_file.startswith("board/") and config_file != "board/Config.in":
            board_config_list += [config_file]

    if board_config_list:
        create_board_config_in(board_config_in, board_config_list)

    # Create app/example/Config.in
    example_config_list = []
    for config_file in config_list:
        if config_file.startswith("app/example") and config_file != "app/example/Config.in":
            example_config_list += [config_file]

    if example_config_list:
        create_app_config_in(example_config_in, example_config_list)

    # Create app/profile/Config.in
    profile_config_list = []
    for config_file in config_list:
        if config_file.startswith("app/profile") and config_file != "app/profile/Config.in":
            profile_config_list += [config_file]

    if profile_config_list:
        create_app_config_in(profile_config_in, profile_config_list)

    # Extra update for build/Config.in
    update_top_config_in(top_config_in)

if __name__ == "__main__":
    main()
