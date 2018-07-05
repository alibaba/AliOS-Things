![](https://i.imgur.com/Q8Jcei1.png)

# AliOS Things Starter Kit Hardware Guide
---

## 1. 开发板概述
### 1.1 主板介绍

本开发板是基于STM32L433CCT6芯片所研发的一款智能开发板，STM32L433CCT6这款芯片具有高性能、低功耗的特点。其内核为ARM 32位Cortex-M4 CPU，最高80MHZ的主频率，256KB的闪存，82个高速IO口以及丰富的通讯接口。  
本开发板包括用户按键Key、用户LED、耳机、光距传感器、加速度传感器、LCD显示屏及Wi-Fi模块。

![](https://i.imgur.com/7P4d26x.png)

图1-1 开发套件组装图（顶视图）
  
本开发板可用于以下多个领域：  
智能家居  
智能交通

### 1.2 模块关系结构

开发套件主板的模块关系结构如图1-2所示。整体电路连接关系如下：5VUSB输入降压至3.3V，3.3V为STM32L433CCT6（MCU）和部分外设供电。LED灯、按键连接至MCU的IO口，由MCU控制； Audio codec 连接至 MCU 的I2S接口，由MCU 控制，光距传感器、加速度传感器通过I2C连接至MCU；LCD显示屏与MCU是SPI总线连接，Wi-Fi 模块通过UART总线连接至MCU。

### 1.3主板功能模块介绍

如下分别介绍主板上的芯片与主要功能模块。
![](https://i.imgur.com/PggFZPd.png)

图1-2 模块关系结构图

![](https://i.imgur.com/buHp7ha.png)


图1-3 主板各功能模块
  
STM32L433CCT6：开发板的主控芯片。主控芯片是整个板子的核心，STM32L433CCT6（以下用MCU称呼）是ST公司产品，我们采用的是LQFP48封装。主控芯片的电路原理图如图1-4所示。
![](https://i.imgur.com/4aZWSLM.png)

图1-4  主控芯片电路原理图  
USB： 开发板的电源输入口，连接PC USB5V 接口。开发板供电电压为 DC5V。  
开发板的下载调试口。连接USB数据线指电脑端，主板Default 配置是NUCLEO	Selected,连接USB至PC，用来进行开发板和PC端之间的交互，打印开发板的调试信息、下载程序。其原理图如图1-5所示。
![](https://i.imgur.com/TSEQ8SD.png)

图1-5 MCU下载调试串口


MCU_RST：MCU的复位按键。按下时MCU侧为低电平，MCU恢复到起始状态。其电路原理图如图1-6所示。
![](https://i.imgur.com/9pJJn5r.png)

图1-6  MCU的复位电路

KEY：3个用户按键。按键按下时，MCU侧为低电平；不按时，MCU侧为高电平。可供用户作为开关等使用，其电路原理图如图1-7所示。
![](https://i.imgur.com/9Ce7QBf.png)

图1-7  用户按键原理图

POWER：电源指示灯。如果5V稳压到3.3V成功，则表明系统供电正常，电源指示灯点亮。电源指示灯原理图如图1-8所示。

图1-8 电源指示灯

LED：2颗单色LED灯，一颗三色LED：当MCU的引脚输出为低电平时，LED会被点亮。可供用户测试开发板性能或用作信号灯。其电路图如图1-9所示。
![](https://i.imgur.com/WUMMXRh.png)

图1-9  用户LED灯
LCD：LCD显示屏接口，其电路原理图如图1-10所示。焊接时请观察FPC的接口信号标识，将其和主板的接口信号按对应关系焊接。
![](https://i.imgur.com/fgsDWQV.png)

图1-10  LCD显示屏接口原理图

Sensor：2个sensor均通过I2C与MCU连接，其电路原理图如图1-11/1-12所示。

![](https://i.imgur.com/feLsQgZ.png)

图1-11 G-sensor原理图

![](https://i.imgur.com/VNdwDoh.png)

     
图1-12 ALS-Prox-Sensor原理图

Audio：Audio通过I2S与MCU连接，电路原理图如1-13所示：
![](https://i.imgur.com/MF6D2DL.png)
 
图1-13 Audio codec原理图

Wi-Fi模块: Wi-Fi 块是通过小板的形式贴片在主板上，和主板的沟通通过UART 通信，电路原理图如1-14所示
![](https://i.imgur.com/NG1mIr5.png)
 
图1-14 Wi-Fi原理图


## 2  配套子板介绍

### 2.1  无线子板    
MW31是一款功能齐全、模块高度集成、低功耗的低成本嵌入式WiFi模组。核心芯片采用ARM968E-S，该模块是具有丰富的外围控制接口，内置IEEE802.11b/g/n无线标准协议栈的专业控制与传输为一体模块。可直接实现对外围数据采集，通过标准网络数据进行传送和接收网络数据控制外置设备。本模组具有非常低的待机电流，同时支持内部定时唤醒以及外部中断唤醒，所以对需要节能减排等要求的各类应用无线的场合都是最佳的选择。图2.1为Wi-Fi子板的正反面图

![](https://i.imgur.com/aytkATP.png)

 
图2.1  MW31子板


图2-1中的接口如表2-2所示：

![](https://i.imgur.com/iRqjf1w.png)

图2-2 MW31接口定义

MW31与主板的连接通过Pin1/2/7/21/22/23/24/25连接，串口UART1进行通讯，还有WAKE_UP和Reset 控制，通过MCU 控制可以连接到AP，将主板上的sensor 数据上传到云端。
MW31出厂时模块已经预烧录软件，如有调试需求可以通过OTA升级。

### 2.2  显示屏

![](https://i.imgur.com/bhERld0.png)

图2-3 LCD显示屏

LCD显示屏主要采用了ST7789V，ST7789V是一款240X240 点阵式SPI屏幕。LCD显示屏与主板的接口分别是：SPI_TX、SPI_FRAM、SPI_CLK、LCD D/C、VCC、VDDIO、。LCD的供电电压VCC是3.3V，SPI_TX、SPI_FRAM、SPI_CLK分别是SPI的数据线、片选和时钟线。3-SPI:IM0 接高，IM1接低；4-SPI:IM0 接低，IM1接高用户只需要按照对应接口焊接即可。

图2-3中的接口说明如表2-4所示：

表2-4  LCD接口说明

Pin | Define|Description
 - | :-: | :-: 
1|GND|地
2|	LEDK|	LED 负极
3|	LEDA|	LED 正极
4|	GND	地|
5|	RESET|	复位
6|	LCD_D/C|	LCD data /command
7|	SPI_TX	|SPI数据
8|	SPI_CLK	|SPI时钟
9|	VCC	|供电电压3.3V
10|	VDDIO|	IO 口工作电压3.3V
11|	SPI_FRAM|	SPI片选
12|	GND|	地
13|	IM0|	3线选择
14|	NC|	NC
15|	IM1|	4线选择
16|	NC|	NC
17|	GND|	地
18|	GND|	地


注意，此处接口定义，与主板上的LCD对应。

## 3  开发板硬件设计说明

### 3.1  电源部分 

主控芯片STM32L433CCT6的供电电压为3.3V，我们需要将输入的5V电压稳压至3.3V。
5V稳压至3.3V，采用的是ETA8120降压DC/DC，ETA8120 是一颗高效率的可调节输出的降压DC/DC，支持轻载高效，输出最大电流为2A，SOT23-6 的通用封装。从图5.42所示电源模块原理图可以看到，5V电源经过一个10uF和一个0.1uF去耦电容进入ETA8120的IN端，其OUT端经过一个0.1uF和一个4.7uF去耦电容输出电压3.3V。输入端的10uF的电容可以储存电量，0.1uF的电容的ESL更低，适应高频电路，所以二者结合使用可以有效的减小电源纹波，减小电源噪声以及提高瞬态电流的响应时间。输出端的0.1uF电容和4.7uF电容的作用也是去耦，减小输出电压的噪声和纹波。

![](https://i.imgur.com/FxjSEAh.png)


图3-1  电源线路图

### 3.2  USB ST-Link 电路
原理图上USB_DM/DP从USB接口连至F103的PA12/PA13引脚上，F103 的功能程序实现了VCP（虚拟COM口）功能：由STLINK_TX(PA2)、STLINK_RX(PA3)引出实现。
ST-LINK 程序下载通过3V3、GND、STM_JTCK、STM_JTMS，主板预留了测试点。ST-LINK程序会在出厂前下载烧录进去。

![](https://i.imgur.com/BkKdpud.png)

图3-3  F103线路图


### 3.3  开发板使用说明

#### 3.3.1开机
直接连接USB至电脑USB，开发板上电开机，这是会看到Power LED 亮起，RGB 三色灯会循环闪一遍，代表电源工作正常，打开PC Device manger,查看是否有映射ST-COM口。

![](https://i.imgur.com/uYsf0yU.png)

图3-3  上电状态

#### 3.3.2下载
通过USB 连接至PC， 打开ST-LINK Utility，点击connect，成功后选择烧录软件：STM32L433RCTxP.bin开始烧录。

![](https://i.imgur.com/qYss3fo.jpg)

图3-4  STM32 ST-LINK Utility程序下载软件

黄色LED 会闪烁，代表开发板正在烧录编程


## 4 小结
本章主要介绍了A10开发板硬件的组成部分、部分硬件电路设计及使用步骤和注意事项。本套开发板硬件是基于STM32L433CCT6芯片研发的一款高性能智能开发套件，为用户提供了更多的学习选择,本章介绍的几个硬件电路可以帮助用户更好的理解开发板，使用开发板。


#NOTE:
购买开发板AliOS Things Starter Kit和Developer kit，请扫描天猫旗舰店二维码：

![](https://i.imgur.com/VxHonLB.png)

获取在线技术支持，请扫描技术支持群：

![](https://i.imgur.com/uyT4v0Y.png)
![](https://i.imgur.com/rqjjjA0.jpg)