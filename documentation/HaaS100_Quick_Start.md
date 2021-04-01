@page HaaS100_Quick_Start HaaS100快速开始

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

![image.png](https://img.alicdn.com/imgextra/i1/O1CN012VNoAn1UV7Y3T6t4o_!!6000000002522-2-tps-412-649.png)


## 1.2 配置VS Code
“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：
### 安装VS Code插件

- **alios-studio**

alios-studio是开发AliOS Things所需的核心插件，安装插件流程如下：

1）点击左侧活动栏的插件图标

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01f8ThZj1ZxaExr3UuC_!!6000000003261-2-tps-1088-526.png)

2）在搜索框中输入插件名称

![image.png](https://img.alicdn.com/imgextra/i4/O1CN01yNiCUt1w6DFUMcN3r_!!6000000006258-2-tps-1078-491.png)

3）搜索到alios-studio插件后点击安装(install)

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01scK59H1uwJ4APemk9_!!6000000006101-2-tps-1798-468.png)


- **aos-tools**

aos-tools主要于编译AliOS-Things代码和镜像烧录。alios-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：
![image.png](https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png)
等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。
**如果没有弹出安装aos-tools的提示请重启VS Code。**
已安装(INSTALLED)插件列表中会显示"alios-studio"和"C/C++"两个插件：
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01uSbuTA1Utqw2mAEEZ_!!6000000002576-2-tps-1348-802.png)

# 2. 创建工程
创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。
## 2.1 开始创建
点击左下角的"+"图标开始创建工程
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01xmJW0b25z1BVejS40_!!6000000007596-2-tps-796-275.png)

## 2.2 选择解决方案和开发板
随后VS Code会依次提示“请选择解决方案”和“请选择开发板”:

- 选择解决方案，即选择什么功能的示例程序，如helloworld demo, hal demo等
- 选择开发板，即选择在哪块开发板上运行这套示例程序，如HaaS100等



用鼠标选择意向的解决方案（本文以 helloworld demo为例）
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01UyKaz91bYyBgPFSzL_!!6000000003478-2-tps-1060-576.png)
选择解决方案，之后用鼠标选择HaaS100开发板
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01eq0SHQ1oskIBE9WuD_!!6000000005281-2-tps-1060-254.png)
## 2.3 输入项目名称和路径
![image.png](https://img.alicdn.com/imgextra/i3/O1CN01unfkTF1bFjYKNXYMp_!!6000000003436-2-tps-1064-186.png)
工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01bcjwNW1QGECspTNCo_!!6000000001948-2-tps-1072-200.png)
经过上述几步，VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png)

# 3. 开发及调试
## 3.1 编译工程
点击状态栏的编译图标，稍等片刻（由于需要安装编译器，第一次编译耗时稍久）就能看到编译成功的提示。
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01IGczJA23UfiDMdaO0_!!6000000007259-2-tps-1466-343.png)
## 3.2 烧录镜像

- 如下图，用数据线把HaaS开发板和电脑连起来，同时断开HaaS开发板的电源。

![image.png](https://img.alicdn.com/imgextra/i3/O1CN01DA4GIL1cL3lxXK5Vv_!!6000000003583-2-tps-1012-747.png)

- 启动烧录
   - 点击VS Code下方状态栏中的烧录图标，随后VS Code会显示电脑已识别到的串口的列表(Avaliable ports)
   - 根据HaaS100开发板的端口号，输入相对应的串口序号

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01ya7crN1dYd7kpi1t1_!!6000000003748-2-tps-1398-289.png)

- 接上HaaS开发板的电源，稍等两分钟就可以看到VS Code提示“成功烧录”

![image.png](https://img.alicdn.com/imgextra/i3/O1CN01Foj1sU1HIDNnvs6gM_!!6000000000734-2-tps-1402-196.png)
## 3.3 查看日志

- 点击VS Code的Serial Monitor按钮, 选择对应串口号。

<div align=left>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01aG5qNC1GvJZsy5419_!!6000000000684-2-tps-2376-1948.png" style="zoom:50%;" />
</div>

- 设置波特率为1500000

<div align=left>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN017FqUBk1OA9iyd6bn7_!!6000000001664-2-tps-2466-652.png" style="zoom:50%;" />
</div>

- 串口日志输出

<div align=left>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01QGiV5M1pSTBde0A23_!!6000000005359-2-tps-2728-1038.png" style="zoom:50%;" />
</div>

