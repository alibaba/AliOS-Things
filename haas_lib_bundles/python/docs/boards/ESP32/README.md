> **学习物联网，来HaaSEDU就对了**

# ESP32开发板介绍

## 一、概述
ESP32 是集成2.4 GHz Wi-Fi 和蓝牙双模的单芯片方案，采用台积电(TSMC) 超低功耗的40 纳米工艺，具有超
高的射频性能、稳定性、通用性和可靠性，以及超低的功耗，满足不同的功耗需求，适用于各种应用场景。
目前ESP32 系列的产品型号包括ESP32-D0WD-V3，ESP32-D0WDQ6-V3，ESP32-D0WD，ESP32-D0WDQ6，
ESP32-D2WD，ESP32-S0WD 和ESP32-U4WDH，其中ESP32-D0WD-V3，ESP32-D0WDQ6-V3 和ESP32-
U4WDH 是基于ECO V3 的芯片型号。

### 1、超低功耗
ESP32 专为移动设备、可穿戴电子产品和物联网(IoT) 应用而设计。作为业内领先的低功耗芯片，ESP32 具有精
细的时钟门控、省电模式和动态电压调整等特性。
例如，在低功耗IoT 传感器Hub 应用场景中，ESP32 只有在特定条件下才会被周期性地唤醒。低占空比可以极
大降低ESP32 芯片的能耗。射频功率放大器的输出功率也可调节，以实现通信距离、数据率和功耗之间的最佳
平衡。

### 2、高集成度
ESP32 是业内领先的高度集成的Wi-Fi+ 蓝牙解决方案，外部元器件只需大约20 个。ESP32 集成了天线开关、射
频Balun、功率放大器、低噪声放大器、滤波器以及电源管理模块，极大减少了印刷电路板(PCB) 的面积。
ESP32 采用CMOS 工艺实现单芯片集成射频和基带，还集成了先进的自校准电路，实现了动态自动调整，可以
消除外部电路的缺陷，更好地适应外部环境的变化。因此，ESP32 的批量生产可以不需要昂贵的专用Wi-Fi 测
试设备。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置

<div align="center">
<img src=../../images/ext_esp32_功能配置.png  width=100%/>
</div>
上图是ESP32-32S开发板，内置蓝牙和Wi-Fi功能，板载USB转TTL芯片，外形大小合理：48.26mm * 25.4mm ，具体功能定义如下表：

| 名称 | 数量 | 描述 | 
| :- | :-: |  :- |
| 核心板 | 1 |  ESP32-32S |
| TTL芯片 | 1 |  型号：CP2102/CH340 |			
| 指示灯 | 1 |  红色电源指示灯|
| 按键 | 2 | 1个BOOT，1个可编程按键 |		
| USB接口 | 1 |  Micro USB，烧录/调试 |




#### （2）扩展接口
下图是ESP32开发板扩展接口，共有38个引脚，其中2个电源，分别是5V和3.3V,3个GND,其余32个是GPIO。除了GPIO34、35、36、39只能作为输入外，其他的GPIO都可以支持PWM输出，多数GPIO还复用了其他功能，比如：IIC、ADC、SPI、UART、DAC等功能。并且每路GPIO的最大驱动电流是10mA，且每个GPIO引脚的额定电流是6mA，最大电流为12mA。具体接口定义如下图所示：
<div align="center">
<img src=../../images/ext_esp32_扩展接口.png  width=100%/>
</div>





#### （3）电气特性
| 工作电压 | 3.5v~5v |
| :- | :- |
|充电电流| 450mA，可充电锂电池|
|工作温度|-20～85度|
|环境温度|5～85%RH（无凝结)|



### 2、芯片资源介绍
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
* 帧聚合(TX/RX A-MPDU, RX A-MSDU)
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
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于HaaS EDU K1的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问HaaS官网(http://haas.iot.aliyun.com)。
