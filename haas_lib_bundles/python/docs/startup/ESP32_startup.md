# ESP32 快速开始
&emsp;&emsp;
ESP32是开源世界中被开发者普遍使用的开发板，在ESP32设备上同样可以使用Python语言基于HaaS开发框架进行轻应用开发。前提是ESP32设备中有烧录HaaS开发团队发布的ESP32 HaaS Python标准固件。

&emsp;&emsp;
本文则主要介绍如何烧录ESP32 HaaS标准固件并在此基础上完成helloworld Python程序的运行。

## ESP32 HaaS固件下载

&emsp;&emsp;
请通过下面固件列表链接下载开发板对应的固件压缩包并解压，解压完成后可以看到其目录结构如下：
```
├── HaaSPython-ESP32-{xxx}.bin   # HaaS官方固件，{xxx}为版本号
```
&emsp;&emsp;
### HaaS Python ESP32标准固件列表
* [HaaSPython-ESP32-v1.0.1](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v1.0.1.zip)
  <details>
  <summary>版本更新说明（2021-01-13）</summary>

  * 动态生成QSTR功能
  * UART增加on/any函数
  * aliyunIoT修复内存泄漏及postProp返回值问题
  * 修复部分BUG

* [HaaSPython-ESP32-v1.0.0](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v1.0.0.zip)

  <details>
  <summary>版本更新说明（2021-12-30）</summary>

  * 新增ESP32开发板NodeMCU-32支持
  * 新增HaaS小程序，方便快速体验数据上云功能
  * 升级HaaS Studio - 精简IDE开发流程，支持固件一键烧写
  * 兼容MicroPython v1.17
  * 支持快速连接阿里云物联网云平台（aliyunIoT），支持设备模式和网关模式
  * 扩展Driver、KV、http、BLE配网、OTA和Modbus等功能
  * 优化ESP32 IDF内存分配机制
  </details>

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
VS Code安装包下载网站： https://code.visualstudio.com/

> 推荐 Windows 系统版本为 win10， MacOS 版本不低于 10.15。
<br>

#### 安装haas-studio插件

&emsp;&emsp;
安装完 VS Code之后，请按照下图中数字的指示步骤完成haas-studio插件的安装。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_安装haas_studio_插件.png width=80%/>
</div>

&emsp;&emsp;
插件安装完成后，则 VS Code 左下角的状态栏会显示"快速开始"的图标，如下图所示。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_新建工程按钮.png width=80%/>
</div>

&emsp;&emsp;
如果你已经打开了某个Python工程，则会在VS Code底部的状态栏显示如下一排按钮，这些按钮的功能如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python工程按钮.png width=40%/>
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
/dev/tty.usbserial-1410
```

&emsp;&emsp;
其中接入ESP32之后新出现的"/dev/tty.usbserial-1410"即为ESP32所对应的串口。
> 注意：每台PC的串口可能都不一样，上面只是笔者电脑上面的串口信息。
<br>

### 固件烧录过程
&emsp;&emsp;
烧录此固件需使用HaaS-Studio集成开发环境。

1. 点击“快速开始”按钮后选择“烧录工具”按钮。如下图所示。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_固件烧录.png width=75%/>
</div>
2. 选择好ESP32对应的“串口名字”和固件所在路径（上面“ESP32 HaaS固件下载”步骤中解压出来的名为HaaSPython-ESP32-{xxx}.bin的文件）之后点击“开始烧录”按钮，HaaS Studio便会将此固件烧录到开发板中，如下图所示。

> 下图中是笔者电脑中的串口好和固件名称，请读者按照根据串口和固件实际路径进行选择。

> 如果“串口名字”下拉框中没有正确的串口号，可以拔插ESP32的USB口后，点击“刷新”按钮刷新串口列表。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_固件烧录_开始烧录.png width=85%/>
</div>

&emsp;&emsp;
烧录过程中命令行窗口会输出如下日志，烧录完成，终端日志中会提示"Hash of data verified."。

```
Serial port /dev/tty.usbserial-1410
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
经过上面的步骤HaaS Python ESP32固件就烧录到ESP32开发板中去了。

### 固件版本确认
&emsp;&emsp;
固件烧录完成后，如何确认固件真的有更新到硬件中呢？可以通过如下的方法确认：

