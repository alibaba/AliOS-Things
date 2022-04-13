# 介绍
该工程是用来构建微内核系统的网络服务进程，该进程为用户应用提供基础的网络服务能力，包含lwip网络协议栈、网络管理以及wifi驱动。

# 特性
- 网络服务进程运行在用户态，可实现网络服务与内核的隔离以及网络服务与用户进程的隔离，提高系统的安全行。
- 用户应用程序可直接在网络服务进程中通过函数调用的方式直接访问系统网络资源
- 用户应用程序可在其他进程通过rpc机制间接的访问网络服务进程中的系统网络资源

# 目录
ulwip_service
├── README.md           #要点介绍
├── src                 #功能源码，提供网络服务进程的应用入口。
├── autobuild.json      #自动化编译配置
├── Config.in           #网络服务进程的构建配置
└── aos.mk              #网络服务进程的构建脚本

# 依赖
- lwip
- netmgr
- netmgr_rpc
- uspace
- uspace_lib
- cli
- activation
- devicevfs
- vfs
- v_common_drv
- haas100-mk.config.partition

# 使用
    aos make ulwip_service@haas100-mk -c config
    aos make MBINS=app MBINS_APP=network

# 参考
N/A