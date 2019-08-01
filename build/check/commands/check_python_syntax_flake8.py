import os
import sys
from util import info, warn

name = "flake8"
short_help = "Check python syntax with flake8"
requires = ["flake9"]

help = """
Usage: aos check <ID> [python files ...]

Check python syntax with Flake8.
"""

## Flake8 check configs
MAX_LINE_LEN = 100


def run():
    """ Check Python Syntax with Flake8 """
    if "help" in sys.argv:
        warn(help)
        return 0

    cmd = "flake8 --max-line-length=%s %s" % (MAX_LINE_LEN, " ".join(sys.argv[1:]))
    info("Running cmd: %s ..." % cmd)
    ret = os.system(cmd)
    if not ret:
        info("Check Python Syntax Pass!")


if __name__ == "__main__":
    run()
