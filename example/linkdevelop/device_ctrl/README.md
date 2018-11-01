


## 简介

本文介绍一个模拟远程”设备控制“的简单示例，即搭载 AliOS Things 的设备，并结合 Link Develop (以下简称LD) 可快速实现远程(云端)控制设备端LED灯。
本示例源码: [device_ctrl](https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/device_ctrl)
```
device_ctrl
├── device_ctrl.c
└── device_ctrl.mk
```

## 功能介绍

设备端 AliOS Things 启动后初始化与 LD 云端的 MQTT 连接，然后在串口 cli 中输入 `netmgr connect ssid password` 命令后连接网络，然后云端可以控制设备端的LED亮/灭。


## 环境要求

### 设备端环境
任意具有接入互联网能力，且已完成移植 AliOS Things 移植的设备，如：[ST开发板](https://linkdevelop.aliyun.com/device-doc#dev-st-guide.html), [NXP开发板](https://linkdevelop.aliyun.com/device-doc#dev-nxp-guide.html), [TI开发板](https://linkdevelop.aliyun.com/device-doc#dev-ti-guide.html), [Developerkit开发板](http://aliosthings.io/#/developerkit)。
亦可请参考其他[已支持的硬件](https://github.com/alibaba/AliOS-Things/tree/master/board)进行适配。

### 云端环境

* 在 [Link Develop](https://linkdevelop.aliyun.com) 注册账号(与阿里云账号通用)。
* 在 LD 产品创建好以后，在设备模型的自定义功能处，新增一个属性, 用于下行消息, 设为布尔类型。如图：
![](https://img.alicdn.com/tfs/TB1yGW0XHPpK1RjSZFFXXa5PpXa-1657-602.png)
同时，这些publish的内容需要按要求填写在设备端代码(example/linkdevelop/device_ctrl/device_ctrl.c)：
    ```
    #define PROP_SET_FORMAT_CMDLED       "\"cmd_led\":"
    ```
* 设备模型创建完成后，新增一个测试设备，记下如下所示的三元组信息。
  该组信息会用于烧写到实际设备中，而这个测试设备，则会是实际设备在云端的一个影子。
	* PRODUCT_KEY
	* DEVICE_NAME
	* DEVICE_SECRET


## 设备端编译构建

前置开发环境搭建请移步[开发准备](https://linkdevelop.aliyun.com/device-doc#dev-prepare.html)。

在 `example/linkdevelop/device_ctrl/device_ctrl.c` 中, 将以下几个字段更新为上述生成的对应信息，如：
```c
#define PRODUCT_KEY             "b1VxeCKSQeO"
#define DEVICE_NAME             "simple"
#define DEVICE_SECRET           "x9D0k0DjwtYG734unBm0YYsyoMRxgCNR"
```

然后在 AliOS Things 代码环境中编译出目标.bin文件，如：
```sh
aos make linkdevelop.device_ctrl@developerkit
```
烧录到板子中：
```sh
aos upload linkdevelop.device_ctrl@developerkit 
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

### 接收下行消息
![](https://img.alicdn.com/tfs/TB1nnk2XQvoK1RjSZPfXXXPKFXa-1690-603.png)
如果我们按上图所示，在LD控制台的调试功能处选择下行消息，方法选设置，点击右侧发送指令按钮，即可在设备的串口日志中，看到如下信息。此时可以实现控制设备端的LED灯的熄灭。
```
# [073510]<V> ----
[073510]<V> Topic: '/sys/b1dl9ooTlYa/Ld_developerkit_test01_sh/thing/service/property/set' (Length: 69)
[073520]<V> Payload: '{"method":"thing.service.property.set","id":"123221190","params":{"cmd_led":0},"version":"1.0.0"}' (Length: 97)
[073540]<V> ----
```

## 结语
至此，已经完成了一个最常用的物联网的应用：设备控制。
