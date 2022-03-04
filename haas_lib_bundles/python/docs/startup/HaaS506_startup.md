# HaaS506 快速开始
&emsp;&emsp;
HaaS 506开发板是一款经过阿里云HaaS团队认证的高性能、多连接的物联网开发板。本文将从硬件介绍、固件更新、开发环境搭建以及案例验证等多方面为开发者提供详细的快速上手教程。
请开发者们认证查阅，如有疑问或需技术支持，可用钉钉扫描开发板背面的二维码加入我们的技术支持群，您的提问都将会得到完善的答复。 

## 一、硬件介绍
&emsp;&emsp;
HaaS 506是一款集成了M601 4G-CAT1模块的开发板，板载三组双色LED灯和一个自定义功能按键方便用户调试开发，并且提供丰富的接口用于外设扩展。选择了MicroPython作为运行引擎，它不仅继承了python优美简洁的语法特点，也提供了便捷的嵌入式硬件操作库，同时提供了阿里云物联网平台的连接控制的能力，极大的降低了嵌入式开发的门槛。
下图是HaaS506的简单介绍：
<div align="center">
<img src=./../images/1_1HaaS506系统框图.png width=80%/>
</div>
<div align="center">
<img src=./../images/1_2HaaS506接口定义图.png width=90%/>
</div>

