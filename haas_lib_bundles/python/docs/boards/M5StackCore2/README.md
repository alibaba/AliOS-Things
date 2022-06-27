> **学习物联网，来HaaSEDU就对了**

# M5StackCore2开发板介绍

## 一、概述
M5StackCore2是M5Stack开发套件系列中第二代主机，在原有一代主机基础上对功能进一步加强，硬件功能更加齐全。主控ESP32型号为D0WDQ6-V3，具有两个可以单独控制的 Xtensa® 32-bit LX6 处理器，主频高达240Mhz，支持Wi-Fi与蓝牙功能，板载16MB Flash与8MB PSRAM，可通过TYPE-C接口下载程序，强劲的配置满足复杂应用的资源开销。正面搭载一块2.0寸一体化电容式触摸屏，为用户带来更流畅的人机交互体验。机身内置震动马达，可提供触觉回馈和震动提醒功能。内建的RTC模块可提供精准计时功能。电源部分搭载AXP192电源管理芯片可有效控制机身功耗，内置绿色电源指示灯，配备390mAh电池，续航时间更持久。同时机身内配备了TF-card(microSD)卡槽与扬声器，为了保证获得更高质量的声音效果，采用I2S数字音频接口的功放芯片，能有效防止信号失真。在机身的左侧和底部配有独立的电源按键与重启(RST)按键，屏幕正面的3个圆点属于触摸屏的一部分，可通过编写程序设置热区映射为3个虚拟按键。机身背部有一块扩展小板，板上带有6轴IMU传感器与麦克风。 

### 产品特性
* 基于 ESP32 开发，支持Wi-Fi、蓝牙
* 16M Flash，8M PSRAM
* 内置扬声器，电源指示灯，震动马达，RTC，I2S功放，电容式触摸屏 幕，电源键，复位按键
* TF卡插槽(支持最大16GB)
* 内置锂电池，配备电源管理芯片
* 独立小板内置6轴IMU，PDM麦克风
* M-Bus bus socket

## 二、硬件资源
### 1、整机配置
<div align="center">
<img src=../../images/ext_m5core2_接口图.png  width=100%/>
</div>

M5StackCore2正面是2寸的LCD，带电容触摸功能。左侧边有电源键，单机即可开机，长按6秒则关机；USB-C可以给设备充电，扩展有一个I2C功能接口。底侧SD卡槽支持16GB扩展，1个复位按键。具体可以参考下表：

| 主控资源 | 参数 |
| :- | :- |
|CPU| ESP32-D0WDQ6-V3，240MHz dual core 600DMIPS，520KB SRAM|
|FLASH|16MB|
|PSRAM|8MB|
|Wi-Fi|2.4G|
|蓝牙|2.4G，dual mode|
|输入电压|5V@500mA|
|主机接口|TypeC*1，GROVE(I2C+I/O+UART)*1|
|LED|绿色电源指示灯|
|按键|电源键*1，复位键*1，屏幕虚拟按键*3|
|震动提醒|震动马达|
|IPS LCD屏幕|2寸 @320*240 ILI9342C|
|TF卡槽|支持最大16GB|
|锂电池|390mAh @ 3.7V|
|天线|2.4G 3D天线|
|工作温度|0度～60度|
|净重|52g|
|毛重|70g|
|产品尺寸|54*54*16|
|包装尺寸|75*60*20|
|外壳材质|Plastic(PC)|

### 2、扩展接口
烧入HaaS Python固件之后，M5StackCore扩展接口定义请参考下图：
<div align="center">
<img src=../../images/ext_m5core2_接口扩展.png  width=100%/>
</div>


### 3、芯片资源介绍
#### （1） CPU和存储器
* Xtensa® 32-bit LX6 单/双核处理器，运算能力高达600 MIPS（除ESP32-S0WD/ESP32-U4WDH 为200
MIPS，ESP32-D2WD 为400 MIPS）
* 448 KB ROM
* 520 KB SRAM
* 16 KB RTC SRAM
* QSPI 支持多个flash/SRAM

#### （2）时钟和定时器
* 内置8 MHz 振荡器，支持自校准
* 内置RC 振荡器，支持自校准
* 支持外置2 MHz 至60 MHz 的主晶振（如果使用Wi-Fi/蓝牙功能，则目前仅支持40 MHz 晶振）
* 支持外置32 kHz 晶振，用于RTC，支持自校准
* 2 个定时器群组，每组包括2 个64-bit 通用定时器和1 个主系统看门狗
* 1 个RTC 定时器
* RTC 看门狗

#### （3）高级外设接口
* 34 个GPIO 口
* 12-bit SAR ADC，多达18 个通道
* 2 个8-bit D/A 转换器
* 10 个触摸传感器
* 4 个SPI
* 2 个I²S
* 2 个I²C
* 3 个UART
* 1 个Host SD/eMMC/SDIO
* 1 个Slave SDIO/SPI
* 带有专用DMA 的以太网MAC 接口，支持IEEE 1588
* CAN2.0
* IR (TX/RX)
* 电机PWM
* LED PWM，多达16 个通道
* 霍尔传感器

#### （4）安全机制
* 安全启动
* flash 加密
* 1024-bit OTP，用户可用的高达768 bit
* 加密硬件加速器：
* AES
* Hash (SHA-2)
* RSA
* ECC
* 随机数生成器(RNG)

#### （5）Wi-Fi主要特性
* 802.11 b/g/n
* 802.11 n (2.4 GHz) 速度高达150 Mbps
* 无线多媒体(WMM)
* 帧聚合(TX/RX A-MPDU， RX A-MSDU)
* 立即块回复(Immediate Block ACK)
* 重组(Defragmentation)
* Beacon 自动监测（硬件TSF）
* 4 × 虚拟Wi-Fi 接口
* 同时支持基础结构型网络(Infrastructure BSS) Station 模式/SoftAP 模式/混杂模式
请注意ESP32 在Station 模式下扫描时，SoftAP 信道会同时改变
* 天线分集

#### （6）蓝牙主要特性
* 蓝牙v4.2 完整标准，包含传统蓝牙(BR/EDR) 和低功耗蓝牙(BLE)
* 支持标准Class-1、Class-2 和Class-3，且无需外部功率放大器
* 增强型功率控制(Enhanced Power Control)
* 输出功率高达+12 dBm
* NZIF 接收器具有–94 dBm 的BLE 接收灵敏度
* 自适应跳频(AFH)
* 基于SDIO/SPI/UART 接口的标准HCI
* 高速UART HCI，最高可达4 Mbps
* 支持蓝牙4.2 BR/EDR 和BLE 双模controller
* 同步面向连接/扩展同步面向连接(SCO/eSCO)
* CVSD 和SBC 音频编解码算法
* 蓝牙微微网(Piconet) 和散射网(Scatternet)
* 支持传统蓝牙和低功耗蓝牙的多设备连接
* 支持同时广播和扫描


## 三、学习指南
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于M5StackCore2开发板的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问[HaaS官网](http://haas.iot.aliyun.com)。


## 参考文献
[1] [M5StackCore2产品介绍](https://docs.m5stack.com/zh_CN/core/core2)


