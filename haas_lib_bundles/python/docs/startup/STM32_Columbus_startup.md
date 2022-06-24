# STM32系列开发板快速开始
&emsp;&emsp;
STM32开发板是开源世界中被开发者普遍使用的开发板，在STM32开发板设备上同样可以使用Python语言基于HaaS开发框架进行轻应用开发。前提是STM32开发板中有烧录HaaS开发团队发布的 HaaS Python标准固件。

&emsp;&emsp;
本文以01Studio的Columbus开发板为例，介绍如何烧录STM32 HaaS Python标准固件并在此基础上完成helloworld Python程序的运行。

## 一、HaaS Python固件下载
&emsp;&emsp;
请从[HaaS Python版本历史](https://haas.iot.aliyun.com/haasapi/index.html?#/Python/docs/zh-CN/startup/startup)下载Columbus对应的固件压缩包并解压，解压完成后可以看到其目录结构如下：
```
├── HaaSPython-Columbus-{xxx}.bin   # HaaS官方固件，{xxx}为版本号
```


## 二、安装HaaS-Studio开发工具

&emsp;&emsp;
HaaS Studio目前是以插件的形式安装在VS Code（Visual Studio Code）工具中，所以安装HaaS Studio之前需要先安装VS Code。

### 2.1 安装VS Code

&emsp;&emsp;
读者请到[微软官方网站](https://code.visualstudio.com/)上下载 VS Code 安装包并进行安装，VS Code安装包要求不低于版本 1.57。

&emsp;&emsp;
vscode有release版本(蓝色图标)和insider版本(绿色图标)，请安装蓝色图标的release版本。

&emsp;&emsp;
VS Code安装包下载网站： https://code.visualstudio.com/

> 推荐 Windows 系统版本为 win10， MacOS 版本不低于 10.15。
<br>

### 2.2 安装haas-studio插件

> 安装完 VS Code之后，windows用户请注意使用管理员权限打开(vscode插件会安装相关工具到C盘，需要管理员权限)

> 请勿修改vscode插件加载位置，需要使用默认位置

&emsp;&emsp;
安装完 VS Code之后，请按照下图中数字的指示步骤完成haas-studio插件的安装。

<div align="center">
<img src=../images/1_安装haas_studio_插件.png width=80%/>
</div>

&emsp;&emsp;
插件第一次安装完成后，会提示安装相关工具才能激活插件，请同意安装相关工具。第一次新建或者打开python轻应用工程，也会安装轻应用开发相关工具，同样需要同意安装。

<div align="center">
<img src=../images/haas-studio-tool-install.png width=80%/>
</div>

&emsp;&emsp;
插件安装完成后，则 VSCode 左下角的状态栏会显示"快速开始"的图标，如下图所示。

<div align="center">
<img src=../images/haas-studio-startup-page.png width=80%/>
</div>

&emsp;&emsp;
一般情况下，左下角只会显示快速开始图标，如果打开或者新建了某个Python工程，则会在VSCode底部的状态栏展开如下一排按钮，这些按钮的功能如下图所示：

<div align="center">
<img src=../images/1_HaaS_Studio_Python工程按钮.png width=40%/>
</div>

&emsp;&emsp;
为了方便开发，还可以打开高级串口模式，在当前的工程目录下，存在.vscode这样一个文件夹，找到里面的settings.json文件，将"haasStudio.pythonAdvanced"选项设置成enable即可（如果没有该选项，则可以手动添加并按照下图所示格式设置即可）。打开方式如下：
* 注意高级模式某些平台可能不支持，比如低版本的linux，M1系列MACOS等，如果平台不支持，会自动设置成 disable。

<div align="center">
<img src=../images/haas-studio-python-advance.png width=80%/>
</div>

&emsp;&emsp;
python高级模式打开之后，这些按钮的功能变成如下图所示：

<div align="center">
<img src=../images/haas-studio-python-advance-enable.png width=80%/>
</div>

## 三、固件烧录

&emsp;&emsp;
进行固件烧录前需要先确认开发板连接到电脑后对应的串口名称。

### 3.1 串口名称确认
#### 3.1.1 Windows系统

&emsp;&emsp;
如果您的电脑是Windows系统，请通过控制面板下的设备管理器，查询当前电脑下Columbus串口插入后新增的端口。下图中显示Columbus连接后新增的串口为“COM7”。
> 注意：每台PC的串口可能都不一样，如果有多个串口，可以断开PC和Columbus之间的连线，然后将PC和Columbus相连，找到新增的那个串口。

<div align="center">
<img src=../images/1_HaaS_EDU_K1_WINDOWS_COM.png width=100%/>
</div>

<br>

#### 3.1.2 MAC系统

&emsp;&emsp;
如果您的电脑是MAC系统，系统会自带Columbus UART驱动程序，无需单独安装。可以在命令行中通过如下命令查看Columbus接到电脑之前和之后串口列表的差异确认Columbus串口名称。

```
# 接入Columbus之前
(base) ➜  ~ ls /dev/tty.usb*
zsh: no matches found: /dev/tty.usb*

# 接入Columbus之后
(base) ➜  ~ ls /dev/tty.usb*
/dev/tty.usbserial-0001
```

&emsp;&emsp;
其中接入Columbus之后新出现的"/dev/tty.usbserial-0001"即为Columbus所对应的串口。
> 注意：每台PC的串口可能都不一样，上面只是笔者电脑上面的串口信息。
<br>

### 3.2 使用HaaS Studio进行固件烧录
&emsp;&emsp;
**STM32的固件烧录需要首先保证设备进入 DFU模式，用电线将开发板上的 3V3 引脚连接到 BOOT（或者BOOT0，或者pyboard开发板上的P1/DFU ）引脚，然后复位设备。**

&emsp;&emsp;
Windows电脑在进入DFU模式后会显示缺少 STM32 BOOTLOADER 驱动，需要安装STM-Bootloader驱动。

* [STM-Bootloader驱动下载](https://hli.aliyuncs.com/o/config/STM32_Tool/STM-Bootloader-Driver.zip)

&emsp;&emsp;

1. 烧录此固件需使用HaaS-Studio集成开发环境。点击“快速开始”按钮后选择“烧录工具”按钮。如下图所示：
<div align="center">
<img src=../images/1_HaaS_Studio_固件烧录.png width=75%/>
</div>
<br>

2. 选择好Columbus对应的“串口名字”和固件所在路径（上面“Columbus HaaS固件下载”步骤中解压出来的名为HaaSPython-Columbus-{xxx}.bin的文件）之后点击“开始烧录”按钮，HaaS Studio便会将此固件烧录到开发板中，如下图所示:

> 下图中是笔者电脑中的串口好和固件名称，请读者按照根据串口和固件实际路径进行选择。

> 如果“串口名字”下拉框中没有正确的串口号，可以拔插Columbus的USB口后，点击“刷新”按钮刷新串口列表。

<div align="center">
<img src=../images/haas-studio-firmware-burn-stm32.png  width=85%/>
</div>

&emsp;&emsp;
烧录过程中命令行窗口会输出如下日志，烧录完成，终端日志中会提示"Finished"。

```
File: build-COLUMBUS/firmware.dfu
    b'DfuSe' v1, image size: 494077, targets: 1
    b'Target' 0, alt setting: 0, name: "ST...", size: 493792, elements: 2
      0, address: 0x08000000, size: 14656
      1, address: 0x08020000, size: 479120
    usb: 0483:df11, device: 0x0000, dfu: 0x011a, b'UFD', 16, 0x51ee01e0
Writing memory...
0x08000000   14656 [=========================] 100% 
0x08020000  479120 [=========================] 100% 
Exiting DFU...
Finished
```

&emsp;&emsp;
经过上面的步骤HaaS Python Columbus固件就烧录到Columbus开发板中去了。

&emsp;&emsp;
**用电线将开发板上的 GND 引脚连接到 BOOT（或者BOOT0，或者pyboard开发板上的P1/DFU ）引脚，然后复位设备**

### 3.3 固件版本确认
&emsp;&emsp;
固件烧录完成后，如何确认固件真的有更新到硬件中呢？可以通过如下的方法确认：

&emsp;&emsp;
通过串口工具打开Columbus开发板串口（注意波特率选择115200），此时在串口工具中敲击回车会出现“>>>”符号，">>>"代表已经进入到Python的REPL模式中。在REPL模式中输入“import uos; uos.version_info()”指令回车执行，HaaS Python则会将版本号信息输出到串口中。如下图所示，其版本信息遵循“HaaSPython-Columbus-\<version>-\<buildtime>”的格式，其中：
* \<version\>：代表HaaS Python版本号。
* \<buildtime\>：代表固件编译时间。
> MACOS建议使用picocom串口工具；Windows系统推荐使用Putty串口工具。

```
>>> import system
>>> system.version_info()
'HaaSPython-STM32F407ZGT6-v2.3.0-2022-06-13'
```

> 打开串口工具后，敲回车后如果未出现">>>"符号，则一般是因为您的开发板正在运行Python脚本。此时，可以同时按下Ctrl+C两个按键，尝试打断当前的python脚本。如果按很多次Ctrl+C之后仍然没有出现">>>"，则大概率是因为开发板运行的程序死机，可以尝试按住“Ctrl+C”再对开发板进行硬件复位。

## 四、运行helloworld例程

### 4.1 创建helloworld工程
&emsp;&emsp;
请遵循如下的步骤完成helloworld Python工程的创建。

&emsp;&emsp;
如下图所示，点击HaaS Studio的"快速开始"按键会弹出HaaS Studio的欢迎页面，请选择“创建项目”，如下图所示：

<div align="center">
<img src=../images/1_HaaS_Studio_创建项目向导.png width=80%/>
</div>


&emsp;&emsp;
根据创建工程向导，开发者输入/选择相关的信息即可。下面以在Columbus上面创建hellworld示例程序为例演示工程进行，步骤如下:
> 注意事项： 文件夹不要有中文，空格及其他异常字符。

1.  选中"ESP32-C3-32S-Kit"开发板，右侧会更新相应的案例列表
2. 选择helloworld案例，点击创建。

<div align="center">
<img src=../images/haas-studio-创建工程-选择helloworld.png width=80%/>
</div>

&emsp;&emsp;
填入项目名称，以及项目工作路径后点击确定。
&emsp;&emsp;
<div align="center">
<img src=../images/haas-studio-创建工程向导.png width=40%/>
</div>

&emsp;&emsp;
在随后的步骤中确认输入的信息无误，点击“确认”，等待工程创建完成后，VS Code会自动打开新创建的工程。就可以在左侧的文件浏览页面中看到刚刚创建的helloworld工程。

<div align="center">
<img src=../images/1_HaaS_Studio_Python_helloworld_代码.png width=80%/>
</div>

### 4.2 推送脚本到设备

&emsp;&emsp;
&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=../images/1_HaaS_Studio_部署运行.png width=10%/>），HaaS Studio工具上面会弹出如下的选择框，请按照如下的步骤逐步选择完成后，HaaS-Studio开始推出送固件。
<div align="center">
<img src=../images/1_HaaS_Studio_Python_本地推送脚本.png width=60%/>
</div>

&emsp;&emsp;
脚本推送完成后，VS Code的命令行窗口会有如下提示：
```
upload success
```
&emsp;&emsp;
如果选择了串口仍然推送失败，请联系HaaS小二解决推送问题。

<br>
&emsp;&emsp;
推送此脚本到Columbus之后，HaaS-Studio同时会自动打开串口工具，并自动执行main.py脚本，此时可以在看到设备周期性的打印如下日志。

```
...
helloworld
helloworld
helloworld
...
```

### 4.3 例程Python脚本说明

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
helloworld例程运行起来就说明HaaS Python开发环境安装好了。

&emsp;&emsp;
快速入门完成之后，建议您进入我们的[创意案例专区](https://haas.iot.aliyun.com/solution)，快速体验更多有意思的案例。

&emsp;&emsp;
如果您想了解如何从浅到深完成一个完整的物联网应用的开发，建议您进入我们的[学习中心](https://haas.iot.aliyun.com/learning)进行学习。

&emsp;&emsp;
如果您想了解HaaS开发框架目前有哪些外设驱动可用，建议您进入我们的[硬件积木](https://haas.iot.aliyun.com/solution/hardware)查看目前支持的硬件积列表。

&emsp;&emsp;
如果您想看HaaS Python都提供哪些库和API，请点击左侧导航栏查看。

## 五、STM32系列开发板列表
&emsp;&emsp;
HaaS Python固件在如下STM32系列的开发板上都经过了功能验证，开发者可以根据自己的喜好选择合适的开发板。

### 5.1 01Studio Columbus
&emsp;&emsp;
HaaS Python固件刷入01Studio Columbus 开发版之后，开发板端口详细定义及说明请参考下图：

<div align="center">
<img src=../images/ext_01Studio_Columbus_扩展接口.png width=100%/>
</div>

<br>
