import os, sys
import re
from aos_parse_components import find_comp_mkfile

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
            print ("- Please config items for it are correct!\n")
        return 1

    if boardname != aos_build_board:
        print ("[ERROR]: Board name Mismatched! (%s -> %s)" % (boardname, aos_build_board))
        print ("- Please make sure \"%s\" is a valid Board and config items for it are correct!\n" % boardname)
        return 1

if __name__ == "__main__":
    ret = main()
    sys.exit(ret)
