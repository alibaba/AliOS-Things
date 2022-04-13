import os, sys
import re

def main():
    if len(sys.argv) < 3:
        print ("Usage: %s </path/to/config.mk> </path/to/config.sh>" % sys.argv[0])
        return 1

    config_mk = sys.argv[1]
    config_sh = sys.argv[2]
    if not os.path.isfile(config_mk):
        print ("[ERROR]: No such file %s" % config_mk)
        return 1

    config_sh_text = ""
    pattern = re.compile(r"(.*)\s*[+:]=\s*(.*)")
    with open (config_mk, "r") as f:
        for line in f.readlines():
            match = pattern.search(line)
            if match:
                newline = match.group(1).strip().replace("-", "_")
                newline += "=\"" + match.group(2).strip() + "\""
                config_sh_text += newline + "\n"
    with open(config_sh, "w+") as f:
        f.write(config_sh_text)             

if __name__ == "__main__":
    main()
