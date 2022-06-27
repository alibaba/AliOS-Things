> **学习物联网，来HaaSEDU就对了**

# Columbus（哥伦布）开发板介绍

## 一、概述
Columbus（哥伦布）开发板是01Stuido推出的开发板，该开发板使用STM32F407ZGT6作为主控芯片，外接丰富的外设接口，包括Camera，LCD，Audio，Ethernet等。

STM32F407ZGT6片内集成ARM Cortex-M4 32位 MCU+FPU，处理速度高达210DMIPS，支持高达1 MB Flash/192 + 4KB RAM， 支持USB OTG HS/FS模式，独立的以太网控制接口，17个Timer，3个ADC，15 个通信接口及摄像头。

## 二、硬件资源
### 1、开发板资源介绍
#### （1） 主板配置

<div align="center">
<img src=../../images/ext_columbus_功能配置.png  width=90%/>
</div>

上图是Columbus开发板，外形大小合理：16.0cm * 11.0cm ，具体开发板资源如下表：

| 名称 | 描述 | 
| :- |  :- |
| 主控 |  STM32F407ZGT6，Cortex M4，主频168MHz |
| 存储 |  1）SRAM: 1M Bytes；2）FLASH: 16M Bytes；3）EEPROM：AT24C02，256 Bytes；4）SDIO：MicroSD 卡插槽 |			
| 传感器 |  1）温度传感器：DS18B20；2）温湿度传感器：DHT11；3）六轴传感器：MPU6050(3轴加速度+3轴陀螺仪)；4）传感器外接：3P防呆接口|
| 输入/输出 | 1）4个LED；2）4个按键；3）电位器：ADC输入；4）有源蜂鸣器；5）OLED显示屏接口；6）4.3寸触摸屏LCD接口 |		
| 多媒体 | 1）音频解码芯片：播放和录音；2）麦克风输入；3）板载扬声器：8欧1瓦；4）立体音频输入/输出接口；5）摄像头接口 |
| 通讯 | 1）USB: TYPE-C接口（供电，烧录，UART，REPL调试多合一）；2）百兆Ethernet接口；3）CAN接口；4）RS485接口；5）pyIoT无线模块接口；6）I2C/UART接口；7）4P防呆接口 |
| 电源 | 1）USB供电；2）5V电源适配器；3）3.6-4.2V锂电池供电；4）RTC后备电池：CR1220；5）3.3V/5V排针 |
| 其他 | 3路舵机接口；GPIO接口 |


#### （2）扩展接口
下图是Columbus开发板扩展接口，共有144个引脚，其中1个3.3V,2个GND，1个BOOT0，多数GPIO还复用了其他功能，比如：IIC、ADC、SPI、UART、ADC等功能。。具体接口定义如下图所示（***其中的5V0管脚是pyBoard_Pro开发板特有功能，在Columbus上该管脚配置为GND***）：
<div align="center">
<img src=../../images/ext_STM32F407_01Studio_pyBoard_Pro_扩展接口.png  width=90%/>
</div>

### 2、芯片资源介绍
#### （1） 内核
* 带有 FPU 的 ARM® 32 位 Cortex®-M4CPU， 主频高达 168MHz
* 在 Flash 存储器中实现零等待状态运行性能的自适应实时加速器 (ART 加速器 ™)
* MPU能够实现高达210 DMIPS/1.25DMIPS/MHz (Dhrystone 2.1)的性能
* 具有 DSP 指令集

#### （2）存储器
* 高达1 MB Flash
* 高达 192+4 KB的SRAM， 包括64-KB的CCM（内核耦合存储器）数据 RAM
* 具有高达 32 位数据总线的灵活外部存储控制器： SRAM、 PSRAM、 NOR/NAND存储器

#### （3）时钟、复位和电源管理
* 1.8 V 到 3.6 V 供电和 I/O
* POR、 PDR、 PVD 和 BOR
* 4 至 26 MHz 晶振
* 内置经工厂调校的 16 MHz RC 振荡器（ 1% 精度）
* 带校准功能的 32 kHz RTC 振荡器
* 内置带校准功能的 32 kHz RC 振荡器

#### （4）低功耗
* 睡眠、停机和待机模式
* VBAT 可为 RTC、 20×32 位备份寄存器 + 可选的 4 KB 备份 SRAM 供电

#### （5）外设接口
* 3 个 12 位、 2.4 MSPS ADC：多达 24 通道，三重交叉模式下的性能高达 7.2 MSPS
* 2 个 12 位 D/A 转换器
* 通用DMA：具有FIFO和突发支持的16路DMA控制器
* 多达17个定时器： 12个16位定时器，和2个频率高达 168 MHz 的 32 位定时器，每个定时器都带有 4 个输入捕获/输出比较/PWM，或脉冲计数器与正交 ( 增量 ) 编码器输入
* 多达 3 个 I2C 接口 (SMBus/PMBus)
* 高达 4 个 USART/4 个 UART（ 10.5 Mbit/s、ISO7816接口、 LIN、 IrDA、 调制解调器控制）
* 高达 3 个 SPI（ 42 Mbits/s），2 个具有复用的全双工 I2S， 通过内部音频 PLL 或外部时钟达到音频级精度
* 2 个 CAN （ 2.0B 主动）以及 SDIO 接口

#### （6）高级连接功能
* 具有片上PHY的USB 2.0全速器件/主机/OTG控制器
* 具有专用 DMA、片上全速 PHY 和 ULPI 的USB 2.0 高速 / 全速器件 / 主机 /OTG 控制器
* 具有专用 DMA 的 10/100 以太网 MAC：支持IEEE 1588v2 硬件， MII/RMII

#### （7）DCMI（数字摄像头接口）
* 位并行照相机接口：速度高达 54MB/s
  
#### （8）其他模块
* 真随机数发生器
* CRC 计算单元
* RTC：亚秒级精度、硬件日历
* 96 位唯一 ID

## 三、学习指南
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于HaaS EDU K1的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问HaaS官网(http://haas.iot.aliyun.com)。
