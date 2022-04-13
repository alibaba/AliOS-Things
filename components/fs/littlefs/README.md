# 介绍
littlefs是一个高度完整的嵌入式文件系统，可以用在自身不带坏块处理、磨损平衡等功能的内存芯片上，如裸的NAND Flash芯片；同时littlefs也充分考虑了异常掉电情况下的数据保护。

# 特性
- 掉电安全
- 磨损平衡
- 体积小
- 坏快处理

# 目录
```sh
littlefs
|-- littlefs-v220      #littlefs v220版本源码
|-- platform_conf      #littlefs在各硬件平台上的配置信息，例如HaaS100。
|-- aos.mk             #AliOS Things的makefile
|-- Config.in          #AliOS Things的menuconfig配置文件
|-- littlefs_vfs.c     #little VFS虚拟文件操作接口的定义与实现。
|-- README.md          #中文版README
```
# 依赖
- rhino nftl

# 使用
(1) littlefs在AliOS Things中是独立的组件，可配置为kernel或者用户态APP中运行。

# 参考
N/A