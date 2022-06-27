> **学习物联网，来HaaSEDU就对了**

# ESP-C3-32S-Kit开发板介绍

## 一、概述
ESP32-C3 是一款安全稳定、低功耗、低成本的物联网芯片，搭载 RISC-V 32 位单核处理器，支持 2.4 GHz Wi-Fi 和 Bluetooth 5 (LE)，为物联网产品提供行业领先的射频性能、完善的安全机制和丰富的内存资源。ESP32-C3 对 Wi-Fi 和 Bluetooth 5 (LE) 的双重支持降低了设备配网难度，适用于广泛的物联网应用场景。

### 1、搭载 RISC-V 处理器
ESP32-C3 搭载 RISC-V 32 位单核处理器，时钟频率高达 160 MHz。具有 22 个可编程 GPIO 管脚、内置 400 KB SRAM，支持通过 SPI、Dual SPI、Quad SPI 和 QPI 接口外接多个 flash，满足各类物联网产品功能需求。此外，ESP32-C3 的耐高温特性也使其成为照明和工控领域的理想选择。

### 2、行业领先的射频性能
ESP32-C3 集成 2.4 GHz Wi-Fi 和支持长距离的 Bluetooth 5 (LE)，有助于构建覆盖范围更广、射频性能更强的物联网设备。它还支持蓝牙 Mesh (Bluetooth Mesh) 协议和乐鑫 Wi-Fi Mesh，在较高的工作温度下仍能保持卓越的射频性能。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置

<div align="center">
<img src=../../images/ext_esp32-c3_功能配置.png  width=100%/>
</div>
上图是ESP-C3-32S-Kit开发板，内置蓝牙和Wi-Fi功能，板载USB转TTL芯片，外形大小合理：48.26mm * 25.4mm ，具体功能定义如下表：

| 名称 | 数量 | 描述 | 
| :- | :-: |  :- |
| 核心板 | 1 |  ESP-C3-32S-Kit |
| TTL芯片 | 1 |  型号：CH340 |			
| 指示灯 | 1 |  白色电源指示灯|
| 按键 | 2 | 1个BOOT，1个可编程按键 |		
| USB接口 | 1 |  Micro USB，烧录/调试 |




#### （2）扩展接口
下图是ESP32-C3开发板扩展接口，共有30个引脚，其中4个电源，分别是1个5V和3个3.3V,4个GND,其余22个是GPIO。所有GPIO管脚都可以支持PWM输出，多数GPIO还复用了其他功能，比如：IIC、ADC、SPI、UART等功能。并且GPIO18/GPIO19的默认驱动电流大约是40mA，其余GPIO的默认驱动电流大约是20mA。具体接口定义如下图所示：
<div align="center">
<img src=../../images/ext_esp32-c3_扩展接口.png  width=100%/>
</div>





#### （3）电气特性
| 工作电压 | 3.5v~5v |
| :- | :- |
|充电电流| 450mA，可充电锂电池|
|工作温度|-40～85度|
|环境温度|5～85%RH（无凝结)|



### 2、芯片资源介绍
#### （1） CPU和存储器
* 32 位 RISC-V 单核处理器，主频高达 160 MHz
* CoreMark® 分数：
– 单核 160 MHz：407.22 CoreMark；2.55
CoreMark/MHz
* 384 KB ROM
* 400 KB SRAM（其中 16 KB 专用于 cache）
* 8 KB RTC SRAM
* SPI、Dual SPI、Quad SPI、QPI 接口外接多个flash
* 通过 cache 加速 flash 访问


#### （2）定时器
* 2 × 54 位通用定时器
* 3 × 看门狗定时器
* 1 × 52 位系统定时器

#### （3）高级外设接口
* 22 或 16 个 GPIO 口
* 2 x 12位 SAR ADC，多达6个通道
* 3 个SPI
* 1 个IIS
* 1 个IIC
* 2 个UART
* 红外收发器，2 个发送通道和 2 个接收通道
* LED PWM，多达16 个通道
* 全速 USB 串口/JTAG 控制器
* 通用 DMA 控制器 (简称 GDMA)，3 个接收
* 1 × 温度传感器

#### （4）安全机制
* 安全启动
* flash 加密
* 4096位OTP，用户可用的高达1792位
* 加密硬件加速器：AES-128/256 (FIPS PUB 197)
* 访问权限管理
* SHA 加速器 (FIPS PUB 180-4)
* RSA 加速器
* 随机数生成器(RNG)

#### （5）Wi-Fi主要特性
* 支持 IEEE 802.11 b/g/n 协议
* 在 2.4 GHz 频带支持 20 MHz 和 40 MHz 频宽
* 支持 1T1R 模式，数据速率高达 150 Mbps
* 无线多媒体 (WMM)
* 帧聚合 (TX/RX A-MPDU, TX/RX A-MSDU)
* 立即块确认 (Immediate Block ACK)
* 分片和重组 (Fragmentation and defragmentation)
* 传输机会 (Transmit opportunity, TXOP)
* Beacon 自动监测（硬件 TSF）
* 4 × 虚拟 Wi-Fi 接口
* 同时支持基础结构型网络 (Infrastructure BSS) Station 模式、SoftAP 模式、Station + SoftAP 模式和混杂模式(
**请注意 ESP32-C3 系列芯片在 Station 模式下扫描时，SoftAP 信道会同时改变**)
* 天线分集
* 802.11 mc FTM
* 支持外部功率放大器

#### （6）蓝牙主要特性
* 低功耗蓝牙 (Bluetooth LE)：Bluetooth 5、Bluetooth mesh)
* 高功率模式（18 dBm）
* 速率支持：125 Kbps、500 Kbps、1 Mbps、2Mbps
* 广播扩展 (Advertising Extensions)
* 多广播 (Multiple Advertisement Sets)
* 信道选择 (Channel Selection Algorithm #2)
* Wi-Fi 与蓝牙共存，共用同一个天线


## 三、学习指南
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于HaaS EDU K1的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问HaaS官网(http://haas.iot.aliyun.com)。
