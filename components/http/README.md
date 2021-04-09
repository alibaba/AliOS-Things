@page http http

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
```sh
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
```C
CONFIG_HTTP_SECURE: 1
```
> CONFIG_HTTP_FILE_OPERATE ：是否使用文件操作，默认关闭
```C
CONFIG_HTTP_FILE_OPERATE: 0
```

http的内部配置在文件**internal/http_opts.h内**
> HTTPCLIENT_AUTHB_SIZE ： http认证数据（用户名、密码）长度
```C
#ifndef HTTPCLIENT_AUTHB_SIZE
#define HTTPCLIENT_AUTHB_SIZE     128
#endif
```
> HTTPCLIENT_CHUNK_SIZE ： http chunk块数据大小
```C
#ifndef HTTPCLIENT_CHUNK_SIZE
#define HTTPCLIENT_CHUNK_SIZE     1024
#endif
```
> HTTPCLIENT_SEND_BUF_SIZE ： http发送缓存大小
```C
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
@ref http API

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
示例代码参考example/http_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> http组件的package.yaml中添加
```C
source_file:
  - "src/*.c"
  - "example/http_example.c" # add http_example.c
```

## app中添加http组件
> helloworld_demo组件的package.yaml中添加
```C
depends:
  - netmgr: dev_aos   # helloworld_demo中引入netmgr组件，用于WiFi联网
  - http: dev_aos     # helloworld_demo中引入http组件
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

## http示例测试
> CLI命令行输入初始化netmgr命令：
```sh
netmgr_example
```
> CLI命令行输入联网命令：
```sh
netmgr -t wifi -c ssid password
```
> 然后，CLI命令行输入：
```sh
http_example
```

## 关键日志
> CLI日志：
```sh
http comp test success!
```

# FAQ


@ref 操作系统内核
