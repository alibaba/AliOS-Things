@page sntp sntp

[更正文档](https://gitee.com/alios-things/sntp/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

简单网络时间协议（Simple Network Time Protocol），由NTP发展而来，为联网设备提供了一种同步网络时间的方式。AliOS Things为用户提供了sntp获取的接口，开发者在联网后，可方便获取网络时间。

## 版权信息

> Apache license v2.0

## 目录结构

```tree
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

```c
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

- 参考 [sntp_api](https://g.alicdn.com/alios-things-3.3/doc/group__sntp__api.html)

## 发送获取sntp请求

```C
int sntp_request(void *arg)
```

| args | description        |
| :--- | :----------------- |
| arg  | 用户数据结构体指针 |

## 获取sntp请求结果

```C
sntp_time *sntp_obtain_time(void)
```

| args | description |
| :--- | :---------- |
| void | NA          |

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
  - sntp: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install sntp

```

上述命令执行成功后，组件源码则被下载到了./components/sntp路径中。

## 步骤4 添加示例

> sntp组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/sntp/tree/master/example)：
```yaml
source_file:
    - src/*.c
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

> CLI命令行输入：
```sh
sntp_example
```
**关键日志**
> CLI日志：

```sh
sntp test
21-3-19: week: 5, 17:59:20
```
# 注意事项

使用sntp服务，需要先连接网络

# FAQ

- NA
