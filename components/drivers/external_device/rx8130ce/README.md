@page rx8130ce rx8130ce

[更正文档](https://gitee.com/alios-things/rx8130ce/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

RX8130CE是一颗RTC芯片，基于I2C总线进行通信，其内建了32.768KHZ的晶振，功能完备，HaaS100的板上就使用了这颗。本文档描述了该芯片的驱动详情，包括目录结构，依赖情况，API，使用等情况，供用户参考。
组件支持以下功能：

- rx8130ce初始化
- 设置时间
- 读取时间

## 版权信息

> Apache license v2.0

## 目录结构

```tree
├── example
│   └── rx8130ce_example.c
├── include
│   └── rx8130ce.h
├── internal
│   └── rtcif.h
├── package.yaml
├── README.md
└── src
    ├── rtcif.c
    └── rx8130ce.c
```

## 依赖组件

- base
- vfs
- pca9544


# 常用配置

# API说明

```c
/*********************************************************
 * @fun    rx8130ce_init
 * @breif  rx8130ce initialization
 * @param  none
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_init(void);

/*********************************************************
 * @fun    rx8130ce_set_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_set_time(const void *buf, uint8_t len);

/*********************************************************
 * @fun    rx8130ce_get_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_get_time(void *buf, uint8_t len);
```

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加rx8130ce组件

> helloworld_demo组件的package.yaml中添加

```bash
depends:
  - rx8130ce: master # helloworld_demo中引入rx8130ce组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install rx8130ce

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/external_device路径中。

## 步骤4 添加示例代码

> 向rx8130ce组件的package.yaml中添加example

```bash
source_file:
  - "src/*.c"
  - "example/rx8130ce_example.c" # add rx8130ce_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 RX8130CE示例测试

> CLI命令行输入：

```bash
rx8130ce_example
```

> CLI日志：

```bash
rx8130ce comp test success!
```
