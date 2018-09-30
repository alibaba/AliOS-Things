## airkiss_smartconfig

### Content
- [airkiss_smartconfig](#airkiss_smartconfig)
    - [Content](#content)
    - [Overview](#overview)
        - [Platform porting](#platform-porting)
        - [Dependent components](#dependent-components)
    - [Supported boards](#supported-boards)
    - [Build](#build)
    - [Run](#run)

### Overview

`airkiss_smartconfig` 是一个基于 AliOS Things 的 airkiss 实现，设备端与手机端配合，传递当前 AP 的 ssid 与密码，完成物联网设备的智能配网（Smartconfig）功能。airkiss_smartconfig 源码目录结构如下:

```sh
airkiss_smartconfig
    ├── airkiss.c
    ├── airkiss.h
    ├── airkiss_smartconfig.c
    ├── airkiss_smartconfig.mk
    ├── raw_packet.c
    ├── README.md
    └── ucube.py
```

`raw_packet.c`：获取 WiFi 模块抓取的 802.11 数据包，解码 len 字段信息，细节见文档[Airkiss 技术实现方案](https://wenku.baidu.com/view/a5d51c18561252d380eb6eab.html)。

`airkiss.c`：基于 AliOS 的 WiFi HAL 和 yloop 组件，完成 airkiss 流程（信道锁定、AP 连接、UDP 包发送等），细节见文档[AirKiss 库文件使用指南](https://iot.weixin.qq.com/wiki/airkiss_developer_manual.pdf)。

`airkiss_smartconfig.c`：用户程序，通过 `airkiss.c` 封装的回调接口使用 airkiss 基本功能，详见代码中的 `airkiss_done()` 函数。

#### Platform porting

example 基于 ESP8266 平台编写，乐鑫 ESP32 与 ESP8266 SDK 接口差别不大，稍加改动即可运行。虽然使用了 AliOS HAL 组件，大大提高了程序的通用性，但移植其他平台仍需修改 `airkiss.c` 少量平台相关代码。根据官方文档说明，满足以下条件的平台理论上都能使用 airkiss 功能：

> 硬件能力要求：
> 
1. 能够切换信道；
2. 具备定时器功能，能够提供 100ms 的定时中断；
3. 能够设置为混杂模式，接收 802.11 网络帧；
4. 提供一种进入 AirKiss 模式的控制方式，例如一个按键；

> 软件能力要求：
> 
1. 能够提供类似标准 memset 函数的功能函数；
2. 能够提供类似标准 memcpy 函数的功能函数；
3. 能够提供类似标准 memcmp 函数的功能函数；
4. 能够提供至少 232 字节的全局缓冲空间（完成 AirKiss 后用户可
用于自己的应用程序或进行释放）；

#### Dependent components

`yloop`  

### Supported boards

* NodeMCU
* 其余 esp8266 开发板

### Build

使用 [aos-cube](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uCube) 编译 [esp8266](../../board/esp8266/README.md) 固件:

```sh
aos make airkiss_smartconfig@esp8266
```

### Run

开发板上电后开始 airkiss 流程，使用手机微信扫描以下二维码进行配网：

![model-two-dimension code](http://odaps2f9v.bkt.clouddn.com/18-9-13/90148586.jpg)

串口打印结果：

```sh
AliOS Airkiss Test Start...
AIRKISS_STATE_WAIT
AIRKISS_STATE_FIND_CHANNEL
T|LOCK CHANNEL : 5
scandone
AIRKISS_STATE_GETTING_SSID_PSWD
T|pswd : ChinaNet-A38E
T|ssid : 12345678
scandone
state: 0 -> 2 (b0)
state: 2 -> 3 (0)
state: 3 -> 5 (10)
add 0
aid 3
pm open phy_2,type:2 0 0
cnt 

connected with ChinaNet-A38E, channel 6
dhcp client start...
ip:192.168.1.103,mask:255.255.255.0,gw:192.168.1.1
AIRKISS_STATE_COMPLETE
Wifi connected.

```