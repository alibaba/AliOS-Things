import os
import sys
import json
import shutil
import re
from lib.comp import find_comp_mkfile, get_comp_name
from lib.code import get_source_file, get_include_comp_list

from imp import reload
try:
    reload(sys)
    sys.setdefaultencoding('UTF8')
except:
    pass

COMP_DEPS_FILE = "comp_deps.json"
COMP_DEPS_TOPO_FILE = "comp_deps_topo.dot"
COMP_DEPS_TOPO_PNG = "comp_deps_topo.png"
DOT_CONFIG_FILE = ".config"

def write_comp_deps_from_mkfile(compname, mkfile, ftopo):
    """ Get component's mandatory and optional dependencies from aos.mk 
    by searching $(NAME)_COMPONENTS and $(NAME)_COMPONENTS-$( """
    aosmk_deps = []
    host_mcu_family = ""
    ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (compname.replace("-","_"), compname))

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

            if line.startswith("$(NAME)_COMPONENTS_CUSTOMIZED"):
                match = p2.match(line)
                if match:
                    orig_comp_names = match.group(1).split()
                    tmp = " ".join(orig_comp_names)
                    if host_mcu_family:
                        tmp = tmp.replace("$(HOST_MCU_FAMILY)", host_mcu_family)
                    comp_names = tmp.split()
                    aosmk_deps += comp_names
            elif line.startswith("$(NAME)_COMPONENTS") and not line.startswith("$(NAME)_COMPONENTS-n"):
                match = p2.match(line)
                if match:
                    orig_comp_names = match.group(1).split()
                    tmp = " ".join(orig_comp_names)
                    if host_mcu_family:
                        tmp = tmp.replace("$(HOST_MCU_FAMILY)", host_mcu_family)
                    tmp = tmp.replace("$($(NAME)_COMPONENTS_CUSTOMIZED)", host_mcu_family)
                    comp_names = tmp.split()
                    if line.startswith("$(NAME)_COMPONENTS-$("):
                        match = p3.match(line)
                        if match:
                            comp_cond = match.group(1)
                            aosmk_deps += comp_names
                            for comp in comp_names:
                                ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (comp.replace("-","_"), comp))
                                ftopo.append('%s->%s[label="%s"]\n' % (compname.replace("-","_"), comp.replace("-","_"), comp_cond))
                    else:
                        aosmk_deps += comp_names
                        for comp in comp_names:
                            ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (comp.replace("-","_"), comp))
                            ftopo.append("%s->%s\n" % (compname.replace("-","_"), comp.replace("-","_")))

    aosmk_deps = list(set(aosmk_deps))

    return aosmk_deps

def write_comp_mandatory_depends(comp_info, comps, ftopo):
    """ Get comp mandatory depends from comp index """
    depends = []
    for comp in comps:
        if comp in comp_info:
            depends += comp_info[comp]["dependencies"]
            for dep in comp_info[comp]["dependencies"]:
                ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (dep.replace("-","_"), dep))
                ftopo.append("%s->%s\n" % (comp.replace("-","_"), dep.replace("-","_")))

    if depends:
        depends += write_comp_mandatory_depends(comp_info, depends, ftopo)

    return list(set(depends))

def write_comp_optional_depends_r(comp_info, comps, ftopo):
    """ Get comp optional depends recursively from comp index """
    depends = []
    """ comps are optional dependency list from last layer """
    for comp in comps:
        if comp in comp_info:
            """ get mandatory dependency list for this optional component """
            for dep_info in comp_info[comp]["dependencies"]:
                """ add to the list with the inherrited dependency"""
                depends.append(dep_info)
                ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (dep_info.replace("-","_"), dep_info))
                ftopo.append("%s->%s\n" % (comp.replace("-","_"), dep_info.replace("-","_")))
            """ get optional dependency list for this optional component """
            for dep_info in comp_info[comp]["optional_dependencies"]:
                """ add to the list with (the inherrited dependency && this condition) """
                depends.append(dep_info["comp_name"])
                for cond in dep_info["condition"]:
                    ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (dep_info["comp_name"].replace("-","_"), dep_info["comp_name"]))
                    ftopo.append('%s->%s[label="%s"]\n' % (comp.replace("-","_"), dep_info["comp_name"].replace("-","_"), cond))

    if depends:
        depends += write_comp_optional_depends_r(comp_info, depends, ftopo)
    return depends

def write_comp_optional_depends(comp_info, comps, ftopo):
    """ Get comp optional depends from comp index """
    depends = []
    """ comps are mandatory components got by get_comp_mandatory_depends,
    here is to find all optional dependencies for comp"""
    for comp in comps:
        if comp in comp_info:
            for dep_info in comp_info[comp]["optional_dependencies"]:
                """ if optional dependency(dep_info) is in mandatory components, ignore it """
                depends.append(dep_info["comp_name"])
                for cond in dep_info["condition"]:
                    ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (dep_info["comp_name"].replace("-","_"), dep_info["comp_name"]))
                    ftopo.append('%s->%s[label="%s"]\n' % (comp.replace("-","_"), dep_info["comp_name"].replace("-","_"), cond))

    if depends:
        depends += write_comp_optional_depends_r(comp_info, depends, ftopo)

