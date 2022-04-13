# 介绍
该工程是用来提供基本的进程管理服务，包括进程的加载和卸载。
此APP相对其他APP比较特别，其特殊性在于：
1、默认情况下，其需要绑定内核进程加载，即内核kernel进程起来后，需要默认启动该进程；
2、该进程为其他所有APP用户态进程提供加卸载服务，即所有用户APP都通过该进程来加载或者卸载。
3、提供路径管理服务：所有APP打开的设备或者文件系统根路径都会在PM进程注册。后续所有对应的操作比如open\write类似操作都会根据PM提供的路径管理来查找目标服务进程。

# 特性
- 用户进程的启动、卸载以及管理服务
- 设备或文件系统路径管理服务

# 目录
├── aos.mk
├── autobuild.json
├── Config.in
├── include
│   ├── u_api.h
│   └── u_config.h
└── src
    ├── app.c                           # app的入口：application_start
    ├── entry_bin.c                     # app编译为bin时使用
    ├── entry_elf.c                     # app编译为elf时使用
    ├── pathserv                        # 设备或文件系统路径管理服务
    │   ├── path_manager.c              # PM路径管理
    │   └── path_manager.h
    ├── procm_load                      # 进程加卸载实现
    │   ├── procm_ash.c                 # 自动解析aos.d，加载默认的APP
    │   └── procm_load.c                # PM加卸载
    └── procserv
        ├── process_cmd.c               # PM的cli命令，执行优先级高于其他所有进程，包括kernel
        ├── process_map.c               # PM的进程管理
        ├── process_service.c           # PM的RPC实现
        └── process_service.h

# 依赖
- uspace_lib
- usyscall
- uspace_pm
- ulog
- cli
- devicevfs
- vfs

# 使用

编译命令：
aos make procmgmt@haas100-mk -c config && aos make MBINS=app MBINS_APP=procmgmt
其中procmgmt指定了链接脚本的名字：procmgmt.ld.S

需要将其二进制拷贝到ROMFS目录，kernel进程会默认在此路径下查找procmgmt.app，并启动PM服务。
cp ./out/procmgmt@haas100-mkapp/binary/procmgmt@haas100-mk.app.bin  ./components/fs/romfs/rootfs/usr/procmgmt.app

如果要配置系统中默认启动加载的APP，将相关命令加入vendor/haas/mcu/prebuild/data/aos.d
如：loadm /data/ulwip_service@haas100-mk.app.bin

则kernel进程启动后，会先查找/usr/procmgmt.app来启动PM进程，PM进程会根据/data/aos.d中的配置，来启动默认的APP进程。

# 参考
N/A