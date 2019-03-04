#!/usr/bin/env python
#
# Tool for generating debug configs at debug/*:
#   ./gen_debug_configs.py
#

import json

#
# Register board name / part of the board name to match multiple boards:
#   { 'board name': [ debug_config_file ], }
#
registry_board = {
    "ALLOTHERS": [
        "stm32_stlink.json"
    ],
    "developerkit": [
        "developerkit.json"
    ],
    "stm32f412zg-nucleo": [
        "stm32f412zg-nucleo.json"
    ],
    "stm32f429zi-nucleo": [
        "stm32f429zi-nucleo.json"
    ],
    "pca10040": [
        "pca10040.json"
    ]
}

#
# Add debug configs with format:
#   debug_config = { 'start-server': [
#                     { host_os: {'cmd': cmd, 'url': url }, ... },
#                     opt1, opt2 ... ],
#                    'start-client': [
#                     { host_os: {'cmd': cmd, 'url': url }, ... },
#                     opt1, opt2 ... ],
#                  }
#
# Note:
#   host_os: specify debug cmd as OS type, valid OS types:
#            Win32, OSX, Linux64, Linux32
#   'cmd': debug tools for starting server / client
#   'url': url for downloading debug tools if the tools are not available
#
# Add new item to dict debug_configs:
#   debug_configs['debug_config'] = debug_config
#
debug_configs = {}

# Debug configs for default config
stm32_stlink = {
    "prompt": "",
    "port": 4242,
    "cmd": [
        {
            "Linux64": "@AOSROOT@/build/cmd/linux64/st-util",
            "OSX": "@AOSROOT@/build/cmd/osx/st-util",
            "Win32": "@AOSROOT@/build/cmd/win32/st-util.exe"
        }
    ],
    "upload": True
}
debug_configs['stm32_stlink'] = stm32_stlink

# Debug configs for pca10040
pca10040 = {
    "prompt": "Please INSTALL Jlink Software Package, and ADD JLink to PATH environment, check: www.github.com/alibaba/AliOS-Things/wiki/debug",
    "port": 4242,
    "upload": True,
    "cmd": [
        {
            "Linux64": "JLinkGDBServerCLExe",
            "OSX": "JLinkGDBServerCLExe",
            "Win32": "JLinkGDBServerCL.exe"
        },
        "-if",
        "swd",
        "-device",
        "nRF52840_xxAA",
        "-port",
        "4242"
    ]
}
debug_configs['pca10040'] = pca10040

# Debug configs for stm32f412zg_nucleo config
stm32f412zg_nucleo = {
    "prompt": "",
    "port": 4242,
    "upload": True,
    "cmd": [
        {
            "Linux64": "@AOSROOT@/build/cmd/linux64/st-util",
            "OSX": "@AOSROOT@/build/cmd/osx/st-util",
            "Win32": "@AOSROOT@/build/cmd/win32/st-util.exe"
        }
    ]
}
debug_configs['stm32f412zg-nucleo'] = stm32f412zg_nucleo

# Debug configs for stm32f429zi_nucleo config
stm32f429zi_nucleo = {
    "prompt": "",
    "port": 4242,
    "upload": True,
    "cmd": [
        {
            "Linux64": "@AOSROOT@/build/cmd/linux64/st-util",
            "OSX": "@AOSROOT@/build/cmd/osx/st-util",
            "Win32": "@AOSROOT@/build/cmd/win32/st-util.exe"
        }
    ]
}
debug_configs['stm32f429zi-nucleo'] = stm32f429zi_nucleo

# Debug configs for developerkit config
developerkit = {
    "prompt": "",
    "port": 4242,
    "cmd": [
        {
            "Linux64": "@AOSROOT@/build/cmd/linux64/st-util",
            "OSX": "@AOSROOT@/build/cmd/osx/st-util",
            "Win32": "@AOSROOT@/build/cmd/win32/st-util.exe"
        }
    ]
}
debug_configs['developerkit'] = developerkit

def main():
    # Write debug configs
    for key in debug_configs:
        with open('debug/' + key + '.json', 'w') as f:
            json.dump(debug_configs[key], f, sort_keys=True, indent=4)

    # Write registered boards
    with open('debug/registry_board.json', 'w') as f:
        json.dump(registry_board, f, sort_keys=True, indent=4)

if __name__ == "__main__":
    main()
