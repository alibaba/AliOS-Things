import os, sys
import re

def main():
    if len(sys.argv) < 2:
        print ("Usage: %s </path/to/autoconf.h>" % sys.argv[0])
        return 1

    autoconf_h = sys.argv[1]
    if not os.path.isfile(autoconf_h):
        print ("[ERROR]: No such file %s" % autoconf_h)
        return 1

    patten = re.compile(r"#define\s+(\w*)\s+(.*)")
    macro_list = []
    with open (autoconf_h, "r") as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                macro = match.group(1)
                value = re.sub('"', '\\"', match.group(2))
                macro_list += [ ("-D%s=%s" % (macro, value)) ]

    if macro_list:
        print (" ".join(macro_list))

if __name__ == "__main__":
    main()
