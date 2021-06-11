@page spi spi

**[更正文档](https://gitee.com/alios-things/spi/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

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
```tree
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

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：
```yaml
depends:
  - spi: master # helloworld_demo中引入spi组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install spi

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/spi路径中。

## 步骤4 添加示例
> spi组件的package.yaml中添加example
```yaml
source_file:
  原有其他文件
  - example/spi_example.c # add spi_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

> CLI命令行输入：
```sh
spi_example id
```
其中id为0-CONFIG_TIMER_NUM之间的数值。

可使用示波器确认通过SPI总线发送的数据是否是spi_example中发送的数据一致来最终确认SPI是否工作正常。

**关键日志**
> CLI日志：
```sh
vfs spi test success, ret:0
```

# 注意事项
无

# FAQ
无
