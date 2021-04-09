@page netmgr netmgr

# 概述

Netmgr是AliOS Things为支持不同的网络连接芯片类型和多元的应用场景，提供的一套完善的网络管理框架，包括网络接入管理、连接设备管理等功能。

## 版权信息

> Apache license v2.0

## 目录结构

```sh
├── example
│   └── netmgr_example.c           # netmgr示例代码
├── include
│   ├── netmgr.h                   # netmgr对外头文件
│   └── netmgr_wifi.h              # netmgr wifi相关的头文件
├── include_inner                  # netmgr内部使用的头文件
│   ├── netmgr_conn.h              
│   ├── netmgr_service.h
│   └── string_convert.h
├── package.yaml                   # netmgr编译配置文件
├── README.md                      # README文档
└── src                            # netmgr核心实现代码
    ├── netmgr_conn.c              # 连接管理
    ├── netmgr_eth.c               # 以太网网络管理
    ├── netmgr_gprs.c              # GPRS网络管理
    ├── netmgr_nbiot.c             # NBIoT网络管理
    ├── netmgr_service.c           # netmgr服务管理
    ├── netmgr_wifi.c              # wifi网络管理
    └── string_convert.c           # 字符转换
```
## 依赖组件

* drivers
* osal_aos
* uservice
* sntp
* posix

# 常用配置

系统中相关配置已有默认值，可以在yaml中**def_config**节点修改，具体如下：
> 是否上报激活信息
```sh
def_config:
  AOS_COMP_ACTIVATION: y
```
# API说明

@ref netmgr_api

# 使用示例
示例代码参考example/netmgr_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> netmgr组件的package.yaml中添加example
```sh
source_file:
  - src/netmgr_service.c
  - src/netmgr_wifi.c
  - src/netmgr_conn.c
  - src/string_convert.c
  - activation/*.c ? <AOS_COMP_ACTIVATION>
  - example/*.c
```
## app中添加netmgr组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - netmgr: dev_aos
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

## netmgr示例测试

@include netmgr_example.c

> CLI命令行输入netmgr_example，初始化netmgr服务
```sh
netmgr_example
```
> CLI命令行输入netmgr -t wifi -c wifi_ssid(根据情况输入自己的wifi名) wifi_password(输入wifi密码)
```sh
netmgr -t wifi -c HUAWEI_AP abcdefg1234
```
## 关键日志
> CLI日志：

```sh
netmgr test
```
```sh
Got IP
```

# 注意事项

- netmgr对系统的event service有依赖，需要在调用netmgr_service_init前，先调用event_service_init函数。

# FAQ

- NA
