## Contents

## Introduction
**vfs** provides a unified user interface for variout files (including device files and system files). It is an abstraction layer on top of a more concrete file system and device files. The purpose is to allow application to access different types of concrete file system and devices in a uniform way.

### Features
- normally file system operations
- normally device file operations
- inode management
- file descriptor management

### Directories

```sh
vfs
├── include
│   ├── vfs_adapt.h   # vfs OS adapt layer declaration
│   ├── vfs_api.h     # vfs module interface declaration
│   ├── vfs_conf.h    # vfs configuration
│   ├── vfs_err.h     # vfs errno defination
│   └── vfs_types.h   # vfs public structure defination
├── vfs_adapt.c       # vfs OS adapt layer implementation
├── vfs.c             # vfs core logic
├── vfs_device_adc.c  # vfs adc device abstract implementation
├── vfs_device_gpio.c # vfs gpio device abstract implementation
├── vfs_device_i2c.c  # vfs i2c device abstract implementation
├── vfs_device_pwm.c  # vfs pwm device abstract implementation
├── vfs_device_rtc.c  # vfs rtc device abstract implementation
├── vfs_device_spi.c  # vfs spi device abstract implementation
├── vfs_device_uart.c # vfs uart device abstract implementation
├── vfs_device_wdg.c  # vfs wdg device abstract implementation
├── vfs_file.c        # vfs file descriptor management implementation
├── vfs_file.h        # vfs file related interface declaration
├── vfs_inode.c       # vfs inode management
├── vfs_inode.h       # vfs inode related interface declaratin
└── vfs.mk

```

### Dependencies

## Reference
- [AliOS Things VFS Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-VFS-Porting-Guide)
