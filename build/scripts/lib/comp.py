import os, sys
import re

def find_comp_mkfile(dirname):
    """ Find component makefiles from dirname """
    mklist = []
    for root, dirs, files in os.walk(dirname):
        if 'out' in root or 'build' in root or 'publish' in root:
            continue

        if 'aos.mk' in files:
            mklist += ["%s/aos.mk" % root]
            continue

    return mklist

def get_comp_name(mkfile):
    """ Get comp name from mkfile """
    name = None
    patten = re.compile(r'^NAME.*=\s*(.*)\s*')
    with open(mkfile, 'r') as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                 name = match.group(1)

    return name


def get_comp_deps(mkfile):
    """ Get component's dependencies """
    abs_mkfile = os.path.abspath(mkfile)
    deps = []
    host_mcu_family = ""

    p1 = re.compile(r'^HOST_MCU_FAMILY.*=\s*(.*)\s*')
    p2 = re.compile(r'^\$\(NAME\)_COMPONENTS.*=\s*(.*)\s*')
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

            if line.startswith("$(NAME)_COMPONENTS"):
                match = p2.match(line)
                if match:
                    deps += match.group(1).split()

    deps = list(set(deps))
    if host_mcu_family:
        tmp = " ".join(deps).replace("$(HOST_MCU_FAMILY)", host_mcu_family)
        deps = tmp.split()

    return deps
