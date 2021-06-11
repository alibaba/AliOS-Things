@page mqtt mqtt

[更正文档](https://gitee.com/alios-things/mqtt/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

MQTT（Message Queuing Telemetry Transport，消息队列遥测传输协议），是一种基于发布/订阅（publish/subscribe）模式的“轻量级”通讯协议，该协议构建于 TCP/IP 协议上，由 IBM 在1999年发布。MQTT 最大优点在于，可以以极少的代码和有限的带宽，为连接远程设备提供实时可靠的消息服务。作为一种低开销、低带宽占用的即时通讯协议，使其在物联网、小型设备、移动应用等方面有较广泛的应用。

MQTT 是一个基于客户端-服务器的消息发布/订阅传输协议。MQTT 协议是轻量、简单、开放和易于实现的，这些特点使它适用范围非常广泛。在很多情况下，包括受限的环境中，如：机器与机器（M2M）通信和物联网（IoT）。其在，通过卫星链路通信传感器、偶尔拨号的医疗设备、智能家居、及一些小型化设备中已广泛使用。

Paho MQTT 是 Eclipse 实现的基于 MQTT 协议的客户端，本软件包是在 Eclipse paho-mqtt 源码包的基础上设计的一套 MQTT 客户端程序。

该组件支持以下功能：
- MQTT Connect
- MQTT Subscribe
- MQTT Publish
- 可连接阿里云物联网平台

## 版权信息
> Eclipse Distribution License v1.0

## 目录结构
```tree
├── MQTTClient-C
│   ├── MQTTClient.c
│   ├── MQTTClient.h
│   ├── MQTTLinux.c
│   └── MQTTLinux.h
├── MQTTPacket
│   ├── MQTTConnectClient.c
│   ├── MQTTConnect.h
│   ├── MQTTConnectServer.c
│   ├── MQTTDeserializePublish.c
│   ├── MQTTFormat.c
│   ├── MQTTFormat.h
│   ├── MQTTPacket.c
│   ├── MQTTPacket.h
│   ├── MQTTPublish.h
│   ├── MQTTSerializePublish.c
│   ├── MQTTSubscribeClient.c
│   ├── MQTTSubscribe.h
│   ├── MQTTSubscribeServer.c
│   ├── MQTTUnsubscribeClient.c
│   ├── MQTTUnsubscribe.h
│   ├── MQTTUnsubscribeServer.c
│   └── StackTrace.h
├── example
│   ├── aiot_mqtt_sign.c  #连接阿里云物联网平台需要加签
│   └── mqtt_example.c    # 示例代码
├── package.yaml          # 编译配置文件
└─── README.md
```

## 依赖组件
* osal_aos
* mbedtls

# 常用配置
http的常用配置在文件**package.yaml内**
> CONFIG_AIOT_SIGN ：是否开启阿里云物联网平台加签
```yaml
CONFIG_AIOT_SIGN: 1
```

> MQTT_PORT ： MQTT默认端口
```C
#ifndef MQTT_PORT
#define MQTT_PORT   443
#endif
```

# API说明

## MQTT初始化
```C
DLLExport void MQTTClientInit(MQTTClient* client, Network* network, unsigned int command_timeout_ms,
		unsigned char* sendbuf, size_t sendbuf_size, unsigned char* readbuf, size_t readbuf_size);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|network                                 |网络对象|
|command_timeout_ms                      |超时时间|

## MQTT连接请求
```C
DLLExport int MQTTConnect(MQTTClient* client, MQTTPacket_connectData* options);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|options                                 |MQTT连接选项|

## MQTT发布主题
```C
DLLExport int MQTTPublish(MQTTClient* client, const char* topic, MQTTMessage* message);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|topic                                   |MQTT发布的主题|
|message                                 |MQTT发布的消息主体|

## MQTT订阅主题
```C
DLLExport int MQTTSubscribe(MQTTClient* client, const char* topicFilter, enum QoS, messageHandler);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|topicFilter                             |MQTT订阅的主题|
|messageHandler                          |MQTT消息处理句柄|

## MQTT取消订阅主题
```C
DLLExport int MQTTUnsubscribe(MQTTClient* client, const char* topicFilter);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|topicFilter                             |MQTT订阅的主题|

## MQTT断开连接请求
```C
DLLExport int MQTTDisconnect(MQTTClient* client);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|

## MQTT等待响应
```C
DLLExport int MQTTYield(MQTTClient* client, int time);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|
|time                                    |等待时间|

## MQTT是否处于连接状态
```C
DLLExport int MQTTIsConnected(MQTTClient* client);
```
|args                                    |description|
|:-----                                  |:----|
|client                                  |MQTT client上下文|


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
  - mqtt: master     # helloworld_demo中引入mqtt组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install mqtt

```

上述命令执行成功后，组件源码则被下载到了./components/mqtt路径中。

## 步骤4 添加示例
在mqtt组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/mqtt/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/*.c"     # 增加示例代码
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

> CLI命令行输入初始化netmgr命令：
```sh
netmgr_example
```
> CLI命令行输入联网命令（ssid和password换成自己的）：
```sh
netmgr -t wifi -c ssid password
```
> 然后，CLI命令行输入：
```sh
mqtt_example
```

**关键日志**
> CLI日志：
```sh
Network Connect success!
Mqtt Connect success!
Mqtt Subscribe success, topic:xxxx!
```

# FAQ

