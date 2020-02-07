import os
import hashlib
import re
import shutil

COMPONENTS_CONFIG_KEYWORD = "KEYWORD: PART2 COMPONENTS CONFIGURATION"

def get_md5sum(config_file):
    """ Get MD5SUM_HEADER from config file .aos """
    ret = None
    with open(config_file, "r") as f:
        for line in f.readlines():
            line = line.strip()
            if "MD5SUM_HEADER" in line:
                ret = line.replace("MD5SUM_HEADER=", "")

    return ret


def write_md5sum(config_file, md5sum_value):
    """ Write MD5SUM_HEADER to config file .aos """
    content = []
    with open(config_file, "r") as f:
        for line in f.readlines():
            if "MD5SUM_HEADER" in line:
                continue
            else:
                content.append(line)

    content.append("MD5SUM_HEADER=%s\n" % md5sum_value)
    with open(config_file, "w") as f:
        for line in content:
            f.write(line)


def md5sum(string):
    """ Calculat md5sum value for string """
    return hashlib.md5(string.encode()).hexdigest()


def get_include_comp_list(code_file):
    """ Get include files from code_file by searching lines started with #include """
    include_list = []

    patten = re.compile(r'#include\s+(<|")(.*)(>|")')
    with open(code_file, "rb") as f:
        for line in f.readlines():
            line = line.decode("utf-8", "ignore")
            line = line.strip()
            if line.startswith("#include"):
                line = line.strip()
                match = patten.match(line)
                if match:
                    include_list.append(match.group(2))

    return include_list

def get_include_file(dirname):
    """ Get *.h from dirname and it's subdir """
    include_files = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".h"):
                include_files += [os.path.abspath("%s/%s" % (root, file))]

    return include_files

def get_source_file(dirname):
    """ Get source files(*.c, *.h, *.cpp) in appdir and its subdir """
    sources = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".c") or file.endswith(".h") or file.endswith(".cpp"):
                sources.append(os.path.join(root, file))

    return sources

def get_configin_file(comp_dir):
    """ Find Config.in in the specific directory """
    config_file = os.path.join(comp_dir, "Config.in").replace("\\", "/")
    if os.path.isfile(config_file):
        return config_file
    else:
        return ""

def compute_header_md5sum(appdir):
    """ Compute header's md5sum for App:
    1. get all source files(*.c, *.h, *.cpp) in appdir;
    2. search include file list in each source file by searching #include
    3. remove redundant include file list and sort
    4. calculate md5 checksum """
    code_list = get_source_file(appdir)
    include_list = []
    for code_file in code_list:
        include_list += get_include_comp_list(code_file)

    include_list = sorted(list(set(include_list)))
    header_md5sum = md5sum(" ".join(include_list))

    return (header_md5sum, include_list)

def compute_aos_config_md5sum(aosconfig_h):
    """ Compute md5sum of aos_config.h, except user defined part  """
    define_list = []
    with open (aosconfig_h, 'r') as f:
        started = False
        for line in f.readlines():
            line = line.strip()
            if line.find(COMPONENTS_CONFIG_KEYWORD) >= 0:
                started = True
            if started:
                if line.startswith("#define"):
                    define_list.append(line)

    define_list = sorted(list(set(define_list)))
    define_md5sum = md5sum(" ".join(define_list))

    return define_md5sum


def get_depends_from_source(comp_info, include_list):
    """ Get comp name by searching include file in comp_info, like
    xxx/cli.h --> cli, xxx/rbtree.h --> lib_rbtree  """
    includes = []
    depends = []

    for include_file in include_list:
        found = False

        for key in comp_info:
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
                    depends.append(key)
                    found = True
                    break
            if found:
                break

    depends = list(set(depends))
    return depends

def copy_file(srcfile, destfile):
    """ Copy srcfile to destfile, create destdir if not existed """
    subdir = os.path.dirname(destfile)

    if not os.path.isdir(subdir):
        os.makedirs(subdir)

    shutil.copyfile(srcfile, destfile)

def write_file(contents, destfile):
    """ Write contents to destfile line by line """
    subdir = os.path.dirname(destfile)

    if not os.path.isdir(subdir):
        os.makedirs(subdir)
    with open(destfile, "w") as f:
        for line in contents:
            f.write(line)

def write_project_config(config_file, config_data):
    """ Write projet config file: KEY1=VALUE1 """
    contents = []
    if os.path.isfile(config_file) and config_data:
        with open(config_file, "r") as f:
            for line in f.readlines():
                line = line.strip()
                tmp = line.split("=")
                key, value = tmp[0], tmp[1]
                if key not in config_data:
                    config_data[key] = value

    for key in config_data:
        contents.append("%s=%s\n" % (key, config_data[key]))

    contents = sorted(contents)
    write_file(contents, config_file)


