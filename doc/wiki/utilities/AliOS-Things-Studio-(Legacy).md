*(本文中提及的许多功能已发生变化或者不再适用，请谨慎参考）*

# 目录
- [1 下载地址](#1-下载安装)
    - [1.1 VS Code安装](#11-vs-code安装)
    - [1.2 AliOS Things Studio安装](#12-alios-studio安装)
- [2 界面介绍](#2-界面介绍)
- [3 VS Code介绍](#3-vs-code介绍)
    - [3.1 C语言支持](#31-C语言支持)
    - [3.2 命令面板与符号搜索](#32-命令面板与符号搜索)
    - [3.3 keymap映射](#33-keymap映射)
    - [3.4 符号视图](#34-符号视图)
    - [3.5 git支持](#35-git支持)
    - [3.6 内置终端](#36-内置终端) 
- [4 新建与导入项目](#4-新建与导入项目)
    - [4.1 新建项目](#41-新建项目)
    - [4.2 导入项目](#42-导入项目)
    - [4.3 导入AliOS](#43-导入AliOS)
- [5 设备管理](#5-设备管理)
- [6 编译功能](#6-编译功能)
- [7 烧写功能](#7-烧写功能)
    - [7.1 串口烧写](#71-串口烧写-mk3060为例)
    - [7.2 J-LINK烧写](#72-j-link烧写-mk3060为例)
- [8 aos-cube支持](#8-aos-cube支持)
- [9 单步调试](#9-单步调试)
- [10 Trace](#10-trace)
- [11 技术支持](#11-技术支持)

------

AliOS Things Studio 是AliOS Things提供的 IDE 开发环境，支持 Windows、Linux、MacOS，基于Windows平台介绍。

# 1 下载安装
AliOS Things Studio 是 VS Code 插件，所以首先安装 VS Code ，通过插件的方式安装 AliOS Things Studio。

## 1.1 VS Code 安装
在 [VS Code](https://code.visualstudio.com) 官网下载安装包并安装：

![](https://img.alicdn.com/tfs/TB1ACciXPihSKJjy0FlXXadEXXa-865-558.png)

VS Code调试等部分功能依赖.net framework 4.5.2。如果您使用的是Windowns 7系统，请确保安装了[.NET Framework 4.5.2](https://www.microsoft.com/en-us/download/details.aspx?id=42643)
## 1.2 AliOS Things Studio安装
打开 VSCode, 点击左侧的扩展按钮，搜索alios-studio，点击安装即可

![](https://img.alicdn.com/tfs/TB1_to5gMoQMeJjy0FpXXcTxpXa-2880-1800.png)

安装完成后，会提示重启 VS Code，重启后 AliOS Things Studio 插件生效。

# 2 界面介绍

![](https://img.alicdn.com/tfs/TB1Vf2mfwMPMeJjy1XbXXcwxVXa-2880-1800.png)

# 3 VS Code介绍

VS Code 是微软推出的一款跨平台编辑器，对 C 语言的编辑，导航，调试有强大的支持。
 
## 3.1 C 语言支持

* VS Code 通过安装 C/C++ 插件提供强大的 C 语言支持。

![](https://img.alicdn.com/tfs/TB1avRUbqagSKJjy0FhXXcrbFXa-2880-1800.png)
 
* 在符号上右键可以进行跳转，查找引用，查找定义等操作。

![](https://img.alicdn.com/tfs/TB1pktRbzihSKJjy0FfXXbGzFXa-926-990.png)
 
*  将鼠标移到函数/宏/变量上可显示定义，也可以在右键菜单通过 peek declaration 直接查看定义附近代码，或者 go to declaration 跳转到定义。
 
![](https://img.alicdn.com/tfs/TB1GS4RbqagSKJjy0FbXXa.mVXa-1926-994.png) 
 
* 函数，变量的自动补全
 
 ![](https://img.alicdn.com/tfs/TB1gjV0bzuhSKJjSspjXXci8VXa-1436-504.png)

* 函数/宏参数提示，支持重载

![](https://img.alicdn.com/tfs/TB1oWviaLBNTKJjy0FdXXcPpVXa-838-162.png)

* 代码检查

C/C++ 插件提供了基本的语法检查功能，会在"问题面板"中显示代码中的语法错误。注意：语法检查功能需要在当前文件的所有 include 都找到后才会工作。未能找到的 include 文件会以绿色波浪线提示，需要在配置文件中添加 include 路径，确保所有 include 的文件都没有绿色波浪线后，语法检查功能才会正常运作。

![](https://img.alicdn.com/tfs/TB1AopZbqmgSKJjSspiXXXyJFXa-1114-590.png)

## 3.2 命令面板与符号搜索

* 按键 ctrl + shift + p 可以打开命令面板，搜索并执行 VS Code 及插件支持的命令。

![](https://img.alicdn.com/tfs/TB1P90RbqagSKJjy0FbXXa.mVXa-1234-766.png)

* 呼出命令面板后，删掉输入框最前面的>，可以搜索当前工作区的文件名/路径

![](https://img.alicdn.com/tfs/TB1cm8RbqagSKJjy0FbXXa.mVXa-1270-490.png)

* 在命令面板中以 @ 开头，可以显示并搜索当前文件中的符号

![](https://img.alicdn.com/tfs/TB1mBVSbqagSKJjy0FaXXb0dpXa-1252-742.png)

* 在命令面板中以 # 开头，可以显示并搜索当前工作区中的符号

![](https://img.alicdn.com/tfs/TB1nXV1bqagSKJjy0FcXXcZeVXa-1246-752.png)

## 3.3 keymap 映射

* 借助 VS Code 强大的插件生态，你可以将几乎任何你习惯的 IDE/编辑器 的快键键映射到 VS Code 上。包括 Visual Studio, Eclipse, Vim, Intellij 等。搜索并安装对应的插件即可。

 ![](https://img.alicdn.com/tfs/TB18P00bzuhSKJjSspjXXci8VXa-2880-1800.png)

## 3.4 符号视图

* 通过安装 code outline 插件，可以显示当前文件的符号列表(对应其他编辑器的 outline / symbol view / structure view)

![](https://img.alicdn.com/tfs/TB1pnhRbqagSKJjy0FbXXa.mVXa-2880-1800.png)
 
## 3.5 git 支持

* 使用 VS Code 可以方便的进行 git 的大部分操作，如分支切换，提交 commit，解决冲突等。详细的 git 使用说明可以参考 [Version Control](https://code.visualstudio.com/docs/editor/versioncontrol)
 
## 3.6 内置终端

* VS Code 可以在编辑器中直接打开 bash/power shell/cmd，免去了在终端和编辑器间切换的麻烦。

![](https://img.alicdn.com/tfs/TB1qqd1bqagSKJjy0FcXXcZeVXa-2150-550.png)
 
# 4 新建/导入项目

**在新建/导入项目前，需要配置工具链和拷贝OpenOCD文件到AliOS Things源代码下面，对应
 [AliOS Things Environment Setup](AliOS-Things-Environment-Setup) 2.2 节和 4.1 节**

## 4.1 新建项目
    
对于应用开发者，通常可以通过新建项目生成一个基本的示例应用，并在此基础上进行开发。首先点击下方状态栏的 Create Project，或者执行 alios-studio: Create Project 命令打开新建项目界面
    
![](https://img.alicdn.com/tfs/TB1OWviaLBNTKJjy0FdXXcPpVXa-216-46.png)
    
第一次创建项目会进入 SDK 管理界面，在此输入一个SDK的路径及对应的版本即可。如果以后需要增加或修改 SDK，也可以通过 alios-studio:Set SDK 命令打开 SDK 管理界面进行 SDK 管理，进行 SDK 的编辑，增加及删除。SDK路径设置成AliOS-Things源码目录即可。
    
![](https://img.alicdn.com/tfs/TB1QzV0bzuhSKJjSspjXXci8VXa-1716-882.png)
    
设置完 SDK 后可以进入新建项目界面，选择创建应用对应的模板，使用的 SDK 版本，以及开发板的类型，并选择一个保存项目的路径，点击 submit 即可创建项目。由于创建项目需要拷贝 AOS 的源码到工程中，需要耐心等待一小会儿。
    
![](https://img.alicdn.com/tfs/TB1yxHXavxNTKJjy0FjXXX6yVXa-865-458.png)
    
新项目建立完成后目录结构如下，.vscode 中存储项目相关的配置文件，应用目录存储应用的源码文件，aos 目录存储系统相关的源码文件，out 目录存储编译输出的文件。每个应用拥有独立的系统源码，可以独立修改而不会互相影响。
    
![](https://img.alicdn.com/tfs/TB12EtZbqmgSKJjSspiXXXyJFXa-2880-1800.png)
    
## 4.2 导入项目

 AliOS Things Studio 没有导入项目的概念，通过菜单里中文件 - 打开 功能打开 AOS 源码或应用项目即可。

## 4.3 导入 AliOS

通过 VS Code - 文件 - 打开文件夹，打开 AliOS 代码目录：

![](https://img.alicdn.com/tfs/TB1xmpKbaagSKJjy0FhXXcrbFXa-865-429.png)

在项目里面编辑代码。

# 5 设备管理

点击状态栏中的 Connect Device 可以连接设备，串口的 COM 口，可以在计算机 - 属性 - 设备管理 - 端口，找到 USB Serial Port端口：

![](https://img.alicdn.com/tfs/TB1UziVXRTH8KJjy0FiXXcRsXXa-865-518.png)

初次连接设备时，VS Code 上方会弹出输入框提示用户选择串口号及输入波特率。MAC 用户需要注意MAC 设备默认可能会有一个蓝牙设备的串口，应该选择包含 usbserial 的串口号。

![](https://img.alicdn.com/tfs/TB1YhHuXS_I8KJjy0FoXXaFnVXa-865-488.png)

![](https://img.alicdn.com/tfs/TB1d8DsXMnD8KJjy1XdXXaZsVXa-865-489.png)

设备连接完成后，Connect Device 按钮隐藏，界面下方会呼出输出界面显示串口日志，同时状态栏会出现一个以设备串口号命名的按钮。点击该按钮会在界面上方弹出设备相关的功能。包括断开设备，烧录镜像，打开 shell 窗口，修改波特率，打开 Trace 窗口，连接其他串口。

![](https://img.alicdn.com/tfs/TB1xtTwXMvD8KJjy0FlXXagBFXa-865-487.png)

在 shell 窗口中，可以在输入框中输入命令执行，也可以点击左侧的按钮列出设备支持的命令，直接填入输入框中，并且可以在 shell 窗口中看到命令执行的返回结果。(部分命令的结果也可能从串口 log 中输出)

![](https://img.alicdn.com/tfs/TB1nmG8aLNNTKJjSspkXXaeWFXa-1066-1488.png)

# 6 编译功能

通过状态栏中的 Build 和 Clean 可以进行编译及清除。点击状态栏中类似 alinkapp@mk3060 的按钮可以修改编译的应用及目标板(应用开发者只能修改目标板)，通过修改 .vscode/settings.json 可以修改多 Bin 相关的编译配置。当 aliosStudio.build.multiBin 为 false 时，进行单 Bin 编译，当 aliosStudio.build.multiBin 为 true 时，根据 aliosStudio.build.multiBinTarget 的值确定编译的目标为 kernal, app 还是framework 等。
    
![](https://img.alicdn.com/tfs/TB1CWHiaLBNTKJjy0FdXXcPpVXa-660-90.png)
    
编译时，在 ALIOS-STUDIO 的输出中可以看到编译的详细 log。
    
![](https://img.alicdn.com/tfs/TB1sX01bqagSKJjy0FcXXcZeVXa-2880-1800.png)
    
编译失败时，ctrl/cmd+左键 点击错误所在的文件路径可以跳转到发生错误的位置。

![](https://img.alicdn.com/tfs/TB11RVSbqagSKJjy0FaXXb0dpXa-2880-1800.png)

# 7 烧写功能

## 7.1 串口烧写 MK3060 为例
配置串口烧写 COM 端口（成功安装串口驱动后，才能正确的识别端口。参考 [AliOS Environment Setup](AliOS-Things-Environment-Setup)）

进入 bootloader 模式（按住boot按键，再按reset按键），点击下 bar 栏的 UpLoad 按钮，烧录时，Alios-Studio 会每隔一秒显示烧录的进度。烧录完成后，会切换到串口日志输出，看到 Programming Completed Successfully 则表示烧录成功。可以点击开发板上的 reset 按钮尝试加载代码执行。

## 7.2 J-Link 烧写 MK3060为例
J-Link 连接模组（成功安装 J-Link 驱动后，才能正确建立 J-Link 连接。参考 [AliOS Things Environment Setup](AliOS-Things-Environment-Setup)），在左侧的 BUILD TARGET 选择 alinkapp@mk3060 JTAG=jlink download：

![](https://img.alicdn.com/tfs/TB1AebMfgoQMeJjy0FpXXcTxpXa-865-441.png)

会编译并且烧写 image 到模组。

# 8 aos-cube支持

AliOS Things Studio 也支持用户自定义 aos-cube 命令并执行。在项目文件树的下方找到 BUILD TARGET 并展开，可以看到当前存储的 aos-cube 命令列表，点击 BUILD TARGET 旁边的加号可以新增 aos-cube 命令，点击垃圾桶后，界面上方会列出当前的 aos-cube 命令列表，用户选择后，对应的aos-cube 命令将被删除。双击 BUILD TARGET 下的 aos-cube 命令，AliOS Studio 就会去执行对应的 aos-cube 命令。

![](https://img.alicdn.com/tfs/TB1apV1bqagSKJjy0FcXXcZeVXa-366-881.png)

# 9 单步调试

AliOS Things Studio 依赖 VS Code 的 C/C++ 插件进行单步调试，调试前务必确认安装好 C/C++ 插件，连接上开发板及 JLINK，Windows 需要安装好J-Link驱动，同时 J-Link 上绿色指示灯点亮，开发板切换到 bootloader 状态,且烧入了与本地代码一致的镜像。

![](https://img.alicdn.com/tfs/TB1VQxMcgoQMeJjy0FoXXcShVXa-865-458.png)

![](https://img.alicdn.com/tfs/TB1BUpZbqmgSKJjSspiXXXyJFXa-2880-1800.png)

![](https://img.alicdn.com/tfs/TB1O7xMcgoQMeJjy0FoXXcShVXa-865-457.png)

默认设置的断点是 application_start 函数，可修改 VS Code gdb 启动配置 launch.json 修改启动断点位置。
目前暂不支持重启(单步调试工具栏从右往左数第二个按钮)。

# 10 Trace
## 10.1 开启 Trace
Trace功能默认是关闭的，在使用Trace前，需要开启Trace功能：
* 打开模组下面的 `k_config.h` `RHINO_CONFIG_TRACE` 为 1：
```
/* kernel trace conf */
#ifndef RHINO_CONFIG_TRACE
#define RHINO_CONFIG_TRACE                   1
#endif
```
如果模组支持 `RHINO_CONFIG_HW_COUNT`，打个 `RHINO_CONFIG_HW_COUNT` 支持：
```
/* kernel timer&tick conf */
#ifndef RHINO_CONFIG_HW_COUNT
#define RHINO_CONFIG_HW_COUNT                1
#endif
```
* Trace使用网络来传输数据，确保模组已经连接网络，并且电脑同板子连接在同一个局域网内。

## 10.2 使用 Trace
在设备功能中点击 Trace 进入 Trace 界面

![](https://img.alicdn.com/tfs/TB1hYK4fwMPMeJjy1XdXXasrXXa-1438-797.png)

在Trace的主界面可以列出当前设备中的几个 task，如果需要重新刷新，可以通过点击左上角的刷新按钮来刷新 task 列表。在 task 列表中，通过点击其中的某一个task名称，可以弹出对话框来进一步查看该单个task随时间推移的变化过程：

![](https://img.alicdn.com/tfs/TB1IMrufwMPMeJjy1XcXXXpppXa-1438-797.png)

将光标移动至滚动区域以后，配合鼠标滚轮滚动可以对该滚动区域进行缩放，可以方便查看全局时间图表。对滚动区域进行拖动，可以使滚动区域停止滚动以便观看到之前的时间变化情况，重新恢复滚动状态可以通过点击滚动区域右上角的圆型按钮：

![](https://img.alicdn.com/tfs/TB1FYK4fwMPMeJjy1XdXXasrXXa-1438-797.png)

点击Close按钮可以关闭单个task的对话框。回到task列表，通过点击第二个Tab，系统信号量(semaphore)来打开系统信号量的相关信息。在系统信号量的主界面，可以看到随时间的推移的变化，光标移动到滚动区域可以对滚动区域进行拖动和缩放。

![](https://img.alicdn.com/tfs/TB1amK8aLNNTKJjSspkXXaeWFXa-1438-797.png)

## 11 技术支持

在命令面板中执行 alios-studio: Technical Support 命令，会自动打开 AliOS Things Studio 用户支持钉钉账号的对话窗口，有任何问题，意见及建议可以直接通过这一渠道反馈。

