@page ulog ulog
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
```sh
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
@ref LOG
@ref LOGF
@ref LOGE
@ref LOGW
@ref LOGI
@ref LOGD
@ref aos_get_ulog_list
@ref ulog_man
@ref ulog


# 使用示例
示例代码参考example/ulog_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> ulog组件的package.yaml中添加example示例代码
```sh
source_file:
  - src/ulog_async.c? <ULOG_CONFIG_ASYNC>
  - src/ulog.c
  - src/ulog_fs_cfg.c? <ULOG_CONFIG_POP_FS>
  - src/ulog_init.c
  - src/ulog_ring_fifo.c
  - src/ulog_session_file.c? <ULOG_CONFIG_POP_FS>
  - src/ulog_session_udp.c? <ULOG_CONFIG_POP_UDP>
  - src/ulog_utility.c
  - example/ulog_example.c #add example code
```
> ulog 设置日志输出等级，默认为不输出
```c
    aos_set_log_level(AOS_LL_DEBUG);
````


## app中添加ulog组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ulog: master # helloworld_demo中引入ulog组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档

## ulog示例测试

### CLI命令行输入：
```sh
ulog_example_test
```
# FAQ
