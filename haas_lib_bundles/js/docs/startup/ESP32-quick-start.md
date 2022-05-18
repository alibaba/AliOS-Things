# ESP32 快速开始
&emsp;&emsp;
ESP32是开源世界中被开发者普遍使用的开发板，在ESP32中同样可以使用JavaScript语言基于HaaS开发框架进行轻应用开发。前提是ESP32设备中有烧录HaaS开发团队发布的ESP32 HaaS JavaScript标准固件。

&emsp;&emsp;
本文则主要介绍如何烧录ESP32 HaaS标准固件并在此基础上完成helloworld JavaScript程序的运行。

## ESP32 HaaS固件下载

&emsp;&emsp;
请通过下面固件列表链接下载开发板对应的固件压缩包并解压，解压完成后可以看到其目录结构如下：
```
├── HaaSJavaScript-ESP32-{xxx}.bin   # HaaS官方固件，{xxx}为版本号
```

&emsp;&emsp;
### HaaS JavaScript ESP32标准固件列表
  * [HaaSJavaScript-ESP32-v1.0.2](https://hli.aliyuncs.com/o/config/HaaSJS-ESP32-Firmware/HaaSJavaScript-ESP32-v1.0.2.zip)

  <summary>版本更新说明（2022-02-14）</summary>

  * ESP32支持HaaS JavaScript轻应用开发。
  * 一分钟上云体验：ADC采样实验，燃气检测系统，火焰检测系统，甲醛浓度检测系统，GNSS定位系统，花卉养殖，起夜灯，智慧路灯系统。
  * 基础模块JSAPI接口示例：GPIO, ADC, FS, I2C, UART, KV, BLE, Network, 阿里云物联网平台等。


&emsp;&emsp;
历史版本：
* 无

## PC环境准备
&emsp;&emsp;
将ESP32开发板用Micro-USB数据线和电脑USB口相连。

### HaaS Studio安装
&emsp;&emsp;
HaaS Studio目前是以插件的形式安装在VS Code（Visual Studio Code）工具中，所以安装HaaS Studio之前需要先安装VS Code。

#### 安装VS Code

&emsp;&emsp;
读者请到[微软官方网站](https://code.visualstudio.com/)上下载 VS Code 安装包并进行安装，VS Code安装包要求不低于版本 1.57。

&emsp;&emsp;
vscode有release版本(蓝色图标)和insider版本(绿色图标)，请安装蓝色图标的release版本。

&emsp;&emsp;
VS Code安装包下载网站： https://code.visualstudio.com/

> 推荐 Windows 系统版本为 win10， MacOS 版本不低于 10.15。
<br>

#### 安装haas-studio插件

> 安装完 VS Code之后，windows用户请注意使用管理员权限打开(vscode插件会安装相关工具到C盘，需要管理员权限)

> 请勿修改vscode插件加载位置，需要使用默认位置

&emsp;&emsp;
安装完 VS Code之后，请按照下图中数字的指示步骤完成haas-studio插件的安装。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_安装haas_studio_插件.png width=80%/>
</div>

&emsp;&emsp;
插件第一次安装完成后，会提示安装相关工具才能激活插件，请同意安装相关工具。

<div align="center">
<img src=../images/haas-studio-tool-install.png width=80%/>
</div>

&emsp;&emsp;
插件安装完成后，则 VSCode 左下角的状态栏会显示"快速开始"的图标，如下图所示。

<div align="center">
<img src=../images/haas-studio-startup-page.png width=80%/>
</div>

&emsp;&emsp;
正常情况下，左下角只会显示快速开始图标，打开和alios things无关的其他工程也一样，如果你已经打开或者新建了某个js工程，则会在VSCode底部的状态栏显示如下一排按钮，这些按钮的功能如下图所示：

<div align="center">
<img src=../images/haas-studio-js-toolbar.png width=40%/>
</div>

### ESP32串口名称确认
#### Windows系统

&emsp;&emsp;
如果您的电脑是Windows系统，请通过控制面板下的设备管理器，查询当前电脑下ESP32插入后新增的端口。下图中显示ESP32连接后新增的串口为“COM7”。
> 注意：每台PC的串口可能都不一样，如果有多个串口，可以断开PC和ESP32之间的连线，然后将PC和ESP32相连，找到新增的那个串口。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_EDU_K1_WINDOWS_COM.png width=70%/>
</div>

&emsp;&emsp;
如果电脑在连接ESP32之前和之后，没有新增串口，则需要安装ESP32的串口驱动。ESP32串口芯片有两种，请根据自己的ESP32型号选择合适的驱动（如果您不知道自己的ESP32型号，两个驱动都安装上即可）：
* [CH340串口驱动下载页](http://www.wch.cn/downloads/CH341SER_ZIP.html)
* [CP2102驱动下载](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip)
<br>

#### MAC系统

&emsp;&emsp;
如果您的电脑是MAC系统，系统会自带ESP32 UART驱动程序，无需单独安装。可以在命令行中通过如下命令查看ESP32接到电脑之前和之后串口列表的差异确认ESP32串口名称。

```
# 接入ESP32之前
(base) ➜  ~ ls /dev/tty.usb*
zsh: no matches found: /dev/tty.usb*

# 接入ESP32之后
(base) ➜  ~ ls /dev/tty.usb*
/dev/tty.usbserial-0001
```

&emsp;&emsp;
其中接入ESP32之后新出现的"/dev/tty.usbserial-0001"即为ESP32所对应的串口。
> 注意：每台PC的串口可能都不一样，上面只是笔者电脑上面的串口信息。
<br>

### 固件烧录过程
&emsp;&emsp;
烧录此固件需使用HaaS-Studio集成开发环境。

1. 点击“快速开始”按钮后选择“烧录工具”按钮。如下图所示。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_固件烧录.png width=75%/>
</div>
2. 选择好ESP32对应的“串口名字”和固件所在路径（上面“ESP32 HaaS固件下载”步骤中解压出来的名为HaaSJavaScript-esp32-{board}-{xxx}.bin的文件）之后点击“开始烧录”按钮，HaaS Studio便会将此固件烧录到开发板中，如下图所示。

> 下图中是笔者电脑中的串口好和固件名称，请读者按照根据串口和固件实际路径进行选择。

> 如果“串口名字”下拉框中没有正确的串口号，可以拔插ESP32的USB口后，点击“刷新”按钮刷新串口列表。

<div align="center">
<img src=../images/haas-studio-firmware-burn.png width=85%/>
</div>

&emsp;&emsp;
烧录过程中命令行窗口会输出如下日志，烧录完成，中断日志中会提示"Hash of data verified."。

```
Serial port /dev/cu.usbserial-0001
Connecting.......
Detecting chip type... Unsupported detection protocol, switching and trying again...
Connecting....
Detecting chip type... ESP32
Chip is ESP32-D0WD (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 8c:ce:4e:9a:67:ec
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 460800
Changed.
Erasing flash (this may take a while)...
Chip erase completed successfully in 13.0s
Hard resetting via RTS pin...

...
Changing baud rate to 460800
Changed.
Configuring flash size...
Flash will be erased from 0x00001000 to 0x001e3fff...
Compressed 1977072 bytes to 1172201...
Wrote 1977072 bytes (1172201 compressed) at 0x00001000 in 31.0 seconds (effective 511.0 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

&emsp;&emsp;
经过上面的步骤HaaS JavaScript ESP32固件就烧录到ESP32开发板中去了。

## ESP32 helloworld例程

### 创建helloworld工程
&emsp;&emsp;
这里通过一个简单的每秒打印一条日志的helloworld用例来介绍。首先下载《[ESP32 JavaScript Helloworld示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/examples/helloworld)》，示例代码目录如下所示：
```bash
app
├── app.json      # 应用与板级配置
└── app.js        # 用户代码
```

### 推送脚本到设备
- [【MacOS】amp工具下载链接](https://hli.aliyuncs.com/o/config/esp32-amp-macos/amp-macos.zip)
- [【Windows】amp工具下载链接](https://hli.aliyuncs.com/o/config/esp32-amp-win/amp-win.zip)
<br>

&emsp;&emsp;
首先，下载HaaS-JS轻应用脚本推送工具amp到本地电脑，解压之后在./amp-clitool/dist/下分别有macOS的工具版本amp-macos，以及windows下的工具版本amp-win，目前暂无Linux版本的工具支持。

&emsp;&emsp;
 注意: ESP32在Windows环境下执行amp-win工具下载JS脚本之前，需要先把ESP32的GPIO_0拉高（和3.3v脚短接），这样才能正常推送。Mac环境下则无此问题，不需该步骤。

&emsp;&emsp;
ESP32板子GPIO_0 硬件连线如下表所示：
|板子|板级pin脚|说明|
|-----|----|----|
| NODEMCU-32 | P0 | GPIO_0对应管脚 |
| M5StackCore2 | G0 | GPIO_0对应管脚 |

&emsp;&emsp;
然后，执行以下amp命令。

> ./amp-clitool/dist/amp-macos/amp serialput ./helloworld /dev/cu.wchusbserial537A003351 -m quickjs -e 0 -b 115200

&emsp;&emsp;
其中：<br>
&emsp;&emsp;
（1）./amp-clitool/dist/amp-macos/amp：下载的JS轻应用命令行工具路径<br>
&emsp;&emsp;
（2） ./helloworld：是helloworld示例代码路径<br>
&emsp;&emsp;
（3） /dev/cu.wchusbserial537A003351：是日志串口号，此处填写自己的串口号<br>
&emsp;&emsp;
（4） mode：js引擎类型，可选[quickjs | duktape]。HaaS100默认应选quickjs<br>
&emsp;&emsp;
（5） encode: 编译，可选项[0 | 1]，默认选择0不编译<br>
&emsp;&emsp;
（6） -b 115200 ESP32硬件的串口波特率为1.5M<br>

注意：执行推送命令的时候需要确保COMXX串口没有被其他工具占用，否则会导致本地更新失败。

&emsp;&emsp;
最后，根据amp工具的提示，按下ESP32硬件上的复位按键重启设备，工具会自动将app例程推送到硬件中执行。
> Wait shakehand with board ...<br>
> Please reboot board now<br>

&emsp;&emsp;
等待脚本传输完成，打开日志串口观察脚本运行情况，传输完成之后，打印
> put complete!
<br>

&emsp;&emsp;
打开串口工具，在/dev/cu.wchusbserial537A003351端口上，波特率115200，可以看到设备周期性的打印如下日志。

```
...
helloworld
helloworld
helloworld
...
```

### 例程JavaScript脚本说明

&emsp;&emsp;
helloworld工程中的app.js脚本内容如下，各行代码的功能请参考下面代码的注释。

```javascript
setInterval(function() {
    console.log('helloworld');
}, 1000);
```

&emsp;&emsp;
helloworld例程运行起来就说明HaaS JavaScript开发环境安装好了。接下来是对公测案例的说明。

&emsp;&emsp;
快速入门完成之后，建议您进入我们的[趣味案例专区](https://haas.iot.aliyun.com/solution)，快速体验更多有意思的案例。

&emsp;&emsp;
如果您想了解如何从浅到深完成一个完整的物联网应用的开发，建议您进入我们的[学习中心](https://haas.iot.aliyun.com/learning)进行学习。

&emsp;&emsp;
如果您想了解HaaS开发框架目前有哪些外设驱动可用，建议您进入我们的[硬件积木](https://haas.iot.aliyun.com/solution/hardware)查看目前支持的硬件积列表。

&emsp;&emsp;
如果您想看HaaS JavaScript都提供哪些库和API，请点击左侧导航栏查看。
<br>