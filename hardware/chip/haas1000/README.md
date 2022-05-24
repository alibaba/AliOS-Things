@page haas1000 haas1000

# 概述
**haas1000** 是一颗高性能的SoC， 内部集成了Cortex-M33双核心和Cortex-A7双核心。

## 芯片资源介绍
### 芯片资源
HaaS1000是一颗高度集成的SoC，其主要集成了如下模块：

（1）Cortex-M33双核心和Cortex-A7双核心<br>
（2）支持内部16MB PSRAM，2.5MB SRAM，16MB NOR flash<br>
（3）2.4G/5G双频 Wi-Fi 4（1x1 802.11 a/b/g/n）<br>
（4）双模蓝牙5.0<br>
（5）音频编解码模块<br>
（6）电源管理功能<br>
（7）远程声场的三路模拟麦克风阵列和六路数字麦克风阵列<br>

HaaS1000的M33核心可以运行蓝牙协议栈，A7核心可以运行语音处理和AI算法任务。其中Wi-Fi和蓝牙支持共天线和双天线方案，为性能和成本提供了灵活的可选择方案。其高集成度让开发者可以用很少的外围组件来设计完成一个完整的解决方案。HaaS1000芯片CPU及蓝牙/Wi-Fi主要特性如下：

#### CPU特性
（1）集成了PMU, CODEC, RF, BB, MCU和AP子系统的CMOS 单芯片<br>
（2）MCU 子系统采用300MHz ARM Cortex-M33 Star双核<br>
（3）AP 子系统采用具有NEON功能的1GHz CortexA7双核<br>
（4）片内共享2.5MB SRAM<br>
（5）16MB片上PSRAM<br>
（6）16MB片上QSPI NOR Flash<br>
（7）支持硬件加密引擎<br>
（8）支持 TrustZone<br>
（9）支持安全启动

#### BT/Wi-Fi
（1）Wi-Fi® IEEE 802.11 a/b/g/n<br>
（2）支持 2.4GHz Wi-Fi<br>
（3）支持 20MHz, 40MHz 带宽模式<br>
（4）Bluetooth® v5.0 双模式<br>
（5）支持 BLE Mesh<br>
（6）A2DP v1.3/AVRCP v1.5/HFP v1.6<br>
（7）支持 Wi-Fi/BT共存<br>

#### 外围接口特性
（1） 支持USB2.0 HS<br>
（2）支持3路UART，最高可到6Mbps<br>
（3）支持2路SPI，最高可到50Mbps，支持串口LCD<br>
（4）支持2路I2C，最高可到1.4Mbps<br>
（5）支持4路PWM<br>
（6）支持3路 10-bit GPADC，7 channels<br>
（7）支持40个GPIO可编程，每路都有独立中断<br>

## 版权信息
> Apache license v2.0

## 目录结构
```tree
haas1000
|-- aos  # 适配AliOS Things
|-- aos_adapter # 适配AliOS Things
|-- csi  # 适配CSI标准的驱动
|-- drivers # 芯片驱动
|-- _haas1000_alios.c
|-- _haas1000_alios.lds
|-- _haas1000_alios_lds.py
|-- _haas1000_alios_sec_b_a7.lds
|-- _haas1000_alios_sec_b.lds
|-- hal  #适配HAL标准的驱动
|-- macro_defines.h
|-- package.yaml # AOS编译配置文件
|-- prebuild  # 二进制文件与库
|-- README.md
|-- release
`-- ucube.py
```

## 依赖组件
无

# 常用配置
无

# 注意事项

# FAQ
