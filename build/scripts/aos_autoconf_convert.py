import os, sys
import re
from lib.code import compute_aos_config_md5sum, write_project_config, get_md5sum
from lib.comp import from_0_1_to_y_n
from lib.config import merge_config

CONFIG_BAK_PATH = ".important.bak"
CONFIG_CHECKSUM_FILE = "aos_config_md5.txt"
DOT_AOS = ".aos"
DOT_CONFIG_FILE = ".config"
APP_CONFIG_FILE = "app.config"
TEMP_CONFIG_FILE = "temp.config"

USER_DEFINED_KEYWORD = "KEYWORD: PART1 USER DEFINED CONFIGURATION"
USER_DEFINED_NOTE = "/* Put user defined macro here */\n"

COMPONENTS_CONFIG_KEYWORD = "KEYWORD: PART2 COMPONENTS CONFIGURATION"
COMPONENTS_CONFIG_NOTE = "/* Put configuration defined in Config.in files of components here.\n" \
                        " It's better to modify the configuration by menuconfig, and the \n"    \
                        " compiler will copy it here automatically.*/\n"

COMPONENT_KEYWORD = "KEYWORD: COMPONENT NAME IS "

def print_usage():
    print ("Usage: %s <option> <source file> [destination file]" % sys.argv[0])
    print ("Merge autoconf.h to aos_config.h:       %s 1 </path/to/autoconf.h> </path/to/aos_config.h>" % sys.argv[0])
    print ("Convert aos_config.h to .config:        %s 2 </path/to/aos_config.h> </path/to/.config>" % sys.argv[0])
    print ("Convert aos_config.h to IAR ASM format: %s 3 </path/to/aos_config.h>" % sys.argv[0])

def append_default_header_files(fhandle, header_files_dir):
    """append header files in backup folder into aos_config.h """
    for root, dirs, files in os.walk(header_files_dir):
        for filename in files:
            if not filename.startswith("comp_"):
                continue
            tempfile = "%s/%s" % (root, filename)
            tempfile = tempfile.replace("\\", "/")
            with open (tempfile, 'r') as f:
                for line in f.readlines():
                    # delete the type
                    pos = line.find("// type:")
                    line = line[:pos] + "\n"
                    fhandle.write(line)


def get_macro_type(name, header_files_dir):
    """search "name" in header_files_dir/comp_*.h files, and get type """
    name = " " + name + " "
    pattern = re.compile(r"\s+type:\s+(\w*)")
    macro_type = ""
    for root, dirs, files in os.walk(header_files_dir):
        for filename in files:
            if not filename.startswith("comp_"):
                continue
            tempfile = "%s/%s" % (root, filename)
            tempfile = tempfile.replace("\\", "/")
            with open (tempfile, 'r') as f:
                for line in f.readlines():
                    if name in line:
                        match = pattern.search(line)
                        if match:
                            macro_type = match.group(1)
                            return macro_type
    return macro_type


def update_items_into_aosconfig(text_autoconf, aosconfig_h):
    lines_autoconf = text_autoconf.split("\n")
    with open(aosconfig_h, "r") as f:
        text_aosconfig = f.read()
    
    patten = re.compile(r"#define\s+(\w*)\s+(.*)")
    for line in lines_autoconf: 
        if line.startswith("#define"):
            match = patten.match(line)
            if match:
                macro = match.group(1)
                p = "// #define %s .*" % macro
                text_aosconfig, count = re.subn(p, line, text_aosconfig, 1)
                if count == 0:
                    text_aosconfig += line + "\n"
    with open(aosconfig_h, "w") as f:
        f.write(text_aosconfig)


