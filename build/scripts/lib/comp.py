import os, sys
import re

CONFIG_BAK_PATH = ".important.bak"
AOS_MAKEFILE = "aos.mk"
COMPONENT_KEYWORD = "KEYWORD: COMPONENT NAME IS "

def find_comp_mkfile(dirname):
    """ Find component makefile (aos.mk) from dirname and its subdirectory, 
    exclude out, build, publish folder """
    mklist = []
    for root, dirs, files in os.walk(dirname):
        tmp = root + '/'
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
            match = patten.match(line)
            if match:
                 name = match.group(1)

    return name

def update_comp_optional_deps(comp_names, comp_cond, mandatory_deps, optional_deps):
    """ put the component and condition into optional_deps, if it is not in the mandatory_deps,
    subroutine for process $(NAME)_COMPONENTS-$(comp_cond) += compa compb compc,
    comp_names is the list of [compa compb compc] """
    for comp in comp_names:
        if comp not in mandatory_deps:
            existed = False
            if len(optional_deps) > 0:
                for dep in optional_deps:
                    if comp == dep["comp_name"]:
                        existed = True
                        if comp_cond[0] not in dep["condition"]:
                            dep["condition"].append(comp_cond[0])
            if not existed:
                dep_info = {}
                dep_info["comp_name"] = comp
                dep_info["condition"] = comp_cond
                optional_deps.append(dep_info)


def get_comp_deps(mkfile):
    """ Get component's mandatory and optional dependencies from aos.mk
    by searching $(NAME)_COMPONENTS and $(NAME)_COMPONENTS-$( """
    abs_mkfile = os.path.abspath(mkfile)
    mandatory_deps = []
    optional_deps = []
    host_mcu_family = ""

    p1 = re.compile(r'^HOST_MCU_FAMILY.*=\s*(.*)\s*')
    p2 = re.compile(r'^\$\(NAME\)_COMPONENTS.*=\s*(.*)\s*')
    p3 = re.compile(r'^\$\(NAME\)_COMPONENTS-\$\((.*)\)')
    with open(mkfile, "r") as f:
        for line in f:
            line = line.strip()

            if not line or "#" in line:
                continue

            while line.endswith('\\'):
                line = line[:-1] + next(f).rstrip('\n')

            if line.startswith("HOST_MCU_FAMILY"):
                match = p1.match(line)
                if match:
                    host_mcu_family = match.group(1)

            if line.startswith("$(NAME)_COMPONENTS") and not line.startswith("$(NAME)_COMPONENTS-n"):
                match = p2.match(line)
                if match:
                    orig_comp_names = match.group(1).split()
                    tmp = " ".join(orig_comp_names)
                    if host_mcu_family:
                        tmp = tmp.replace("$(HOST_MCU_FAMILY)", host_mcu_family)
                    comp_names = tmp.split()
                    if line.startswith("$(NAME)_COMPONENTS-$("):
                        match = p3.match(line)
                        if match:
                            comp_cond = (match.group(1).split())
                            update_comp_optional_deps(comp_names, comp_cond, mandatory_deps, optional_deps)
                    else:
                        mandatory_deps += comp_names

    mandatory_deps = list(set(mandatory_deps))

    return mandatory_deps, optional_deps

def get_comp_mandatory_depends(comp_info, comps):
    """ Get comp mandatory depends from comp index """
    depends = []
    for comp in comps:
        if comp in comp_info:
            depends += comp_info[comp]["dependencies"]

    if depends:
        depends += get_comp_mandatory_depends(comp_info, depends)

    return list(set(depends))

