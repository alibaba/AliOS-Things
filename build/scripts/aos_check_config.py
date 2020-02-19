import os, sys
import re
from lib.comp import find_comp_mkfile
from lib.config import merge_config

DOT_CONFIG_FILE = ".config"
APP_CONFIG_FILE = "app.config"
BOARD_CONFIG_FILE = "board.config"

def merge_new_config_with_default(dirname, boardname, appname):
    """ merge config file(.config) with app.config and board.config if existed, 
    and remove app.config and board.config """
    if os.path.isdir(os.path.join(dirname, "core/rhino")) or os.path.isdir(os.path.join(dirname, "include/aos")):
        board_dir = os.path.join(dirname, "platform/board", boardname)
        if not os.path.isdir(board_dir):
            board_dir = os.path.join(dirname, "platform/board/board_legacy", boardname)
        board_config = os.path.join(board_dir, BOARD_CONFIG_FILE)
        appname = appname.replace(".", "/")
        app_dir = os.path.join(dirname, "application/example", appname)
        if not os.path.isdir(app_dir):
            app_dir = os.path.join(dirname, "application/example/example_legacy", appname)
        app_config = os.path.join(app_dir, APP_CONFIG_FILE)
        in_appdir = False
    else:
        board_config = os.path.join(dirname, "board", boardname, BOARD_CONFIG_FILE)
        app_config = os.path.join(dirname, APP_CONFIG_FILE)
        in_appdir = True

    config_file = os.path.join(dirname, DOT_CONFIG_FILE)
    if os.path.isfile(board_config):
        merge_config(config_file, board_config)
        if in_appdir:
            os.remove(board_config)
    if os.path.isfile(app_config):
        merge_config(config_file, app_config)
        if in_appdir:
            os.remove(app_config)
    

def check_appname(appname):
    appname = appname.replace(".", "/")
    mklist = []
    if os.path.isdir("kernel/rhino"):
        if os.path.isdir("app"):
            mklist += find_comp_mkfile("app")
        if os.path.isdir("test/develop"):
            mklist += find_comp_mkfile("test/develop")
    else:
        mklist += find_comp_mkfile("./")

    if mklist:
        for mkfile in mklist:
            appdir = os.path.dirname(os.path.abspath(mkfile))
            if appdir.endswith(appname):
                return appdir

    return False

def main():
    """ after get default configuraiton by running app@board.config, check the appname 
    and boardname between build string and .config file, and merge .config file with 
    app.board and board.config """
    if len(sys.argv) < 3:
        print ("Usage: %s <config> <build_string>" % sys.argv[0])
        return 1

    aos_config = sys.argv[1]
    build_string = sys.argv[2]

    print ("Checking config output: %s ..." % aos_config)
    if not os.path.isfile(aos_config):
        print ("[ERROR]: No such file %s" % aos_config)
        return 1

    if not "@" in build_string:
        print ("[ERROR]: The build string is invalid %s" % build_string)
        return 1

    targets = build_string.split("@")
    appname = targets[0]
    boardname = targets[1]

    patten = re.compile(r'(AOS_BUILD_BOARD|AOS_BUILD_APP)="(.*)"')
    aos_build_board = ""
    aos_build_app = ""
    with open (aos_config, "r") as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                if "AOS_BUILD_BOARD" == match.group(1):
                    aos_build_board = match.group(2)
                elif "AOS_BUILD_APP" == match.group(1):
                    aos_build_app = match.group(2)
                    break
                else:
                    pass

    if not aos_build_app:
        print ("[ERROR]: The app name from config is null, please check config file firstly!")
        return 1

    if appname != aos_build_app:
        print ("[ERROR]: App name Mismatched! (%s -> %s)" % (appname, aos_build_app))
        appdir = check_appname(appname)
        if not appdir:
            print ("- No such App '%s' found!" % appname)
        else:
            print ("- Please make sure config items for '%s' are correct!\n" % appname)
        return 1

    if boardname != aos_build_board:
        print ("[ERROR]: Board name Mismatched! (%s -> %s)" % (boardname, aos_build_board))
        print ("- Please make sure \"%s\" is a valid Board and config items for it are correct!\n" % boardname)
        return 1

    merge_new_config_with_default(os.path.dirname(aos_config), boardname, appname)
    return 0

if __name__ == "__main__":
    ret = main()
    sys.exit(ret)
