# coding: utf-8
import os
import sys
from util import info, warn, error

name = "license"
short_help = "Check for 3rd Licenses"
log_file = "check_%s.log" % name

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

    cmd = 'grep -w -E -i -r --color "%s" %s > %s 2>&1' % ("|".join(black_list), " ".join(check_files), log_file)
    info("Running cmd: %s ..." % cmd)
    ret = os.system(cmd)

    if ret == 0:
        os.system("cat %s" % log_file)
        error("Check 3rd License Failed!")
    elif ret == 256:
        info("Check 3rd License Passed!")
    elif ret == 512:
        os.system("cat %s" % log_file)
        error("Run cmd error: %s" % cmd)


if __name__ == '__main__':
    sys.exit(run())
