import os
import sys

try:
    from util import info, warn, error
except ImportError:
    from .util import info, warn, error

name = "flake8"
short_help = "Check python syntax with flake8"
requires = ["flake8"]
log_file = "check_%s.log" % name

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

    cmd = "flake8 --max-line-length=%s %s > %s 2>&1" % (MAX_LINE_LEN, " ".join(sys.argv[1:]), log_file)
    info("Running cmd: %s ..." % cmd)
    ret = os.system(cmd)
    if not ret:
        info("Check Python Syntax Passed!\n")
    else:
        os.system("cat %s" % log_file)
        error("Check Python Syntax Failed, details refer to: '%s'!\n" % log_file)

    return ret


if __name__ == "__main__":
    sys.exit(run())
