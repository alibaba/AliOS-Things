@page flash flash

[更正文档](https://gitee.com/alios-things/flash/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
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

- 参考 [flash_device_api](https://g.alicdn.com/alios-things-3.3/doc/group__flash__device__api.html)

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
  - flash: master          # helloworld_demo中引入flash组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install flash

```

上述命令执行成功后，组件源码则被下载到了./components/flash路径中。

## 步骤4 添加示例

在flash组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/flash/tree/master/example)：

```yaml
source_file:
  - src/*.c
  - example/flash_example.c
  - example/mtd_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell

flash_example <分区号> # 执行flash示例

```

> 关键日志：
```shell

vfs_flash_test success!

```

# 注意事项
建议应用通过文件系统来读取或写入文件，或者通过KV组件提供的接口操作Flash， 不要直接使用Flash驱动子系统提供的接口直接操作Flash。 因为直接操作裸的Flash需要应用自身考虑磨损平衡，坏块管理等功能，否则可能导致Flash损坏。另外一些设备的认证信息，WIFI MAC，出厂信息等也存在Flash中，如果直接操作Flash不当，可能导致这些信息丢失，造成设备不可用。

# FAQ
无
