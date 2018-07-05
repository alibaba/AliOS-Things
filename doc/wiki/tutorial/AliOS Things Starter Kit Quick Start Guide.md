![](https://i.imgur.com/Q8Jcei1.png)

# AliOS Things Starter Kit Quick Start Guide
---


## 1. 概述
### 1.1 开发平台
Starter Kit是一款由上海诺行信息技术有限公司基于STM32L433CCT6 设计的高性能物联网开发板。它的主要软件平台为开源的AliOS Things，开发者可以基于此快速的开发出各种物联网设备与产品。  
AliOS Things 是面向IoT 领域的轻量级物联网嵌入式操作系统。致力于搭建云端一体化IoT 基础设备。具备极致性能，极简开发、云端一体、丰富组件、安全防护等关键能力，并支持终端设备连接到阿里云Link，可广泛应用在智能家居、智慧城市、新出行等领域。

### 1.2 出厂软件和SDK
Starter Kit 的软件分成了出厂版本和SDK。

出厂版本：提供给工厂生产人员使用，用来检验测试出厂硬件各个外设器件是否能够工作正常。工具包套件中仅包含了编译好的二进制文件，开发者也可以用来对单板进行测试或者观察外设的功能特性。相关使用方法请参考文档[《AliOS Things Starter Kit User Basic Operation Guide》](AliOS-Things-Starter-Kit-User-Basic-Operation-Guide)。  
SDK：提供给物联网开发者与高校教育培训机构使用，用来快速评估与开发出各种物联网产品。SDK 内容包括编译烧写相关的工具和主要软件平台AliOS Things。Starter Kit板级代码已经加入AliOS Things 源码树中，以后也会不断地进行完善与更新。 

## 2.开始开发
### 2.1 环境搭建
开始开发AliOS Things 之前，需要安装一系列的工具软件。本章节主要介绍Windows 环境下开发环境的搭建过程。

#### 2.1.1 驱动程序
打开软件包，解压en.stsw-link009.rar 到当前目录。  
将附带的USB 线缆（目前产品出货没有配套USB 线，用户可以使用自己手机USB线）一头插入Starter Kit 的USB ST-Link 口，另外一头连接PC。打开设备管理器，将多出来的未知设备更新驱动程序，选中刚才解压的目录路径，完成安装。  
#### 2.1.2 ST-LINK Utility
打开软件包，解压en.stsw-link004.zip 到当前目录。打开解压后的安装程序一路完成安装。

#### 2.1.3 Python2
打开Python 官网，选择相应的2.x 版本（当前最新版本为Python 2.7.15）下载并安装：
[https://www.python.org/downloads/windows/](https://www.python.org/downloads/windows/)
安装完成后，在系统属性中为的系统环境变量PATH 加入相应的路径（根据自
己的安装目录）：
![](https://i.imgur.com/N8yMy55.jpg)

#### 2.1.4 pip
打开pip 官方网站，选择最新的压缩包版本（当前最新版本为pip-10.0.1.tar.gz）
下载：
[https://pypi.org/project/pip/#files](https://pypi.org/project/pip/#files)
完成后解压到任意目录，打开命令提示符，进入到刚才解压到的根目录中，输入命令安装pip：python setup.py install

#### 2.1.5 aos
Aos是阿里的代码管理编译工具。在安装aos之前，需要安装或更新相关依赖软件。在刚才的命令提示符中输入如下命令：  
$ pip install --upgrade setuptools  
$ pip install --upgrade wheel  
完成后输入如下命令安装aos：  
$ pip install --upgrade aos-cube  

#### 2.1.6 AliOS Things Studio
AliOS Things Studio 是阿里基于Visual Studio Code 的一套图形化IDE。进入官网下载并安装：
[https://code.visualstudio.com/](https://code.visualstudio.com/)
完成后打开工具，在最左边的工具条中选择点击扩展，分别安装C/C++ 插件和alios-studio 插件：

![](https://i.imgur.com/OA6Wckd.jpg)


#### 2.1.7 git
为了同步最新的AliOS Things 代码，需要安装git。进入官网下载并安装：
[https://git-scm.com/downloads](https://git-scm.com/downloads)
完成后使用Git GUI 或者Git Bash 将最新代码克隆到本地：  
git clone https://github.com/alibaba/AliOS-Things.git  
git checkout master

### 2.2 工程关系
AliOS Things 代码的工程名格式为”应用名字@目标板名字”。应用名字对应为example 目录下对应的文件夹名；目标板名字对应为board 目录下对应的文件夹名。例如Starter Kit 的helloworld 例程工程名为helloworld@starterkit。

### 2.3 开始编译
打开AliOS Things Studio，点击左上角菜单栏上的文件->打开文件夹，选择AliOS Things 代码根目录。  
打开后点击工具左下角蓝色框条中的![](https://i.imgur.com/JHUoMmu.jpg)图标，先输入应用名字，敲回车，再输入目标板名字，敲回车。然后点击旁边的![](https://i.imgur.com/B3tCFZR.jpg)  图标开始编译。第一次编译有可能报错， 再点击一次一般就能够成功了。编译成功后的二进制文件放在out\helloworld@starterkit\binary 里面。

### 2.4 开始烧写
Starter Kit 提供了三种烧写方式，分别为虚拟U 盘烧写、ST-LINK Utility 烧写和AliOS Things Studio 烧写。使用方法如下（推荐使用虚拟U 盘烧写方式）：
烧写和调试之前需要使用附带的USB 线缆一头插入Starter Kit 的USB ST-Link口，另外一头连接PC。此时在PC 设备管理器中可以看到会自动生成一个虚拟UART口。这个UART 是单板的软件调试口，系统运行后可以观察到调试输出信息。 

### 2.4.1 虚拟U盘烧写  
将单板 USB ST-Link 连接到 PC，此时资源管理器会自动装载一个虚拟的 USB
存储器，直接将编译好的 binary 拷贝到这个 USB 存储器，即可自动烧写。
烧写完成后单板会自动reset，并且可以观察到虚拟的USB 存储器被重新装载。

### 2.4.2 ST-LINK Utility 烧写  
打开 ST-LINK Utility，点击工具栏中的![](https://i.imgur.com/JncVqOI.jpg)图标连接设备，连接成功后，点击![](https://i.imgur.com/oHQ7mIX.jpg) 打开 binary，然后点击![](https://i.imgur.com/2h4qB6b.jpg)打开烧写界面，最后点击 start 开始烧写。
烧写完成后单板会自动 reset，可以观察到调试 UART 口输出信息。

### 2.4.3 AliOS Things Studio 烧写  
AliOS Things Studio 在最后一次编译完成后，直接点击左下角蓝色框条中的![](https://i.imgur.com/r05xPt5.jpg)
图标进行烧写。注意这种方法烧写单板后，可能不会自动reset。如果出现烧写后没有运行或者运行异常，尝试拔插 USB 线缆即可。

## 2.5 开始调试
在开始调试之前，需要先对调试器进行板级配置。
使用 AliOS Things Studio 打开单板代码目录，可以观察到左侧资源视图中生成了一个.vscode 目录，打开此目录下的文件 launch.json。

![](https://i.imgur.com/Xj9sAfj.jpg)

可以看到默认会有几个单板调试配置，开发者可以选择在已经存在配置里面修改，或者创建一组新的配置。  
这里以创建新配置为例，将Debug @ Windows 这一组配置复制下来，添加到最后。修改并适配到目前的工程，主要注意四条配置：name、program、setupCommands 和miDebuggerPath，相关路径使用斜杠或者双反斜杠。我这里以设置name 为Starterkit Debug @ Windows 为例，如下：
  
    {
       "name": "Starterkit Debug @ Windows",
       "type": "cppdbg",
       "request": "launch", 
       "program":"${workspaceRoot}/out/helloworld@starterkit/binary/helloworld@starterkit.elf", "args": [],
       "stopAtEntry": true,
       "cwd": "${workspaceRoot}",
       "environment": [], "externalConsole": true,
       "miDebuggerServerAddress": "localhost:4242",
       "serverLaunchTimeout": 2000,
       "targetArchitecture": "ARM", 
       "setupCommands": [
           {
               "text": "cd ${workspaceRoot}"
           },
           {
               "text": "source .gdbinit"
           },
           {
               "text": "target remote localhost:4242"
           },
           {
               "text": "file out/helloworld@starterkit/binary/helloworld@starterkit.elf"
           },
           {
               "text": "break application_start"
           }
       ],
       "customLaunchSetupCommands": [], "launchCompleteCommand": "exec-run", "osx": {
            "MIMode": "gdb",
            "miDebuggerPath": "arm-none-eabi-gdb"
       },
       "linux": {
            "MIMode": "gdb",
            "miDebuggerPath": "arm-none-eabi-gdb"
       },
       "windows": {
            "MIMode": "gdb", 
            "miDebuggerPath":
    "${workspaceRoot}/build/compiler/gcc-arm-none-eabi/Win32/bin/arm-none-eabi-gdb.exe"
       }
    }

调试器配置完成后，在最左侧的工具栏中点击调试图标，并选择调试器配置：

![](https://i.imgur.com/EZjLKUp.jpg)

最后，在最左侧的工具栏中点击资源管理器，选择你想要调试的代码段下断
点，按下F5 开始进行在线调试。


## 3.例程介绍
### 3.1 starterkitgui
受限于Starter Kit 上MCU 的RAM 大小，入云和GUI 无法共存。这里简单介绍一下Starter Kit 上GUI 的例程，该例程演示了相关外设的能力，包括KEY/LED/LCD/GSENSOR/ALSSENSOR/PSENSOR/CODEC 等，开发者可以直观感受到外设提供的底层数据效果。  
直接编译目标starterkitgui@starterkit（参考2.3 章节），并完成烧写。
单板启动后，可以看到LCD 被点亮，并显示AliOS Things 的LOGO，然后切换到第一个界面，显示版本信息。同时开发者通过调试串口感受AliOS Things 的命令行。  
按下Wi-Fi 按键继续切换GUI 界面，第二个界面显示相关sensor 提供的底层数据，并点亮相关LED；第三个界面演示GUI 能力，将GSENSOR 的三轴加速数据以动态曲线的形式显示出来；第四个界面展示录音播放能力，在此界面下按A 键进行录音，按B 键播放。
  
### 3.2 其他例程陆续完善中

其他问题请参考AliOS-Things WIKI：
[https://github.com/alibaba/AliOS-Things/wiki](https://github.com/alibaba/AliOS-Things/wiki)

#NOTE:
购买开发板AliOS Things Starter Kit和Developer kit，请扫描天猫旗舰店二维码：

![](https://i.imgur.com/VxHonLB.png)

获取在线技术支持，请扫描技术支持群：

![](https://i.imgur.com/uyT4v0Y.png)
![](https://i.imgur.com/rqjjjA0.jpg)