想了解HaaS 506更多信息，请参考[ HaaS506硬件规格 ](https://blog.csdn.net/HaaSTech/article/details/119379587)
&emsp;&emsp;

## 二、固件更新
### 固件下载烧录
&emsp;&emsp;
HaaS 506开发板在出厂时已经烧录了最新的固件，但由于固件一直在不断更新，开发者可根据自身需求决定是否需要更新。
下面将为大家演示如何烧录最新的固件：
第一步：下载最新版本固件：
&emsp;&emsp;

请通过下面固件列表链接下载开发板对应的固件压缩包并解压，解压完成后可以看到其目录结构如下：
&emsp;&emsp;
```bash
├── haas_release_vxxx               # HaaS Python官方固件，{xxx}为版本号
    ├── M601.pac                     # 固件
    └── ChangeNotes.txt               # 更新日志
```

&emsp;&emsp;
### HaaS506固件包列表

* [HaaS506-Windows固件](https://hli.aliyuncs.com/o/config/haas506/haas506.zip)
  * 版本更新说明（2021-12-03）


* [USB烧录驱动](https://hli.aliyuncs.com/o/config/usb_driver/8910_module_usb_driver_20191011_signed.7z)
├── 20210708
├── DriversForWin10              # Windows10 驱动包
├── DriversForWin78              # Windows7/8 驱动包
├── DriverUninstall32.exe
└── DriverUninstall64.exe        # 64位系统安装文件
* [USB转串口驱动](https://files.alicdn.com/tpsservice/ccef039a8fdcb74602c346505baa4814.rar)
├── 安装失败解决办法
├── readme
└── SETUP              # 驱动安装文件
* [烧录工具](https://hli.aliyuncs.com/o/config/haas506/flashtool.zip)
├── Bin 
&emsp;&emsp;└── UpgradeDownload.exe
├── Doc
├── Readme.txt
├── Release notes.xls
<br>


第二步：分别安装下载好的USB驱动和固件烧录工具
1、安装相应的USB驱动文件：解压 8910_module_usb_driver_20191011_signed.7z文件，选择对应操作系统的.exe文件进行安装。

驱动安装完成后，当连接USB接口后，会枚举出Unisoc Usb Serial Port 八个端口，USB具体端口信息如下：
<div align="center">
<img src=./../images/2_2USB端口信息.png width=80%/>
</div>

 > * 注：安装成功后的驱动可在本地右击此电脑-管理-设备管理器-端口（COM和LPT）中查看；如没有，请查看是否插入USB，或重启电脑后再进行查看。
 > * 注：后续脚本推送选择port5对应的COM端口。


2、固件烧录工具无需安装，解压直接使用.exe文件即可
<div align="center">
<img src=./../images/2-3固件烧录工具使用.png width=80%/>
</div>

第三步：开发板通过USB连接到电脑
通常情况下，连接成功以后，电脑上会多出来八个串口设备.

第四步：烧录
1、打开解压好的UpgradeDownload.exe烧录软件；
2、点击软件左上角第一个Load packet选项选择所要烧录的.pac文件；
3、选择好.pac文件，点击软件左上角的第三个“Start Downloading”按钮后，先按住BOOT键不动再按一下RST键，即开始烧录（开始烧录后即可松开按键，无需持续按键）；
4、烧录完成后会显示Passed字样，具体操作请参考下图；
5、烧录完成以后，重按RST复位键 重启设备，通过串口工具连接机器（选择Port5）进入命令行。
<div align="center">
<img src=./../images/2_4烧录.gif width=80%/>
</div>

>* 注：如若无法烧录，请观察按住BOOT键后再按RST键后，设备管理器中串口驱动的显示是否为SPRD U2S Diag，显示异常的情况下，按住BOOT键后再按RST键，在BOOT模式下重新安装驱动（无需卸载原来的驱动）

>* 注：HaaS506暂不支持使用MAC系统进行固件烧录
&emsp;&emsp;

第五步：链接USB串口，查看系统Log
* 使用USB TO TLL和原装杜邦线，将设备uart口和PC进行连接

 >> * USB串口连接注意事项
设备端RX接 USB TO TTL模块的TX
设备端TX接 USB TO TTL模块的RX
设备端GND接 USB TO TTL模块的GND
3.3V不要连接
USB串口除了用来查看log，repl交互之外，HaaS Studio推送文件也是使用的这个接口
&emsp;&emsp;

## 快速验证
&emsp;&emsp;
开机默认会启动之前烧录进去的程序，打开串口工具，选择 USB TO TTL 串口设备，设置波特率为 115200 并连接。


Python轻应用默认支持两种运行模式：交互式 和 文件执行。系统开机默认执行/data/pyamp/main.py 文件， 如果文件不存在或者文件执行退出，都会进入repl 模式。


1） 在交互模式输入代码就能看到执行结果：
<div align="center">
<img src=./../images/2_5快速验证.gif width=80%/>
</div>

2） 在交互模式下输入 execfile(‘filepath’) 可以执行Python文件。 假设我们想执行 /data/pyamp/ 路径下的 main.py文件，输入下面的命令即可：
```
execfile('/data/pyamp/main.py')
```

> * REPL环境下查看文件信息
不同于CLI模式下的文件查看命令，REPL模式下需要使用uos下的listdir函数查看：
```
>>> import uos
>>> uos.listdir('/data')
['pyamp']
>>> uos.listdir('/data/pyamp')
['main.py']
>>>
```

## 三、搭建开发环境
&emsp;&emsp;
### PC环境准备
&emsp;&emsp;
将HaaS506用USB数据线和电脑相连。

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

<br>

### HaaS506串口名称确认
#### Windows系统

&emsp;&emsp;
读者请通过控制面板下的设备管理器，查询当前电脑下HaaS506插入后新增的端口。下图中显示HaaS 506连接后新增的八个串口。

<div align="center">
<img src=./../images/2_5设备管理器端口图.png width=80%/>
</div>

&emsp;&emsp;
如果连接HaaS506之前和之后，没有新增串口，则需要安装HaaS506的串口驱动，[请点我下载](https://files.alicdn.com/tpsservice/ccef039a8fdcb74602c346505baa4814.rar)。

<br>

## 四、HaaS506 HelloWord例程

### 创建helloworld工程
&emsp;&emsp;
请遵循如下的步骤完成helloworld Python工程的创建。

&emsp;&emsp;
如下图所示，点击HaaS Studio的"快速开始"按键会弹出HaaS Studio的欢迎页面，请选择“创建项目”，如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_创建项目向导.png width=60%/>
</div>

&emsp;&emsp;
根据创建工程向导，开发者输入/选择相关的信息即可。下面以在HaaS EDU K1上面创建hellworld示例程序为例演示工程进行，步骤如下:
> 注意事项： 文件夹不要有`中文，空格及其他异常`字符。

1. 输入项目名称
2. 选择工作区所在路径
3. 选择硬件类型
4. 选择编程语言
5. 选择解决方案模板
<div align="center">
<img src=../images/1_HaaS_Studio_Python创建工程_项目名称_HaaSEDUK1.png width=40%/>
</div>

&emsp;&emsp;
然后点击“立即创建”按钮，在随后的步骤中确认输入的信息无误，点击“确认”，等待工程创建完成后，VS Code会自动打开新创建的工程。就可以在左侧的文件浏览页面中看到刚刚创建的helloworld工程。

<div align="center">
<img src=./../images/4_2_HaaS_Studio_Python_helloworld_代码_HaaS506.png width=80%/>
</div>
### 推送脚本到设备

&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_部署运行.png width=5%/>），HaaS Studio工具上面会弹出如下的选择框，请按照如下的步骤逐步选择完成后，HaaS-Studio开始推出送固件。
<div align="center">
<img src=./../images/4_3推送脚本.png width=80%/>
</div>

&emsp;&emsp;
推送固件过程中会提示重启开发板，此时需要开发者按下HaaS100上复位按钮重启开发板。
<div align="center">
<img src=./../images/4_4推送提示.png width=80%/>
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


</details>

<br>
&emsp;&emsp;
推送此脚本到HaaS506之后，点击HaaS Studio的“串口”按钮后选择正确的串口并设定波特率为115200之后，就可以看到设备周期性的打印如下日志。

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