> **学习物联网，来HaaSEDU就对了**

# HaaS200开发板介绍

## 一、概述
HaaS200是一款由阿里云智能HaaS团队认证的高性能、多连接的物联网开发板。内嵌HaaS201核心模组，包含一个Realtek KM4内核的高性能MCU和一个KM0内核的低功耗MCU；自带512KB SRAM，4MB PSRAM和外置4MB FLASH；集成双频Wi-Fi 2.4G/5G 和蓝牙5.0。搭载全新的AliOS Things 3.3操作系统和HaaS轻应用开发框架，以及丰富的接口资源，可以满足更多应用开发需求。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置

<div align="center">
<img src=../../images/ext_haas200_功能配置.png  width=100%/>
</div>
上图是HaaS200硬件配置图，核心板是HaaS201模组，其采用的是Realtek RTL8721DM芯片，搭载2.4GHZ/5GHZ 双频Wi-Fi和蓝牙，板载天线。HaaS201模组上外扩了4MB Nor FLASH。USB接口是常用的Type-C数据接口，通过一颗TTL芯片完成固件烧写和调试。两颗Boot启动和Reset复位按键，一颗可编程RGB LED，和扩展的42Pin标准接口释放出来的丰富的IO资源，满足很多场景的物联网解决方案。外形大小(L * W * H)：61.88mm * 27.94mm * 3.15mm。具体功能定义如下表：

| 名称 | 数量 | 描述 | 
| :- | :-: |  :- |
| 核心板 | 1 |  HaaS201模组（RTL8721DM） |
| TTL芯片 | 1 |  型号：CP2102|
| 指示灯 | 2 |  1个红色电源指示灯，1个可编程RGB LED|
| 按键 | 2 | 1个BOOT，1个Reset |
| USB接口 | 1 |  Type-C USB，烧录/调试 |

#### （2）扩展接口
下图是HaaS200开发板扩展接口，共有42个引脚，含5V和3.3V电源，8个GND可就近选择，IO资源更是丰富，有2路SPI，2路I2C，2路UART，7路ADC，2路I2S，10路PWM，共计27个GPIO，还支持2路Mic和2路Speak音频功能。丰富的扩展资源可以满足不同的应用开发和场景解决方案。具体接口定义如下图所示：
<div align="center">
<img src=../../images/ext_haas200_扩展接口.png  width=100%/>
</div>

上图中PA0-PA31分别对应GPIO PORT 0-31， PB0-PB31 分别对应GPIO PORT 32-63。 

#### （3）电气特性
| 工作电压 | 5v |
| :- | :- |
|工作温度|-40 ～ +125度|
|储存温度|-55 ～ +125度|
|环境湿度|5% ～ 95%|


### 2、芯片资源介绍
#### （1） CPU和存储器
* ARMv8-M架构的RTL8721DM，包含Realtek KM4和KM0核心。其中KM4兼容Cortex-M33指令集，主频高达200MHZ，支持浮点，DSP，TrustZone-M功能，可以运行物联网OS和应用。
KM0兼容Cortex-M23指令集，主频20MHZ，适用于低功耗场景，运行低功耗链接。
* KM4的存储：32KB I-CACHE/4KB D-CACHE, 512KB SRAM, 4MB SPRAM
* KM0的存储：16KB I-CACHE/4KB D-CACHE, 64KB SRAM, 1KB retension SRAM
* 外扩4MB Nor Flash

#### （3）高级外设接口
**这里介绍的是芯片RTL8721DM支持的外设接口，外设数量可能与板子支持的外设不同，板级外设请参考开发板资源介绍章节的扩展接口**

* 3个UART接口, 1x高速UART，波特率高达6Mbps；1x低速UART；1x Debug UART
* 2个SPI接口，速度高达50MHz
* 1个RTC
* 1个红外接口，载波频率25kHz-500kHz
* 1个I2C接口，速度可达400Kbps
* 17路PWM
* 1个I2S接口
* 1个DMIC接口
* 1个Q-Decoder接口
* 1个SGPIO（单线通信）接口
* 7x3/5x5 Key-Scan
* 10个Wake Pin
* 2个高速定时器
* 1个USB接口
* 7路ADC
* 1路电池电压监测接口
* 4路电容触摸按键
* 2路音频输出（Single-ended）
* 2路音频输入（Single-ended）

#### （4）安全机制
* AES/DES/SHA硬件加密
* 支持TrustZone-M
* 支持Secure boot
* SWD保护，支持调试端口保护和禁止模式
* 支持Secure eFuse

#### （5）Wi-Fi主要特性
* 802.11 a/b/g/n 1x1, 2.4GHz & 5GHz
* 支持HT20/HT40模式
* 支持低功耗beacon侦听模式，低功耗接收模式，极低功耗待机模式（DLPS）
* 支持超低功耗挂起模式
* 支持天线分集
* 支持STA，AP和STA+AP模式
* WiFi和蓝牙共存，共用同一天线

#### （6）蓝牙主要特性
* 支持BLE5.0
* 支持Central和Peripheral模式
* 全功耗模式（8dbm，和WiFi共用同一PA）
* WiFi和蓝牙共存，共用同一天线

## 三、学习指南
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于HaaS200的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问HaaS官网(http://haas.iot.aliyun.com)。
