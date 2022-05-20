<div align="left">
<img src=./../images/haas510.png width=60%/>
</div>

HaaS510-24B


HaaS510 开板式DTU场景，是针对用户已开发好的设备快速增加4G连云能力的4G CAT1数传模块，通过将模组与用户设备集成到一个外壳内，以保持设备的一体性的同时，降低重新开发PCB的时间消耗，降低模组开发的难度。
HaaS510产品通过JS脚本编程进行二次开发，将模组的本地串口通信及通过4G连接云端平台的能力开放给用户，同时为用户提供编写本地业务逻辑的能力，使得用户可以针对不同的应用场景，在设备侧完成数据清洗甚至一些简单的控制功能，给用户更大的创新自有度。


HaaS510 套件包含以下物料：


- 主板 x 1
- FPC天线 x 1
## 包装形式
MPQ：1pcs，采用静电袋单板独立包装出货。
整箱数量：100pcs，包装箱尺寸：350x190x230 mm
## 规格
数量：2路，1路用户串口，1路debug串口
电平：TTL
数量：1路
输入电压：DC5 2A
数量：1路
电平：1.8V
​

## 电气特性
| 参数 | 值 |
| --- | --- |
| 输入电压 | 4.5V - 28V |
| 输入电流 | 2A |
| 正常工作温度 | -35℃- +75℃ |
| 存储温度 | -40℃- +90℃ |

ESD性能

| 接口 | 接触放电 | 空气放电 | 单位 |
| --- | --- | --- | --- |
| 电源端口 | ±1.5 | ±4 | KV |
| 天线接口 | ±10 | ±12 | KV |
| 串口 | ±0.5 | ±1 | KV |

## 尺寸

<div align="left">
<img src=./../images/haas510-outline.png width=80%/>
</div>

# 使用说明
## 接口说明

<div align="left">
<img src=./../images/haas510-pins.png width=80%/>
</div>

1. 用户串口：- 为用户提供的与本地设备进行串口通信的接口。- TTL电平。- 在精简连接设计时，可以从该口提供系统电源。
2. 调试串口：- 使用轻应用开发时的调试信息输出串口。

- TTL电平。

1. 电源输入：- 电源输入接口。
1. 扩展口：- 上电时，通过将Download key和GND短接，进入下载模式。- 正常运行时，Download key应悬空
1. APP下载口：- 在使用轻应用开发时，用户APP的本地下载接口。
1. 天线IPX接口：- 外接天线馈线IPX连接口，必须！

*** 注：为了用户使用方便，HAAS510的电源输入，和用户串口的Vin以及调试串口的Vin是直接接通的，用户使用时，需要避免多个电源同时输入。**
### 
### 开发工具
轻应用开发使用一套命令行工具来管理和推送应用脚本到设备。
下载命令行工具：
[amp-clitool.zip](https://hli.aliyuncs.com/o/config/firmware/amp-clitool-haas510.zip)


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



此处使用uart收发数据来作为第一个轻应用示例来介绍。
使用下面的`app.js` 和 `app.json`文件替换命令行工具示例代码目录下的同名文件，如下：
uart `app.json`
```json
{
    "version": "1.0.0",
    "io": {
        "serial": {
          "type": "UART",
          "port": 2,
          "dataWidth": 8,
          "baudRate": 115200,
          "stopBits": 1,
          "flowControl": "disable",
          "parity": "none"
        }
    },
    "debugLevel": "DEBUG"
}
```


uart `app.js`：
```javascript
var uart = require('uart');

var msgbuf = [0x01, 0x25, 0x55];
var com_485 = uart.open({
  id: 'serial'
});

com_485.write(msgbuf);

com_485.on('data', function(data) {
	console.log('com_485 receive data is ' + data);
});
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


### 硬件资源映射表
| **功能/接口** | **PIN/IO/资源映射** | **轻应用 port 配置** | **开发板丝印** |
| --- | --- | --- | --- |
| UART0 | 默认板级log打印 | 0 | DEBUG_UART |
| UART1 | TXD(IO54)
RXD(IO53) | 1 | GNSS_UART |
| UART2 | TXD(IO52)  
RXD(IO51) | 2 | MAIN_UART |

### app.json 参考示例
```json
{
  "io": {
    "UART1":{
      "type":"UART",
      "port":1,
      "dataWidth":8,
      "baudRate":9600,
      "stopBits":1,
      "flowControl":"ctsrts",
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
    }
  },
  "debugLevel": "DEBUG"
}
```


### 更多示例代码
Git 仓库：[https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600Kit](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600Kit)
打包下载：[https://gitee.com/alios-things/amp-examples/repository/archive/master.zip](https://gitee.com/alios-things/amp-examples/repository/archive/master.zip)
