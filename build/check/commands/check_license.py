# coding: utf-8
import os, sys, subprocess
from util import info, warn

name = "license"
short_help = "Check for 3rd Licenses"

help = """
Usage: aos check %s [source files ...]

Check python syntax with Flake8.
""" % name

black_list = [
    "GPL",
    "GPL2.0",
    "GENERAL PUBLIC LICENSE",
    "AGPL",
    "AFFERO"
    "Affero General Public License",
    "CPAL",
    "COMMON PUBLIC ATTRIBUTION LICENSE",
    "OSL",
    "OPEN SOFTWARE LICENSE",
    "LGPL",
]

def run():
    """ Check for 3rd Licenses """
    if "help" in sys.argv:
        warn(help)
        return 0

    check_files = ["./"]
    if len(sys.argv) > 1:
        check_files = sys.argv[1:]

    cmd = 'grep -w -E -i -r "%s" %s' % ("|".join(black_list), " ".join(check_files))
    info("Running cmd: %s ..." % cmd)
    ret = subprocess.call(cmd, shell=True)
    if not ret:
        info("Check License Completed!")


if __name__ == '__main__':
    run()
