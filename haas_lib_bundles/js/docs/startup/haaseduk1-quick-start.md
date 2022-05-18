<div align="left">
<img src=./../images/haaseduk1.png width=80%/>
</div>

[HaaS EDU K1说明书](https://help.aliyun.com/document_detail/205267.html?spm=a2c4g.11186623.6.699.49c01a11H)


## 1、硬件与系统环境
### 硬件连接与启动

- USB Type-C供电
- 电源开关拨到ON
### 系统环境
#### Windows
安装 [旧版haaseduk1 USB转串口驱动](https://ftdichip.com/drivers/vcp-drivers/?spm=a2cre.b5050543.0.0.777a222avvAJTT) 或者 [新版haaseduk1 USB转串口驱动](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip)。
在设备管理器中可以看到以下串口（不同PC的串口号可能不一致）：

<div align="left">
<img src=./../images/haaseduk1-serial.png width=40%/>
</div>

#### macOS
安装 [旧版haaseduk1 USB转串口驱动](https://ftdichip.com/drivers/vcp-drivers/?spm=a2cre.b5050543.0.0.777a222avvAJTT) 或者 [新版haaseduk1 USB转串口驱动](https://www.silabs.com/documents/public/software/Mac_OSX_VCP_Driver.zip)。

在dev目录下可以看到以下串口：

> crw-rw-rw-  1 root  wheel   18,   4  9 21 16:23 /dev/tty.usbserial-AU03OJU1

## 2、JS轻应用固件

- [HaaS EDU K1 JS固件下载链接](https://hli.aliyuncs.com/o/config/haas_firmware/js_firmware_haaseduk1.zip)

HaaS EDU K1和 HaaS100 支持多种固件烧录方式，

- 脚本烧录方式：将固件包解压，进入aos_burn_tool目录，打开cmd或者powershell终端，执行以下命令
> python flash_program.py --bin=ota_rtos.bin

- GUI烧录工具方式：windows用户也可参考《[HaaS100快速开始](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.701.60b15dbeSs37wz)》使用GUI工具烧录步骤进行固件更新。

## 3、JS轻应用命令行工具
- [amp工具下载链接](https://hli.aliyuncs.com/o/config/haas_firmware/amp-clitool.zip)

## 4、运行示例应用
### 获取示例代码
这里通过一个简单的LED闪灯的用例来介绍。首先下载《[HaaS EDU K1 LED亮灭示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/gpio/output)》，示例代码目录如下所示：
```bash
app
├── app.json      # 应用与板级配置
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
#### 整机接口
<div align="left">
<img src=./../images/haaseduk1-outline.png width=90%/>
</div>

#### 扩展接口
<div align="left">
<img src=./../images/haaseduk1-pins.png width=90%/>
</div>

更多详细硬件资源请参考《[HaaS EDU K1硬件介绍](https://blog.csdn.net/HaaSTech/article/details/113985910?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522162331632416780366524613%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=162331632416780366524613&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_v2~rank_v29-1-113985910.nonecase&utm_term=HaaS+EDU+K1&spm=1018.2226.3001.4450)》


### 功能列表（部分）

- 文件系统 FS
- 系统信息 SYSTEM
- 键值对存储 KV
- 硬件I/O
   - UART
   - GPIO
   - I2C
   - SPI
   - ADC
   - PWM
- 网络协议
   - UDP
   - TCP
   - MQTT
   - HTTP
- 高阶功能
   - IoT 阿里云IoT平台连接组件

### app.json 参考示例
- 详细配置信息请参考示例代码中的app.json。

```json
{
    "version": "1.0.0",
    "io": {
        "KEY1": {
            "type": "GPIO",
            "port": 23,
            "dir": "irq",
            "pull": "pullup",
            "intMode":"falling"
        },
        "KEY2": {
            "type": "GPIO",
            "port": 20,
            "dir": "irq",
            "pull": "pullup",
            "intMode":"falling"
        },
        "KEY3": {
            "type": "GPIO",
            "port": 21,
            "dir": "irq",
            "pull": "pullup",
            "intMode":"falling"
        },
        "KEY4": {
            "type": "GPIO",
            "port": 26,
            "dir": "irq",
            "pull": "pullup",
            "intMode":"falling"
        },
        "L1":{
            "type":"GPIO",
            "port":36,
            "dir":"output",
            "pull":"pulldown"
        },
        "L2":{
            "type":"GPIO",
            "port":35,
            "dir":"output",
            "pull":"pulldown"
        },
        "L3":{
            "type":"GPIO",
            "port":34,
            "dir":"output",
            "pull":"pulldown"
        },
        "P04":{
            "type":"GPIO",
            "port":4,
            "dir":"output",
            "pull":"pulldown"
        },
        "P05":{
            "type":"GPIO",
            "port":5,
            "dir":"output",
            "pull":"pulldown"
        },
        "P06":{
            "type":"GPIO",
            "port":6,
            "dir":"output",
            "pull":"pulldown"
        },
        "P07":{
            "type":"GPIO",
            "port":7,
            "dir":"output",
            "pull":"pulldown"
        },
        "oled_dc": {
            "type": "GPIO",
            "port": 28,
            "dir": "output",
            "pull": "pulldown"
        },
        "oled_res": {
            "type": "GPIO",
            "port": 30,
            "dir": "output",
            "pull": "pulldown"
        },
        "oled_spi": {
            "type": "SPI",
            "port": 1,
            "mode": "mode3",
            "freq": 26000000
        },
        "SPI0":{
            "type": "SPI",
            "port": 0,
            "mode": "mode1",
            "freq": 26000000
        },
        "serial": {
            "type": "UART",
            "port": 2,
            "dataWidth": 8,
            "baudRate": 115200,
            "stopBits": 1,
            "flowControl": "disable",
            "parity": "none"
        },
        "sensor": {
            "type": "I2C",
            "port": 1,
            "addrWidth": 7,
            "freq": 400000,
            "mode": "master",
            "devAddr": 64
        },
        "ADC0":{
            "type": "ADC",
            "port": 0,
            "sampling": 12000000
        },
        "ADC1":{
            "type": "ADC",
            "port": 1,
            "sampling": 12000000
        },
        "ADC2":{
            "type": "ADC",
            "port": 2,
            "sampling": 12000000
        },
        "PWM0": {
            "type": "PWM",
            "port": 0
        },
        "PWM1":{
            "type":"PWM",
            "port": 1
        },
        "PWM2":{
            "type":"PWM",
            "port": 2
        },
        "TIMER0": {
            "type": "TIMER",
            "port": 0
        },
        "TIMER1": {
            "type": "TIMER",
            "port": 1
        }
    },

    "pages": [
        "data/jsamp/uipages/page/waring"
    ],

    "debugLevel": "DEBUG",
    "repl":"disable"
}
```
