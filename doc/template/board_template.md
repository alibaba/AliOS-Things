## board README.md模板

### 规范

#### 语言规范

文档默认使用英文编写,当然也可以中英文都支持，中文的文档名称为`README-zh.md`，跟英文文档`README.md`同一个目录，文档中按照如下方式建立中英文索引：

[EN](README.md) | [中文](README-zh.md)

#### 图片规范

文档中涉及到的图片都保存到`AliOS-Things/doc/assets/`里面，通过相对路径引入图片，比如`board/stm32f429zi-nucleo/README.md`文件中可以这样引入图片：
`![](../../doc/assets/board_stm32f429zi-nucleo_1.jpg)`

图片类型不做要求，图片名称请按照如下格式命名：`board_<board_name>_<idx>`，比如：
* `board_stm32f429zi-nucleo_1.jpg`
* `board_linuxhost_10.gif`

#### 文档内容规范

board的文档按照以下几种部分依次编写：
* 目录
* 开发板介绍（Overview）
* 开发板硬件资源（hardware Overview）
* AliOS Things支持的硬件资源
* 烧录和调试
* 其他（参考网站、官网）

### 目录
* [开发板介绍（Overview）](#开发板介绍（Overview）)
* [开发板硬件资源（Hardware Overview）](#开发板硬件资源（Hardware-Overview）)
* [AliOS Things 支持的硬件资源](#AliOS-Things-支持的硬件资源)
* [烧录和调试(Download and Debug)](#烧录和调试（Download-and-Debug）)

### 开发板介绍（Overview）

开发板介绍不做过多要求，建议按照如下几个部分介绍：
* 开发板简介
* 开发板图片
* 官网
* 文档资料获取方式

### 开发板硬件资源（Hardware Overview）

请详细列出开发板的板级硬件资源，包括cpu型号，cpu架构,cpu主频,flash大小,ram大小等,建议按照如下示例编写:

* cpu: STM32L496AGI6
* arch: Cortex-M4 with FPU
* frequency: 80 MHz
* flash: 1 MB
* SRAM: 320 KB
* debug interface: serial wire debug (SWD), JTAG, Embedded Trace Macrocell*
* communication interfaces:
    * USB
    * 2x SPI
    * 1x I2S
    * 2x I2C
    * CAN
    * SDIO
* other:
    * 4x ADC
    * DAC
    * PWM

More information about STM32L496AG can be found here:
- STM32L496AG on www.st.com
- STM32L496 reference manual

### AliOS Things 支持的硬件资源
该内容主要描述AliOS Things现在支持的硬件特性已经分区规划:

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

**按键:**


### 烧录和调试（Download and Debug）

### 其他
* 官网
* 参考网站
* 开发者文档或者博客