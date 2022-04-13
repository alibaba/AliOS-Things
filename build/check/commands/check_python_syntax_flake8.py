import os
import sys
from util import info, warn


# Global Configs
MAX_LINE_LEN = 100
HELP = """
Usage: aos check <ID> [python files ...]

Check python syntax with Flake8.
"""


def main():
    """ Check Python Syntax with Flake8 """

    if "help" in sys.argv:
        warn(HELP)
        return 0

    cmd = "flake8 --max-line-length=%s %s" % (MAX_LINE_LEN, " ".join(sys.argv[1:]))
    info("Running cmd: %s ..." % cmd)
    ret = os.system(cmd)
    if not ret:
        info("Check Pass!")


if __name__ == "__main__":
    main()
