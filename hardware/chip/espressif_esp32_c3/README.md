### 1、芯片资源介绍
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
