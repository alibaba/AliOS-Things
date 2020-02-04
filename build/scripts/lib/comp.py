import os, sys
import re

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
