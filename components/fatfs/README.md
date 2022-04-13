@page fatfs

## 概述
**fatfs** is a generic FAT/exFAT filesystem module for small embedded system. The fatfs module is written in compliance with ANSI C (C89). It attempts to provide AOS-compatible filesystem behaviour.
组件支持以下功能
- DOS/Windows compatible FAT/exFAT filesystem
- Very small footprint for program code and work area
- Various configuration options to support for:
    - Long file name in ANSI/OEM or Unicode.
    - exFAT filesystem.
    - Thread safe for RTOS.
    - Multiple volumes (physical drives and partitions)
    - Variable sector size.
    - Multiple code page including DBCS.
    - Read-only, optional API, I/O buffer and etc...

## 版权信息
> Apache license v2.0

### 目录结构
```sh
fatfs
├── src
│   ├── mmc_disk
│   │   ├── mmc_disk.c
│   │   └── mmc_disk.h
│   ├── ram_disk
│   │   ├── ram_disk.c
│   │   └── ram_disk.h
│   ├── sd_disk
│   │   ├── sd_disk.c
│   │   └── sd_disk.h
│   └── usb_disk
│       ├── usb_disk.c
│       └── usb_disk.h
├── include
│   ├── diskio.h
│   ├── fatfs_diskio.h
│   ├── fatfs.h
│   ├── ff.h
│   ├── ffconf.h
│   ├── integer.h
├── package.yaml     # 编译配置文件
├── example
│    └── fatfs_example.c # 示例代码
└─── README.md
```

## 依赖组件
* minilibc: v7.2.0
* aos: >=v7.2.0

## 配置信息
def_config:
  CONFIG_AOS_FATFS_SUPPORT_MMC: 1
  AOS_COMP_VFS: 1
  AOS_COMP_FATFS: 1

# 使用示例
示例代码参考example/fatfs_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> fatfs组件的package.yaml中在source_file部分添加example
```sh
source_file:
  - "src/*.c"
  - "example/fatfs_example.c" # add fatfs_example.c
```

## app中添加fatfs组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - fatfs: master # helloworld_demo中引入fatfs组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## 示例测试
> CLI命令行输入：
```sh
fatfs_example
```

## 关键日志
> CLI日志：
```sh
fatfs component example start!
aos_open fd1=xxx
aos_write num=xxx
aos_open fd2=xxx
aos_lseek position=xxx
aos_read: xxxx
fatfs component example end !
```

