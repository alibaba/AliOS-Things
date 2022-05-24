### 1、芯片资源介绍
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
