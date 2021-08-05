@page haas-studio HaaS开发环境 haas-studio

HaaS 开发 官方 IDE

# 主要特性

1. 支持AliOS-Things Native应用开发，JavaScript 轻应用开发，Python 轻应用开发。
2. 支持通过界面创建和管理工程
3. 支持AliOS-Things 组件管理
4. 支持HaaS 解决方案展示

*请先卸载alios-studio插件，再安装HaaS-Studio插件，安装完成后重启下vscode。*



# 搭建开发环境
## 安装工具
* Visual Studio Code

Visual Studio Code(下称VS Code) 是一款广泛使用的IDE工具。请根据环境下载对应版本并安装。

注意点: Windows 系统下请以管理员身份 运行VS Code，避免因为python权限问题导致创建失败

## 配置VS Code
“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：

* aos-tools

aos-tools主要于编译AliOS-Things代码和镜像烧录。HaaS-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png" style="max-width:800px;" />
</div>

等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。已安装(INSTALLED)插件列表中会显示"HaaS-studio"和"C/C++"两个插件：

![](https://img.alicdn.com/imgextra/i1/O1CN01tDiYVM1dMiqwXlLoK_!!6000000003722-0-tps-1898-1030.jpg)

安装插件后请重启下vscode让插件生效。


# 创建工程
创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。

## 开始创建
点击左侧H形图标，选择快速开始选项，打开HaaS快速开始页面。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i4/O1CN01EqwPwV1Q1ZYFhQR9c_!!6000000001916-2-tps-2804-1750.png" style="max-width:800px;" />
</div>

下面以C/C++开发为例,介绍AliOS-Things Native应用开发环境。

## 选择开发语言

选择C/C++开发选项，点击创建项目按钮，打开创建工程向导。

## 输入项目信息

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01f0Z3rN1qIgMKYVmau_!!6000000005473-2-tps-1164-1002.png" style="max-width:800px;" />
</div>

* 项目名字，给这个项目起个名字
* 工作区路径，工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)
* 选择硬件类型，即选择在哪块开发板上运行这套示例程序，如HaaS100等
* 选择解决方案，即选择什么功能的示例程序，如helloworld demo, hal demo等

点击“立即创建按钮”。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01Lf7GHh27Z1y0w9u36_!!6000000007810-2-tps-1156-916.png" style="max-width:800px;" />
</div>

点击“确认”按钮。

VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png" style="max-width:800px;" />
</div>

# 开发及调试
## 编译工程
点击状态栏的编译图标，稍等片刻（由于需要安装编译器，第一次编译耗时稍久）就能看到编译成功的提示。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01IGczJA23UfiDMdaO0_!!6000000007259-2-tps-1466-343.png" style="max-width:800px;" />
</div>

## 烧录镜像

* 如下图，用数据线把HaaS开发板和电脑连起来，同时断开HaaS开发板的电源。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN01DA4GIL1cL3lxXK5Vv_!!6000000003583-2-tps-1012-747.png" style="max-width:800px;" />
</div>

* 启动烧录
  * 点击VS Code下方状态栏中的烧录图标，随后VS Code会显示电脑已识别到的串口的列表(Avaliable ports)
  * 根据HaaS100开发板的端口号，输入相对应的串口序号

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01ya7crN1dYd7kpi1t1_!!6000000003748-2-tps-1398-289.png" style="max-width:800px;" />
</div>

* 接上HaaS开发板的电源，稍等两分钟就可以看到VS Code提示“成功烧录”

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN01Foj1sU1HIDNnvs6gM_!!6000000000734-2-tps-1402-196.png" style="max-width:800px;" />
</div>

## 查看日志

* 点击VS Code的串口按钮（插头图标）, 配置串口端口号，并配置波特率至1500000，点击HaaS开发板Reset按键后，就可以在串口工具中看到日志打印了。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01WFJrEO2AI25lZacbI_!!6000000008179-2-tps-1066-374.png" style="max-width:800px;" />
</div>

* 串口异常情况处理

如果遇到打开串口失败，或者想要修改串口号和波特率时可以先删除.vscode/setting.json里关于串口的信息后再重新配置。

串口配置信息位置参考下图：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN012PQ3mf1IDvJXTCGyQ_!!6000000000860-0-tps-1536-558.jpg" style="max-width:800px;" />
</div>