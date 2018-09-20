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
    'developerkit': ['stm32l4x.json'],
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

# Debug configs for stm32l4xx
stm32l4x = {
'start_server': [
    {
        'Linux64': {
            'cmd': '@AOSROOT@/build/openocd_master/bin/openocd_linux',
            'url': 'https://gdb-archive.oss-cn-shanghai.aliyuncs.com/openocd_linux.tar.gz', },
        'OSX': {
            'cmd': '@AOSROOT@/build/openocd_master/bin/openocd_osx',
            'url': 'https://gdb-archive.oss-cn-shanghai.aliyuncs.com/openocd_osx.tar.gz', },
    },
    '-f',
    '@AOSROOT@/build/openocd_master/share/openocd/scripts/board/stm32l4discovery.cfg',
],
'start_client': [
    {
        'Linux64': {
            'cmd': '@AOSROOT@/build/compiler/gcc-arm-none-eabi/Linux64/bin/arm-none-eabi-gdb',
            'url': 'https://gdb-archive.oss-cn-shanghai.aliyuncs.com/arm-none-eabi-gdb-linux64.tar.gz', },
        'OSX': {
            'cmd': '@AOSROOT@/build/compiler/gcc-arm-none-eabi/OSX/bin/arm-none-eabi-gdb',
            'url': 'https://gdb-archive.oss-cn-shanghai.aliyuncs.com/gdb-arm-none-eabi-osx.tar.gz', },
    },
    '@AOSROOT@/out/@TARGET@/binary/@TARGET@.elf'
]
}
debug_configs['stm32l4x'] = stm32l4x

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
