> **学习物联网，来HaaSEDU就对了**

# ESP-S3-12K-Kit开发板介绍

## 一、概述
ESP32-S3 是一款集成 2.4 GHz Wi-Fi 和 Bluetooth 5 (LE) 的 MCU 芯片，支持远距离模式 (Long Range)。ESP32-S3 搭载 Xtensa® 32 位 LX7 双核处理器，主频高达 240 MHz，内置 512 KB SRAM (TCM)，具有 45 个可编程 GPIO 管脚和丰富的通信接口。ESP32-S3 支持更大容量的高速 Octal SPI flash 和片外 RAM，支持用户配置数据缓存与指令缓存。

### 1、支持 AI 加速
ESP32-S3 MCU 增加了用于加速神经网络计算和信号处理等工作的向量指令 (vector instructions)。AI 开发者们通过 ESP-DSP 和 ESP-NN 库使用这些向量指令，可以实现高性能的图像识别、语音唤醒和识别等应用。ESP-WHO 和 ESP-Skainet 也将支持此功能。

### 2、丰富的 IO 接口
ESP32-S3 拥有 45 个可编程 GPIO 以及 SPI、I2S、I2C、PWM、RMT、ADC、UART、SD/MMC 主机控制器和 TWAITM 控制器等常用外设接口。其中的 14 个 GPIO 可被配置为 HMI 交互的电容触摸输入端。此外，ESP32-S3 搭载了超低功耗协处理器 (ULP)，支持多种低功耗模式，广泛适用于各类低功耗应用场景。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置

<div align="center">
<img src=../../images/ext_esp32-s3_功能配置.png  width=100%/>
</div>
上图是ESP-S3-12K-Kit开发板，内置蓝牙和Wi-Fi功能，板载USB转TTL芯片，外形大小合理：60.8mm * 25.4mm ，具体功能定义如下表：

| 名称    | 数量  | 描述                   |
| :------ | :---: | :--------------------- |
| 核心板  |   1   | ESP-S3-12K-Kit           |
| TTL芯片 |   1   | 型号：CH340            |
| 指示灯  |   1   | 橙色电源指示灯         |
| 按键    |   2   | 1个BOOT，1个可编程按键 |
| USB接口 |   1   | Micro USB，烧录/调试   |

#### （2）扩展接口
下图是ESP-S3-12K-Kit开发板扩展接口，共有42个引脚，其中2个电源（分别是1个5V和1个3.3，2个GND，1个EN控制，2个UART0管教，其余37个是GPIO。所有GPIO管脚都可以支持PWM输出，多数GPIO还复用了其他功能，比如：IIC、ADC、SPI、UART等功能。并且GPIO19/GPIO20的默认驱动电流大约是40mA，其余GPIO的默认驱动电流大约是20mA。具体接口定义如下图所示：
<div align="center">
<img src=../../images/ext_esp32-s3_扩展接口.png  width=100%/>
</div>


#### （3）电气特性
| 工作电压 | 3.5v~5v             |
| :------- | :------------------ |
| 充电电流 | 450mA，可充电锂电池 |
| 工作温度 | -40～85度           |
| 环境温度 | 5～85%RH（无凝结)   |

### 2、芯片资源介绍
#### （1） CPU和存储器
* Xtensa® 32 位 LX7 双核处理器，主频高达 240
MHz
* CoreMark® 得分：
  - 单核，主频 240 MHz：613.86 CoreMark；2.56 CoreMark/MHz
  - 双核，主频 240 MHz：1181.60 CoreMark；4.92 CoreMark/MHz
* 128 位数据总线位宽，支持 SIMD 指令
* 384 KB ROM
* 512 KB SRAM
* 16 KB RTC SRAM
* SPI、Dual SPI、Quad SPI、Octal SPI、QPI、OPI接口外接多个 flash 和片外 RAM
* 引入 cache 机制的 flash 控制器
* 支持 flash 在线编程


#### （2）定时器
* 4 × 54 位通用定时器
* 1 × 52 位系统定时器
* 3 × 看门狗定时器

#### （3）高级外设接口
* 45 × GPIO 口
* 4 × SPI
* 1 × LCD 接口（8 位 ~16 位并行 RGB, I8080, MOTO6800）, 支持 RGB565, YUV422, YUV420, YUV411 之间互相转换
* 1 × DVP 8 位 ~16 位摄像头接口
* 3 × UART
* 2 × I2C
* 2 × I2S
* 1 × RMT (TX/RX)
* 1 × 脉冲计数器
* LED PWM 控制器，多达 8 个通道
* 1 × 全速 USB OTG
* 1 × USB Serial/JTAG 控制器
* 2 × MCPWM
* 1 × SDIO 主机接口，具有 2 个卡槽
* 通用 DMA 控制器 (简称 GDMA)，5 个接收通道和 5 个发送通道
* 1 × TWAI ® 控制器，兼容 ISO11898-1（CAN 规范 2.0）
* 2 × 12 位 SAR ADC，多达 20 个通道
* 1 × 温度传感器
* 14 × 电容式传感 GPIO

#### （4）安全机制
* 安全启动
* flash 加密
* 4096位OTP，用户可用的高达1792位
* 加密硬件加速器：
  - AES-128/256 (FIPS PUB 197)
  - Hash (FIPS PUB 180-4)
  - RSA
  - 随机数生成器 (RNG)
  - HMAC
  - 数字签名

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
**请注意 ESP32-S3 系列芯片在 Station 模式下扫描时，SoftAP 信道会同时改变**)
* 天线分集
* 802.11 mc FTM
* 支持外部功率放大器

#### （6）蓝牙主要特性
* 低功耗蓝牙 (Bluetooth LE)：Bluetooth 5、Bluetooth mesh)
* 高功率模式（20 dBm，与 Wi-Fi 共用 PA）
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
