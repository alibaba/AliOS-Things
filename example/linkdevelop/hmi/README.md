## 简介

物联网通信常常会涉及到人和物之前的数据交互，本文介绍一种非常简单的人机交互例子，搭载 AliOS Things 的设备并基于 Link Develop(以下简称LD) 平台，可以实现本地设备端在某种场景下（如光线强度低于某个下限）上报到云端，云端那边实时发送消息到设备端。
本示例源码: [hmi](https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/hmi)
```
hmi
├── devkit_display
│   ├── AliOS_Things_logo.c
│   ├── chat_display.c
│   ├── chat_display.h
│   └── lv_conf_developerkit.h
├── hmi.c
├── hmi.mk
└── README.md
```

## 功能介绍

设备端 AliOS Things 启动后初始化与 LD 云端的 MQTT 连接，然后在串口 cli 中输入`netmgr connect ssid password` 命令进行网络连接，然后设备端在感应到当前环境发生变化时上报给云端，同时云端也可以发送消息给设备端。
简而言之，打通设备端和云端双向通信后可以实现简单的人机交互。

## 环境要求

### 设备端环境
任意具有接入互联网能力，且已完成移植 AliOS Things 移植的设备，如：[ST开发板](https://linkdevelop.aliyun.com/device-doc#dev-st-guide.html), [NXP开发板](https://linkdevelop.aliyun.com/device-doc#dev-nxp-guide.html), [TI开发板](https://linkdevelop.aliyun.com/device-doc#dev-ti-guide.html)，[Developerkit开发板](http://aliosthings.io/#/developerkit)
亦可请参考其他[已支持的硬件](https://github.com/alibaba/AliOS-Things/tree/master/board)。

### 云端环境

* 在 [Link Develop](https://linkdevelop.aliyun.com) 注册账号(与阿里云账号通用)。
* 在 LD 产品创建好以后，在设备模型的自定义功能处，新增两个属性, 一个用于上行消息, 另一个用于下行消息, 均设为字符串类型。如图：
![](https://img.alicdn.com/tfs/TB1zaWLJ7KWBuNjy1zjXXcOypXa-2570-1540.png)
因此，对应[代码](https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/hmi/hmi.c)修改如下所示：
    ```
  #define PROP_SET_FORMAT_DOWN       "\"down\":"
  #define PROP_SET_FORMAT_UP         "{\"up\":\"%s\"}"
    ```
* 设备模型创建完成后，新增一个测试设备，记下如下所示的三元组信息。
  该组信息会用于烧写到实际设备中，而这个测试设备，则会是实际设备在云端的一个影子。
	* PRODUCT_KEY
	* DEVICE_NAME
	* DEVICE_SECRET


## 设备端编译构建

前置开发环境搭建请移步[开发准备](https://linkdevelop.aliyun.com/device-doc#dev-prepare.html)。

在 `example/linkdevelop/hmi.c` 中, 将以下几个字段更新为上述生成的对应信息，如：
```c
#define PRODUCT_KEY             "b1VxeCKSQeO"
#define DEVICE_NAME             "simple"
#define DEVICE_SECRET           "x9D0k0DjwtYG734unBm0YYsyoMRxgCNR"
```

然后在 AliOS Things 代码环境中编译出目标.bin文件，如：
```sh
aos make linkdevelop.hmi@developerkit
```
烧录到板子中：
```sh
aos upload linkdevelop.hmi@developerkit
```
_注意: 此处烧录方式根据不同硬件会有所不同，此处以 [developer kit](https://github.com/alibaba/AliOS-Things/tree/developer/board/developerkit) 为例_

## 运行

### 启动并连网
烧录完成以后重新上电板子，在串口中 cli 中使用 `netmgr` 命令进行连网：
```
# netmgr connect ssid password
```
其中 `ssid` 和 `password` 是WiFi路由器的名字和密码。
看到如下信息以后即说明连网完成，并已经和 LD 云端建立好 MQTT 连接。
```
[inf] _TLSConnectNetwork(357):   . Setting up the SSL/TLS structure...
[inf] _TLSConnectNetwork(367):  ok
[inf] _TLSConnectNetwork(402): Performing the SSL/TLS handshake...
[inf] _TLSConnectNetwork(410):  ok
[inf] _TLSConnectNetwork(414):   . Verifying peer X.509 certificate..
[inf] _real_confirm(93): certificate verification result: 0x00
[005598]<V> mqtt service
[005652]<V> subscribe success, packet-id=1
# 
```

### 发送上行消息

本次选用Developerkit开发板(板载含有ALS光感传感器)，如果周围环境变暗，此时设备端会主动上报给云端，下图是LD的设备调试日志，也可以在串口中看到上行的消息。  
![](https://img.alicdn.com/tfs/TB1.164X9zqK1RjSZPcXXbTepXa-1699-448.png)  
```
[100570]<V> id: 1, publish msg: {"id":"1","version":"1.0","method":"thing.event.property.post","params":{"up":"Env light seems diminished!"}}
[101570]<V> id: 2, publish msg: {"id":"2","version":"1.0","method":"thing.event.property.post","params":{"up":"Env light seems diminished!"}}
```

### 接收下行消息
如果我们按上图所示，在LD控制台的调试功能处选择`下行消息`，方法选`设置`，点击右侧`发送指令`按钮，即可在设备的串口日志中看到如下信息。如果使用Developerkit，自带的LCD上可以实时显示从云端接收到的信息。
![](https://img.alicdn.com/tfs/TB1.164X9zqK1RjSZPcXXbTepXa-1699-448.png)
```
[1222700]<V> ----
[1222700]<V> Topic: '/sys/b1dl9ooTlYa/Ld_developerkit_test01_sh/thing/service/property/set' (Length: 69)
[1222710]<V> Payload: '{"method":"thing.service.property.set","id":"124036077","params":{"down":"Hello, this Msg is from Cloud!"},"version":"1.0.0"}' (Length: 125)
[1222730]<V> ----
[1222740]<V> >>>>>>"Hello, this Msg is from Cloud!"
```

## 结语
至此，已经完成了物联网的一个人机交互的简单示例。
一切尽在掌控，可以开始进一步的探索了，请参考更多[示例](https://github.com/alibaba/AliOS-Things/tree/master/example)。
