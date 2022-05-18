<div align="left">
<img src=./../images/ec600s.png width=90%/>
</div>

HaaS600 Kit 是一款基于 HaaS600系列模组评估的高性价比开发板。可应用于共享控制、金融支付、智能语音、泛工业等场景的智能硬件产品开发。


HaaS600 Kit 套件包含以下物料：


- 开发板 x 1
- micro-USB线缆 x 1
- 5V/2A电源适配器  x 1
- 4G天线 x 1
- GPS天线 x 1



## 系统环境


### 连接设备
将开发电脑跟设备的调试/用户串口通过MicroUSB连接，连接后，将在PC设备管理器中出现两个串口，分别用于调试和用户串口输入输出。

<div align="left">
<img src=./../images/ec600s-uart.png width=90%/>
</div>

### 安装USB转串口驱动

- [Windows XP/Vista](https://www.silabs.com/documents/public/software/CP210x_VCP_Windows.zip)
- [Windows7/8/8.1](https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers.zip)
- [Windows10](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip)
- [MacOS](https://www.silabs.com/documents/public/software/Mac_OSX_VCP_Driver.zip)



安装完成之后，在设备管理器中可以看到以下虚拟串口：


**Windows**

<div align="left">
<img src=./../images/ec600s-win.png width=80%/>
</div>

> Enhanced COM Port 为日志串口
> Standard COM Port 为用户串口

**MacOS**

<div align="left">
<img src=./../images/ec600s-macos.png width=80%/>
</div>

> Note：
> 1. 不同的电脑生成的串口号和名称不一样，以实际生成的为准。
> 1. 生成的两个串口，一个是debug串口(查看运行日志，只读)，另一个是用户串口(JavaScript交互式解释器或其他用途)



### 开发工具
轻应用开发使用一套命令行工具来管理和推送应用脚本到设备。
下载命令行工具：
[amp-clitool.zip](https://hli.aliyuncs.com/o/config/firmware/amp-clitool-ec600s.zip)


将命令行工具解压到工作目录，如图所示对应文件
```bash
├── amp             # 命令行工具
├── app             # 示例代码(hello world)
│   ├── app.json
│   └── app.js
└── bindings.node   # 本地更新库
```
#### 
## 第一个轻应用示例


- 查看设备连接状态

在命令行工具下使用该命令查看设备连接状态
```bash
./amp.exe seriallist
COM194
COM195
```
> 如果返回值跟设备管理器中生成的串口号一致，说明设备连接和命令行工具运行正常



命令行工具默认示例代码运行hello world程序，此处使用LED Blink来作为第一个轻应用示例来介绍。
使用下面的`app.js` 和 `app.json`文件替换命令行工具示例代码目录下的同名文件，如下：
LED Blink `app.json`
```json
{
    "version": "1.0.0",
    "io": {
        "D13": {
            "type": "GPIO",
            "port": 33,
            "dir": "output",
            "pull": "pulldown"
        }
    },
    "debugLevel": "DEBUG"
}
```


LED Blink `app.js`：
```javascript
var gpio = require('gpio');

var led = gpio.open({
  id: 'D13'
});

var vol = 0;

setInterval(function() {
	vol = 1 - vol;

	led.writeValue(vol);
	console.log('gpio: led set value ' + vol);

	vol = led.readValue();
	console.log('gpio: led get value ' + vol);	
}, 1000);

```
### 
### 本地更新

- 打开命令行工具
- 推送脚本到设备
   - 执行推送命令
```bash
/* app是JavaScript应用脚本目录 */
/* COMXX是用户串口号，此处填写Standard COM Port对应的端口号 */
/* -b 115200 HaaS600串口波特率为115200 */

./amp.exe seriallist
COM194
COM195

./amp.exe serialput ./app COM194 -b 115200
```
> 一般情况下，应用下载串口是Silicon Labs Dual CP2105 USB to UART Bridge:Standard COM Port 

   - 复位开发板
   - 等待脚本传输完成，打开日志串口观察脚本运行情况



- 运行效果如图：

<div align="left">
<img src=./../images/ec600s-gif.gif width=80%/>
</div>

## 附录
### 硬件资源介绍

<div align="left">
<img src=./../images/ec600s-pins.png width=80%/>
</div>

### 硬件资源映射表
| **功能/接口** | **PIN/IO/资源映射** | **轻应用 port 配置** |
| --- | --- | --- |
| I2C0 | SDA(IO50) SCL(IO49) | 0 |
| ADC0 |  A0 | 0 |
| ADC1 | A1 | 1 |
| UART0 | 默认板级log打印 | 0 |
| UART1 | TXD(IO54)
RXD(IO53) | 1 |
| UART2 | TXD(IO52)  
RXD(IO51) | 2 |
| GPIO | IO51
IO52
IO17
IO16
IO59
IO121
IO54
IO53
IO124
IO122
IO34
IO36
IO35
IO33
IO57
IO0
IO1 | D0
D1
D2
D3
D4
D5
D6
D7
D8
D9
D10
D11
D12
D13
D14
D15
D16 |
| SPI0 | MISO(IO35)
MOSI(IO36)
SCK(IO33) | 0 |



### app.json 参考示例
```json
{
  "io": {
    "I2C0": {
      "type": "I2C",
      "port": 0,
      "addrWidth": 7,
      "freq": 100000,
      "mode": "master",
      "devAddr": 118
    },
    "ADC0": {
      "type": "ADC",
      "port": 0,
      "sampling": 12000000
    },
    "ADC1": {
      "type": "ADC",
      "port": 1,
      "sampling": 12000000
    },
    "UART1":{
      "type":"UART",
      "port":1,
      "dataWidth":8,
      "baudRate":115200,
      "stopBits":1,
      "flowControl":"disable",
      "parity":"none"
    },
    "UART2":{
      "type":"UART",
      "port":2,
      "dataWidth":8,
      "baudRate":115200,
      "stopBits":1,
      "flowControl":"disable",
      "parity":"none"
    },
    "SPI0":{
      "type":"SPI",
      "port":0,
      "mode":"master",
      "freq":3250000
    },
    "D0": {
      "type": "GPIO",
      "port": 51,
      "dir": "output",
      "pull": "pulldown"
    },
		"D1": {
      "type": "GPIO",
      "port": 52,
      "dir": "output",
      "pull": "pulldown"
    },
    "D2": {
      "type": "GPIO",
      "port": 17,
      "dir": "output",
      "pull": "pulldown"
    },
    "D3": {
      "type": "GPIO",
      "port": 16,
      "dir": "output",
      "pull": "pulldown"
    },
    "D4": {
      "type": "GPIO",
      "port": 59,
      "dir": "output",
      "pull": "pulldown"
    },
    "D5": {
      "type": "GPIO",
      "port": 121,
      "dir": "output",
      "pull": "pulldown"
    },
    "D6": {
      "type": "GPIO",
      "port": 54,
      "dir": "output",
      "pull": "pulldown"
    },
    "D7": {
      "type": "GPIO",
      "port": 53,
      "dir": "output",
      "pull": "pulldown"
    },
    "D8": {
      "type": "GPIO",
      "port": 124,
      "dir": "output",
      "pull": "pulldown"
    },
    "D9": {
      "type": "GPIO",
      "port": 122,
      "dir": "output",
      "pull": "pulldown"
    },
    "D10": {
      "type": "GPIO",
      "port": 34,
      "dir": "output",
      "pull": "pulldown"
    },
    "D11": {
      "type": "GPIO",
      "port": 36,
      "dir": "output",
      "pull": "pulldown"
    },
    "D12": {
      "type": "GPIO",
      "port": 35,
      "dir": "output",
      "pull": "pulldown"
    },
    "D13": {
      "type": "GPIO",
      "port": 33,
      "dir": "output",
      "pull": "pulldown"
    },
    "D14": {
      "type": "GPIO",
      "port": 57,
      "dir": "output",
      "pull": "pulldown"
    },
    "D15": {
      "type": "GPIO",
      "port": 0,
      "dir": "output",
      "pull": "pulldown"
    },
    "D16": {
      "type": "GPIO",
      "port": 1,
      "dir": "output",
      "pull": "pulldown"
    },
  },
  "debugLevel": "DEBUG"
}
```


### 更多示例代码
Git 仓库：[https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600Kit](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600Kit)
打包下载：[https://gitee.com/alios-things/amp-examples/repository/archive/master.zip](https://gitee.com/alios-things/amp-examples/repository/archive/master.zip)
