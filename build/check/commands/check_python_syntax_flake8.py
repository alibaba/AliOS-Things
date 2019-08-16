import os
import sys
from util import info, warn, error

name = "flake8"
short_help = "Check python syntax with flake8"
requires = ["flake8"]

help = """
Usage: aos check %s [python files ...]

Check python syntax with Flake8.
""" % name

# Flake8 check configs
MAX_LINE_LEN = 120


def run():
    """ Check Python Syntax with Flake8 """
    if "help" in sys.argv:
        warn(help)
        return 0

    cmd = "flake8 --max-line-length=%s %s > %s.log 2>&1" % (MAX_LINE_LEN, " ".join(sys.argv[1:]), name)
    info("Running cmd: %s ..." % cmd)
    ret = os.system(cmd)
    if not ret:
        info("Check Python Syntax Pass!")
    else:
        os.system("cat %s.log" % name)
        error("Check Python Syntax Failed")

    return ret


if __name__ == "__main__":
    sys.exit(run())
