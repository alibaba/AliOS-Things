#!/usr/bin/env python
import os, sys, re, codecs, shutil, platform

# read params from file if exists
params = sys.argv[1:]
if os.path.isfile(sys.argv[1]):
    with codecs.open(sys.argv[1], 'r', 'UTF-8') as fh:
        params = fh.read().split("\n")
    # appdend user-defined args
    if len(sys.argv) > 2:
        params += sys.argv[2:]

# choose those arguments we wanted, and discard others
key_value = {}
pattern = re.compile(r'--(.+?)=(.*)')
for arg in params:
    arg = arg.strip()
    if arg.startswith("--"):
        match = pattern.match(arg)
        if match:
            key = match.group(1)
            value = match.group(2)
            if key in ["toolchain", "lib", "target", "cflag", "cxxflag", "asmflag", "ldflag", "cpu", "macro_list", "global_inc", "solution_dir", "comp_name"]:
                key_value[key] = value
            else:
                print("ignore %s=%s" %(key, value))

# format string
for key in ["cflag", "cxxflag", "asmflag", "ldflag"]:
    if key in key_value.keys():
        key_value[key] = key_value[key].replace("#", " ")
        # escape the string: "aos.map" -> \"aos.map\"
        # but keep \"ALIBABA IOT\"
        value = ""
        ch_last = ""
        for ch in key_value[key]:
            if (ch == '"') and (ch_last != '\\'):
                value += '\\'
            value += ch
            ch_last = ch
        # escape the string: \"aos.map\" -> \\\"aos.map\\\"
        key_value[key] = value.replace('\\\"', '\\\\\\\"')

for key in ["macro_list", "global_inc"]:
    if key in key_value.keys():
        # escape the string: \"aos.map\" -> "aos.map"
        value = key_value[key].replace("#", " ")
        key_value[key] = value.replace('\\\"', '\"')

# strip "
for key in ["toolchain", "lib", "target", "solution_dir"]:
    if key in key_value.keys():
        key_value[key] = key_value[key].strip('"')

print("the script is %s" % sys.argv[0])
print("current dir is %s" % os.getcwd())
for key in key_value.keys():
    print("%s is %s" %(key, key_value[key]))

# =======================================================
# do our work

# format file path in windows
if platform.system() == "Windows":
    for key in ["toolchain", "macro_list", "global_inc"]:
        if key in key_value.keys():
            key_value[key] = key_value[key].replace("\\", "\\\\")

# update flags
key_value["cflag"] += " --specs=nosys.specs"
key_value["cxxflag"] += " --specs=nosys.specs"
key_value["ldflag"] += " --specs=nosys.specs"

# update add_params_from_aostools.cmake with macro and include path
# and call cmake to build
comp_path = os.path.dirname(sys.argv[0])

macro_path = "macro_defines.h"
f = open(macro_path, 'w')
data_list = key_value["macro_list"].split('-D')

for item in data_list:
    k_list = item.split('=')
    if len(k_list) < 2:
        continue
    head = "#ifndef " + k_list[0] + "\n"
    f.write(head)
    data = item.replace('=', "    ")
    f.write("#define "+ data + "\n")
    f.write("#endif\n\n")

f.close()

build_cmd = '%s-gcc %s -imacros %s -E -D__ARM__  -D__ALIGN__=4 -P %s/_haas1000_alios.c -o %s/_haas1000_alios.lds' % (key_value["toolchain"], key_value["cflag"], macro_path, comp_path, comp_path)
print(build_cmd)
ret = os.system(build_cmd) >> 8
if ret != 0:
    exit(ret)

# =======================================================    

# result
print("run external script success")
exit(0)
