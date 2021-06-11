@page netmgr netmgr

[更正文档](https://gitee.com/alios-things/netmgr/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

Netmgr是AliOS Things为支持不同的网络连接芯片类型和多元的应用场景，提供的一套完善的网络管理框架，包括网络接入管理、连接设备管理等功能。

## 版权信息

> Apache license v2.0

## 目录结构

```tree
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
```yaml
def_config:
  AOS_COMP_ACTIVATION: y
```
# API说明

- 参考 [netmgr_api](https://g.alicdn.com/alios-things-3.3/doc/group__netmgr__api.html)

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
  - netmgr: master
```
## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install netmgr

```

上述命令执行成功后，组件源码则被下载到了./components/netmgr路径中。

## 步骤4 添加示例

> netmgr组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/netmgr/tree/master/example)：

```sh
source_file:
  - src/netmgr_service.c
  - src/netmgr_wifi.c
  - src/netmgr_conn.c
  - src/string_convert.c
  - activation/*.c ? <AOS_COMP_ACTIVATION>
  - example/*.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

> CLI命令行输入netmgr_example，初始化netmgr服务
```sh
netmgr_example
```
> CLI命令行输入netmgr -t wifi -c wifi_ssid(根据情况输入自己的wifi名) wifi_password(输入wifi密码)
```sh
netmgr -t wifi -c HUAWEI_AP abcdefg1234
```
**关键日志**
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
