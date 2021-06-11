@page ulog ulog

[更正文档](https://gitee.com/alios-things/ulog/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

ulog是AliOS Things的日志模块，同时支持经典同步机制和异步日志机制，异步机制使其他逻辑任务保持正常工作，基于简短的日志格式和syslog协议格式，日志内容可以通过不同方式输出（uart udp vfs uagent）， 此外，文件系统中的日志文件可以通过http上传至云端。
组件支持以下功能：
- 通过串口输出日志
- 通过UDP输出日志
- 通过文件系统输出日志
- 通过uagent输出日志到云端


## 版权信息
> Apache license v2.0

## 目录结构
```tree
ulog
├── README.md #ulog说明文档
├── example
│   └── ulog_example.c      #ulog使用demo
├── include
│   ├── ulog_config.h       #ulog相关配置头文件
│   └── ulog
│       └── ulog.h          #ulog使用API头文件
├── internal                #内部头文件
│   ├──ulog_api.h
│   ├──ulog_ring_fifo.h
│   └──ulog_session_file.h
├── src
│   ├── ulog_async.c        #ulog 异步日志输出功能
│   ├── ulog.c              #ulog 整体功能
│   ├── ulog_fs_cfg.c       #ulog 文件系统配置
│   ├── ulog_init.c         #ulog 初始化
│   ├── ulog_ring_fifo.c    #ulog ring fifo功能，供异步使用
│   ├── ulog_session_file.c #ulog 日志到文件系统
│   ├── ulog_session_udp.c  #ulog 日志通过UDP发出
│   └── ulog_utility.c      #ulog 通用功能
```

## 依赖组件
* vfs
* uagent
* linksdk
* fatfs

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 开启异步打印: 默认0, 如需修改，在yaml中修改ULOG_CONFIG_ASYNC配置
```sh
def_config:
  ULOG_CONFIG_ASYNC: 1
```
> 支持日志实时上传到云，默认不开启，可按照实际需要修改yaml配置如：
```sh
def_config:
  ULOG_CONFIG_POP_CLOUD: 0
  ULOG_CONFIG_STOP_FILTER_CLOUD: 5
```


# API说明

- 参考 [aos_ulog](https://g.alicdn.com/alios-things-3.3/doc/group__aos__ulog.html)

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

```sh
depends:
  - ulog: master # helloworld_demo中引入ulog组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ulog

```

上述命令执行成功后，组件源码则被下载到了./components/ulog路径中。

## 步骤4 添加示例

> ulog组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/ulog/tree/master/example)：

```yaml
source_file:
  - src/ulog_async.c? <ULOG_CONFIG_ASYNC>
  - src/ulog.c
  - src/ulog_fs_cfg.c? <ULOG_CONFIG_POP_FS>
  - src/ulog_init.c
  - src/ulog_ring_fifo.c
  - src/ulog_session_file.c? <ULOG_CONFIG_POP_FS>
  - src/ulog_session_udp.c? <ULOG_CONFIG_POP_UDP>
  - src/ulog_utility.c
  - example/ulog_example.c #add ulog example code
```
> ulog 设置日志输出等级，默认为不输出

```c
    aos_set_log_level(AOS_LL_DEBUG);
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
```sh
ulog_example_test
```
# FAQ
