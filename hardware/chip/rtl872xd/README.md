@page rtl872xd rtl872xd

# 概述
**rtl872xd** 包含一个Realtek KM4内核的高性能MCU和一个KM0内核的低功耗MCU；自带512KB SRAM，4MB PSRAM和外置4MB FLASH；集成双频Wi-Fi 2.4G/5G 和蓝牙5.0。

## 芯片资源介绍
#### （1） CPU和存储器
* ARMv8-M架构的RTL8721DM，包含Realtek KM4和KM0核心。其中KM4兼容Cortex-M33指令集，主频高达200MHZ，支持浮点，DSP，TrustZone-M功能，可以运行物联网OS和应用。
KM0兼容Cortex-M23指令集，主频20MHZ，适用于低功耗场景，运行低功耗链接。
* KM4的存储：32KB I-CACHE/4KB D-CACHE, 512KB SRAM, 4MB SPRAM
* KM0的存储：16KB I-CACHE/4KB D-CACHE, 64KB SRAM, 1KB retension SRAM
* 外扩4MB Nor Flash

#### （2）高级外设接口
* 3个UART接口, 1x高速UART，波特率高达6Mbps；1x低速UART；1x Debug UART
* 3个SPI接口，速度高达50MHz
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

#### （3）安全机制
* AES/DES/SHA硬件加密
* 支持TrustZone-M
* 支持Secure boot
* SWD保护，支持调试端口保护和禁止模式
* 支持Secure eFuse

#### （4）Wi-Fi主要特性
* 802.11 a/b/g/n 1x1, 2.4GHz & 5GHz
* 支持HT20/HT40模式
* 支持低功耗beacon侦听模式，低功耗接收模式，极低功耗待机模式（DLPS）
* 支持STA，AP和STA+AP模式
* WiFi和蓝牙共用同一天线

#### （5）蓝牙主要特性
* 支持蓝牙5.0
* 全功耗模式（8dbm，和WiFi共用同一PA）

## 版权信息
> Apache license v2.0

## 目录结构
```tree
rtl872xd/
|-- 2nd_boot   # 二级boot
|-- aos        # 适配AliOS Things的文件
|-- aos_adapter # 适配AliOS Things的文件
|-- arch
|-- ate.bin
|-- bin
|-- build_bin.py
|-- CheckSumUtils.c
|-- CheckSumUtils.h
|-- csi_driver   # 适配CSI标准的驱动
|-- gen_crc_bin.py
|-- hal          # 适配HAL标准的驱动
|-- hci_driver
|-- lib
|-- ota.py
|-- package.yaml  # AOS编译配置文件
|-- pad.py
|-- pad.sh
|-- prebuild  # 二进制bin文件, 文件系统预置的资源文件
|-- prepend_header.py
|-- prepend_header.sh
|-- README.md
|-- release  # 烧录工具，及其烧录的系统镜像
|-- rlx8721d.ld # 系统链接脚本
|-- script  # 脚本文件
|-- sdk     # 芯片原厂驱动
|-- system.bin
|-- test
|-- test.sh
`-- tools
```

## 依赖组件
无

# 常用配置
无

# 注意事项

# FAQ
