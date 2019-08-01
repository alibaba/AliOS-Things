#!/usr/bin/env python

import os
import sys
import yaml
import traceback
from commands.util import info, warn, error, which


def help(commands):
    """ Show help message """
    warn("Usage: aos check [COMMAND] [ARGS]\n")
    warn("Check Functions Available:")
    index = 0
    for cmd in commands:
        warn("[%s] %-10s: %s" % (index, cmd["name"], cmd["short_help"]))
        index = index + 1


def check_requires(requires):
    """ Check if required command existing """
    if not requires:
        return True

    for cmd in requires:
        if not which(cmd):
            error("Required command \"%s\" is not installed!" % (cmd))

    return True

def main():
    check_tool = ""
    args = ""
    if len(sys.argv) > 1:
        check_tool = sys.argv[1]

    if len(sys.argv) > 2:
        args = " ".join(sys.argv[2:])

    # Load check commands
    currdir = os.path.dirname(os.path.abspath(__file__))
    check_command_dir = os.path.join(currdir, "commands")
    commands = []
    for cmdfile in os.listdir(check_command_dir):
        if cmdfile == "__init__.py" or cmdfile == "util.py" or cmdfile.endswith(".pyc"):
            continue

        if cmdfile.endswith(".py"):
            cmd = cmdfile[:-3]
            try:
                tmp = __import__("commands." + cmd, None, None,
                                 ["name", "short_help", "requires"])

                if hasattr(tmp, "name"):
                    name = tmp.name
                else:
                    name = os.path.basename(cmdfile).replace(".py", "")

                short_help = ""
                if hasattr(tmp, "name"):
                    short_help = tmp.short_help

                requires = []
                if hasattr(tmp, "requires"):
                    requires = tmp.requires

                cmd_dict = {"name": name,
                            "short_help": short_help,
                            "requires": requires,
                            "script": cmdfile,
                           }

                commands.append(cmd_dict)
            except:
                traceback.print_exc()
                return False

    if not check_tool or check_tool == "help":
        help(commands)

    if check_tool:
        script = ""
        requires = []
        for tmp in commands:
            if tmp["name"] == check_tool:
                script = tmp["script"]
                requires = tmp["requires"]
                break

        check_requires(requires)
        cmd = "/usr/bin/python %s/%s %s" % (check_command_dir, script, args)
        info("Running cmd: %s ..." % cmd)
        os.system(cmd)


if __name__ == "__main__":
    main()
