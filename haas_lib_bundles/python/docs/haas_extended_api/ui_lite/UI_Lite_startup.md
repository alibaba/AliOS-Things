# UI Lite快速开始

UI Lite for Python以LVGL 8.1为基础进行打造，最低只需要64k Flash和16k RAM即可运行。当前UI Lite for Python固件可运行在M5StackCore2开发板上。

本文将介绍如何快速完成UI版本的helloworld程序。在进行应用程序开发之前，请务必确保您的开发板已经烧录了HaaS Python for ESP32 8M的标准固件，详细操作步骤请参考[M5StackCore2快速开始](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/startup/M5StackCore2_startup)。

* 创建本地工程<br>

完成固件烧录并安装好HaaS-Studio开发环境之后，请基于helloworld新建一个基于helloworld的程序，命名为“HaaS_UI_Lite_helloworld”，如下图所示。

<div align="center">
<img src=../../images/HaaS_UI_Lite_helloworld.png width=50%/>
</div>

工程创建完成后，HaaS-Studio会创建一个main.py脚本，里边是通过串口打印helloworld的例子程序。
接下来我们只需要下面3步就可以在这个main.py中调用lvgl和display组件完成向屏幕上输出"Welcome to HaaS UI Lite"。

1. 对屏幕进行初始化

在调用lvgl提供的API之前，需要先对其进行初始化操作，代码如下：

```python
import display_driver            # 导入display_driver库，初始化display和lvgl
```

2. 创建屏幕对象和label

创建一个lvgl的对象，创建好对象之后便可以使用lvgl提供的各种控件功能组合设计出自己的UI。本案例只需要显示一个简单的字串到屏幕上，因此只需要建立一个最简单的label即可。请参考如下代码：

```python
scr = lv.obj()                  # 创建屏幕对象

label = lv.label(scr)           # 创建Label
```
3. 在Label上面显示字串

创建好label之后就可以调用label提供的API设置要显示的内容及其属性，代码如下：
```python

label.set_text("Welcome to HaaS UI Lite *_*")   # 设置要显示的文字

label.center()                  # 设置Label在屏幕中心

lv.scr_load(scr)                # 加载屏幕对象为当前屏幕
```
> 在设置好要显示的文字内容之后请务必调用src_load函数将内容传递给lvgl对象将画面内容渲染到屏幕上。

* 运行效果

上述代码编写完成并保存后，将此工程推送到您的开发板之后，脚本会自动运行，运行效果如下图所示：


<div align="center">
<img src=../../images/HaaS_UI_Light_helloworld_ui.png width=20%/>
</div>

至此，UI Lite版本的HelloWorld程序就完成了。

目前UI Lite覆盖了常用的33个控件，其例子程序请参考[UI Lite控件合集文章](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)

另外我们提供了温湿度面板、计时器、音乐播放器等模版，只需要几行代码就能使用这些功能，详情请参考如下趣味案例。
* [温湿度面板](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AEA703FFFF1A300000001&dataId=800AEA703FFFF1A3)
* [倒计时计时器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800A1DAF896F599700000001&dataId=800A1DAF896F5997)
* [音乐播放器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AE9E2A8B7DCDB00000001&dataId=800AE9E2A8B7DCDB)




