<div align="left">
<img src=./../images/HaaS100.png width=80%/>
</div>

HaaS100是高性价比的软硬件创新积木平台开发版，集成的是阿里云IoT团队自主研发的AliOS Things物联网操作系统。主控芯片HaaS1000是我们定制的高性价比物联网芯片。软件支持JS/Python轻应用和C/C++开发。本文主要介绍HaaS100如何快速开始HaaS-JS轻应用开发。

[HaaS100简介](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.641.bab84a12n8ZY7e)
[HaaS100硬件规格](https://help.aliyun.com/document_detail/184186.html?spm=a2c4g.11186623.6.643.6d2a7a29WmxqGh)


## 1、硬件与系统环境

### 硬件连接与启动

- 电源适配器供电
- 使用MicroUSB数据线连接开发板与PC

### 系统环境

#### Windows

安装[USB转串口驱动](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)，在设备管理器中可以看到以下串口：
> 不同PC的串口号不一致

<div align="left">
<img src=./../images/HaaS100-serial.png width=40%/>
</div>

#### macOS

安装[USB转串口驱动](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)，在dev目录下可以看到以下串口：

> crw-rw-rw-  1 root  wheel   18,   4  9 21 16:23 /dev/tty.SLAB_USBtoUART


## 2、JS轻应用固件

- [HaaS100 JS固件下载链接](https://hli.aliyuncs.com/o/config/haas_firmware/js_firmware_haas100.zip)

HaaS EDU K1和 HaaS100 支持多种固件烧录方式，

- 脚本烧录方式：将固件包解压，进入aos_burn_tool目录，打开cmd或者powershell终端，执行以下命令
> python flash_program.py --bin=ota_rtos.bin
- GUI烧录工具方式：windows用户也可参考《[HaaS100快速开始](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.701.60b15dbeSs37wz)》使用GUI工具烧录步骤进行固件更新。


## 3、JS轻应用命令行工具

- [amp工具下载链接](https://hli.aliyuncs.com/o/config/haas_firmware/amp-clitool.zip)


## 4、运行示例应用

### 获取示例代码

这里通过一个简单的LED闪灯的用例来介绍。首先下载《[HaaS100 LED亮灭示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haas100/gpio/output)》，示例代码目录如下所示：
```bash
app
├── app.json    # 应用与板级配置
└── app.js        # 用户代码
```

### 本地更新
- 推送脚本到设备
> ./amp-clitool/dist/amp-macos/amp serialput ./output /dev/cu.SLAB_USBtoUART  -m quickjs -e 0 -b 1500000

其中：    
（1）./amp-clitool/dist/amp-macos/amp：下载的JS轻应用命令行工具路径     
（2） ./output：是HaaS100 GPIO output控制板载LED闪烁的app示例代码路径     
（3） /dev/cu.SLAB_USBtoUART：是日志串口号，此处填写自己的串口号     
（4） mode：js引擎类型，可选[quickjs | duktape]。HaaS100默认应选quickjs     
（5） encode: 编译，可选项[0 | 1]，默认选择0不编译     
（6） -b 1500000 HaaS100串口波特率为1.5M    
注意：执行推送命令的时候需要确保COMXX串口没有被其他工具占用，否则会导致本地更新失败。

   - 复位开发板
执行以上amp serialput 命令之后，amp工具会有以下提示，此时尝试按HaaS100复位按键，复位硬件。
> Wait shakehand with board ...
> Please reboot board now

   - 等待脚本传输完成，打开日志串口观察脚本运行情况，传输完成之后，打印
> put complete!



## 5、附录： 
### 硬件资源介绍

#### 排针接口
<div align="left">
<img src=./../images/HaaS100-pins.png width=50%/>
</div>

#### 板载LED灯序号示意图
<div align="left">
<img src=./../images/HaaS100-leds.png width=20%/>
</div>

### 硬件资源映射表

| **功能/接口** | **PIN/IO/资源映射** | **轻应用 Port 配置** |
| --- | --- | --- |
| KEY1 | 板载按键 | port=26 |
| L1 | 板载LED1 | port=40 |
| L2 | 板载LED2 | port=41 |
| L3 | 板载LED3 | port=36 |
| L4 | 板载LED4 | port=35 |
| L5 | 板载LED5 | port=34 |
| I2C1 | SDA2 SCL2 | port=0 |
| I2C2 | SDA3 SCL3 | port=1 |
| ADC0 | ADC0 | port=0 |
| ADC1 | ADC1 | port=1 |
| ADC2 | ADC2 | port=2 |
| PWM0 | PWM0 | port=0 |
| PWM1 | PWM1 | port=1 |
| PWM2 | PWM2 | port=2 |
| PWM3 | PWM3 | port=3 |
| UART2 | 板级用户串口 | port=2 |
| SPI0 | 用户SPI0 | port=0 |

- 详细配置信息请参考示例代码中的app.json。