def get_comp_optional_depends_r(comp_info, comps, mandatory_comps):
    """ Get comp optional depends recursively from comp index """
    depends = []
    """ comps are optional dependency list from last layer """
    for comp in comps:
        # print("comp name is:", comp["comp_name"])
        if comp["comp_name"] not in comp_info:
            continue
        """ get mandatory dependency list for this optional component """
        for dep_info in comp_info[comp["comp_name"]]["dependencies"]:
            if dep_info not in mandatory_comps:
                """ add to the list with the inherrited dependency"""
                tmp = {}
                tmp["comp_name"] = dep_info
                tmp["condition"] = comp["condition"]
                depends.append(tmp)
                # print("add mandatory depend r:", tmp)
        """ get optional dependency list for this optional component """
        for dep_info in comp_info[comp["comp_name"]]["optional_dependencies"]:
            if dep_info["comp_name"] not in mandatory_comps:
                """ add to the list with (the inherrited dependency && this condition) """
                tmp = {}
                tmp["comp_name"] = dep_info["comp_name"]
                tmp["condition"] = comp["condition"] + ["and"] + dep_info["condition"]
                depends.append(tmp)
                # print("add optional depend r:", tmp)

    if depends:
        depends += get_comp_optional_depends_r(comp_info, depends, mandatory_comps)
    return depends

def get_comp_optional_depends(comp_info, comps):
    """ Get comp optional depends from comp index """
    depends = []
    """ comps are mandatory components got by get_comp_mandatory_depends,
    here is to find all optional dependencies for comp"""
    for comp in comps:
        if comp in comp_info:
            for dep_info in comp_info[comp]["optional_dependencies"]:
                """ if optional dependency(dep_info) is in mandatory components, ignore it """
                if dep_info["comp_name"] not in comps:
                    depends.append(dep_info)
                    # print("add depend:", dep_info)

    merge_depends = []
    if depends:
        depends += get_comp_optional_depends_r(comp_info, depends, comps)
        depends.sort(key=lambda x: x["comp_name"])
        last_dep = ""
        """ merge the condition for the dependency of same component name """
        for dep in depends:
            # print("optional dependency is", dep)
            if dep["comp_name"] != last_dep:
                """ new deps """
                tmp = {}
                tmp["comp_name"] = dep["comp_name"]
                tmp["condition"] = []
                tmp["condition"].append(dep["condition"])
                last_dep = dep["comp_name"]
                merge_depends.append(tmp)
            else:
                """ deps with the prio one """
                duplicated = False
                for cond in merge_depends[-1]["condition"]:
                    if cond == dep["condition"]:
                        duplicated = True
                        break
                if not duplicated:
                    merge_depends[-1]["condition"].append(dep["condition"])

    # for dep in merge_depends:
    #     print("dep is", dep)
    return merge_depends

def get_comp_optional_depends_text(conditions_list, config_file):
    """ format optional Config.in string, like
    if (((cond1 || cond2) && cond3) || (cond4 || cond5))
    source $AOS_SDK_PATH/core/cli/Config.in
    endif
    condition_list is [[cond1, cond2, and cond3], [cond4, cond5]]
    config_file is filename of Config.in 
    """
    line = "if ("
    conds_line = ""
    for conds in conditions_list:
        conds_line += "("
        cond_line = ""
        for cond in conds:
            if cond == "and":
                cond_line = "(" + cond_line[:-4] + ") && "
            else:
                cond_line += "%s || " % cond
        conds_line += cond_line[:-4]
        conds_line += ") || "
    line += conds_line[:-4]
    line += ")\n" + 'source "$AOS_SDK_PATH/%s"\n' % config_file + "endif\n"
    return line

