@page mqtt mqtt

# 概述

MQTT（Message Queuing Telemetry Transport，消息队列遥测传输协议），是一种基于发布/订阅（publish/subscribe）模式的“轻量级”通讯协议，该协议构建于 TCP/IP 协议上，由 IBM 在1999年发布。MQTT 最大优点在于，可以以极少的代码和有限的带宽，为连接远程设备提供实时可靠的消息服务。作为一种低开销、低带宽占用的即时通讯协议，使其在物联网、小型设备、移动应用等方面有较广泛的应用。

MQTT 是一个基于客户端-服务器的消息发布/订阅传输协议。MQTT 协议是轻量、简单、开放和易于实现的，这些特点使它适用范围非常广泛。在很多情况下，包括受限的环境中，如：机器与机器（M2M）通信和物联网（IoT）。其在，通过卫星链路通信传感器、偶尔拨号的医疗设备、智能家居、及一些小型化设备中已广泛使用。

Paho MQTT 是 Eclipse 实现的基于 MQTT 协议的客户端，本软件包是在 Eclipse paho-mqtt 源码包的基础上设计的一套 MQTT 客户端程序。

该组件支持以下功能：
- MQTT Connect
- MQTT Subscribe
- MQTT Publish
- 可连接阿里云物联网平台

# 版权信息
> Eclipse Distribution License v1.0

# 目录结构
```sh
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

# 依赖组件
* osal_aos
* mbedtls

# 常用配置
http的常用配置在文件**package.yaml内**
> CONFIG_AIOT_SIGN ：是否开启阿里云物联网平台加签
```C
CONFIG_AIOT_SIGN: 1
```

> MQTT_PORT ： MQTT默认端口
```C
#ifndef MQTT_PORT
#define MQTT_PORT   443
#endif
```

# API说明
@ref mqtt API

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
示例代码参考example/mqtt_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> mqtt组件的package.yaml中添加
```C
source_file:
  - "src/*.c"
  - "example/*.c"     # 增加示例代码
```

## app中添加mqtt组件
> helloworld_demo组件的package.yaml中添加
```C
depends:
  - netmgr: dev_aos   # helloworld_demo中引入netmgr组件，用于WiFi联网
  - mqtt: dev_aos     # helloworld_demo中引入mqtt组件
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

## mqtt示例测试
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

## 关键日志
> CLI日志：
```sh
Network Connect success!
Mqtt Connect success!
Mqtt Subscribe success, topic:xxxx!
```

# FAQ


@ref 操作系统内核