def merge_autoconf_aosconfig(autoconf_h, aosconfig_h):
    """ copy autoconf.h into aos_config.h, but keep user-defined marco in aos_config.h
    TODO: append components' defaut configuration for user reference """
    if not os.path.isfile(autoconf_h):
        print ("[ERROR]: No such file %s" % autoconf_h)
        return 1

    root_dir = os.path.dirname(aosconfig_h)
    with open (autoconf_h, "r") as f:
        text_autoconf = f.read()
        pattern = "\/\*[\s\S]+(Automatically generated file; DO NOT EDIT)[\s\S]+?\*\/"
        text_autoconf= re.sub(pattern, "", text_autoconf, 1)

    text_aosconfig = ""
    if os.path.isfile(aosconfig_h):
        with open (aosconfig_h, "r") as f:
            text_aosconfig = f.read()
    
    pos = text_aosconfig.find(COMPONENTS_CONFIG_KEYWORD)
    if pos >= 0:
        # delete the old components config
        pos1 = text_aosconfig.find("//================This is split line================\n", pos)
        if pos1 > 0:
            pos = pos1
        else:
            pos = text_aosconfig.find("\n#define", pos)
            if pos > 0:
                # skip \n
                pos += 1
        text_aosconfig = text_aosconfig[:pos]
    else:
        # first time to generate aos_config.h
        text_aosconfig = "/* %s */\n" % USER_DEFINED_KEYWORD + USER_DEFINED_NOTE + "\n\n\n\n"
        text_aosconfig += "/* %s */\n" % COMPONENTS_CONFIG_KEYWORD + COMPONENTS_CONFIG_NOTE

    # copy components configuration
    with open (aosconfig_h, "w+") as f:
        f.write(text_aosconfig)
        append_default_header_files(f, os.path.join(root_dir, CONFIG_BAK_PATH))

    # update item of autoconf.h into aos_config.h
    update_items_into_aosconfig(text_autoconf, aosconfig_h)

    # store md5 checksum if in appdir which has .aos
    if os.path.isfile(os.path.join(root_dir, DOT_AOS)):
        define_md5sum = compute_aos_config_md5sum(aosconfig_h)
        config_data = {
            "MD5SUM_HEADER": define_md5sum,
        }
        write_project_config(os.path.join(root_dir, CONFIG_BAK_PATH, CONFIG_CHECKSUM_FILE), config_data)

def convert_aosconfig_config(aosconfig_h, config_file):
    """ convert components configuration porting into .config """
    if not os.path.isfile(aosconfig_h):
        print ("[WARNING]: No such file %s" % aosconfig_h)
        return 1

    root_dir = os.path.dirname(aosconfig_h)
    backup_dir = os.path.join(root_dir, CONFIG_BAK_PATH)
    old_md5sum = get_md5sum(os.path.join(backup_dir, CONFIG_CHECKSUM_FILE))
    define_md5sum = compute_aos_config_md5sum(aosconfig_h)
    if old_md5sum == define_md5sum:
        return 1

    print("[INFO]: converting aos_config.h to .config ...")
    patten = re.compile(r"#define\s+(\w*)\s+(.*)")
    macro_list = []
    with open (aosconfig_h, "r") as f:
        started = False
        for line in f.readlines():
            if line.find(COMPONENTS_CONFIG_KEYWORD) >= 0:
                started = True
            if started:
                if line.startswith("#define"):
                    match = patten.match(line)
                    if match:
                        macro = match.group(1)
                        value = match.group(2)
                        if not value.startswith("\""):
                            value = value.split()[0]
                        if value == "0" or value == "1" or value == "2":
                            macro_type = get_macro_type(macro, backup_dir)
                            value = from_0_1_to_y_n(value, macro_type)
                        macro_list += [ ("%s=%s" % (macro, value)) ]

    if macro_list:
        temp_config_file = os.path.join(root_dir, TEMP_CONFIG_FILE)
        with open (temp_config_file, "w+") as f:
            for macro in macro_list:
                f.write("%s\n" % macro)
        merge_config(config_file, temp_config_file)
        os.remove(temp_config_file)
    return 0

def convert_aosconfig_asm(aosconfig_h):
    """ convert aos_config.h to IAR asm format """
    if not os.path.isfile(aosconfig_h):
        print ("[ERROR]: No such file %s" % aosconfig_h)
        return 1

    patten = re.compile(r"#define\s+(\w*)\s+(.*)")
    macro_list = []
    with open (aosconfig_h, "r") as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                macro = match.group(1)
                value = re.sub('"', '\\"', match.group(2))
                macro_list += [ ("-D%s=%s" % (macro, value)) ]

    if macro_list:
        print (" ".join(macro_list))

def main():
    if len(sys.argv) < 2:
        print_usage()
        return 1
    option = int(sys.argv[1])
    if option == 1:
        if len(sys.argv) < 4:
            print_usage()
            return 1
        else:
            merge_autoconf_aosconfig(sys.argv[2], sys.argv[3])
    elif option == 2:
        if len(sys.argv) < 4:
            print_usage()
            return 1
        else:
            convert_aosconfig_config(sys.argv[2], sys.argv[3])
    elif option == 3:
        if len(sys.argv) < 3:
            print_usage()
            return 1
        else:
            convert_aosconfig_asm(sys.argv[2])
    else:
        print_usage()
        return 1


if __name__ == "__main__":
    main()
