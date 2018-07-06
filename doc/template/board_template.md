## board README.md模板

EN | [中文](./board_template-zh.md)

board README.md的模板可以参考[developerkit](../../board/developerkit/README.md).
### 规范

#### 语言规范

文档默认使用英文编写,当然也可以中英文都支持，中文的文档名称为`README-zh.md`，跟英文文档`README.md`同一个目录，文档中按照如下方式建立中英文索引：

[EN](README.md) | [中文](README-zh.md)

#### 图片规范

文档中涉及到的图片为了防止仓库过大，**请尽量使用外部链接**，当然，如果涉及到的图片需要保存到AliOS Things仓库里的，请统一保存到`AliOS-Things/doc/assets/`里面，通过相对路径引入图片，比如`board/developerkit/README.md`文件中可以这样引入图片：

`![](../../doc/assets/board_developerkit_1.jpg)`

图片类型不做要求，图片名称请按照如下格式命名：`board_<board_name>_<idx>`，比如：

* `board_developerkit_1.jpg`
* `board_linuxhost_10.gif`

#### 文档内容规范

board的README文档按照以下几种部分依次编写：

* 目录
* 开发板介绍（Overview）
* 开发板硬件资源（hardware Overview）
* AliOS Things支持的硬件资源
* 烧录和调试
* 其他（参考网站、官网）

### 目录规范

目录的链接直接使用`[<目录>](#目录)`的方式链接，比如[其他](#其他):

`[其他](#其他)`

如果**标题中包含空格**的话,`#`后面的目录标题需要使用`-`替代空格，比如[AliOS Things 支持的硬件资源](#AliOS-Things-支持的硬件资源):

`[AliOS Things 支持的硬件资源](#AliOS-Things-支持的硬件资源)`

如果**标题中包含括号**的话，括号中需要把括号删掉，比如[开发板介绍（Overview）](#开发板介绍overview):

`* [开发板介绍（Overview）](#开发板介绍overview)`

目录示例:

* [开发板介绍(Overview)](#开发板介绍overview)
* [开发板硬件资源（Hardware Overview）](#开发板硬件资源（Hardware-Overview）)
* [AliOS Things 支持的硬件资源](#AliOS-Things-支持的硬件资源)
* [烧录和调试(Download and Debug)](#烧录和调试（Download-and-Debug）)
* [其他](#其他)

### 开发板介绍格式说明

开发板介绍不做过多要求，建议按照如下几个部分介绍：
* 开发板简介
    可以引用官方的开发板简介
* 开发板图片
    可以引用官方的开发板宣传图片
* 官网
* 文档资料获取方式

### 开发板硬件资源格式说明

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

更多关于 **Developer Kit** 开发板的资料可以参考一下网址:
- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)
- [AliOS Things Developer Kit Hardware Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Hardware-Guide)

### AliOS Things 支持的硬件资源说明
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

### 烧录和调试格式说明

主要说明调试接口，调试方式，固件下载方式，运行效果图片等.

### 其他
* 官网
* 参考网站
* 开发者文档或者博客