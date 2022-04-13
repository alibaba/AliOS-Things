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



## 1.2 配置VS Code
“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：
### 安装VS Code插件

- **alios-studio**

alios-studio是开发AliOS Things所需的核心插件，安装插件流程如下：

1）点击左侧活动栏的插件图标

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01f8ThZj1ZxaExr3UuC_!!6000000003261-2-tps-1088-526.png#align=left&display=inline&height=263&id=YtB1l&margin=%5Bobject%20Object%5D&name=image.png&originHeight=526&originWidth=1089&size=61292&status=done&style=none&width=544.5)

2）在搜索框中输入插件名称

![image.png](https://img.alicdn.com/imgextra/i4/O1CN01yNiCUt1w6DFUMcN3r_!!6000000006258-2-tps-1078-491.png#align=left&display=inline&height=246&id=nWDAD&margin=%5Bobject%20Object%5D&name=image.png&originHeight=491&originWidth=1079&size=79565&status=done&style=none&width=539.5)

3）搜索到alios-studio插件后点击安装(install)

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01scK59H1uwJ4APemk9_!!6000000006101-2-tps-1798-468.png#align=left&display=inline&height=246&id=r0TkG&margin=%5Bobject%20Object%5D&name=image.png&originHeight=479&originWidth=818&size=67644&status=done&style=none&width=420)


- **aos-tools**

aos-tools主要于编译AliOS-Things代码和镜像烧录。alios-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：
![image.png](https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png#align=left&display=inline&height=161&id=bgg9o&margin=%5Bobject%20Object%5D&name=image.png&originHeight=321&originWidth=785&size=22589&status=done&style=none&width=392.5)
等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。已安装(INSTALLED)插件列表中会显示"alios-studio"和"C/C++"两个插件：
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01uSbuTA1Utqw2mAEEZ_!!6000000002576-2-tps-1348-802.png#align=left&display=inline&height=401&id=7JDEN&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1015&originWidth=1707&size=303706&status=done&style=none&width=674)

# 2. 创建工程
创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。
## 2.1 开始创建
点击左下角的"+"图标开始创建工程
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01xmJW0b25z1BVejS40_!!6000000007596-2-tps-796-275.png#align=left&display=inline&height=138&id=KS37Y&margin=%5Bobject%20Object%5D&name=image.png&originHeight=275&originWidth=796&size=35109&status=done&style=none&width=398)

然后设置内测参数，包括ssh域账号和代码分支，域账号为申请的账号名，分支名为dev_aos
![image.png](https://img.alicdn.com/imgextra/i4/O1CN01tzEYEI1XLuQddHMUK_!!6000000002908-2-tps-1200-120.png#align=left&display=inline&height=138&id=KS37Y&margin=%5Bobject%20Object%5D&name=image.png&originHeight=275&originWidth=796&size=35109&status=done&style=none&width=398)
![image.png](https://img.alicdn.com/imgextra/i3/O1CN01OsuGtp1NZ3dsIzHwM_!!6000000001583-2-tps-1194-166.png#align=left&display=inline&height=138&id=KS37Y&margin=%5Bobject%20Object%5D&name=image.png&originHeight=275&originWidth=796&size=35109&status=done&style=none&width=398)

## 2.2 选择解决方案和开发板
随后VS Code会依次提示“请选择解决方案”和“请选择开发板”:

- 选择解决方案，即选择什么功能的示例程序，如helloworld demo, hal demo等
- 选择开发板，即选择在哪块开发板上运行这套示例程序，如HaaS100等



用鼠标选择意向的解决方案（本文以 helloworld demo为例）
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01UyKaz91bYyBgPFSzL_!!6000000003478-2-tps-1060-576.png#align=left&display=inline&height=288&margin=%5Bobject%20Object%5D&name=image.png&originHeight=576&originWidth=1060&size=174002&status=done&style=none&width=530)
选择解决方案，之后用鼠标选择HaaS100开发板
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01eq0SHQ1oskIBE9WuD_!!6000000005281-2-tps-1060-254.png#align=left&display=inline&height=127&margin=%5Bobject%20Object%5D&name=image.png&originHeight=254&originWidth=1060&size=95392&status=done&style=none&width=530)
## 2.3 输入项目名称和路径
![image.png](https://img.alicdn.com/imgextra/i3/O1CN01unfkTF1bFjYKNXYMp_!!6000000003436-2-tps-1064-186.png#align=left&display=inline&height=93&margin=%5Bobject%20Object%5D&name=image.png&originHeight=186&originWidth=1064&size=68106&status=done&style=none&width=532)
工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01bcjwNW1QGECspTNCo_!!6000000001948-2-tps-1072-200.png#align=left&display=inline&height=100&margin=%5Bobject%20Object%5D&name=image.png&originHeight=200&originWidth=1072&size=60957&status=done&style=none&width=536)
经过上述几步，VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：
![image.png](https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png#align=left&display=inline&height=211&id=wCzUL&margin=%5Bobject%20Object%5D&name=image.png&originHeight=422&originWidth=689&size=34659&status=done&style=none&width=344.5)

# 3. 开发及调试
## 3.1 编译工程
点击状态栏的编译图标，稍等片刻（由于需要安装编译器，第一次编译耗时稍久）就能看到编译成功的提示。
![image.png](https://img.alicdn.com/imgextra/i2/O1CN01IGczJA23UfiDMdaO0_!!6000000007259-2-tps-1466-343.png#align=left&display=inline&height=189&id=Ru3vI&margin=%5Bobject%20Object%5D&name=image.png&originHeight=377&originWidth=1611&size=73726&status=done&style=none&width=805.5)
## 3.2 烧录镜像

- 如下图，用数据线把HaaS开发板和电脑连起来，同时断开HaaS开发板的电源。

![image.png](https://img.alicdn.com/imgextra/i3/O1CN01DA4GIL1cL3lxXK5Vv_!!6000000003583-2-tps-1012-747.png#align=left&display=inline&height=374&id=pX0el&margin=%5Bobject%20Object%5D&name=image.png&originHeight=945&originWidth=1280&size=1583661&status=done&style=none&width=506)

- 启动烧录
   - 点击VS Code下方状态栏中的烧录图标，随后VS Code会显示电脑已识别到的串口的列表(Avaliable ports)
   - 根据HaaS100开发板的端口号，输入相对应的串口序号

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01ya7crN1dYd7kpi1t1_!!6000000003748-2-tps-1398-289.png#align=left&display=inline&height=145&id=zl0Jd&margin=%5Bobject%20Object%5D&name=image.png&originHeight=365&originWidth=1764&size=81828&status=done&style=none&width=699)

- 接上HaaS开发板的电源，稍等两分钟就可以看到VS Code提示“成功烧录”

![image.png](https://img.alicdn.com/imgextra/i3/O1CN01Foj1sU1HIDNnvs6gM_!!6000000000734-2-tps-1402-196.png#align=left&display=inline&height=98&id=FUsoz&margin=%5Bobject%20Object%5D&name=image.png&originHeight=219&originWidth=1563&size=35567&status=done&style=none&width=701)
## 3.3 查看日志

- 点击VS Code的monitor按钮, 配置串口端口号，并配置波特率至1500000，点击HaaS开发板Reset按键后，就可以在串口工具中看到日志打印了。

![image.png](https://img.alicdn.com/imgextra/i1/O1CN01WFJrEO2AI25lZacbI_!!6000000008179-2-tps-1066-374.png#align=left&display=inline&height=337&id=pUJpU&margin=%5Bobject%20Object%5D&name=image.png&originHeight=674&originWidth=672&size=60690&status=done&style=none&width=336)

