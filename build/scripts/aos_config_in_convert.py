import os, sys, re, io

def find_comp_mkfile(dirname):
    """ Find component makefile (aos.mk) from dirname and its subdirectory, 
    exclude out, build, publish folder """
    mklist = []
    for root, dirs, files in os.walk(dirname):
        tmp = (root + '/').replace("\\", "/")
        if '/out/' in tmp or '/build/' in tmp or '/publish/' in tmp:
            continue

        if 'aos.mk' in files:
            mklist += ["%s/aos.mk" % root]
            continue

    return mklist

def get_comp_name(mkfile):
    """ Get comp name from mkfile by searching lines started with NAME """
    name = None
    patten = re.compile(r'^NAME.*=\s*(.*)\s*')
    with open(mkfile, 'r') as f:
        for line in f.readlines():
            line = line.strip()
            match = patten.match(line)
            if match:
                 name = match.group(1)
                 break

    return name

def get_configin_file(comp_dir):
    """ Find Config.in in the specific directory """
    config_file = os.path.join(comp_dir, "Config.in").replace("\\", "/")
    if os.path.isfile(config_file):
        return config_file
    else:
        return ""

def from_y_n_to_0_1(from_y_n, type):
    if type == "bool":
        if from_y_n == "y":
            to_0_1 = "1"
        else:
            to_0_1 = ""
    elif type == "tristate":
        if from_y_n == "m":
            to_0_1 = "1"
        elif from_y_n == "y":
            to_0_1 = "2"
        else:
            to_0_1 = "0"
    else:
        to_0_1 = from_y_n
    return to_0_1

def parse_block_of_config(lines):
    """ parse a config block of Config.in file to get macro name, type, value """
    p1 = re.compile(r"(config|menuconfig)\s+(\w*)$")
    p2 = re.compile(r"(bool|int|string|hex|tristate)(\s+\"(.*)\")?")
    p3 = re.compile(r"default\s+(\w*)(\s+if(.*))?")
    p4 = re.compile(r"default\s+\"(.*)\"(\s+if(.*))?")
    p5 = re.compile(r"select\s+(\w*)(\s+if(.*))?")
    new_macro = {}
    for line in lines:
        if line.startswith("config") or line.startswith("menuconfig"):
            # config xxx, menuconfig xxx
            match = p1.match(line)
            if match:
                new_macro["name"] = match.group(2)
                # add default data type
                new_macro["type"] = "bool"
                new_macro["hint"] = ""
                new_macro["value"] = []
                new_macro["select"] = []
                new_macro["depends"] = ""
            else:
                return new_macro
        elif line.startswith("bool") or line.startswith("int") or line.startswith("string") \
        or line.startswith("hex") or line.startswith("tristate"):
        # bool, bool "description"
            match = p2.match(line)
            if match:
                new_macro["type"] = match.group(1)
                if match.group(3):
                    new_macro["hint"] = match.group(3)
        elif line.startswith("default"):
            # default xxx, default xxx if yyy, default xxx if !yyy
            default_val = {}
            if new_macro["type"] == "string":
                match = p4.match(line)
                if match:
                    default_val["val"] = "\"" + match.group(1) + "\""
                    default_val["cond"] = ""
                    if match.group(3):
                        default_val["cond"] = match.group(3)
            else:
                match = p3.match(line)
                if match:
                    val = from_y_n_to_0_1(match.group(1), new_macro["type"])
                    if new_macro["type"] == "bool":
                        if new_macro["name"].startswith("AOS_COMP_") \
                        or new_macro["name"].startswith("AOS_ARCH_") \
                        or new_macro["name"].startswith("AOS_MCU_") \
                        or new_macro["name"].startswith("AOS_BOARD_") \
                        or new_macro["name"].startswith("AOS_APP_"):
                        # it will be put into aos.mk
                            val = "y"
                    default_val["val"] = val
                    default_val["cond"] = ""
                    if match.group(3):
                        default_val["cond"] = match.group(3)
            if default_val:
                new_macro["value"].append(default_val)
        elif line.startswith("depends on"):
            # depends on xxx, or depends on xxx || yyy 
            new_macro["depends"] = line.replace("depends on", "")
        elif line.startswith("select"):
            # select xxx, select xxx if yyy, select xxx if !yyy
            match = p5.match(line)
            if match:
                select_comp = {}
                select_comp["comp"] = match.group(1).replace("AOS_COMP_", "").lower()
                select_comp["cond"] = ""
                if match.group(3):
                    select_comp["cond"] = match.group(3)
                new_macro["select"].append(select_comp)

    return new_macro

