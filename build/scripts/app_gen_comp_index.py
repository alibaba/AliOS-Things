#!/usr/bin/env python

import os
import sys
import re
import json
from lib.comp import find_comp_mkfile, get_comp_name, get_comp_deps


network_includes = {
    "umesh": ["network/umesh"],
    "breeze": [
        "network/bluetooth/breeze",
        "network/bluetooth/breeze/api",
        "network/bluetooth/breeze/core/include",
        ],
    "bt_host": [
        "network/bluetooth/bt_host",
        "network/bluetooth/bt_host/bt_common/include",
        "network/bluetooth/bt_host/bt_common/port/include",
        "network/bluetooth/bt_host/bt_common/tinycrypt/include",
        "network/bluetooth/bt_host/include",
        "network/bluetooth/bt_host/include/drivers",
        "network/bluetooth/bt_host/profile",
        ],
    "tmall_model": ["network/bluetooth/tmall_model"],
    "bt_mesh": [
        "network/bluetooth/bt_mesh",
        "network/bluetooth/bt_mesh/inc",
    ],
    "http": ["network/http"],
    "lorawan_4_4_2": [
        "network/lorawan/lorawan_4_4_2",
        "network/lorawan/lorawan_4_4_2/radio",
        "network/lorawan/lorawan_4_4_2/radio/sx1276",
        "network/lorawan/lorawan_4_4_2/mac",
        "network/lorawan/lorawan_4_4_2/mac/region",
        "network/lorawan/lorawan_4_4_2/system",
    ],
    "lorawan_4_4_0": [
        "network/lorawan/lorawan_4_4_0",
        "network/lorawan/lorawan_4_4_0/radio",
        "network/lorawan/lorawan_4_4_0/radio/sx1276",
        "network/lorawan/lorawan_4_4_0/mac",
        "network/lorawan/lorawan_4_4_0/mac/region",
        "network/lorawan/lorawan_4_4_0/system",
        ],
    "lwm2m": ["network/lwm2m"],
    "sal": ["network/nal/sal"],
    "atparser": ["network/nal/atparser"],
    "athost": ["network/nal/athost"],
    "libcoap": ["network/coap"],
    "netmgr": ["network/netmgr"],
    "lwip": ["network/lwip"],
    "yloop": ["network/yloop"],
    "und": ["und"],
}

middleware_includes = {
    "ota": {"dirs": ["ota"]},
    "ulocation": {"dirs": ["ulocation"]},
    "udata": {"dirs": ["udata"]},
    "uai": {"dirs": ["uai"]},
    "uagent": {"dirs": ["uagent"]},
    "ulog": {"dirs": ["ulog"]},
    "libiot_infra": {"dirs": ["linkkit/infra"]},
    "libiot_wrappers": {"dirs": ["linkkit/wrappers"]},
    "libiot_awss": {"files": ["linkkit/wifi_provision_api.h", "linkkit/dev_bind_api.h"]},
    "libiot_reset": {"files": ["linkkit/dev_reset_api.h"]},
    "libiot_devmodel": {"files": ["linkkit/alcs_api.h", "linkkit/dev_model_api.h"]},
    "libiot_http2stream": {"files": ["linkkit/http2_api.h", "linkkit/http2_upload_api.h"]},
    "libiot_sign": {"files": ["linkkit/dev_sign_api.h"]},
    "libiot_http": {"files": ["linkkit/http_api.h"]},
    "libiot_dynreg": {"files": ["linkkit/dynreg_api.h"]},
    "libiot_mqtt": {"files": ["linkkit/mqtt_api.h"]},
    "libiot_ota": {"files": ["linkkit/ota_api.h"]},
    "libiot_coap": {"files": ["linkkit/coap_api.h"]},
}


def find_include_file(dirname):
    """ Find *.h from dirname """
    include_files = []
    for root, dirs, files in os.walk(dirname):
        for file in files:
            if file.endswith(".h"):
                include_files += [os.path.abspath("%s/%s" % (root, file))]

    return include_files


def get_comp_configin(comp_dir):
    """ Find Config.in of comp """
    config_file = os.path.join(comp_dir, "Config.in")
    if os.path.isfile(config_file):
        return config_file
    else:
        return ""


