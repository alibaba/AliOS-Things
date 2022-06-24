# HaaS UI Lite案例合集
&emsp;&emsp;
为了让用户更容易上手HaaS UI Lite开发，更便捷的上云用云，我们准备了HaaS UI Lite案例合集供大家参考使用。本案例无需硬件连线，体验功能包含开机连网向导、UI基础组件演示，传感器UI模版演示以及HaaS云端AI能力演示等。

<div align="center">
<img src=./../../images/ui_collection/操作步骤.png width=100%/>
</div>
<br>

## 1、简介
&emsp;&emsp;
通过UI界面和用户交互已经成为各种设备的首选，界面上不但可以展示各种信息，并且用户根据屏幕上提示进行点击就能进行各种操作。为了让用户更容易上手HaaS UI Lite开发，更便捷的上云用云，我们准备了HaaS UI Lite案例合集供大家参考使用。本案例包含了开机连网向导、系统设置、UI基础组件示例，传感器UI模版以及HaaS云端一体案例等示例。

<br>

### 1.1、准备
&emsp;&emsp;
本案例体验需要使用到如下硬件：
* [M5StackCore2开发板](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A)      一台
* 或者
* [HaaSEDUK1开发板](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5AB3B8A4A88800000001&dataId=800C5AB3B8A4A888) + [配套LCD屏](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.59cd2e8drEjhUO&id=649091087858&_u=aofbqa5070)     一台

<br> 
<div align="center">
<img src=./../../images/ui_collection/硬件外观.png width=100%/>
</div>
<br>

## 2、设备端开发

<br>

### 2.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2或者HaaSEDUK1开发环境已经搭建完毕，详情请参考[M5StackCore2快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16834026.0.0.7d8560b1BNIQMo#/Python/docs/zh-CN/startup/M5StackCore2_startup)或者[HaaSEDUK1快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16834026.0.0.7d8560b1BNIQMo#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup)中的说明。

<br>

### 2.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“HaaS UI Lite案例合集”，点击“立即创建”即可。

<div align="center">
<img src=./../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
工程源码如下图所示：
<div align="center">
<img src=./../../images/ui_collection/工程源码界面.png width=100%/>
</div>

<br>

### 2.3、推送脚本到硬件

&emsp;&emsp;
用户可以选择通过本地串口，也可以通过在线推送（OTA）方式推送脚本到硬件执行。

* 本地串口推送
<br> 
&emsp;&emsp;
点击HaaS-Studio的“部署运行”按钮（<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_部署运行.png width=5%/>），HaaS Studio工具上面会弹出如下的选择框，请按照如下的步骤逐步选择完成后，HaaS-Studio开始推出送固件。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/1_HaaS_Studio_Python_本地推送脚本.png width=60%/>
</div>

&emsp;&emsp;
脚本推送完成后，VS Code的命令行窗口会有如下提示：
```
upload success
```

<br>

* 在线推送

&emsp;&emsp;
在线推送脚本到设备执行，请参考文档[《HaaS python在线更新》](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1Zu2Epa#/Python/docs/zh-CN/core_function/HaaS_Python_app_ota)

<br>

### 2.4、运行效果
&emsp;&emsp;
将工程推送到开发板之后，脚本会自动运行。屏幕上面显示如下信息：

<div align="center">
<img src=./../../images/ui_collection/首屏页面.png width=35%/>
</div>

<br>

## 3、HaaS UI Lite案例集合说明
&emsp;&emsp;
本案例包含开机连网向导、系统设置、UI基础组件示例，传感器UI模版以及HaaS云端一体案例等示例。

<br>

### 3.1、开机连网向导
&emsp;&emsp;
开机联网向导包含搜索Wi-Fi、输入Wi-Fi密码、连接Wi-Fi等引导功能。

<div align="center">
<img src=./../../images/ui_collection/开机联网向导.png width=100%/>
</div>

<br>

### 3.2、系统设置
&emsp;&emsp;
系统设置包含打开/关闭Wi-Fi、打开关闭蓝牙、设置屏幕亮度以及查询系统软件信息等UI演示。

<div align="center">
<img src=./../../images/ui_collection/设置.png width=35%/>
</div>

<br>

### 3.3、UI基础组件示例
&emsp;&emsp;
UI基础组件示例包含HaaS UI Lite基础控件在硬件上的演示集合，目前包含Arc, Button, Image, Bar, Checkbox等21个基础组件的演示。更多UI基础组件演示还在准备中。UI基础组件文档和示例源码可以查询官网文档[《UI Lite控件集合》](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1Zu2Epa#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)。

<div align="center">
<img src=./../../images/ui_collection/uiBasics.png width=100%/>
</div>

<br>

### 3.4、传感器UI模版
&emsp;&emsp;
嵌入式智能硬件一般都集成有丰富的外设传感器，为了方便用户基于HaaS UI Lite快速在屏幕上动态展示传感器数据，我们一期提供了12个常见传感器UI模版，例如温度、湿度、燃气检测、心率、水位监测、舵机角度控制、气压检测和超声波测距等。更多传感器UI模版还在设计中。

<div align="center">
<img src=./../../images/ui_collection/传感器UI模版.png width=100%/>
</div>

<br>

### 3.5、HaaS云端一体案例
&emsp;&emsp;
HaaS硬件联网成功之后，用户无需额外任何操作即可快速体验HaaS云端一体案例。一期我们为用户准备了云端AI的“车牌识别”能力体验，用户可以替换项目工程下的"./images/plate.jpg"图片为自己的其他车牌照片用于体验。

<div align="center">
<img src=./../../images/ui_collection/车牌识别.png width=100%/>
</div>