def parse_block_of_choice(lines):
    """ parse a choice block of Config.in file to get macro name """
    p1 = re.compile(r"prompt\s+\"(.*)\"")
    p2 = re.compile(r"default\s+(\w*)")
    new_macro = {}
    for line in lines:
        if line.startswith("choice"):
            new_macro["name"] = ""
            new_macro["type"] = "bool"
            new_macro["hint"] = ""
            new_macro["value"] = []
            default_val = {}
            default_val["val"] = "1"
            default_val["cond"] = ""
            new_macro["value"].append(default_val)
            new_macro["select"] = []
            new_macro["depends"] = ""
        elif line.startswith("prompt"):
            match = p1.match(line)
            if match:
                new_macro["hint"] = match.group(1)
        elif line.startswith("default"):
            match = p2.match(line)
            if match:
                new_macro["name"] = match.group(1)
    return new_macro

def append_a_block_to_header(fn, lines):
    macro = {}
    if lines[0].startswith("choice"):
        macro = parse_block_of_choice(lines)
    else:
        macro = parse_block_of_config(lines)
    if macro:
        if macro["hint"]:
            fn.write("// description:%s\n" % macro["hint"])
        if macro["depends"]:
            fn.write("#if %s\n" % macro["depends"])

        if macro["name"]:
            fn.write("#ifndef %s\n" % macro["name"])
            if macro["value"]:
                for default_val in macro["value"]:
                    if default_val["cond"]:
                        fn.write("#if %s\n" % default_val["cond"])
                        if default_val["val"]:
                            fn.write("#define %s %s\n" % (macro["name"], default_val["val"]))
                        else:
                            fn.write("// #define %s 1\n" % macro["name"])
                        fn.write("#endif\n")
                    else:
                        if default_val["val"]:
                            fn.write("#define %s %s\n" % (macro["name"], default_val["val"]))
                        else:
                            fn.write("// #define %s 1\n" % macro["name"])
            else:
                fn.write("// #define %s 1\n" % macro["name"])
            fn.write("#endif\n")

        if macro["depends"]:
            fn.write("#endif\n")

        if macro["select"]:
            fn.write("#ifdef %s\n" % macro["name"])
            for feature in macro["select"]:
                if feature["cond"]:
                    fn.write("#if %s\n" % feature["cond"])
                    fn.write("#define %s 1\n" % feature["comp"])
                    fn.write("#endif\n")
                else:
                    fn.write("#define %s 1\n" % feature["comp"])
            fn.write("#endif\n")

def append_a_block_to_mkfile(mkfile, lines):
    macro = {}
    macro = parse_block_of_config(lines)
    if macro:
        fn = open (mkfile, 'a')
        if macro["hint"]:
            fn.write("\n# description:%s\n" % macro["hint"])
        if macro["depends"]:
            # TODO: if has multiply depends, like "depends on xxx || yyy"
            macro["depends"] = macro["depends"].strip()
            if macro["depends"].startswith("!"):
                fn.write("ifeq ($(%s),)\n" % macro["depends"])
            else:
                fn.write("ifneq ($(%s),)\n" % macro["depends"])

        if macro["name"]:
            if macro["value"]:
                for default_val in macro["value"]:
                    if default_val["cond"]:
                        default_val["cond"] = default_val["cond"].strip()
                        if default_val["cond"].startswith("!"):
                            fn.write("ifeq ($(%s),)\n" % default_val["cond"])
                        else:
                            fn.write("ifneq ($(%s),)\n" % default_val["cond"])
                        fn.write("%s ?= %s\n" % (macro["name"], default_val["val"]))
                        fn.write("endif\n")
                    else:
                        fn.write("%s ?= %s\n" % (macro["name"], default_val["val"]))
            else:
                fn.write("%s ?= \n" % macro["name"])

        if macro["depends"]:
            fn.write("endif\n")

        fn.write("ifneq ($(%s),)\n" % macro["name"])
        fn.write("GLOBAL_DEFINES  += %s=1\n" % macro["name"])

        if macro["select"]:
            for feature in macro["select"]:
                if feature["cond"]:
                    feature["cond"] = feature["cond"].strip()
                    if feature["cond"].startswith("!"):
                        fn.write("ifeq ($(%s),)\n" % feature["cond"])
                    else:
                        fn.write("ifneq ($(%s),)\n" % feature["cond"])
                    fn.write("$(NAME)_COMPONENTS += %s\n" % feature["comp"])
                    fn.write("endif\n")
                else:
                    fn.write("$(NAME)_COMPONENTS += %s\n" % feature["comp"])

        fn.write("endif\n")
        
        fn.close()

