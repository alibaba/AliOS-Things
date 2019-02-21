import os, sys
import re

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

    if appname != aos_build_app:
        print ("[ERROR]: App name Missmatched! (%s -> %s)" % (appname, aos_build_app))
        print ("- Please make sure \"%s\" is a valid App and config items for it are correct!\n" % appname)
        return 1

    if boardname != aos_build_board:
        print ("[ERROR]: Board name Missmatched! (%s -> %s)" % (boardname, aos_build_board))
        print ("- Please make sure \"%s\" is a valid Board and config items for it are correct!\n" % boardname)
        return 1

if __name__ == "__main__":
    ret = main()
    sys.exit(ret)
