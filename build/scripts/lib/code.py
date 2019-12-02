import os
import hashlib
import re


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
    """ Get md5sum value for string """
    return hashlib.md5(string.encode()).hexdigest()


def get_include_file(code_file):
    """ Get include files from sources """
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


def get_source_file(dirname):
    """ Get source files in appdir """
    sources = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".c") or file.endswith(".h") or file.endswith(".cpp"):
                sources.append(os.path.join(root, file))

    return sources


def compute_header_md5sum(appdir):
    """ Compute header's md5sum for App """
    code_list = get_source_file(appdir)
    include_list = []
    for code_file in code_list:
        include_list += get_include_file(code_file)

    include_list = sorted(list(set(include_list)))
    header_md5sum = md5sum(" ".join(include_list))

    return (header_md5sum, include_list)


def get_depends_from_source(comp_info, include_list):
    """ Get comp depends from source """
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
