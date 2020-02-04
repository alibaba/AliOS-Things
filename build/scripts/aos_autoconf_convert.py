import os, sys
import re

def print_usage():
    print ("Usage: %s <option> <source file> [destination file]" % sys.argv[0])
    print ("Merge autoconf.h to aos_config.h:       %s 1 </path/to/autoconf.h> </path/to/aos_config.h>" % sys.argv[0])
    print ("Convert aos_config.h to .config:        %s 2 </path/to/aos_config.h> </path/to/.config>" % sys.argv[0])
    print ("Convert aos_config.h to IAR ASM format: %s 3 </path/to/aos_config.h>" % sys.argv[0])

def merge_autoconf_aosconfig(autoconf_h, aosconfig_h):
    """ copy autoconf.h into aos_config.h, but keep user-defined marco in aos_config.h
    TODO: append components' defaut configuration for user reference """
    if not os.path.isfile(autoconf_h):
        print ("[ERROR]: No such file %s" % autoconf_h)
        return 1

    with open (autoconf_h, "r") as f:
        text_autoconf = f.read()
        pattern = "\/\*[\s\S]+(Automatically generated file; DO NOT EDIT)[\s\S]+?\*\/"
        text_autoconf= re.sub(pattern, "", text_autoconf, 1)

    text_aosconfig = ""
    if os.path.isfile(aosconfig_h):
        with open (aosconfig_h, "r") as f:
            text_aosconfig = f.read()
    
    keyword_autoconf = "/* The following is copied from the autoconf.h file */\n"
    pos = text_aosconfig.find(keyword_autoconf)
    if pos > 0:
        text_aosconfig = text_aosconfig[:pos]
    else:
        text_aosconfig = "/* Note: put user defined macro here! */\n\n\n\n"
    text_aosconfig += keyword_autoconf
    text_aosconfig += "/* You can update the value if you know the meanings exactly, \n  and you can also copy system and component defined macro here! */\n"
    text_aosconfig += text_autoconf
    with open (aosconfig_h, "w+") as f:
        f.write(text_aosconfig)

def convert_aosconfig_config(aosconfig_h, config_file):
    """ convert components configuration porting into .config.
    TODO: to be implemented """
    pass

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