def find_config_in_file(app_config_in):
    """find Config.in files in application's Config.in """
    config_in_list = []
    if not os.path.isfile(app_config_in):
        return config_in_list

    aos_sdk_path = os.environ["AOS_SDK_PATH"]
    user_app_path = os.path.dirname(app_config_in)

    pattern = re.compile(r'source \"\$(AOS_SDK_PATH|USER_APP_PATH)\/([\w\-\.\/]+)\"')
    with open (app_config_in, 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith("source"):
                match = pattern.match(line)
                if match:
                    if "AOS_SDK_PATH" == match.group(1):
                        config_in_list.append(os.path.join(aos_sdk_path, match.group(2)))
                    elif "USER_APP_PATH" == match.group(1):
                        config_in_list.append(os.path.join(user_app_path, match.group(2)))

    config_in_list.append(app_config_in)
    return config_in_list

def get_comp_name_from_configin(config_in_list):
    """ read aos.mk file in the same directory of Config.in, and get component name from
    aos.mk """
    comp_list = []
    for config_in_file in config_in_list:
        dirname = os.path.dirname(config_in_file)
        mkfile = os.path.join(dirname, "aos.mk")
        if(os.path.isfile(mkfile)):
            comp = {}
            comp["comp_name"] = get_comp_name(mkfile)
            comp["config_file"] = config_in_file
            comp_list.append(comp)

    return comp_list

def from_y_n_to_0_1(from_y_n, type):
    if type == "bool":
        if from_y_n == "y":
            to_0_1 = "1"
        else:
            to_0_1 = "0"
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

def from_0_1_to_y_n(from_0_1, type):
    if type == "bool":
        if from_0_1 != "0":
            to_y_n = "y"
        else:
            to_y_n = "n"
    elif type == "tristate":
        if from_0_1 == "1":
            to_y_n = "m"
        elif from_0_1 == "2":
            to_y_n = "y"
        else:
            to_y_n = "n"
    else:
        to_y_n = from_0_1
    return to_y_n

def parse_block_of_configin(lines):
    """ parse a block of Config.in file to get macro name, type, value """
    p1 = re.compile(r"(config|menuconfig)\s+(\w*)$")
    p2 = re.compile(r"(bool|int|string|hex|tristate)(\s+\"(.*)\")?")
    p3 = re.compile(r"default\s+(\w*)")
    p4 = re.compile(r"default\s+\"(.*)\"")
    new_macro = {}
    for line in lines:
        if line.startswith("config") or line.startswith("menuconfig"):
            match = p1.match(line)
            if match:
                new_macro["name"] = match.group(2)
            else:
                return new_macro
        elif line.startswith("bool") or line.startswith("int") or line.startswith("string") \
        or line.startswith("hex") or line.startswith("tristate"):
            match = p2.match(line)
            if match:
                new_macro["type"] = match.group(1)
                if new_macro["type"] == "string":
                    new_macro["value"] = "\"\""
                else:
                    new_macro["value"] = "0"
                if match.group(3):
                    new_macro["hint"] = match.group(3)
                else:
                    new_macro["hint"] = ""
        elif line.startswith("default"):
            if new_macro["type"] == "string":
                match = p4.match(line)
                if match:
                    new_macro["value"] = "\"" + match.group(1) + "\""
            else:
                match = p3.match(line)
                if match:
                    val = from_y_n_to_0_1(match.group(1), new_macro["type"])
                    new_macro["value"] = val

    return new_macro

def convert_configin_to_header(config_in_file, comp_name, destdir):
    """ read Config.in file, and convert to C header file """
    if not os.path.isfile(config_in_file):
        return False
    if not os.path.isdir(destdir):
        return False
    if not comp_name:
        return False
    macro_list = []
    with open (config_in_file, 'r') as f:
        lines = []
        new_block = False
        for line in f.readlines():
            line = line.strip()
            if line:
                if line.startswith("config ") or line.startswith("menuconfig "):
                    if new_block:
                        macro = parse_block_of_configin(lines)
                        if macro:
                            macro_list.append(macro)
                    new_block = True
                    lines = []
                if new_block:
                    lines.append(line)
        # last block
        if new_block:
            macro = parse_block_of_configin(lines)
            if macro:
                macro_list.append(macro)
    if macro_list:
        filename = os.path.join(destdir, "comp_%s.h" % comp_name)
        with open (filename, 'w+') as f:
            f.write("//================This is split line================\n")
            f.write("// %s %s\n\n" % (COMPONENT_KEYWORD, comp_name))
            for macro in macro_list:
                f.write("// description:%s\n" % macro["hint"])
                f.write("// #define %s %s // type: %s\n\n" % (macro["name"],
                    macro["value"], macro["type"]))

        
def generate_default_header_file(config_in_file):
    """ read application Config.in file, and get all components' Config.in file.
     Convert to C header file for every components used by this application """
    if not os.path.isfile(config_in_file):
        return False
    
    dirname = os.path.dirname(config_in_file)
    destdir = os.path.join(dirname, CONFIG_BAK_PATH)
    config_in_list = find_config_in_file(config_in_file)
    comp_list = get_comp_name_from_configin(config_in_list)
    for comp in comp_list:
        convert_configin_to_header(comp["config_file"], comp["comp_name"], destdir)