def get_include_files(appdir):
    """ get include files for app source code"""
    code_list = get_source_file(appdir)
    include_list = []
    for code_file in code_list:
        include_list += get_include_comp_list(code_file)

    include_list = sorted(list(set(include_list)))
    return include_list

def write_depends_from_source(comp_info, appname, appcomps, include_list, ftopo):
    """ Get comp name by searching include file in comp_info, like
    xxx/cli.h --> cli, xxx/rbtree.h --> lib_rbtree  """
    for include_file in include_list:
        found = False
        for key in appcomps:
            if key not in comp_info:
                continue
            # Don't depends on board/arch/mcu/app
            if "location" in comp_info[key]:
                loc = comp_info[key]["location"]
                if loc.startswith("board/") or loc.startswith("platform/") or \
                    loc.startswith("application/") or loc.startswith("test/develop/"):
                    continue

            if "include_dirs" not in comp_info[key]:
                continue

            dirs = comp_info[key]["include_dirs"]
            files = comp_info[key]["include_files"]
            for dir in dirs:
                tmp = os.path.abspath(os.path.join(dir, include_file)).replace("\\", "/")
                if tmp in files:
                    ftopo.append('aaaaaaaa %s[label="%s", shape="box"]\n' % (key.replace("-","_"), key))
                    ftopo.append('%s->%s[label="%s"]\n' % (appname.replace("-","_"), key.replace("-","_"), include_file))
                    found = True
                    break
            if found:
                break

def write_source_code_depends(appdir, comp_info, appname, appcomps, ftopo):
    include_list = get_include_files(appdir)
    if include_list:
        write_depends_from_source(comp_info, appname, appcomps, include_list, ftopo)

def write_depends_topo_file(appdir, comp_deps_file, boardname, topo_file):
    """ find mandatory and optitional depends, and generate topo file """
    comp_infos = {}
    comps = []
    ftopo = []
    with open(comp_deps_file, "r") as f:
        comp_infos = json.load(f)
    
    # find deps in aos.mk of app and board in APPDIR
    appmkfile = os.path.join(appdir, "aos.mk")
    appname = get_comp_name(appmkfile)
    appcomps = write_comp_deps_from_mkfile(appname, appmkfile, ftopo)
    comps += appcomps
    if boardname:
        boardmkfile = os.path.join(appdir, "board", boardname, "aos.mk")
        comps += write_comp_deps_from_mkfile(boardname, boardmkfile, ftopo)
        comps = list(set(comps))

    # find deps in aos.mk of sdk components
    if comp_infos:
        comps += write_comp_mandatory_depends(comp_infos, comps, ftopo)
        comps = list(set(comps))
        write_comp_optional_depends(comp_infos, comps, ftopo)
        comps = list(set(comps))
    
    # find deps in source code of app in APPDIR
    if comp_infos:
        write_source_code_depends(appdir, comp_infos, appname, appcomps, ftopo)

    with open(topo_file, "w+") as f:
        ftopo = list(set(ftopo))
        ftopo.sort()
        f.write('digraph demo {\nlabel="components dependency"\n')
        for line in ftopo:
            line = line.replace("aaaaaaaa ", "")
            f.write(line)
        f.write('}\n')

def get_boardname_from_dir(dirname):
    boardname = ""
    mklist = find_comp_mkfile(os.path.join(dirname, "board"))
    if mklist:
        for line in mklist:
            p1 = re.compile(r'\/board\/(.*)\/aos.mk')
            match = p1.search(line)
            if match:
                boardname = match.group(1)
                break
    return boardname

def main(argv):
    if len(argv) == 1:
        appdir = os.getcwd()
        boardname = get_boardname_from_dir(appdir)
    elif len(argv) == 2:
        appdir = argv[1]
        boardname = get_boardname_from_dir(appdir)
    elif len(argv) == 3:
        appdir = argv[1]
        boardname = argv[2]
        if not os.path.isdir(os.path.join(appdir, "board", boardname)):
            print("Usage: board %s is not existed in APPDIR folder" % boardname)
            return 1
    else:
        print("Usage: %s [appdir] [boardname] for dependency analysis in APPDIR folder" % argv[0])
        return 1

    aos_sdk = os.environ.get("AOS_SDK_PATH")
    if not aos_sdk:
        print("[Error] AliOS Things SDK is not found!")
        return 1

    comp_deps_file = os.path.join(appdir, COMP_DEPS_FILE)
    scriptdir = os.path.join(aos_sdk, "build", "scripts")
    os.system("python %s/app_gen_comp_index.py %s %s" % (scriptdir, aos_sdk, comp_deps_file))

    comp_topo_file = os.path.join(appdir, COMP_DEPS_TOPO_FILE)
    write_depends_topo_file(appdir, comp_deps_file, boardname, comp_topo_file)
    os.remove(comp_deps_file)

    try:
        os.system("dot -Tpng %s -o %s" % (comp_topo_file, os.path.join(appdir, COMP_DEPS_TOPO_PNG)))
    except:
        pass
    return 0

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