def get_comp_include_info(comp_name, mkfile, common_include_dir):
    """ Get include infos from mkfile:
    return data: {"include_dirs": [], "include_files": []}
    """
    include_info = {}

    if "/platform/" in mkfile:
        return include_info

    dirs = []
    with open(mkfile, "r") as f:
       for line in f:
            line = line.strip()
            while line.endswith('\\'):
                line = line[:-1] + next(f).rstrip('\n')

            if not line or "#" in line:
                continue

            patten = re.compile(r"^(GLOBAL_INCLUDES)\s?.*=\s?(.*)")
            if "$(NAME)_INCLUDES" in line or "GLOBAL_INCLUDES" in line:
                match = patten.match(line)
                if match:
                    dirs += match.group(2).split()

    base_dir = os.path.dirname(mkfile)
    include_info["include_dirs"] = []
    include_info["include_files"] = []
    if dirs:
        dirs = list(set(dirs))
        include_info["include_dirs"] = [os.path.abspath(os.path.join(base_dir, dir)) for dir in dirs]

    ret = find_include_file(base_dir)
    extra_dir = os.path.join(common_include_dir, comp_name)
    if os.path.isdir(extra_dir):
        ret = find_include_file(extra_dir) + ret
        include_info["include_dirs"].insert(0, extra_dir)

    if "/kernel/" in mkfile:
        tmp_dir = os.path.join(os.path.join(common_include_dir, "aos"))
        tmp_file = os.path.join(tmp_dir, "%s.h" % comp_name)
        if os.path.isfile(tmp_file):
            ret.insert(0, tmp_file)
    elif "/network/" in mkfile:
        if comp_name in network_includes:
            for tmp_dir in network_includes[comp_name]:
                tmp_dir = os.path.abspath(os.path.join("include", tmp_dir))
                include_info["include_dirs"].insert(0, tmp_dir)
                ret = find_include_file(tmp_dir) + ret

            tmp_dir = os.path.join(os.path.join(common_include_dir, "network"))
            include_info["include_dirs"].insert(0, tmp_dir)
    elif "/middleware/" in mkfile:
        if comp_name in middleware_includes:
            if "dirs" in middleware_includes[comp_name]:
                for tmp_dir in middleware_includes[comp_name]["dirs"]:
                    tmp_dir = os.path.abspath(os.path.join("include", tmp_dir))
                    include_info["include_dirs"].insert(0, tmp_dir)
                    ret = find_include_file(tmp_dir) + ret
            if "files" in middleware_includes[comp_name]:
                for tmp_file in middleware_includes[comp_name]["files"]:
                    tmp_file = os.path.abspath(os.path.join("include", tmp_file))
                    ret.insert(0, tmp_file)

            tmp_dir = os.path.join(os.path.join(common_include_dir, "linkkit"))
            include_info["include_dirs"].insert(0, tmp_dir)

    include_info["include_dirs"].insert(0, common_include_dir)

    if ret:
        include_info["include_files"] = ret

    return include_info


def get_comp_info(source_root, mklist, common_include_dir):
    """ Get comp infos that includes:
    comp_dir, include_dir, include_files, Config.in, dependencies
    """
    comp_info = {}
    source_root = os.path.abspath(source_root)

    for mkfile in mklist:
        mkfile = mkfile.replace("\\", "/")
        comp_name = get_comp_name(mkfile)
        if comp_name:
            if comp_name == "system_include" or comp_name == "buildsystem" or comp_name == "buildconfigs":
                continue

            comp_dir = os.path.dirname(mkfile)
            include_info = get_comp_include_info(comp_name, mkfile, common_include_dir)
            config_file = get_comp_configin(comp_dir)
            deps = get_comp_deps(mkfile)
            comp_info[comp_name] = include_info
            comp_info[comp_name]["location"] = comp_dir.replace(source_root + "/", "")
            comp_info[comp_name]["config_file"] = config_file.replace(source_root + "/", "")
            comp_info[comp_name]["dependencies"] = deps

            # Keep a copy for board with dirname
            if comp_name.startswith("board_"):
                board_name = mkfile.split("/")[-2]
                comp_info[board_name] = comp_info[comp_name]

    return comp_info


def main():
    if len(sys.argv) < 3:
        print("Usage: %s SOURCE_ROOT OUTPUT_FILE" % sys.argv[0])
        return 1

    source_root = sys.argv[1]
    output_file = sys.argv[2]
    mklist = find_comp_mkfile(source_root)
    common_include_dir = os.path.abspath(os.path.join(source_root, "include"))

    appdir = os.environ.get("APPDIR")
    if appdir:
        mklist += find_comp_mkfile(appdir)

    includes = get_comp_info(source_root, mklist, common_include_dir)
    with open(output_file, "w") as f:
        json.dump(includes, f, sort_keys=True, indent=4)

if __name__ == "__main__":
    main()
