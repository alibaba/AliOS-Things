@page websocket websocket

[更正文档](https://gitee.com/alios-things/http/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)


# 概述

websocket是一个用C实现的轻量级的websocket客户端，支持TLS。

## 版权信息
> MIT License

## 目录结构
```tree
.
├── examples
│   └── test_librws_creation.c   # 测试代码
├── include
│   ├── librws.h                 # 对外头文件
│   ├── rws_common.h             # 内部头文件
│   ├── rws_error.h              # 内部头文件
│   ├── rws_frame.h              # 内部头文件
│   ├── rws_list.h               # 内部头文件
│   ├── rws_memory.h             # 内部头文件
│   ├── rws_network.h            # 内部头文件
│   ├── rws_socket.h             # 内部头文件
│   ├── rws_string.h             # 内部头文件
│   └── rws_thread.h             # 内部头文件
├── package.yaml
├── README.md
└── src
    ├── librws.c
    ├── rws_common.c
    ├── rws_error.c              # 错误处理
    ├── rws_frame.c              # 帧处理
    ├── rws_list.c               # 链表
    ├── rws_memory.c             # 内存
    ├── rws_socketpriv.c         # 主处理流程
    ├── rws_socketpub.c          # 对外接口
    ├── rws_string.c             # 字符串
    └── rws_thread.c             # 线程
```

## 依赖组件

* posix
* ulog
* mbedtls

# 常用配置
websocket的常用配置在文件**package.yaml内**
> WEBSOCKET_SSL_ENABLE ：是否开启SSL支持，默认开启
```yaml
WEBSOCKET_SSL_ENABLE: 1
```

# API说明
- 参考 [websocket_api](https://g.alicdn.com/alios-things-3.3/doc/group__websocket__api.html)

## 创建websocket上下文所需空间，返回具柄。注意，此时尚未连接。
```C
rws_socket rws_socket_create(void)
```

|args                                    |description|
|:-----                                  |:----|
|void                                    |N/A|

## 设置websocket连接URL信息，包括安全/非安全模式、域名、端口、路径
```C
void rws_socket_set_url(rws_socket socket, 
                        const char * scheme,
                        const char * host,
                        const int port,
                        const char * path)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|scheme                                  |模式，"ws"或者"wss"|
|host                                    |域名，如"echo.websocket.org"|
|port                                    |端口，如443|
|path                                    |路径，如"/"|

## 设置websocket连接安全/非安全模式。
```C
void rws_socket_set_scheme(rws_socket socket, const char * scheme)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|scheme                                  |模式，"ws"或者"wss"|

## 设置websocket连接域名。
```C
void rws_socket_set_host(rws_socket socket, const char * host)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|host                                    |域名，如"echo.websocket.org"|

## 设置websocket连接端口。
```C
void rws_socket_set_port(rws_socket socket, const int port)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|port                                    |端口，如443|

## 设置websocket连接路径。
```C
void rws_socket_set_path(rws_socket socket, const char * path)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|port                                    |路径，如"/"|

## 设置websocket连接请求报文中协议字段。
```C
void rws_socket_set_protocol(rws_socket socket, const char * protocol)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|protocol                                |协议字段，如"echo-protocol"或 "chat,  superchat"|


## 获取websocket连接域名。
```C
const char * rws_socket_get_host(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 获取websocket连接模式。
```C
const char * rws_socket_get_host(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 获取websocket连接端口。
```C
int rws_socket_get_port(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 获取websocket连接路径。
```C
const char * rws_socket_get_path(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 设置websocket在安全模式下服务端的证书。
```C
void rws_socket_set_server_cert(rws_socket socket, const char *server_cert, int server_cert_len)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|server_cert                             |服务证书|
|socket                                  |服务证书长度|

## 获取websocket当前错误。
```C
rws_error rws_socket_get_error(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 启动连接websocket。
```C
rws_bool rws_socket_connect(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 断开websocket连接并释放资源，释放后勿再操作该具柄。
```C
void rws_socket_disconnect_and_release(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 判断该websocket是否已连接。
```C
rws_bool rws_socket_is_connected(rws_socket socket);
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|


## 向服务端发送text数据。
```C
rws_bool rws_socket_send_text(rws_socket socket, const char * text)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|text                                    |需要发送的text数据|

## 开始向服务端发送二进制数据，该接口需要和rws_socket_send_bin_continue、rws_socket_send_bin_finish配合使用。
```C
rws_bool rws_socket_send_bin_start(rws_socket socket, const char *bin, size_t len)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|bin                                     |需要发送二进制数据|
|len                                     |二进制数据长度|

## 继续向服务端发送二进制数据，该接口需要和rws_socket_send_bin_start、rws_socket_send_bin_finish配合使用。
```C
rws_bool rws_socket_send_bin_continue(rws_socket socket, const char *bin, size_t len)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|bin                                     |需要发送二进制数据|
|len                                     |二进制数据长度|

## 向服务端发送结束二进制数据，该接口需要和rws_socket_send_bin_start、rws_socket_send_bin_continue配合使用。
```C
rws_bool rws_socket_send_bin_finish(rws_socket socket, const char *bin, size_t len)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|bin                                     |需要发送二进制数据|
|len                                     |二进制数据长度|

## 向服务端发送ping包。
```C
rws_bool rws_socket_send_ping(rws_socket socket)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|

## 设置websocket连接成功后的回调函数。
```C
void rws_socket_set_on_connected(rws_socket socket, rws_on_socket callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|

## 设置websocket连接断开后的回调函数。
```C
void rws_socket_set_on_disconnected(rws_socket socket, rws_on_socket callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|

## 设置websocket成功发送ping后回调函数。
```C
void rws_socket_set_on_send_ping(rws_socket socket, rws_on_socket_send_ping callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|

## 设置websocke接收到text数据的回调函数。
```C
void rws_socket_set_on_received_text(rws_socket socket, rws_on_socket_recvd_text callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|


## 设置websocke接收到bin数据的回调函数。
```C
void rws_socket_set_on_received_bin(rws_socket socket, rws_on_socket_recvd_bin callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|


## 设置websocke接收到pong消息的回调函数。
```C
void rws_socket_set_on_received_pong(rws_socket socket, rws_on_socket_recvd_pong callback)
```

|args                                    |description|
|:-----                                  |:----|
|socket                                  |websocket具柄|
|callback                                |回调函数|


## 获取错误码。
```C
int rws_error_get_code(rws_error error)
```

|args                                    |description|
|:-----                                  |:----|
|error                                   |错误|

## 获取http错误码
```C
int rws_error_get_http_error(rws_error error)
```

|args                                    |description|
|:-----                                  |:----|
|error                                   |错误|

## 获取获取错误描述
```C
const char * rws_error_get_description(rws_error error)
```

|args                                    |description|
|:-----                                  |:----|
|error                                   |错误|

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
  - netmgr: master        # helloworld_demo中引入netmgr组件，用于WiFi联网
  - websocket: master     # helloworld_demo中引入websocket组件
  - lwip: master          # helloworld_demo中引入lwip组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install websocket

```

上述命令执行成功后，组件源码则被下载到了./components/websocket路径中。

## 步骤4 添加示例

在websocket组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/http/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/*.c" # add websocket_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 websocket测试示例
> CLI命令行输入初始化netmgr命令：

```shell
netmgr_example
```

> CLI命令行输入联网命令：
```shell
netmgr -t wifi -c ssid password
```

> 然后，CLI命令行输入：
```shell
websocket_example
```

**关键日志**
> CLI日志：
```shell
example websocket test success!
```

# FAQ

@ref 操作系统内核
