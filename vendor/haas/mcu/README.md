# 介绍
支持高性能HaaS1000 MCU的BSP代码。HaaS1000是一颗高度集成的MCU芯片, 基于Cortex-M33架构，主频300MHZ, 支持低功耗WIFI。

# 特性
- 基于Cortex-M33架构， 主频300MHZ。
- 存储：2.5M SRAM / 16M PSRAM / 16M NOR FLASH / 64KB BOOT ROM。
- 连接：2.4GHZ WIFI (802.11 a/b/g/n)
- 支持丰富的外设接口（I2C/UART/SPI/PWM/ADC/FLASH/GPIO）

# 目录
```sh
mcu
|-- aos            #适配AliOS Things操作系统的核心代码
|-- aos.mk         #AliOS Things的makefile
|-- Config.in      #AliOS Things的menuconfig配置文件
|-- drivers        #HaaS1000 MCU的驱动代码
|-- hal            #实现AliOS Things规定的HAL接口
|-- ota            #支持OTA升级的MCU级的代码
|-- prebuild       #预编译的bin文件
|-- README.md      #中文版README
|-- release        #编译的最终产物和烧录工具
|-- ucube.py
`-- vfs            #各驱动对接VFS并向上提供统一的VFS接口（open/read/write/close等）

```
# 依赖
- arch_armv8m-mk
- rhino
- drivers

# 使用
请参照集成HaaS1000 MCU的板子的使用说明。

# 参考
N/A