&emsp;&emsp;
通过串口工具打开ESP32开发板（注意波特率选择115200），此时敲击回车会出现“>>>”符号，">>>"代表已经进入到Python的REPL模式中。在REPL模式中输入“import uos; uos.version_info()”指令回车执行，HaaS Python则会将版本号信息输出到串口中。如下图所示，其版本信息遵循“HaaSPython-ESP32-\<version>-\<buildtime>”的格式，其中：
* \<version\>：代表HaaS Python版本号。
* \<buildtime\>：代表固件编译时间。
> MACOS建议使用picocom串口工具；Windows系统推荐使用Putty串口工具。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaSPython_版本号确认.png width=80%/>
</div>

## ESP32 helloworld例程

### 创建helloworld工程
&emsp;&emsp;
请遵循如下的步骤完成helloworld Python工程的创建。

&emsp;&emsp;
如下图所示，点击HaaS Studio的"快速开始"按键会弹出HaaS Studio的欢迎页面，请选择“创建项目”，如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_创建项目向导.png width=60%/>
</div>

&emsp;&emsp;
根据创建工程向导，开发者输入/选择相关的信息即可。下面以在ESP32上面创建hellworld示例程序为例演示工程进行，步骤如下:
> 注意事项： 文件夹不要有中文，空格及其他异常字符。

1. 输入项目名称
2. 选择工作区所在路径
3. 选择硬件类型
4. 选择编程语言
5. 选择解决方案模板
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python创建工程_项目名称.png width=40%/>
</div>

&emsp;&emsp;
然后点击“立即创建”按钮，在随后的步骤中确认输入的信息无误，点击“确认”，等待工程创建完成后，VS Code会自动打开新创建的工程。就可以在左侧的文件浏览页面中看到刚刚创建的helloworld工程。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python_helloworld_代码.png width=80%/>
</div>


### 推送脚本到设备

&emsp;&emsp;
&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_部署运行.png width=5%/>），HaaS-Studio会将脚本推送到开发板上。

&emsp;&emsp;
脚本推送完成后，VS Code的命令行窗口会有如下提示：
```
upload success
```

&emsp;&emsp;
如果`推送不成功`请点击下面"推送失败的解决方案"按钮查看解决方法。
<details>
<summary>推送失败的解决方案</summary>
&emsp;&emsp;
一般情况下，推送失败是因为电脑上外接了多个USB转串口的设备导致的。此时，VS Code的命令行中会列出系统的串口列表，需要您在命令行中敲入ESP32串口名称（前面“ESP32串口名称确认”步骤中有说明）对应的序号之后敲回车。如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_选择串口序号.png width=150%/>
</div>

&emsp;&emsp;
如果选择了串口仍然推送失败，请联系HaaS小二解决推送问题。

</details>

<br>
&emsp;&emsp;
推送此脚本到ESP32之后，HaaS-Studio同时会自动打开串口工具，并自动执行main.py脚本，此时可以在看到设备周期性的打印如下日志。

```
...
helloworld
helloworld
helloworld
...
```

### 例程Python脚本说明

&emsp;&emsp;
helloworld工程中的main.py脚本内容如下，各行代码的功能请参考下面代码的注释。

```python
#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import utime   # 延时函数在utime库中

if __name__ == '__main__':
    while True:             # 无限循环
        print("helloworld")  # 打印"helloworld"字串到串口中
        utime.sleep(1)      # 打印完之后休眠1秒
```

&emsp;&emsp;
helloworld例程运行起来就说明HaaS Python开发环境安装好了。接下来是对公测案例的说明。

&emsp;&emsp;
快速入门完成之后，建议您进入我们的[趣味案例专区](https://haas.iot.aliyun.com/solution)，快速体验更多有意思的案例。

&emsp;&emsp;
如果您想了解如何从浅到深完成一个完整的物联网应用的开发，建议您进入我们的[学习中心](https://haas.iot.aliyun.com/learning)进行学习。

&emsp;&emsp;
如果您想了解HaaS开发框架目前有哪些外设驱动可用，建议您进入我们的[硬件积木](https://haas.iot.aliyun.com/solution/hardware)查看目前支持的硬件积列表。

&emsp;&emsp;
如果您想看HaaS Python都提供哪些库和API，请点击左侧导航栏查看。
<br>