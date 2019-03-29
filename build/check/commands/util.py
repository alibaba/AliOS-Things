import click
import platform
import os
import sys


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
