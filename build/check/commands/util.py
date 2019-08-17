import click
import platform
import os
import sys
import subprocess


def info(message):
    """ Show info message """
    click.secho(message, fg="cyan")


def warn(message):
    """ Show warning message """
    click.secho(message, fg="yellow")


def error(message, code=1):
    """ Show warning message """
    click.secho(message, fg="red")
    sys.exit(code)


def which(program):
    if platform.system() == 'Windows' and not program.endswith('.exe'):
        program += '.exe'

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None


def check_win_format(filename):
    """ Check if the file has Windows CRLF line terminators """
    ret = False
    try:
        command = "file %s" % filename
        output = subprocess.check_output(command, shell=True)
        if "with CRLF line terminators" in output:
            ret = True
    except Exception as e:
        print(e)
        pass

    return ret


def check_invalid_chars(filename, matchlist):
    ret = []
    command = 'grep -w -E -i -r -n "%s" %s' % ("|".join(matchlist), filename)
    try:
        output = subprocess.check_output(command, shell=True)
        for line in output.splitlines():
            ret.append(line.split(":")[0])
    except Exception:
        pass

    return ret


def check_extra_blank(filename):
    """ Check if there's extra blank spaces in file """
    matchlist = ["^\\s+$", "\\s+$"]
    return check_invalid_chars(filename, matchlist)
