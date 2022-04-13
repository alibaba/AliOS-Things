#!/usr/bin/env python

import os
import sys
import yaml
from commands.util import info, warn, error, which


def read_yaml(filename):
    """ Read data from yaml file """
    data = None
    with open(filename, "r") as f:
        data = yaml.load(f)

    return data


def help(check_configs):
    """ Show help message """
    check_name = ""
    check_help = ""
    warn("Usage: aos check [ID] [ARGS]\n")
    warn("Check Functions Available:")
    for key in check_configs:
        if "name" in check_configs[key]:
            check_name = check_configs[key]["name"]
        if "help" in check_configs[key]:
            check_help = check_configs[key]["help"]

        warn("[%s] %-10s: %s" % (key, check_name, check_help))


def verify_check_config(check_config):
    """ Check if required command existing """

    prefix = "Verify check configs failed:"
    no_script_defined = "%s \"script\" is not defined!" % prefix
    if "script" in check_config:
        if not check_config["script"]:
            error(no_script_defined)
    else:
        error(no_script_defined)

    if "requires" not in check_config:
        return

    for cmd in check_config["requires"]:
        if not which(cmd):
            error("%s \"%s\" is not installed!" % (prefix, cmd))


def main():
    check_tool = ""
    args = ""
    if len(sys.argv) > 1:
        check_tool = sys.argv[1]

    if len(sys.argv) > 2:
        args = " ".join(sys.argv[2:])

    check_commands_dir = os.path.join(os.path.split(os.path.realpath(__file__))[0], "commands")
    check_function = os.path.join(check_commands_dir, "check_function.yaml")
    check_configs = read_yaml(check_function)

    if not check_tool or check_tool == "help":
        help(check_configs)

    # Get check tool and run it
    check_config = {}
    for key in check_configs:
        if str(key) == check_tool or check_configs[key]["name"] == check_tool:
            check_config = check_configs[key]
            break

    if check_config:
        if check_tool != "help":
            verify_check_config(check_config)

        cmd = "/usr/bin/python %s/%s %s" % (check_commands_dir, check_config["script"], args)
        info("Running cmd: %s ..." % cmd)
        os.system(cmd)


if __name__ == "__main__":
    main()
