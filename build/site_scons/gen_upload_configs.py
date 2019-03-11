#!/usr/bin/env python
#
# Tool for generating flash configs at upload/*:
#   ./gen_upload_configs.py
#

import json

#
# Register board name / part of the board name to match multiple boards:
#   { 'board name': [cmdfile1, cmdfile2, ...], }
#
registry_board = {
    'atsame54': ['atsam.json'],
    'esp32': ['esp32.json'],
    'esp8266': ['esp8266.json'],
    'mk3060': ['mk3060.json'],
    'stm32': ['stm32_stlink.json'],
    'starterkit': ['starterkit_erase.json', 'stm32_stlink.json'],
    'developerkit': ['stm32_stlink.json'],
    'b_l475e': ['stm32_stlink.json'],
    'eml3047': ['stm32_stlink.json'],
    'm400': ['stm32_stlink.json'],
    'mk3165': ['stm32_stlink.json'],
    'mk3166': ['stm32_stlink.json'],
    'mk3239': ['stm32_stlink.json'],
    'pca10040': ['pca10040.json'],
    'ALLOTHERS': ['stm32_stlink.json'],
}

#
# Add flash configs with format:
#   board_name = { 'cmd': [ flashcmd, opt1, opt2 ... ], }
#   Or:
#   board_name = { 'cmd': [ { host_os: flashcmd, ... }, opt1, opt2 ... ],
#                  'description': "any msg shows to users"
#                }
#
# Note:
#   host_os: specify differenet flashcmd as OS type, valid OS names:
#            Win32, OSX, Linux64, Linux32
#   'description': Messages that shows to users before flashcmd running
#
# Add new item to dict flash_configs:
#   flash_configs['board_name'] = board_name
#
flash_configs = {}

# Flash configs for atsam* board
atsam = {
'cmd': [
    {
        'Win32': 'atprogram.exe',
    },
    '-t', 'edbg',
    '-i', 'SWD',
    '--clock', '4mhz',
    '-d', 'ATSAME54P20A'
    'program',
    '--chiperase',
    '--verify',
    '-f',
    '@AOSROOT@/out/@TARGET@/binary/@TARGET@.elf',
],
'description': 'Prerequisite to use this function:\n1. Atmel Studio 7 installed\n2. Add atprogram.exe file path (<AS7 install folder>/Atmel/Studio/7.0/atbackend) to Windows PATH environment variable',
}
flash_configs['atsam'] = atsam

# Flash configs for esp32* board
esp32 = {
'cmd': [
    {
        "Linux32": "esptool.py",
        "Linux64": "esptool.py",
        "OSX": "esptool.py",
        "Win32": "esptool.exe"
    },
    '--chip',
    'esp32',
    '--port', '@PORT@',
    '--baud', '921600',
    '--before', 'default_reset',
    '--after', 'hard_reset',
    'write_flash',
    '-z',
    '--flash_mode',
    'dio',
    '--flash_freq',
    '40m',
    '--flash_size',
    'detect',
    '0x1000', '@AOSROOT@/platform/mcu/esp32/bsp/bootloader.bin',
    '0x8000', '@AOSROOT@/platform/mcu/esp32/bsp/custom_partitions.bin',
    '0x10000', '@AOSROOT@/out/@TARGET@/binary/@TARGET@.bin'
],
}
flash_configs['esp32'] = esp32

# Flash configs for esp8266 board
esp8266 = {
'cmd': [
    {
        "Linux32": "esptool.py",
        "Linux64": "esptool.py",
        "OSX": "esptool.py",
        "Win32": "esptool.exe"
    },
    '--chip', 'esp8266',
    '--port', '@PORT@',
    '--baud', '921600',
    'write_flash',
    '--flash_size',
    'detect',
    '0x0', '@AOSROOT@/platform/mcu/esp8266/bsp/boot_v1.7_921600.bin',
    '0x3fc000', '@AOSROOT@/platform/mcu/esp8266/bsp/esp_init_data_default.bin',
    '0x3fe000', '@AOSROOT@/platform/mcu/esp8266/bsp/blank.bin',
    '0x1000', '@AOSROOT@/out/@TARGET@/binary/@TARGET@.bin'
],
}
flash_configs['esp8266'] = esp8266

# Flash configs for esp8266 board
mk3060 = {
'cmd': [
    '@AOSROOT@/build/scripts/aos_firmware_update.py',
    '@PORT@',
    '0x13200',
    '@AOSROOT@/out/@TARGET@/binary/@TARGET@.bin'
],

}
flash_configs['mk3060'] = mk3060

# Flash configs for erasing starterkit
starterkit_erase = {
'cmd': [
    {
        'Win32': '@AOSROOT@/build/cmd/win32/st-flash.exe',
        'Linux64': '@AOSROOT@/build/cmd/linux64/st-flash',
        'Linux32': '@AOSROOT@/build/cmd/linux32/st-flash',
        'OSX': '@AOSROOT@/build/cmd/osx/st-flash'
    },
    'erase',
],
}
flash_configs['starterkit_erase'] = starterkit_erase

# Flash configs for stm32* boards with ST-Link
stm32_stlink = {
'cmd': [
    {
        'Win32': '@AOSROOT@/build/cmd/win32/st-flash.exe',
        'Linux64': '@AOSROOT@/build/cmd/linux64/st-flash',
        'Linux32': '@AOSROOT@/build/cmd/linux32/st-flash',
        'OSX': '@AOSROOT@/build/cmd/osx/st-flash'
    },
    '--reset',
    'write',
    '@AOSROOT@/out/@TARGET@/binary/@TARGET@.bin',
    '0x08000000'
],
}
flash_configs['stm32_stlink'] = stm32_stlink

# TODO: Flash configs for stm32* boards with openocd
stm32_openocd = {
'cmd': [
    {
        'Win32': '@AOSROOT@/build/openocd_master/bin/openocd_win',
        'Linux64': '@AOSROOT@/build/openocd_master/bin/openocd_linux',
        'Linux32': '@AOSROOT@/build/openocd_master/bin/openocd_linux',
        'OSX': '@AOSROOT@/build/openocd_master/bin/openocd_osx'
    },
    '-f', '@AOSROOT@/build/openocd_master/share/openocd/scripts/board/st_nucleo_l4.cfg'
    '-c',
    'init; reset halt; flash write_image erase @AOSROOT@/out/@TARGET@/binary/@TARGET@.bin 0x08000000; reset run; reset; exit;'
],
}
#flash_configs['stm32_openocd'] = stm32_openocd

pca10040 = {
'cmd': [
    'python',
    '@AOSROOT@/build/site_scons/jlink.py',
    '-d', 'nRF52840_xxAA',
    '-i', 'swd',
    '-f', '@AOSROOT@/out/@TARGET@/binary/@TARGET@.bin',
    '-p', '0x00010000'
],
}
flash_configs['pca10040'] = pca10040

def main():
    # Write flash commands to json file
    for key in flash_configs:
        with open('upload/' + key + '.json', 'w') as f:
            json.dump(flash_configs[key], f, sort_keys=True, indent=4)

    # Write registered boards to json file
    with open('upload/registry_board.json', 'w') as f:
        json.dump(registry_board, f, sort_keys=True, indent=4)

if __name__ == "__main__":
    main()
