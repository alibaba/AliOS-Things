# HaaS506-HD1 RTU 快速开始
&emsp;&emsp;
HaaS506-HD1是一款经过阿里云HaaS团队认证的高性能、多连接的基于4G-CAT1通信的工业级RTU，本文将从硬件介绍、固件更新、开发环境搭建以及案例验证等多方面为开发者提供详细的快速上手教程。
请开发者们认证查阅，如有疑问或需技术支持，可在钉钉群内提问，您的提问都将会得到完善的答复。 

## 一、硬件介绍
&emsp;&emsp;
HaaS506-HD1是一款集成了M601 4G-CAT1模块的开发板，支持一路485、一路232、2路AI、2路DI、2路继电器接口。支持TCP、HTTP、MQTT等多种网络协议。适用于各种数据采集、传输、控制场景。
HaaS506-HD1内置了HAAS框架，作为一款边缘计算RTU，客户可以方便的通过Python编程，在RTU端执行定制化的业务逻辑与数据处理。

&emsp;&emsp;
下图是HaaS506-DH1 RTU各接口。
<div align="center">
<img src=https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/HaaS506-HD1接口图1.png width=90%/>
</div>

<div align="center">
<img src=https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/HaaS506-HD1接口图2.png width=90%/>
</div>

## 二、HaaS506-HD1 Python固件下载
&emsp;&emsp;
HaaS506-HD1 RTU在出厂时已经烧录了最新的固件，但由于固件一直在不断更新，开发者可根据自身需求决定是否需要更新。

&emsp;&emsp;
请通过下面固件列表链接下载固件压缩包并解压，解压完成后可以看到其目录结构如下：
&emsp;&emsp;
```bash
├── haas_release_v{xxx}               # HaaS Python官方固件，{xxx}为版本号
    ├── haas-320p.pac                    # HaaS506-HD1固件
    └── ChangeNotes.txt             # 更新日志
```

### HaaS506-HD1 固件包列表列表

* [HaaS506-HD1-v2.01](https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/haas506_320p_v2.02.zip)
  * 版本更新说明（2022-06-10）
    * 版本发布
  </details>

<br>

### 安装USB烧录驱动
1、下载[USB烧录驱动](https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/Cat1WindowsUSB驱动.rar)并解压，解压后目录结构如下，请选择您的操作系统类型对应的.exe文件进行烧录驱动的安装。
```bash
├── 20210708
├── 雁飞Cat.1 Windows USB Driver V1.0.exe              # Windows 驱动包
└── 雁飞Cat.1 Windows USB驱动安装指导_V1.0.pdf        # 驱动安装指导文件
```
&emsp;&emsp;
USB烧录驱动安装完成后，当**连接HaaS506-HD1的USB**接口后，Windows系统会枚举出**MEIG USB**开头的八个串口，USB具体端口信息如下：
<div align="center">
<img src=https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/USB端口信息.png width=80%/>
</div>

 > * 注：安装成功后的驱动可在本地右击此电脑-管理-设备管理器-端口（COM和LPT）中查看串口设备；如没有，请确保在插入USB的情况下重启电脑后再进行查看。

