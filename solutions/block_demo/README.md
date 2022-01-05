# 零代码编程开发文档



# 创建项目



首先[参考文档](https://g.alicdn.com/alios-things-3.3/doc/haas_setup_env.html?spm=a2cti.24227744.0.0.3aa77a20cBvrMg) 安装HaaS Studio 1.1.20或者更高版本。

打开HaaS 快速开始页面，点击“零代码编程”选项卡。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01icE49Q1exC1bx9Cyx_!!6000000003937-2-tps-2828-1754.png" style="zoom:50%;" />
</div>



点击“创建项目”按钮，在弹出的“创建工程向导”对话框中填写项目信息。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01xwGny223IlSI8vVm9_!!6000000007233-2-tps-1338-992.png" style="zoom:50%;" />
</div>

- 项目名字：给这个项目起个名字。

- 工作区路径：工作区路径即为项目文件存储路径(注：此目录名称必须为连续的英文字符)。

- 选择硬件类型：即选择在哪个HaaS物联网终端设备上运行这套示例程序，如HaaS EDUK1等。

- 选择解决方案：即选择示例程序，如gpio, led等。

  

点击“确认”按钮

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01PGuTle204cNMyWBwA_!!6000000006796-2-tps-1340-912.png" style="zoom:50%;" />
</div>



这时会下载零代码编程示例项目，下载完成后会打开这个项目所在文件夹，打开零代码编程工作区，自动导入这个项目。



<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN015zxBtW1IXcMHchDxI_!!6000000000903-2-tps-2814-1754.png" style="zoom:50%;" />
</div>



也可以通过侧边栏HaaS按钮，点击“零代码编程”标签来打开零代码编程的工作区。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01SJJvZX1ui6pdJeZhz_!!6000000006070-0-tps-2802-1746.jpg" style="zoom:50%;" />
</div>



# 零代码编程工作区介绍

最左侧的是编程模块区，这里有各种功能的编程模块，在这里选择编程模块将它拖拽到工作区进行编程。右下角是居中，放大，缩小按钮用来控制工作区显示情况。将模块拖入到右下角的垃圾桶图标上可以将模块从工作区中删除。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01I4COGz1mxfMIOpU1g_!!6000000005021-0-tps-2258-1502.jpg" style="zoom:50%;" />
</div>





在一个模块上点击鼠标右键，可以看到快捷操作菜单。

Duplicate ： 复制模块

Add Comment ：添加注释

Collapse Block ：折叠模块

Disable Block  ：使该模块不生效

Delete Block： 删除模块

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01eRCrnV1FLIrZeR3NI_!!6000000000470-2-tps-754-612.png" style="zoom:50%;" />
</div>





# 编译

点击底部菜单栏中的编译按钮，编译当前项目。可以在终端输出的结果检查编译是否通过。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01YWgbeB1EujtLdL9QS_!!6000000000412-0-tps-2808-272.jpg" style="zoom:50%;" />
</div>



# 烧录

HaaS物联网终端设备中需要提前烧录好对应的固件程序。对应的固件程序和烧录方式[参考这里](https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/index.html)HaaS物联网终端对应链接里的内容。

点击底部菜单栏中的编译按钮，会保存，编译并烧录当前项目。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01JvjBsV1icorxFJLlx_!!6000000004434-0-tps-2802-382.jpg" style="zoom:50%;" />
</div>

首次烧录时需要配置烧录参数：

选择本地更新。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01EEdRH01RKdgAwhvgN_!!6000000002093-2-tps-1212-226.png" style="zoom:50%;" />
</div>



选择串口号。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01cZQmBA1DlkVBpDvKQ_!!6000000000257-2-tps-1220-356.png" style="zoom:50%;" />
</div>

选择波特率。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN017mi5eG1DKj8HX8poR_!!6000000000198-2-tps-1226-404.png" style="zoom:50%;" />
</div>

开始烧录。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN019j8MMR1y950C5IElM_!!6000000006535-2-tps-2816-370.png" style="zoom:50%;" />
</div>



按下HaaS物联网终端设备上的复位按钮手动重启设备，让设备进入烧录模式。



烧录成功时终端中的日志信息。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN013p5K931r21agsfAQ5_!!6000000005572-2-tps-2258-1230.png" style="zoom:50%;" />
</div>



首次配置后，烧录配置信息会存储在项目的.vscode/upgrade.json文件中。以后烧录的时候会默认使用这里的配置信息，当由于串口号发生变化导致烧录失败时，请用右键删除这个烧录配置文件，重新配置。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01HVglsQ1IUPaabdHTL_!!6000000000896-2-tps-1424-654.png" style="zoom:50%;" />
</div>





# 串口

点击底部菜单栏中的串口按钮

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01SMkIRX1JmYtjFGMvO_!!6000000001071-0-tps-2800-64.jpg" style="zoom:50%;" />
</div>

选择串口号

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01wwXafx1V4qTyLIRoN_!!6000000002600-2-tps-1206-306.png" style="zoom:50%;" />
</div>

选择波特率

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01zngSzt1nxxDytv5vZ_!!6000000005157-2-tps-1202-356.png" style="zoom:50%;" />
</div>

打开串口终端

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01qhl7a21kLX9HjFwas_!!6000000004667-2-tps-1714-660.png" style="zoom:50%;" />
</div>



串口配置文件：

串口配置信息会存储在项目的.vscode/settings.json文件中。下载打开串口的时候会默认使用这里的配置信息，当串口号发生变化时，请删除下图中haasStudio.comPort和haasStudio.hardware.baudrate这2行串口配置信息，重新配置串口参数。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01MjWFT51JpJH47rqEb_!!6000000001077-2-tps-1684-680.png" style="zoom:50%;" />
</div>

# 清理

点击底部菜单栏中的清理按钮。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01fIvJrX1xtVXHsoFi3_!!6000000006501-0-tps-2802-66.jpg" style="zoom:50%;" />
</div>

清理成功的日志。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01ijM3gu1Dxenbl10uE_!!6000000000283-2-tps-1600-132.png" style="zoom:50%;" />
</div>



# 项目管理

在快速开始页面零代码编程页面中点击打开项目按钮，在弹出的对话框中可以选择之前创建的项目路径，打开新的项目。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01XfA2731HcMqtUilHY_!!6000000000778-0-tps-2820-1754.jpg" style="zoom:50%;" />
</div>



另外通过快速开始页面创建的项目路径会保存在最近的项目中，点击最近的项目路径可以直接打开最近的项目。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN011nbBTk1K7d7JzWcbq_!!6000000001117-0-tps-2820-1754.jpg" style="zoom:50%;" />
</div>

# 项目切换



可以在同一路径下创建多个不同名字的项目。



<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01xwGny223IlSI8vVm9_!!6000000007233-2-tps-1338-992.png" style="zoom:50%;" />
</div>

点击底部菜单栏项目名字可以切换不同的项目。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01lGIJ081fA16zwYNKJ_!!6000000003965-0-tps-2814-1740.jpg" style="zoom:50%;" />
</div>



选在不同的项目名字，会自动打开对应的零代码编程工作区。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01Tz8jbG1oHeFt93ZMG_!!6000000005200-2-tps-1302-218.png" style="zoom:50%;" />
</div>



