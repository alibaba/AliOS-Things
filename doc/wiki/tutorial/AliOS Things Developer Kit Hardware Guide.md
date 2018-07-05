![](https://i.imgur.com/Q8Jcei1.png)

# AliOS Things Developer Kit Hardware Guide
---

## 1. 概述
### 1.1 工具包概述
这是一款由上海诺行信息技术有限公司基于STM32L496VGx(本文MCU代替)设计的高性能物联网开发板。用于提供给开发者评估，设计相关物联网的应用产品。本文档将简要介绍开发板的硬件开发工具包，MCU开发主板的特性，各个主要接口外设信息，以及协助使用者做物联网的开发设计事项。
开发板功能区分布图：

![](https://i.imgur.com/TNU2FCM.png)

图1-1开发板功能区分布


### 1.2 系统要求
- Windows○REA OS（7,8 and 10），LinuxA○REA 64-bits or macOSA○RE   
- USB Type-A to Micro USB cable  
- Stereo headset line

### 1.3 开发工具
- KeiA○REA:MDK-ARM  
- IARTM:EWARM  
- GCC-base IDEs includeing free SW4STM32 from AC6  


## 2. STM32L496VGx开发主板
### 2.1特性
开发板是基于STM32L496VGTx芯片研发的一款物联网开发板，STM32L496VGTx这款芯片具有高性能、低功耗的特点。 其内核为ARM 32位Cortex-M4 CPU,最高80MHZ的主频率，1MB的闪存320KB的SRAM，最多支持136个高速IO口，还支持SPI，CAN,I2C,I2S,USB,UART等常用的外设接口。

- 特性:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;•CPU: STM32L496VGTx, 80MHz  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 存储: 1MB Flash, 320KB SRAM  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• Wi-Fi：- 2.4G,802.11b/g/n 协议  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 支持 OTA 升级  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 支持一键配网  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 音频: 带麦克风，带耳机，支持语音识别功能  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 传感器：  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 3D 加速度传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 陀螺仪感应器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 磁力计传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 压力传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 温湿度传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 光线传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 距离传感器  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 摄像头: 支持8-bits并行接口的摄像头，像素为0.3M;分辨率640*480  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 主板供电：通过USB 5V供电或者外部5V供电；也可以单独3V供电；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 显示屏: 1.3’ TFT ,240*240分辨率；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 支持红外发送和红外接收；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• LED 灯：  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 三色 RGB LED ，侦测WIFI状态；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 上电指示LED，绿色；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 下载指示LED ，橙色；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- 三个用户定义 LED ，橙色  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 系统: Ali-OS Things  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• USB 支持OTG 功能  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• On-board ST-Link/V2  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• PCIe: 支持外接USB 接口的LTE模块；也支持UART 接口的Zigbee、Lora、NB模块；  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• 按键：一个复位按键，三个功能按键  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;• SD卡：系统支持最大32Gb的SD 卡存储扩展；  

- 电源特性 
 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Micro USB接口，5V供电  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;内部有5V转3V的DCDC, 和1.8V LDO  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MCU供电电压为3V，系统IO电压也为3V  

- XTAL  

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;8MHz  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;32.768KHz

- 调试接口 
 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;USB 转ST-Link  

开发板给使用者预留很多通用接口：
  
- Zigbee/LoRa 接口  
- I2C 接口  
- ADC 差分/单端接口  
- Arduino 接口  
- SPI 接口  

### 2.2 开发板硬件信息
开发板实物正面图：

![](https://i.imgur.com/gX9BYa1.jpg)

图2-1 开发板正面图


开发板背面图：

![](https://i.imgur.com/cKrftvW.jpg)

图2-2 开发板背面图


### 2.3开发板框架

开发板的系统框图如图2-3：

![](https://i.imgur.com/OT0e1rp.png)

图2-3 开发板框图

电路连接关系如下：系统由USB 5V供电，经过DCDC降压至3 V给系统大部分器件供电，为系统主要电源。MCU 由STM32F103 的MCO 提供8MHz；LED灯、按键连接至MCU的GPIO；Audio codec（芯唐电子 ISD9160 方案）连接至 MCU 的I2S接口，控制信号走I2C4；ACC/Gyro-sensor 和电子罗盘连接至MCU的I2C4 接口上，PL-sensor、压力传感器、温湿度传感器是接在MCU 的I2C2上。MCU 支持外接8bit Camera, 同时I2C3 也用来走Camera控制信号；开发板自带1.44’ LCD，属于SPI 4-line 接口；WI-FI module 采用的是BK7231芯片方案，与MCU 通过UART接口通讯，板载WI-FI 天线。开发板支持USB OTG 接口，同时也支持PCIe 接口的LTE 模块（USB 接口形式），但是MCU自身只有一个USB PHY，所以USB OTG 和LTE模块在开发板上是通过软件配置做二选一。LTE 模块供电是3.8V, 中间加了一个5V-3.8V 的DCDC。STMF103 的晶振是8MHz和32.768KHz,然后输出MCO 8MHz 给STM32L496,Audio Codec也需要挂一个32.768KHz 晶体。

### 2.4 MCU 主板外设接口介绍
MCU主板使用到的主要外设接口有：8bits 并行DCMI（Digital camera interface） 接口，4-Line SPI 接口，SDMMC 接口，I2C 接口，UART接口，USB接口，I2S audio 接口，还有若干GPIO 配置。图2-4为STM32L496VGTx 的系统框图：
![](https://i.imgur.com/Syl3Rj3.png)

图2-4 STM32L496 BLOC

DCMI接口

MCU自身带一个DCMI接口，支持接入8-bits至14bits的并行数据口，其最大速率可以支持到54MB/s。开发板套件配的Camera 是由深圳影领电子有限公司提供的YL-F170066-0329.采用的是8bits 数据接口，通过24Pin BTB连接器外接一个Camera模组。Camera配置通过I2C3接入MCU，地址为0x78/0x79。BTB连接器的接口定义如下表：

![](https://i.imgur.com/Q3O6jxD.png)


表2-1 Camera 接口定义

USB接口

开发板有两个USB 接口，一个为USB ST-link接口，作用为软件下载/调试/系统供电输入口。另一个为USB OTG，用户可以外接USB 设备，支持1000mA,USB2.0 设备接入。USB接口截图如图2-5：

![](https://i.imgur.com/kUoRvKP.png)

图2-5 USB 接口截图

USB ST-Link（USB1） 接口除了给系统提供电源之外，还是开发板的下载接口,与STM32F103的USB接口相连接，插入PC之后会映射出一个COM 口设备，用来下载MCU 的软件，也可以用来打印调试信息。
STM32F103 与MCU 之间是通过SWD接口相连接，其原理图如图2-6所示。USB数据线连接至PC，用来进行MCU开发板和PC端之间的交互，打印开发板的调试信息、下载MCU程序。

![](https://i.imgur.com/i7ssQe3.png)

图2-6 SWD下载调试串口

PCIe模块

开发板中还带有一个PCIe卡槽，用来做模块（LTE模块，Zigbee/Lora/NB-IoT模块）接入，其安装方式参考图2-7。

![](https://i.imgur.com/6eRLX01.gif)

图2-7 LTE 模块

LTE模块由上海诺行信息技术有限公司提供，采用Marvell 的MP1802 作为CPU,具有传输速率高，功耗低，尺寸小等特点。模块支持多个Band，能兼容移动/联通/电信多种SIM 卡实现LTE网络接入。LTE 模块与USB OTG 同时挂在MCU 的USB 外设下，只能做二选一，不可以同时工作。LTE 模块与MCU 之间通过USB 接口通讯，MCU提供给模块一个RESET控制信号。模块供电可以通过J408来选择3V和3.3V，具体Pin 定义请参考原理图2.8，也可以参考PCIe的Pin定义说明。J403 是预留给LTE-module 下载软件使用，当跳线帽选择ST-link（默认配置），USB 连接器是直接与MCU接通，当跳线帽是选择Module ，USB 连机器是与模块的USB接通，就可以做模块软件的升级；模块还附带有SIM 卡接口，图2-8为其接口原理图。

![](https://i.imgur.com/rpMW5HJ.png)

图2-8 PCIe接口原理图


PCIe 接口还支持接入Zigbee/Lora/NB-IoT模块，具体请参考Pin 定义。

按键

开发板带有三个功能按键，和一个系统Reset按键。功能按键可以提供给使用者做功能定义开发，都是使用GPIO 口，方向为输入，低电平有效。  
复位按键是直接接入STM32F103和MCU 的硬件复位Pin，按下复位按键，系统自动重启复位。图2-9 为复位按键。
  
![](https://i.imgur.com/MMLVAuQ.png)

图2-9 复位按键

LED指示灯

MCU开发板总计有6个LED 灯，其中有1个电源指示LED（绿色） 灯上电就被点亮，1个下载调试LED 灯（橙色）上电也常亮，当下载的时候会闪烁，3个提供给用户定义的LED灯，都是接入MCU 的GPIO ,拉低IO口即可点亮。开发板还有一个三色LED灯，是有WI-FI 模块控制的LED 灯。  
电源LED 灯在USB 供电正常之后会常亮，如果插入USB 之后电源LED 没有被点亮，证明USB 供电异常。其原理图如图2-10。

![](https://i.imgur.com/jFBOld8.png)

图2-10 电源和调试LED 灯

4-Line SPI接口

4-Line SPI是LCD显示屏的接口，其电路原理图接口定义如图2-7所示。开发板套件中包含有一个FPC 材质的LCD 屏幕，选用的为深圳市禹龙现代电子有限公司的H144TS121F, 出厂时候就已经装配好。其Pin定义如下表格，详情请参考附录。

![](https://i.imgur.com/o1kaUrp.png)


表2-2 OLED显示屏接口原理图

传感器

MCU 开发板为了丰富用户开发，提供有8个sensor，其中light &proximity sensor，pressure传感器，H&T传感器是接在MCU 的I2C2，G-sensor 和Compass sensor是接在MCU 的I2C4上。开发板上的MCU 总共使用了3组I2C，具体接口和I2C 地址分配请参考下面表格。

![](https://i.imgur.com/ev43DA7.png)


表2-3 I2C分配以及地址

I2S audio接口

Audio 部分是采用芯唐电子的ISD9160方案。ISD9160 是一款非常优秀，支持audio录制以及播放，内嵌ARMCotex-M0的低功耗微控芯片。最高处理速度可以达到50MHz，有145KB Flash和12KB的SRAM，具备支持多种常用的外设接口。ISD9160 的内部框图如图2-11所示。芯唐电子的audio方案除了正常语音功能外，还支持云端存储方式的语音识别功能。

![](https://i.imgur.com/ol396la.png)

图2-11 ISD9160 框图

STM32L496VGT本身具有两个SAI(Serial audio interface) 接口，在本开发板中使用了SAI2, 并且配置成I2S协议与ISD9160通讯。此语音模块的带一个外置的Mic做语音录制，带3.5mm标准耳机插座，支持CTIA标准耳机（不支持耳机麦克功能）。Audio模块与主板沟通除了I2S，还需要I2C 做指令控制，I2C地址为软件配置。Audio部分原理框图如图2-12：

![](https://i.imgur.com/c3A75eE.png)

图2-12 Audio 模块原理框图

WI-FI模块

WIFI模块采用的是由上海诺行生产的MW31模块。这是一款功能齐全、集成度高、功耗低的低成本嵌入式WI-FI模组。模块采用BK7231方案实现WI-FI无线网络接入。该模块是具有丰富的外围控制接口，支持IEEE802.11b/g/n无线标准协议栈。本模组具有非常低的软关机电流和睡眠电流，同时支持内部定时唤醒以及外部中断唤醒，所以对需要节能减排等要求的各类应用无线的场合都是最佳的选择。模块由上海诺行信息技术有限公司设计，其外形如图2-13，与主板采用邮票孔方式焊接。

![](https://i.imgur.com/ogfoSwm.png)

图 2-13 WI-FI 模块外形图


WI-FI 模块与MCU 之间采用的是UART接口通讯。MCU 通过下发AT指令，可以控制模块连接到WI-FI AP，实现网络接入，将sensor以及其他数据上传到Ali云端。AT指令请参考附录表格，模块接口的Pin定义如图2-14：

![](https://i.imgur.com/6qrMdOO.png)

图2-14 WI-FI 模块pin 定义

SDMMC 接口  
MCU的SDMMC接口兼容MMC V4.2接口规格，能支持1-bits,4-bits，8-bits三种不同的数据总线。在8bits模式下，最高数据传输速率支持48MHz。开发板支持外挂4bits模式的SD卡。如图2-15为SD卡接口原理图：

![](https://i.imgur.com/pzqqyKB.png)

图 2-15 SD 卡接口原理图

扩展接口（Arduino 接口） 
 
目前开发板有给用户预留很多扩展接口在开发板背面。有预留 Uart4，I2C3,SPI2，ADC,PWM，以及一些 GPIO ，供用户自定义开发使用。定义如图 2-16

![](https://i.imgur.com/dPH7aG5.png)

图 2-16 扩展接口定义图

目前在开发板上，Arduino 接口和 Camera ，TF 卡，PCIE 接口是复用的。当用户使用 Arduino 接口时候，请注意：  
当使用 Arduino PD7 时候，需要将 Security 功能关闭;    
当使用 Arduino 接口的 Uart4 时候，需要将 Lora/NB 模块断电（J408 去掉跳线帽）；    
当使用 Arduino 接口的 I2C3 时候需要软件关闭 Camera 和 TF 卡功能；    
当使用 Arduino 接口的 SPI 接口的时候需要将 Camera 功能关闭 。    
下面表格 2-4 是 Arduino 使用的 Pin 定义。


位号 Arduino 接口 Pin Name 说明

![](https://i.imgur.com/fzld3Ta.png)


表 2-4 Arduino 接口定义说明

表格 2-4 中，没有特别说明的，都是 GPIO 口，请用户自定义配置。  
另外说明，A20V12 版本中 J903 的前两个丝印是 RX/PA3，TX/PA2，对应的是 Uart2； 但是在 A20V13 版本中改成 RX/PA1，TX/PA0 对应的是 Uart4（由于A20V13 增加了 Security ，Uart2 被占用），请使用时候注意这个改动。  
Camera 使用的 I2C3 是 PC0/PC1，在添加 Arduino 接口的时候，由于 PC1 被SPI2 占用，将 I2C3 更换成 PC0/PC9(Camera 不变)，所以，若用户需要使用这个外接的 I2C3，请重新配置这个接口。


## 3.主板电源部分

开发板上除了温湿度传感器的供电电压为1.8V和PCIe模组供电为3.3V，其他IC 供电电压为3V，所以我们需要将输入的5V电压转换成3组电，3V，3.3V和1.8V。  
5V转换至3 V和3.3V，都是采用的是ETA8120降压DC/DC。ETA8120 是一颗高效率的可调节输出的降压DC/DC，支持轻载高效，输出最大电流为2A，SOT23-6 的通用封装。  
3V经过LDO转换至1.8V，采用的是SGM2019 LDO，输出最大电流为0.3A。1.8V只供给温湿度传感器使用，需要的电流较小。

![](https://i.imgur.com/U1Zzbtl.png)

图3-1 电源原理图

## 4.下载

下载之前，需要先安装STM32 ST-LINK Utility。安装完成之后，将开发板通过Micro USB Cable 与PC连接之后，按照提示安装ST 驱动，安装之后如果在设备管理器中能找到ST端口，证明驱动安装成功，如图4-1。

![](https://i.imgur.com/W0Xl1ND.png)

图4-1 安装ST设备驱动


然后打开STM32 ST-LINK Utility，先通过File Open file加载需要下载的Bin文件，然后点击Target . Connect，正常时候会提示connect 成功，如图4-2 所示。提示成功之后，证明软件下载环境已经配置好，请通过Target . Program，在弹出的对话框中点击Start 直接下载，如图4-3所示。下载时候调试LED 灯应该是持续闪烁，直到退出下载（关闭ST-LINK Utility）。

![](https://i.imgur.com/t0zulsr.png)

图4-2 下载Connect成功图

![](https://i.imgur.com/A8O5ZK1.png)

图4-3 STM32 ST-LINK Utility程序下载软件

# 5 Pinout Configurations

![](https://i.imgur.com/NpeEPI5.png)

# 6 Pins Configuration

![](https://i.imgur.com/VTCDGWv.png)

![](https://i.imgur.com/NSSUgNB.png)

![](https://i.imgur.com/qwmxvYe.png)


*The pin is affected with an I/O function


## 7 Clock Tree Configuration

![](https://i.imgur.com/uyimqaL.jpg)


## 8 GPIO configuration
![](https://i.imgur.com/caEUSdW.png)

![](https://i.imgur.com/ReHnBEt.jpg)

![](https://i.imgur.com/fkciS1U.jpg)

![](https://i.imgur.com/gK33Dj2.png)

## 9 Camera结构图

![](https://i.imgur.com/AohAavC.jpg)

## 10 LCD结构图
![](https://i.imgur.com/RKCOZCR.png)

## 11 Layout Mark
![](https://i.imgur.com/u2ihkG0.png)

![](https://i.imgur.com/s5oOePw.png)


#NOTE:
购买开发板AliOS Things Starter Kit和Developer kit，请扫描天猫旗舰店二维码：

![](https://i.imgur.com/VxHonLB.png)

获取在线技术支持，请扫描技术支持群：

![](https://i.imgur.com/uyT4v0Y.png)
![](https://i.imgur.com/rqjjjA0.jpg)