# C-SKY IoT开发板（CB2201）快速上阿里云指南

本文基于 [AliOS Things](https://github.com/alibaba/AliOS-Things) 1.3.x版本，手把手教你在C-SKY CB2201开发板上使用MQTT通道上云

## **1 硬件环境搭建**

### 1.1 开发板准备

#### 1.1.1 CB2201开发板介绍

**①** CB2201开发板是杭州中天微自主设计的一款用于开发 IoT 应用的开发板。

**②** 板上集成 CH2201 芯片，集成 CPU 调试器CKLink，只需要一根 USB 线就可以进行供电、调试、下载等操作。

**③** 集成两个子板接口，每个子板接口中都集成了UART/SPI/IIC/ADC/PWM/GPIO等外设，可以连接各类接口兼容的功能子板，包括中天微设计的ENC28J60 SPI有线网卡子板，ESP8266-WiFi子板，传感器子板等。

#### 1.1.2 CH2201芯片介绍

**①** 内置32BIT C-SKY CK802T CPU@up to 48MHz

**②** 256KB XIP eFlash，80KB SRAM

**③** REE/TEE运行环境隔离

**④** 硬件加解密引擎，支持AES、RSA、SHA、TRNG等算法

**⑥** 34个独立可编程、复用通用输入输出接口，包括：SPI×2, UART×3, I2C×2, ADC×16, PWM×4, GPIO×34, I2S×1, ACMP×1

**⑦** 定时器相关资源：CoreTIM×1, RTC×2, Timer×2, WDT×1

**⑧** 待机功耗 <3uA

**⑨** 使用QFP-64-0.4mm封装

![CB2201](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/CB2201.png)

#### 1.1.3 ESP8266 WiFi子板

WiFi子板介绍：略

![ESP8266_evb](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/ESP8266_evb.png)

#### 1.1.4 开发板获取：

https://item.taobao.com/item.htm?id=570014022304

### 1.2 开发板连接方法

#### 1.2.1 WiFi子板连接

![CB2201_ESP8266_evb](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/CB2201_ESP8266_evb.png)

#### 1.2.2 串口线连接

![USB_Serial](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/USB_Serial.png)

#### 1.2.3 电源连接

通过USB线供电，图略

## 2 云端和通道环境搭建

### 2.1 在云端主要包括以下几步

参考链接（ https://github.com/alibaba/AliOS-Things/wiki/Manual-Channel-MQTT ），做如下操作：

1、创建阿里云账号

2、创建测试产品，拿到ProductKey

3、创建测试设备，拿到DeviceName和DeviceSecret

4、下载测试工具

注意：请无视该文档中关于linuxhost的示例，编译方式请参考下面章节。

### 2.2 三要素设置

修改./framework/protocol/linkkit/iotkit/sdk-encap/imports/iot\_import\_product.h 中三个宏定义，修改为上一步骤中创建产品和设备时拿到的三要素（ProductKey、DeviceName和DeviceSecret），如下：

```

#elif  MQTT_TEST

#define PRODUCT_KEY    "......"

#define DEVICE_NAME    "......"

#define DEVICE_SECRET  "......"

#define PRODUCT_SECRET ""

#else

```

注：mqttapp程序所在源码为AliOS-Things/example/mqttapp/mqtt-example.c ( https://github.com/alibaba/AliOS-Things/blob/master/example/mqttapp/mqtt-example.c )。

此时在云端获取的三个参数ProductKey，DeviceName和DeviceSecret分别对应代码中的PRODUCT\_KEY，DEVICE\_NAME和DEVICE\_SECRET三个宏，宏PRODUCT_SECRET无需理会。

## 3 mqttapp编译

AliOS-Things可以通过命令行和AliOS-Things IDE开发，详见下面说明。

### 3.1 命令行编译

1、命令行环境搭建：

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;参考https://github.com/alibaba/AliOS-Things/wiki/Quick-Start

2、命令行编译方式如下：

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`$ aos make mqttapp@cb2201`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;build完成后可在out/mqttapp@cb2201/binary/ 目录找到生成的bin文件和hex文件。

### 3.2 AliOS-Things IDE编译

1、AliOS-Things IDE环境搭建：

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;参考https://github.com/alibaba/AliOS-Things/wiki/Starter-Kit-Tutorial

2、开发环境搭建好后，导入Alios-Things源码。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;导入方法1：鼠标直接拖入；

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;导入方法2：点击菜单栏 “文件(F)” -> “打开文件夹(F)”。

3、Build如下图，选中mqttapp@cb2201，点击右侧&quot;√&quot;

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;build完成后可在out/mqttapp@cb2201/binary/ 目录找到生成的bin文件。

## 4 固件烧录

### 4.1 安装烧录软件

1、烧录软件获取：链接：https://pan.baidu.com/s/1CcbCXZ3SJwfBL13_MP9SIg 密码：s6j5

2、解压后，双击CSKYFlashProgrammer.exe打开烧录软件

### 4.2 烧录

1、选择User Config： AliOS-Things-CB2201-MQTTAPP，并更改AliOS-Things存放路径（即修改下图中“G:\”）

![FLASH_PROGRAMMER_Config](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/FLASH_PROGRAMMER_Config.png)

2、开发板首次烧录程序需要选择 “Chip Erase” 用以擦除eFlash，之后开发则选择 “Erase Sectors” 即可。

![FLASH_PROGRAMMER_Advance_Chip_Erase](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/FLASH_PROGRAMMER_Advance_Chip_Erase.png)

3、点击下方 "Start Program" 按钮烧写（注意：烧写前需要先退出CskyDebugServer）

## 5 WiFi配网及数据连接阿里云

### 5.1 WiFi配网

烧录完成后，点击开发板复位键重启，串口打印如下图所示：

在串口命令行中敲入如下配网命令：

`# netmgr connect <ssid> <password>`

![PeiWang](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/PeiWang.png)

正常联网后，mqttapp会真正开始运行。下图为mqttapp运行日志：

![mqttapp_log](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/mqttapp_log.png)

### 5.2 查看设备是否在线

点击下面链接，登录阿里云账户查看：

[http://iot.console.aliyun.com/#/product/newlist/region/cn-shanghai](http://iot.console.aliyun.com/#/product/newlist/region/cn-shanghai)

![AliYun_Device_information](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/AliYun_Device_information.png)

## 6 调试

### 6.1 CskyDebugServer安装和使用

1、获取CskyDebugServer（若已安装CDS/CDK，则可略过1和2的步骤）

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;链接：https://pan.baidu.com/s/1lT7gIoJZylQEOUvXnrpFUg 密码：6bu9

2、安装

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;解压后双击默认安装。

3、端口设置，如下图：

![CskyDebugserver_port](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/CskyDebugServer_port.png)

4、连接开发板

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;点击 “红色三角形” 按钮，连接成功后，“红色三角形” 按钮会变成 “红色圆形” 按钮，如下图：

![CskyDebugServer_connect](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/CskyDebugServer_connect.png)

### 6.2 VS Code调试设置

根据已编译并烧录的app@board信息，更新 AliOS-Things/.vscode/launch.json 调试配置文件，比如：已编译并烧录 mqttapp@cb2201以后，更改相关配置如下图：

![VSCode_launch](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/VSCode_launch.png)

### 6.3 开始调试

1、点击 ![VSCode_Debug_Page_Button](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/VSCode_Debug_Page_Button.png) 按钮，进入调试界面。

2、选择 “CSKY DEBUG @ Windows/Linux”：![VSCode_CSKYDebug_Button](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/VSCode_CSKYDebug_Button.png)

3、点击左上方的 ![VSCode_Continue](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/VSCode_Continue.png) 按钮(或F5)启动调试。

4、启动调试以后会自动停到已设置的断点 application\_start 函数处；

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;同时上方会出现调试工具栏，提供常用的单步调试功能；

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;左侧边栏可以查看变量和函数调用栈。

![VSCode_debug](https://raw.githubusercontent.com/chenlf123/MarkdownPhotos/master/AliOS-Things/VSCode_Debug_Page.png)

