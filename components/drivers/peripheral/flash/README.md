@page flash flash

# 概述
Flash（闪存）是一种速度非常快的非易失性存储设备，允许在操作中被多次擦或写的存储器，目前被广泛应用于嵌入式设备中。
该组件是Flash VFS驱动子系统，目前支持Nor Flash 和 Nand Flash给应用或组件提供通过VFS形式（open/close/ioctl）访问Flash设备驱动对上层提供服务的接口。该组件初始化过程中，会根据板子中定义的分区数量来依此创建如下名称的设备节点：
* /dev/flash0
* /dev/flash1
* ...
* /dev/flashN

组件支持以下功能：
- 从Flash设备某个分区的区域读数据
- 向Flash设备某个分区的区域写数据
- 擦除Flash设备某个分区的区域
- 获取Flash设备的信息
- 打开或关闭Flash设备区域的安全功能
- 设置/清理/获取Flash设备上的Boot信息

## 分区定义
操作Flash需要通过分区号作为Flash设备的索引，所以操作Flash设备前需要了解板子的分区配置。
AliOS Things的全部标准分区在components/drivers/peripheral/flash/include/aos/hal/flash.h中的hal_partition_t定义，
但是这些分区每个板子不一定全都配置了，具体需要参考板子的分区配置，hardware/board/目录下的每个板子都会定义自己的分区在hal_partitions数组中。
比如haas100的分区配置在hardware/board/haas100/config/partition_conf.c 的hal_partitions。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
flash/
|-- example
|   |-- flash_example.c   # Flash示例代码
|-- include
|   |-- aos
|   |   |-- hal
|   |       |-- flash.h   # Flash HAL API申明
|   |-- vfsdev
|       |-- flash_dev.h   # Flash设备API申明
|-- package.yaml          #编译及配置文件
|-- README.md             #README文档
|-- src
    |-- flash_dev.c       #Flash设备子系统实现
```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库
* epoll    # 增强型多路复用IO接口 - epoll机制所在库

# 常用配置
无

# API说明
@ref flash_device_api

# 使用示例
示例代码参考example/flash_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> flash组件的package.yaml中添加example
```sh
source_file:
  原有其他文件
  - example/flash_example.c # add flash_example.c
```

## 添加flash组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - flash: master # helloworld_demo中引入flash组件
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

## KV示例测试
> CLI命令行输入：
```sh
flash_example <分区号>
```

## 关键日志
> CLI日志：
```sh
vfs_flash_test success!
```

# 注意事项
建议应用通过文件系统来读取或写入文件，或者通过KV组件提供的接口操作Flash， 不要直接使用Flash驱动子系统提供的接口直接操作Flash。 因为直接操作裸的Flash需要应用自身考虑磨损平衡，坏块管理等功能，否则可能导致Flash损坏。另外一些设备的认证信息，WIFI MAC，出厂信息等也存在Flash中，如果直接操作Flash不当，可能导致这些信息丢失，造成设备不可用。

# FAQ
无
