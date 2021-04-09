@page sntp sntp

# 概述

简单网络时间协议（Simple Network Time Protocol），由NTP发展而来，为联网设备提供了一种同步网络时间的方式。AliOS Things为用户提供了sntp获取的接口，开发者在联网后，可方便获取网络时间。

## 版权信息

> Apache license v2.0

## 目录结构

```sh
├── example
│   └── sntp_example.c # sntp使用示例代码
├── include
│   └── sntp
│       └── sntp.h     # sntp头文件
├── package.yaml       # 编译配置文件
├── README.md          # README文档
└── src
    └── sntp.c         # sntp核心实现代码
```
## 依赖组件

* lwip

# 常用配置

```
// sntp.h

SNTP端口号：
#define SNTP_PORT                     123

SNTP是否支持多服务器，以及服务器的个数
#define SNTP_SUPPORT_MULTIPLE_SERVERS 1
#if SNTP_SUPPORT_MULTIPLE_SERVERS
#define SNTP_MAX_SERVERS              3
#endif

SNTP接收超时时间
#define SNTP_RECV_TIMEOUT             3000 /* ms */

SNTP重传时的接收超时
#define SNTP_RETRY_TIMEOUT            SNTP_RECV_TIMEOUT

SNTP最大重传次数
#define SNTP_RETRY_TIMES              3

SNTP服务器地址
#define SNTP_SERVER_ADDRESS          "pool.ntp.org"
```

# API说明

@ref sntp_api

# 使用示例
示例代码参考example/sntp_example.c，以运行link_ntp_demo为例，具体步骤如下：

## 添加示例代码

> sntp组件的package.yaml中添加example
```sh
source_file:
    - src/*.c
    - example/*.c
```
## app中添加sntp组件

> link_ntp_demo组件的package.yaml中添加
```sh
depends:
  - sntp: dev_aos
```
## 编译

```sh
cd solutions/link_ntp_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make link_ntp_demo@haas100 -c config
```
## 烧录固件

> 参考具体板子的快速开始文档。

## sntp示例测试

> CLI命令行输入：
```sh
sntp_example
```
## 关键日志
> CLI日志：

```sh
sntp test
21-3-19: week: 5, 17:59:20
```
# 注意事项

使用sntp服务，需要先连接网络

# FAQ

- NA
