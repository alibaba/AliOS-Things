@page haas100_quick_start HaaS100快速开始

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/quickstart/haas100_quick_start.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**


本文向开发者展示在HaaS100开发板上快速上手AliOS-Things 3.3.
具体操作分为三步：

- 搭建开发环境
- 创建工程
- 开发及调试



# 1. 搭建开发环境
## 1.1 安装工具

- **Visual Studio Code**

Visual Studio Code(下称VS Code) 是一款广泛使用的IDE工具。请根据环境下载对应版本并安装。

注意点: Windows 系统下请以管理员身份 运行VS Code，避免因为python权限问题导致创建失败

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN012VNoAn1UV7Y3T6t4o_!!6000000002522-2-tps-412-649.png" style="max-width:800px;" />
</div>

## 1.2 配置VS Code
“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：
### 安装VS Code插件

- **alios-studio**

alios-studio是开发AliOS Things所需的核心插件，安装插件流程如下：

1）点击左侧活动栏的插件图标

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01f8ThZj1ZxaExr3UuC_!!6000000003261-2-tps-1088-526.png" style="max-width:800px;" />
</div>

2）在搜索框中输入插件名称

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01yNiCUt1w6DFUMcN3r_!!6000000006258-2-tps-1078-491.png" style="max-width:800px;" />
</div>

3）搜索到alios-studio插件后点击安装(install)

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01scK59H1uwJ4APemk9_!!6000000006101-2-tps-1798-468.png" style="max-width:800px;" />
</div>

- **aos-tools**

aos-tools主要于编译AliOS-Things代码和镜像烧录。alios-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png" style="max-width:800px;" />
</div>

等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。
**如果没有弹出安装aos-tools的提示请重启VS Code。**
已安装(INSTALLED)插件列表中会显示"alios-studio"和"C/C++"两个插件：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01uSbuTA1Utqw2mAEEZ_!!6000000002576-2-tps-1348-802.png" style="max-width:800px;" />
</div>

# 2. 创建工程
创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。
## 2.1 开始创建
点击左下角的"+"图标开始创建工程

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01xmJW0b25z1BVejS40_!!6000000007596-2-tps-796-275.png" style="max-width:800px;" />
</div>

## 2.2 选择解决方案和开发板
随后VS Code会依次提示“请选择解决方案”和“请选择开发板”:

- 选择解决方案，即选择什么功能的示例程序，如helloworld demo, linksdk demo等
- 选择开发板，即选择在哪块开发板上运行这套示例程序，如HaaS100等



用鼠标选择意向的解决方案（本文以 helloworld demo 为例）

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01UyKaz91bYyBgPFSzL_!!6000000003478-2-tps-1060-576.png" style="max-width:800px;" />
</div>

选择解决方案，之后用鼠标选择HaaS100开发板

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01eq0SHQ1oskIBE9WuD_!!6000000005281-2-tps-1060-254.png" style="max-width:800px;" />
</div>

## 2.3 输入项目名称和路径

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01unfkTF1bFjYKNXYMp_!!6000000003436-2-tps-1064-186.png" style="max-width:800px;" />
</div>

工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01bcjwNW1QGECspTNCo_!!6000000001948-2-tps-1072-200.png" style="max-width:800px;" />
</div>

经过上述几步，VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png" style="max-width:800px;" />
</div>

# 3. 开发及调试
## 3.1 编译工程
点击状态栏的编译图标，稍等片刻（由于需要安装编译器，第一次编译耗时稍久）就能看到编译成功的提示。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01IGczJA23UfiDMdaO0_!!6000000007259-2-tps-1466-343.png" style="max-width:800px;" />
</div>

## 3.2 烧录镜像

- 如下图，用数据线把HaaS开发板和电脑连起来，同时断开HaaS开发板的电源。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01DA4GIL1cL3lxXK5Vv_!!6000000003583-2-tps-1012-747.png" style="max-width:800px;" />
</div>

- 启动烧录
   - 点击VS Code下方状态栏中的烧录图标，随后VS Code会显示电脑已识别到的串口的列表(Avaliable ports)
   - 根据HaaS100开发板的端口号，输入相对应的串口序号

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01ya7crN1dYd7kpi1t1_!!6000000003748-2-tps-1398-289.png" style="max-width:800px;" />
</div>

- 接上HaaS开发板的电源，稍等两分钟就可以看到VS Code提示“成功烧录”

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01Foj1sU1HIDNnvs6gM_!!6000000000734-2-tps-1402-196.png" style="max-width:800px;" />
</div>

## 3.3 查看日志

- 点击VS Code的Serial Monitor按钮, 选择对应串口号, 下图是windows系统的示例，如果是MacOS系统的话，串口一般为/dev/cu.SLAB_USBtoUART。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN010WZPcO1GczjjpWVp5_!!6000000000644-2-tps-2144-1508.png" style="max-width:800px;" />
</div>

- 设置波特率为1500000

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN017FqUBk1OA9iyd6bn7_!!6000000001664-2-tps-2466-652.png" style="max-width:800px;" />
</div>

- 串口日志输出

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01QGiV5M1pSTBde0A23_!!6000000005359-2-tps-2728-1038.png" style="max-width:800px;" />
</div>

# 4. 打开已有工程

第2章节中创建的工程关闭后，可以通过VS Code再次打开已有工程。
点击VS Code的欢迎页中的“最近”或英文版的“Recent”，选择相应的工程即可。
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01ml6cGW1RR3BO9xYMa_!!6000000002107-2-tps-1454-1070.png" style="max-width:800px;" />
</div>

