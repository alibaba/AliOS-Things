1、芯片资源介绍
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

