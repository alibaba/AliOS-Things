# 介绍
alios_kernel是AliOS Things的微内核的默认的内核APP。

# 特性
- 提供支持用户态应用运行必要的初始化
- 加载第一个用户态应用

# 目录
```sh
alios_kernel/
|-- alios_kernel.c           #内核APP的初始化代码
|-- aos.mk                   #Makefile
|-- autobuild.json
|-- Config.in                #Menuconfig 配置文件
|-- k_app_config.h           #内核APP的配置文件
|-- pm_load.c                #加载第一个用户态进程(PM进程)
|-- README.md
|-- rpc_benchmark.c          #测试代码
|-- rpc_benchmark_client.c   #测试代码
|-- rpc_benchmark_server.c   #测试代码
`-- sqlitedemo.c             #测试代码
```
# 依赖
- osal_aos
- uspace
- ulog
- cli
- irq
- kv
- vfs
- romfs

# 使用
编译后烧录到板子上，如在HaaS100板子可以参考[HaaS100快速开发](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.641.7aeb37dd8XJm7T)

# 参考
N/A
