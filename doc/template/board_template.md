## board README.md模板

EN | [中文](./board_template-zh.md)

board README.md的模板可以参考[developerkit](../../board/developerkit/README.md).

### 规范

文档应该简明扼要，只需要简单的说明即可，更具体的内容请引用外部的链接（github wiki，云栖博客等）。

#### 语言规范

文档**默认使用英文编写**，当然也可以中英文都支持，中文的文档名称为`README-zh.md`，跟英文文档`README.md`同一个目录，文档中按照如下方式建立中英文索引：

[EN](README.md) | [中文](README-zh.md)

#### 图片规范

为了防止文档过于复杂，**文档中不建议引用图片**，如果一定要引用图片的话，**请尽量引用外部链接**。

#### 文档内容规范

board的README文档按照以下几种部分依次编写：

* 目录
* 开发板介绍
* 开发板硬件资源
* AliOS Things支持的硬件资源
* 参考例子
* 烧录和调试
 
### 开发板介绍

开发板介绍不做过多要求，建议按照如下几个部分介绍：

* 开发板简介
* 官网及参考网站
* 开发者文档或者博客

### 开发板硬件资源

请详细列出开发板的板级硬件资源，包括cpu型号，cpu架构，cpu主频，flash大小，ram大小等，建议按照如下示例编写:

* cpu: STM32L496AGI6
    * arch: Cortex-M4 with FPU
    * frequency: 80 MHz
* D-Camera: 640*480 , 8-bits parallel interface
* flash: 1 MB
* SRAM: 320 KB
* Audio: Speech recognition resolution, Headphone and Microphone
* SDMMC: support 4bits mode, Maximum support for 32G
* LED: 1RGB LED ,5 user LEDs
* Four Keys: one reset key and three for user
* sensor
    * 3D Digital accelerometer
    * Gyroscope
    * Compass sensor
    * Pressure sensor
    * Humidity and Temperature Sensor
    * IR detector and emitter
    * Digital ALS and Proximity sensor
* Micro-USB: 5V power supply
* debug interface: serial wire debug (SWD), JTAG, Embedded Trace Macrocell*

更加详细的说明可以引用外部链接。

### AliOS Things 支持的硬件资源

该内容主要描述AliOS Things现在支持的硬件特性以及分区规划:

**分区表(flash partitions):**

|起始地址    |大小            |可读|可写|说明       |
|-----------|---------------|:-:|:-:|-----------|
|0x000000   |0x08000 (32k)  |:o:|:x:|bootloader |
|0x00B000   |0xF2000 (568k) |:o:|:o:|Application|
|0x0FD000   |0x01000 (4k)   |:o:|:o:|PARAMETER1 |
|0x00B000   |0x02000 (8k)   |:o:|:o:|PARAMETER2 |
|0x100000   |0x8E000 (568k) |:o:|:o:|OTA Storage|
|0x1FD000   |0x01000 (4k)   |:o:|:o:|PARAMETER3 |
|0x1FE000   |0x02000 (8k)   |:o:|:o:|PARAMETER4 |

**LCD:**

**按键和LED:**

**传感器:**

### 参考例子

**参考例子**主要列出来一些官方验证的example。

### 烧录和调试

**烧录和调试**主要说明调试接口，调试方式，固件下载方式，也可以引用外部链接。
