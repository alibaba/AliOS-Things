import os
import sys
import platform
import subprocess
import contextlib

# Directory navigation
@contextlib.contextmanager
def _cd(newdir):
    prevdir = os.getcwd()
    os.chdir(newdir)
    try:
        yield
    finally:
        os.chdir(prevdir)

def _get_host_os():
    host_os = platform.system()
    if host_os == 'Windows':
        host_os = 'Win32'
    elif host_os == 'Linux':
        if platform.machine().endswith('64'):
            bit = '64'
        else:
            bit = '32'
        host_os += bit
    elif host_os == 'Darwin':
        host_os = 'OSX'
    else:
        host_os = None
    return host_os

def popen_subprocess(command, suppress_error=None, stdin=None, **kwargs):
    """ excute command by calling subprocess.Popen, and block it until 
    subprocess return """
    # fix error strings
    try:
        command_line = command.split()
    except:
        command_line = command

    try:
        proc = subprocess.Popen(command, **kwargs)
    except OSError as e:
        if e[0] == errno.ENOENT:
            error(
                "Could not execute \"%s\".\n"
                "Please verify that it's installed and accessible from your current path by executing \"%s\".\n" % (
                command_line[0], command_line[0]), e[0])
        else:
            raise e

    if proc.wait() != 0:
        if not suppress_error:
            raise ProcessException(proc.returncode, command_line[0], ' '.join(command_line), os.getcwd())

    return proc.returncode

def gen_kconfig(appdir, appname, board):
    """ call external process to run makefile and generate .config and aos_config.h:
    1. aos make app@board.config to generate default configuration .config, 
      and update .config with app.config and board.config
    2. aos make silentoldconfig to generate autoconf.h, and merge to aos_config.h """
    if not appdir or not os.path.isdir(appdir):
        print("Not a valid APP directory: %s" % appdir)
        return 1

    if not appname or not board:
        print("Invalid app name(%s) or board name(%s)" % (appname, board))
        return 1

    if "AOS_SDK_PATH" not in os.environ:
        print("No AliOS SDK installed! Please install it and specifify "
              "its root path in AOS_SDK_PATH environment variable.")
        return 1
    else:
        aos_sdk_path = os.environ["AOS_SDK_PATH"]

    if not os.path.isdir(aos_sdk_path):
        print("Can't access AOS_SDK_PATH, no such directory: %s" % aos_sdk_path)
        return 1

    host_os = _get_host_os()

    make_cmds = {
        'Win32': 'cmd/win32/make.exe',
        'Linux64': 'cmd/linux64/make',
        'Linux32': 'cmd/linux32/make',
        'OSX': 'cmd/osx/make'
        }

    if not host_os or host_os not in make_cmds.keys():
        print("Invalid host OS!")
        return 1

    tools_dir = os.path.join(aos_sdk_path, 'build').replace('\\', '/')
    make_cmd = os.path.join(tools_dir, make_cmds[host_os])

    # Run make commands
    outdir = os.path.join(appdir, "out").replace(os.path.sep, "/")
    make_cmd_str = ' '.join([make_cmd,
                             '-e -f %s/build/Makefile' % aos_sdk_path,
                             'HOST_OS=' + host_os,
                             'TOOLS_ROOT=' + tools_dir,
                             'SOURCE_ROOT=' + aos_sdk_path,
                             'APPDIR=' + appdir,
                             'BUILD_DIR=' + outdir])
    # print("make_cmd_str: %s" % make_cmd_str)

    with _cd(aos_sdk_path):
        popen_subprocess(make_cmd_str + " %s@%s.config" % (appname, board), 
            shell=True, cwd=os.getcwd())
        # print(".config generated for %s@%s" % (appname, board))

        popen_subprocess(make_cmd_str + " silentoldconfig", shell=True, 
            cwd=os.getcwd())
        # print("aos_config.h generated for %s@%s" % (appname, board))


def main():
    if len(sys.argv) < 4:
        print("Usage: %s <app_dir> <appname> <board>" % sys.argv[0])
        return 1

    appdir = sys.argv[1]
    appname = sys.argv[2]
    boardname = sys.argv[3]

    gen_kconfig(appdir, appname, boardname)

if __name__ == "__main__":
    main()
