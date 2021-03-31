@page spi spi

# 概述
本组件为SPI VFS驱动子系统。给应用或组件提供通过VFS形式（open/close/ioctl）访问硬件SPI驱动对上层提供服务的接口。该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_SPI_NUM来依此创建如下名称的设备节点：
* /dev/spi0
* /dev/spi1
* ...
* /dev/spi[CONFIG_SPI_NUM - 1]

组件支持以下功能：
- 设置主从模式
- 设置时序模式（CPOL/CPHA）
- 是否使能DMA
- 设置数据BIT宽度
- CS片选
- 数据发送
- 数据接收
- 同时发送和接收

## 版权信息
> Apache license v2.0

## 目录结构
```sh
.
.
├── example
│   └── spi_example.c    #SPI示例代码
├── include
│   ├── aos
│   │   └── hal
│   │       └── spi.h    #SPI HAL API申明
│   └── vfsdev
│       └── spi_dev.h    #SPI设备API申明
├── package.yaml         #编译和配置文件
├── README.md            #使用文档
└── src
    └── spi_dev.c        #SPI设备实现代码

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
本组件可以通过CONFIG_SPI_NUM配置对上层提供设备节点的数量，CONFIG_SPI_NUM代表芯片内部的SPI控制器的数量。
CONFIG_SPI_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
如果没有定义CONFIG_SPI_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_SPI_NUM配置
```sh
def_config:
  CONFIG_SPI_NUM: 1
```

# API说明
@ref spi_device_api

# 使用示例
示例代码参考example/spi_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> spi组件的package.yaml中添加example
```sh
source_file:
  原有其他文件
  - example/spi_example.c # add spi_example.c
```

## 添加spi组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - spi: master # helloworld_demo中引入spi组件
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

## SPI示例测试
> CLI命令行输入：
```sh
spi_example id
```
其中id为0-CONFIG_TIMER_NUM之间的数值。

可使用示波器确认通过SPI总线发送的数据是否是spi_example中发送的数据一致来最终确认SPI是否工作正常。

## 关键日志
> CLI日志：
```sh
vfs spi test success, ret:0
```

# 注意事项
无

# FAQ
无
