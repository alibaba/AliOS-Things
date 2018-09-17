## Overview

Infrastructure for supporting SCons build.

### Directories

### Functions for upload

The upload functions are split into two parts:
* Independent upload logics reside in `scons_upload.py`;
* Board specific upload configs reside in `upload/*.json`.

A common upload process is:
1. Start upload with `aos upload ...`;
2. Parse board configs from `upload/*.json`;
3. Run board specific commands to finish upload process.

By using *upload configs*, users can add upload support easily. They only need to focus on board specific informations, such as flash commands, arguments, binaries location ..., but don't need to care about how the common logic runs.

For more details about *upload configs*, please refer to `upload/README.md`.

#### How to upload images to board

After build done, run below command to upload images to board:
```
$ aos upload appname@board
```

#### Add upload support for boards

Add upload support for boards, just creating upload configs for them simply. We strongly recommend that using `gen_upload_configs.py` to update or create upload configs:

Step 1: Update `gen_upload_configs.py`
```
# Register board with format:
#   { 'board name': [upload_config_file], }
# For example:

registry_board = {
    'esp32': ['esp32.json'],
    ...
}

# Create python dict for upload_config_file, for example:

# Flash configs for esp32* board
esp32 = {
'cmd': [
    'esptool.py',
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

```

Step 2: Run `gen_upload_configs.py` to create `upload/*.json`

```
$ cd build; ./gen_upload_configs.py
```

Step 3: Run `aos upload appname@board` to test the upload process.

### Functions for debug

Similiar with upload, the debug functions are split into:
* Independent debug logic reside in `scons_debug.py`;
* Board specific debug configs reside in `debug/*.json`.

For more details about *debug configs*, please refer to `debug/README.md`.

#### How to run debug with `aos-cube`

Start GDB server first:
```
$ aos debug appname@board
```

Open a new terminal and start GBD client:
```
$ aos debug application@board -c

# Connect to server and start debug:
gdb) target remote localhost:<port>
...
```

#### Add debug support for boards

Use `gen_debug_configs.py` to update or create debug configs for boards:

Step 1: Update `gen_debug_configs.py`
```
# Register board name with format:
#   { 'board name': [ debug_config_file ], }
# For example:

registry_board = {
    'developerkit': ['stm32l4x.json'],
}

# Create python dicts for debug_config_file, for example:

# Debug configs for stm32l4xx
stm32l4x = {
'start_server': [
    {
        'Linux64': {
            'cmd': '@AOSROOT@/build/openocd_master/bin/openocd_linux',
            'url': '', },
        'OSX': {
            'cmd': '@AOSROOT@/build/openocd_master/bin/openocd_osx',
            'url': '', },
    },
    '-f',
    '@AOSROOT@/build/openocd_master/share/openocd/scripts/board/stm32l4discovery.cfg',
],
'start_client': [
    {
        'Linux64': {
            'cmd': '@AOSROOT@/build/compiler/gcc-arm-none-eabi/Linux64/bin/arm-none-eabi-gdb',
            'url': '', },
        'OSX': {
            'cmd': '@AOSROOT@/build/compiler/gcc-arm-none-eabi/OSX/bin/arm-none-eabi-gdb',
            'url': 'https://gdb-archive.oss-cn-shanghai.aliyuncs.com/gdb-arm-none-eabi-osx.tar.gz', },
    },
    '@AOSROOT@/out/@TARGET@/binary/@TARGET@.elf'
]
}
debug_configs['stm32l4x'] = stm32l4x
```

Step 2: Run `gen_debug_configs.py` to create `debug/*.json`
```
$ cd build; ./gen_debug_configs.py
```

Step 3: Run `aos debug ...` to test debug process

## Reference

- [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide)