def convert_configin_to_header(mkfile, config_in_file, comp_name, destdir):
    """ read Config.in file, and convert to C header file and mkfile """
    if not os.path.isfile(mkfile):
        return False
    if not os.path.isfile(config_in_file):
        return False
    if not os.path.isdir(destdir):
        return False
    if not comp_name:
        return False

    filename = os.path.join(destdir, "%s_config.h" % comp_name)
    fn = open (filename, 'w+')
    COMP_NAME = comp_name.upper()
    fn.write("#ifndef %s_CONFIG_H\n" % COMP_NAME)
    fn.write("#define %s_CONFIG_H\n" % COMP_NAME)

    with io.open(config_in_file, "r", encoding="utf8") as f:
        lines = []
        # new_block means multiply lines of text for one config
        new_block = False
        select_comp = False
        p1 = re.compile(r"if (.*)=\s*(y|n)")
        p2 = re.compile(r"select\s+AOS_COMP_\w+")
        p3 = re.compile(r"(menu)?config\s+AOS_(COMP|ARCH|MCU|BOARD|APP)_\w+")
        for line in f.readlines():
            line = line.strip()
            if line:
                if line.startswith("if ") or line.startswith("endif") \
                or line.startswith("choice") or line.startswith("endchoice") \
                or line.startswith("config ") or line.startswith("menuconfig "):
                    # last block is completed, convert it to header file format
                    if new_block:
                        if select_comp:
                            append_a_block_to_mkfile(mkfile, lines)
                        else:
                            append_a_block_to_header(fn, lines)
                    new_block = False
                    select_comp = False
                    lines = []
                if line.startswith("if "):
                    # if xxx, if !xxx, if xxx=y, if xxx=n
                    match = p1.match(line)
                    if match:
                        if match.group(2) == "y":
                            fn.write("\n#if " + match.group(1) + "\n")
                        else:
                            fn.write("\n#if !" + match.group(1) + "\n")
                    else:
                        fn.write("\n#" + line + "\n")
                elif line.startswith("endif"):
                    fn.write("#" + line + "\n")
                elif line.startswith("choice"):
                    new_block = True
                    fn.write("\n// " + line + "\n")
                elif line.startswith("endchoice"):
                    fn.write("// " + line + "\n\n")
                elif line.startswith("config ") or line.startswith("menuconfig "):
                    new_block = True

                if new_block:
                    lines.append(line)
                    if line.startswith("select "):
                        match = p2.match(line)
                        if match:
                            select_comp = True
                    elif line.startswith("config ") or line.startswith("menuconfig "):
                        match = p3.match(line)
                        if match:
                            select_comp = True

        # last block
        if new_block:
            if select_comp:
                append_a_block_to_mkfile(mkfile, lines)
            else:
                append_a_block_to_header(fn, lines)

    fn.write("#endif\n\n")
    fn.close()

def main():
    if len(sys.argv) < 2:
        print("Usage: %s APPDIR" % sys.argv[0])
        return 1

    app_dir = sys.argv[1]
    if not os.path.isdir(app_dir):
        print("Directory %s is not exists" % app_dir)
        return 1

    mklist = find_comp_mkfile(app_dir)
    for mkfile in mklist:
        comp_dir = os.path.dirname(mkfile)
        config_file = get_configin_file(comp_dir)
        comp_name = get_comp_name(mkfile)
        convert_configin_to_header(mkfile, config_file, comp_name, comp_dir)

if __name__ == "__main__":
    main()