2、下载[固件烧录工具](https://hli.aliyuncs.com/o/config/haas506/flashtool.zip)并解压，解压后目录结构如下，解压直接使用UpgradeDownload.exe文件即可运行。
```bash
├── Bin
└── UpgradeDownload.exe
├── Doc
├── Readme.txt
├── Release notes.xls
```

### 烧录固件

1、打开UpgradeDownload.exe烧录软件。<br>
2、点击软件左上角第一个“Load packet”选项选择所要烧录HaaS Python固件（以.pac为后缀的文件）。<br>
3、选择好HaaS Python固件后，点击软件左上角的第三个“Start Downloading”按钮后，通过串口工具选择AT指令串口发送“ AT^FORCEDNLD ”，即开始烧录。<br>
4、烧录完成后会显示Passed字样，具体操作请参考下图。<br>
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/2_4烧录.gif width=80%/>
</div>
<div align="center">
<img src=https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/串口指令烧写图.png width=80%/>
</div>


>* 注：如因操作不当造成无串口显示并无法烧写时，可拆开RTU壳体，在保持烧写工具处于烧写状态下，按住BOOT键后再按RST键，即可重新开始烧录；
>* 注：串口工具任意一款可在AT串口下发指令的即可；
>* 注：HaaS506-HD1暂不支持使用MAC系统进行固件烧录。

&emsp;&emsp;
完成上面的步骤，HaaS Python固件就烧录到HaaS506-HD1上了。

## 三、搭建开发环境

### PC环境准备
&emsp;&emsp;
用**USB转UART模块**连接HaaS506-HD1的**TTL串口**后，将USB转UART模块连接电脑的USB口。

> USB转UART模块连接注意事项<br>
>>* **RTU设备端RX** 接 **USB转UART模块的TX**<br>
>>* **RTU设备端TX** 接 **USB转UART模块的RX**<br>
>>* **RTU设备端GND** 接 **USB转UART模块的GND**<br>


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
插件第一次安装完成后，会提示安装相关工具才能激活插件，请同意安装相关工具。第一次新建或者打开python轻应用工程，也会安装轻应用开发相关工具，同样需要同意安装。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas-studio-tool-install.png width=80%/>
</div>

&emsp;&emsp;
插件安装完成后，则 VSCode 左下角的状态栏会显示"快速开始"的图标，如下图所示。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas-studio-startup-page.png width=80%/>
</div>

&emsp;&emsp;
一般情况下，左下角只会显示快速开始图标，如果打开或者新建了某个Python工程，则会在VSCode底部的状态栏展开如下一排按钮，这些按钮的功能如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python工程按钮.png width=40%/>
</div>

&emsp;&emsp;
为了方便开发，还可以打开高级串口模式，在当前的工程目录下，存在.vscode这样一个文件夹，找到里面的settings.json文件，将pythonAdvanced选项设置成enable即可，打开方式如下：
* 注意高级模式某些平台可能不支持，比如低版本的linux，M1系列MACOS等，如果平台不支持，会自动设置成 disable。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas-studio-python-advance.png width=80%/>
</div>

&emsp;&emsp;
python高级模式打开之后，这些按钮的功能变成如下图所示：

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas-studio-python-advance-enable.png width=40%/>
</div>


### HaaS506-HD1串口名称确认
#### Windows系统

&emsp;&emsp;
读者请通过设备管理器，查询当前电脑插入**USB转串口模块**后新增的端口（注意不是以`MEIG USB`开头的固件烧录串口）。下图中显示HaaS506-HD1连接后新增的串口（笔者电脑中为COM146）。

<div align="center">
<img src=https://hli.aliyuncs.com/o/config/HaaS506-HD1快速开始/新增CH340串口图.png width=80%/>
</div>

&emsp;&emsp;
如果连接HaaS506-HD1之前和之后，没有新增串口，则需要安装**USB转串口模块**的驱动，驱动下载地址请咨询**USB转串口模块**商家。

<br>

## 四、HaaS506-HD1 HelloWord例程

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
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python创建工程_项目名称_HaaS506.png width=40%/>
</div>

&emsp;&emsp;
然后点击“立即创建”按钮，在随后的步骤中确认输入的信息无误，点击“确认”，等待工程创建完成后，VS Code会自动打开新创建的工程。就可以在左侧的文件浏览页面中看到刚刚创建的helloworld工程。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/4_2_HaaS_Studio_Python_helloworld_代码_HaaS506.png width=80%/>
</div>

### 推送脚本到设备

&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_部署运行.png width=5%/>），HaaS Studio工具上面会弹出如下的选择框，请按照如下的步骤逐步选择完成后，HaaS-Studio开始推出送固件。
<div align="center">
<img src=../images/1_HaaS_Studio_Python_本地推送脚本.png width=60%/>
</div>

&emsp;&emsp;
**推送固件过程中会提示重启设备，此时需要为RTU设备重新上电，以此来重启RTU**
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/4_4推送提示.png width=80%/>
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
推送此脚本到HaaS506-HD1之后，点击HaaS Studio的“串口”按钮后选择**USB转串口模块**对应的串口并设定波特率为115200之后，就可以看到设备周期性的打印如下日志。

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



### 查看系统日志

* 用USB转UART模块连接HaaS506-HD1的**TTL串口**后，将USB转UART模块连接电脑的USB口。

