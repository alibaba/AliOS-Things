# HaaS100 快速开始
&emsp;&emsp;
HaaS100是高性价比的软硬件创新积木平台开发版，集成的是阿里云IoT团队自主研发的AliOS Things物联网操作系统。主控芯片HaaS1000是阿里云IoT定制的高性价比物联网芯片，软件支持JS/Python轻应用和C/C++开发。
本节则主要介绍如何烧录HaaS100 Python标准固件并在此基础上完成helloworld Python程序的运行。

## HaaS100 Python固件下载

&emsp;&emsp;
HaaS100和HaaS EDU K1共用Python版本固件，请通过下面固件列表链接下载开发板对应的固件压缩包并解压，解压完成后可以看到其目录结构如下：
```bash
├── HaaSPython-HaaSEDUK1-{xxx}.bin   # HaaS Python官方固件，{xxx}为版本号
└── littlefs.bin                     # 文件系统
```

&emsp;&emsp;
### HaaS100固件列表

* [HaaSPython-HaaS100-v1.0.2](https://hli.aliyuncs.com/p/config/HaaS_Python/HaaSPython-HaaSEDUK1-v1.0.2.zip)
  * 版本更新说明（2022-01-20）
    * aliyunIoT稳定性增强

* [HaaSPython-HaaS100-v1.0.1](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaSEDUK1-1.0.1.zip)
  * 版本更新说明（2022-01-13）
    * 对齐到HaaS Python 2.0 API标准
<br>

## PC环境准备
&emsp;&emsp;
将HaaS100用microUSB数据线和电脑相连。

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

### HaaS100串口名称确认
#### Windows系统

&emsp;&emsp;
读者请通过控制面板下的设备管理器，查询当前电脑下HaaS100插入后新增的端口。下图中显示HaaS EDUK1连接后新增的串口为“COM7”。
> 注意：每台PC的串口可能都不一样，如果有多个串口，可以断开PC和HaaS100之间的连线，然后将PC和HaaS100相连，找到新增的那个串口。

![HaaS100串口示意图](https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_EDU_K1_WINDOWS_COM.png)

&emsp;&emsp;
如果连接HaaS100之前和之后，没有新增串口，则需要安装HaaS100的串口驱动，[请点我下载](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip)。

<br>

#### MAC系统

&emsp;&emsp;
如果您的电脑是MAC系统，系统会自带HaaS100 UART驱动程序，无需单独安装。可以在命令行中通过如下命令查看HaaS100接到电脑之前和之后串口列表的差异确认HaaS100串口名称。

```
# 接入HaaS100之前
(base) ➜  ~ ls /dev/tty.usb*
zsh: no matches found: /dev/tty.usb*

# 接入HaaS100之后
(base) ➜  ~ ls /dev/tty.usb*
/dev/tty.usbserial-A908XGRO
```

&emsp;&emsp;
其中接入HaaS100之后新出现的"/dev/tty.usbserial-A908XGRO"即为HaaS100所对应的串口。
> 注意：每台PC的串口可能都不一样，上面只是笔者电脑上面的串口信息。

<br>

## 固件烧录过程

&emsp;&emsp;
烧录此固件需使用HaaS-Studio集成开发环境。

<br>
1. 点击“快速开始”按钮后选择“烧录工具”按钮。如下图所示。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_固件烧录.png width=75%/>
</div>
2. 选择好HaaS100对应的“串口名字” <br>
3. 选择固件所在路径（上面“固件下载”步骤中解压出来的名为HaaSPython-HaaS-EDU-K1-{xxx}.bin的文件） <br>
4. 选择文件系统所在路径（上面“固件下载”步骤中解压出来的名为lilttlefs.bin的文件） <br>
5. 点击“开始烧录”按钮，HaaS Studio便会将此固件烧录到开发板中，如下图所示。 <br>

> 下图中是笔者电脑中的串口好和固件名称，请读者按照根据串口和固件实际路径进行选择。

> 如果“串口名字”下拉框中没有正确的串口号，可以拔插HaaS100的USB口后，点击“刷新”按钮刷新串口列表。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_固件烧录_HaaS_EDU_K1_开始烧录.png width=75%/>
</div>

&emsp;&emsp;
烧录过程中命令行窗口会输出如下日志，烧录完成，终端日志中会提示"Burn xxxx success."。

```log
...
CCCC
<<< 0x43
Packet 0 >>>
<<< 0x6
<<< 0x43
Packet 2656 / 2656 >>>  <<< ACK
File: HaaSPython-HaaSEDUK1-1.0.1.bin
Size: 2718744Bytes
...
CCCC
<<< 0x43
Packet 0 >>>
<<< 0x6
<<< 0x43
Packet 4792 / 4792 >>>  <<< ACK
EOF
>>> EOT
<<< 0x15
>>> EOT
<<< 0x6
<<< 0x43
Packet End >>>
<<< 0x6
File: littlefs.bin
Size: 4907008Bytes
Swap AB partition
...
Burn "[('/Users/xxx/Downloads/HaaSPython-HaaSEDUK1/HaaSPython-HaaSEDUK1-1.0.1.bin', '0'), ('/Users/xxx/Downloads/HaaSPython-HaaSEDUK1/littlefs.bin', '0xB32000')]" success.
```

<br>

&emsp;&emsp;
如果在烧录的过程中如果命令行中会出现如下提示，请按下HaaS100上复位按钮重启设备。

```log
"Please reboot the board manually"
```

<br>

&emsp;&emsp;
经过上面的步骤HaaS Python固件就烧录到HaaS100开发板中去了。

## HaaS100 helloworld例程

### 创建helloworld工程
&emsp;&emsp;
请遵循如下的步骤完成helloworld Python工程的创建。

&emsp;&emsp;
如下图所示，点击HaaS Studio的"快速开始"按键会弹出HaaS Studio的欢迎页面，请选择“创建项目”，如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_创建项目向导.png width=60%/>
</div>

&emsp;&emsp;
根据创建工程向导，开发者输入/选择相关的信息即可。下面以在HaaS100上面创建hellworld示例程序为例演示工程进行，步骤如下:
> 注意事项： 文件夹不要有`中文，空格及其他异常`字符。

1. 输入项目名称
2. 选择工作区所在路径
3. 选择硬件类型
4. 选择编程语言
5. 选择解决方案模板
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python创建工程_项目名称_HaaS100.png width=60%/>
</div>

&emsp;&emsp;
然后点击“立即创建”按钮，在随后的步骤中确认输入的信息无误，点击“确认”，等待工程创建完成后，VS Code会自动打开新创建的工程。就可以在左侧的文件浏览页面中看到刚刚创建的helloworld工程。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python_helloworld_代码_HaaS100.png width=80%/>
</div>


### 推送脚本到设备

&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_部署运行.png width=5%/>），HaaS Studio工具上面会弹出如下的选择框，请按照如下的步骤逐步选择完成后，HaaS-Studio开始推出送固件。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python_HaaS_EDU_K1_推送脚本.png width=50%/>
</div>

&emsp;&emsp;
推送固件过程中会提示重启开发板，此时需要开发者按下HaaS100上复位按钮重启开发板。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python_HaaS_EDU_K1_推送脚脚本提示.png width=70%/>
</div>

&emsp;&emsp;
脚本开始推送脚本后，VS Code的命令行窗口会有如下提示：
```
amp shakehand begin...
b'Z'
b'Z'
b'Z'
b'Z'
start to send amp_boot cmd
start to send file cmd
amp shakehand success
<<< 0x43
Packet 0 >>>
<<< 0x6
<<< 0x43
Packet 1 >>>
...
```

&emsp;&emsp;
脚本推送完成后，VS Code的命令行窗口会有如下提示：
```
Ymodem transfer file finish
send cmd exit
```

> 如果烧录出现问题，请联系HaaS小二解决推送问题。

</details>

<br>
&emsp;&emsp;
推送此脚本到HaaS100之后，点击HaaS Studio的“串口”按钮后选择正确的串口并设定波特率为1500000之后，就可以看到设备周期性的打印如下日志。

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