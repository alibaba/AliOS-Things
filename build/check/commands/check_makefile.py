# coding: utf-8
import os
import sys
from util import info, warn, error, check_win_format, check_extra_blank, check_invalid_chars

name = "aosmk"
short_help = "Check for aos.mk"
log_file = "check_%s.log" % name

help = """
Usage: aos check %s [source files ...]

Check component's makefile aos.mk.
""" % name

SKIP_CHECK = {
    "global_includes": ["/board/", "/platform/", "/2ndboot/", "/kernel/",
                        "/security/", "/3rdparty/", "/utility/", "/tools/",
                        "/test/"
                        ],
}


def find_comp_mkfile(dirname):
    """ Find component makefiles from dirname """
    mklist = []
    for root, dirs, files in os.walk(dirname):
        if 'out' in root or 'build' in root or 'publish' in root:
            continue

        if 'aos.mk' in files:
            mklist += ["%s/aos.mk" % root]
            continue

    return mklist


def check_global_include(mkfile):
    """ Check if GLOBAL_INCLUDES defined """
    if "global_includes" in SKIP_CHECK:
        mkfile_abspath = os.path.abspath(mkfile)
        for item in SKIP_CHECK["global_includes"]:
            if item in mkfile_abspath:
                print("Skipp check GLOBAL_INCLUDES for %s" % mkfile)
                return []

    matchlist = ["^GLOBAL_INCLUDES"]
    return check_invalid_chars(mkfile, matchlist)


def run():
    """ Check for aos.mk """
    if "help" in sys.argv:
        warn(help)
        return 0

    info("Checking Component's Makefile ...")
    check_files = ["./"]
    if len(sys.argv) > 1:
        check_files = sys.argv[1:]

    mkfiles = []
    for item in check_files:
        if os.path.isfile(item):
            mkfiles.append(item)
        elif os.path.isdir(item):
            mkfiles += find_comp_mkfile(item)
        else:
            error("No such file or directory: %s" % item)

    result = {}
    for mkfile in mkfiles:
        tmp = {}
        if os.access(mkfile, os.X_OK):
            tmp["is_executable"] = "Fix it with: 'chmod -x %s'" % mkfile
        if check_win_format(mkfile):
            tmp["is_win_format"] = "Fix it with: 'dos2unix %s'" % mkfile

        ret = check_global_include(mkfile)
        if ret:
            tmp["has_global_includes"] = "Line: " + ",".join(ret)

        ret = check_extra_blank(mkfile)
        if ret:
            tmp["has_extra_blank"] = "Line: " + ",".join(ret)

        if tmp:
            result[mkfile] = tmp

    if result:
        with open(log_file, "w") as f:
            index = 0
            for key in result:
                f.write("[%d] %s:\n" % (index, key))
                for issue in result[key]:
                    f.write("  %s: %s\n" % (issue.upper(), result[key][issue]))
                index += 1
        os.system("cat %s" % log_file)
        error("Check Component's Makefile Failed, details refer to: '%s'!\n" % log_file)
    else:
        info("Check Component's Makefile Passed!\n")


if __name__ == '__main__':
    sys.exit(run())
