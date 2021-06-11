@page http http

[更正文档](https://gitee.com/alios-things/http/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

HTTP(HyperText Transfer Protocol)是一款用于传输超文本的应用层协议。AliOS Things的网络协议栈包含了HTTP组件，提供HTTP客户端标准能力。开发者可以通过组件API，在设备端快速实现通过HTTP GET、POST等方法与服务端进行数据交互。例如，设备端发起HTTP GET请求https://www.aliyun.com/，获取网页数据。

该组件支持以下功能：
- HTTP GET
- HTTP HEAD
- HTTP POST
- HTTP PUT

## 版权信息
> Apache 2.0 License

## 目录结构
```tree
.
├── include
│   └── httpclient.h          # 对外头文件
├── internal                  # 内部头文件
├── package.yaml              # 编译配置文件
└── src
    ├── http_aos_wrapper.c    # http收发适配层
    ├── http_client.c         # http核心处理层
    ├── http_formdata.c       # http表单数据处理
    └── http_method_api.c     # http方法接口
```

## 依赖组件

* osal_aos
* mbedtls

# 常用配置
http的常用配置在文件**package.yaml内**
> CONFIG_HTTP_SECURE ：是否开启HTTPS支持，默认开启
```yaml
CONFIG_HTTP_SECURE: 1
```
> CONFIG_HTTP_FILE_OPERATE ：是否使用文件操作，默认关闭
```yaml
CONFIG_HTTP_FILE_OPERATE: 0
```

http的内部配置在文件**internal/http_opts.h内**
> HTTPCLIENT_AUTHB_SIZE ： http认证数据（用户名、密码）长度
```c
#ifndef HTTPCLIENT_AUTHB_SIZE
#define HTTPCLIENT_AUTHB_SIZE     128
#endif
```
> HTTPCLIENT_CHUNK_SIZE ： http chunk块数据大小
```c
#ifndef HTTPCLIENT_CHUNK_SIZE
#define HTTPCLIENT_CHUNK_SIZE     1024
#endif
```
> HTTPCLIENT_SEND_BUF_SIZE ： http发送缓存大小
```c
#ifndef HTTPCLIENT_SEND_BUF_SIZE
#define HTTPCLIENT_SEND_BUF_SIZE  512
#endif
```
> HTTPCLIENT_MAX_HOST_LEN ： URL中host段最大长度
```C
#ifndef HTTPCLIENT_MAX_HOST_LEN
#define HTTPCLIENT_MAX_HOST_LEN   64
#endif
```
> HTTPCLIENT_MAX_URL_LEN ： URL最大长度
```C
#ifndef HTTPCLIENT_MAX_URL_LEN
#define HTTPCLIENT_MAX_URL_LEN    512
#endif
```

> HTTPCLIENT_MAX_RECV_WAIT_MS ： 接收超时时间
```C
#ifndef HTTPCLIENT_MAX_RECV_WAIT_MS
#define HTTPCLIENT_MAX_RECV_WAIT_MS 5000
#endif
```
> HTTP_PORT ： HTTP默认端口
```C
#ifndef HTTP_PORT
#define HTTP_PORT   80
#endif
```

> HTTPS_PORT ： HTTPS默认端口
```C
#ifndef HTTPS_PORT
#define HTTPS_PORT  443
#endif
```

# API说明
- 参考 [aos_httpclient_api](https://g.alicdn.com/alios-things-3.3/doc/group__aos__httpclient__api.html)


## 分配HTTP请求头部缓存和响应缓存
```C
HTTPC_RESULT httpclient_prepare(httpclient_data_t *client_data, int header_size, int resp_size)
```

|args                                    |description|
|:-----                                  |:----|
|client_data                             |用户数据结构体指针|
|header_size                             |头部大小|
|resp_size                               |回复大小|

## 释放HTTP请求头部缓存和响应缓存
```C
HTTPC_RESULT httpclient_unprepare(httpclient_data_t *client_data);
```

|args                                    |description|
|:-----                                  |:----|
|client_data                             |用户数据结构体指针|

## 重置HTTP请求头部缓存和响应缓存
```C
void httpclient_reset(httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client_data                             |用户数据结构体指针|

## 向URL发送GET请求，等待回复。
```C
HTTPC_RESULT httpclient_get(httpclient_t *client, char *url, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|
|client_data                             |用户数据结构体指针|

## 向URL发送POST请求，等待回复。
```C
HTTPC_RESULT httpclient_post(httpclient_t *client, char *url, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|
|client_data                             |用户数据结构体指针|

## 向URL发送PUT请求，等待回复。
```C
HTTPC_RESULT httpclient_put(httpclient_t *client, char *url, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|
|client_data                             |用户数据结构体指针|

## 向URL发送DELETE请求，等待回复。
```C
HTTPC_RESULT httpclient_delete(httpclient_t *client, char *url, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|
|client_data                             |用户数据结构体指针|

## 建立连接
```C
HTTPC_RESULT httpclient_conn(httpclient_t *client, const char *url)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|


## 发送请求
```C
HTTPC_RESULT httpclient_send(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|url                                     |URL地址|
|method                                  |请求方法，具体值参考枚举HTTP_REQUEST_TYPE|
|client_data                             |用户数据结构体指针|

## 接收响应
```C
HTTPC_RESULT httpclient_recv(httpclient_t *client, httpclient_data_t *client_data)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|client_data                             |用户数据结构体指针|

## 关闭连接
```C
void httpclient_clse(httpclient_t *client)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|

## 设置请求自定义头部
```C
void httpclient_set_custom_header(httpclient_t *client, char *header)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|
|header                                  |自定义header|

## 获取响应码
```C
int httpclient_get_response_code(httpclient_t *client)
```

|args                                    |description|
|:-----                                  |:----|
|client                                  |HTTP client上下文，包含配置参数，如服务端口号、服务端证书等|

## 获取响应头部字段值
```C
int httpclient_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len)
```

|args                                    |description|
|:-----                                  |:----|
|header_buf                              |响应头部缓存|
|name                                    |字段名|
|val_pos                                 |字段起始位置|
|val_len                                 |字段长度|

## 添加文本表单数据
```C
int httpclient_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len);
```

|args                                    |description|
|:-----                                  |:----|
|client_data                             |用户字段指针|
|content_disposition                     |待添加的内容地址|
|content_type                            |内容类型|
|name                                    |名称的地址|
|data                                    |表单数据地址|
|data_len                                |表单数据长度|

## 添加文件表单数据
```C
int httpclient_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path);
```

|args                                    |description|
|:-----                                  |:----|
|client_data                             |用户字段指针|
|content_disposition                     |待添加的内容地址|
|name                                    |名称的地址|
|content_type                            |内容类型|
|file_path                               |文件路径|


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
  - netmgr: master   # helloworld_demo中引入netmgr组件，用于WiFi联网
  - http: master     # helloworld_demo中引入http组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install http

```

上述命令执行成功后，组件源码则被下载到了./components/http路径中。

## 步骤4 添加示例

在http组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/http/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/http_example.c" # add http_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 http测试示例
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
http_example
```

**关键日志**
> CLI日志：
```shell
http comp test success!
```

# FAQ


@ref 操作系统内核
