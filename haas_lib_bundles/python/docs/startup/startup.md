# HaaS Python快速开始

&emsp;&emsp;
HaaS Python是阿里云IoT HaaS团队推出的HaaS物联网设备云端一体开发框架的重要组成部分。

&emsp;&emsp;
HaaS Python的目标是帮助中小开发者聚焦业务，实现设备安全上云，加速设备创新迭代，真正做到“Python也可以轻松开发智能设备”。

## HaaS Python版本历史

|版本号|版本说明|固件下载链接|详细说明|
|:-:|-|-|:-:|
|v2.2<br>2022-05-30|HaaS Studio进阶版上线<br>新增6个传感器支持<br>新增15个创意案例<br>HaaS EDU K1支持彩色触摸屏<br>HaaS毕设计划启动<br>HaaS开源百校科技助力校园大使招募中<br>HaaS星案例征集火热进行中<br>HaaS合作伙伴计划全面启动|[HaaS EDU K1](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaSEDUK1-v2.2.0.zip)<br>[HaaS200](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaS200-v2.2.0.zip)<br><br>[乐鑫 ESP32_DevKitC](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.2.0.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.2.0.zip)<br>[01Studio pyWiFi-ESP32](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.2.0.zip)<br>[安信可 NodeMCU-ESP-S3-32S-Kit](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP-S3-12K-Kit-v2.2.0.zip)<br>[安信可 NodeMCU-ESP-C3-32S-Kit](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP-C3-32S-Kit-v2.2.0.zip)<br>[明栈 M5StackCore2](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-8M-v2.2.0.zip)<br>| <div align="center"> <img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaSPython_2.2_overview_1.png width=30%/></div> <br>[查看详情](https://haas.iot.aliyun.com/haasapi/index.html?#/Python/docs/zh-CN/startup/HaaSPython_2.2_detail)|
|v2.1<br>2022-04-30|HaaS Python标准升级到2.1<br>新增HaaS Python在线热更新功能<br>新增行为识别API<br>新增8款传感器（含工业类）<br>|[HaaS EDU K1](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaSEDUK1-v2.1.0.zip)<br>[HaaS200](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaS200-v2.1.0.zip)<br>[HaaS506](https://hli.aliyuncs.com/o/config/haas506/HaaS506_v2.01.zip)<br>[乐鑫 ESP32_DevKitC](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.1.0.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.1.0.zip)<br>[01Studio pyWiFi-ESP32](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v2.1.0.zip)<br>[安信可 NodeMCU-ESP-S3-32S-Kit]()<br>[安信可 NodeMCU-ESP-C3-32S-Kit]()<br>[明栈 M5StackCore2](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-8M-v2.1.0.zip)<br>|<div align="center"><img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaSPython_2.1_overview_1.png width=30%/></div><br>[查看详情](https://haas.iot.aliyun.com/haasapi/index.html?#/Python/docs/zh-CN/startup/HaaSPython_2.1_detail)|

<details>
<summary> 更多历史版本 </summary>

|版本号|版本说明|固件连接|详细说明|
|:-:|-|-|:-:|
|v2.0<br>2022-05-30|HaaS Python标准升级到2.0<br>（新增AI，UI等能力、AliyunIoT增加文件上传功能，<br>Driver SPI API增加多字节连续读写）<br>新增ESP32乐鑫官方开发板和01Studio ESP32开发板支持<br>新增30+传感器支持<br>新增30+创意案例<br>新增3个精美UI模板（带UI能力开发板）<br>新增9种AI聚合服务（带UI和AI能力开发板）|[HaaS EDU K1](https://hli.aliyuncs.com/o/config/HaaSPython/HaaSPython-HaaSEDUK1-v2.0.0.zip)<br>[HaaS200](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaS200-v2.1.0.zip)<br>[HaaS506](https://hli.aliyuncs.com/o/config/haas506/HaaS506_v2.01.zip)<br>[乐鑫 ESP32_DevKitC](https://hli.aliyuncs.com/o/config/HaaSPython/HaaSPython-ESP32-v2.0.0.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/o/config/HaaSPython/HaaSPython-ESP32-v2.0.0.zip)<br>[01Studio pyWiFi-ESP32](https://hli.aliyuncs.com/o/config/HaaSPython/HaaSPython-ESP32-v2.0.0.zip)<br>[明栈 M5StackCore2](https://hli.aliyuncs.com/o/config/HaaSPython/HaaSPython-ESP32-8M-v2.0.0.zip)<br>|<div align="center"><img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/HaaSPython_2.0_overview_1.png width=200%/></div><br>[查看详情](https://haas.iot.aliyun.com/haasapi/index.html?#/Python/docs/zh-CN/startup/HaaSPython_2.0_detail)|
|v1.0.2<br>2022-01-20|aliyunIoT库功能优化<br>新增开源HaaS EDU K1教育案例<br>新增5个创意案例|[HaaS EDU K1](https://hli.aliyuncs.com/p/config/HaaS_Python/HaaSPython-HaaSEDUK1-v1.0.2.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/p/config/HaaS_Python/HaaSPython-ESP32-v1.0.2.zip)|-|
|v1.0.1<br>2022-01-13|动态生成QSTR功能<br>UART增加on/any函数<br>aliyunIoT修复内存泄漏及postProp返回值问题<br>修复部分BUG|[HaaS EDU K1](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-HaaSEDUK1-1.0.1.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v1.0.1.zip)|-|
|v1.0.2<br>2022-01-20|aliyunIoT库功能优化<br>新增开源HaaS EDU K1教育案例<br>新增5个创意案例|[HaaS EDU K1](https://hli.aliyuncs.com/p/config/HaaS_Python/HaaSPython-HaaSEDUK1-v1.0.2.zip)<br>[安信可 NodeMCU-32S](https://hli.aliyuncs.com/p/config/HaaS_Python/HaaSPython-ESP32-v1.0.2.zip)|-|
|v1.0.0<br>2021-12-30|新增ESP32开发板NodeMCU-32支持<br>新增HaaS小程序，方便快速体验数据上云功能<br>升级HaaS Studio - 精简IDE开发流程，支持固件一键烧写<br>兼容MicroPython v1.17<br>扩展BLE配网、OTA和Modbus等功能<br>优化ESP32 IDF内存分配机制|[安信可 NodeMCU-32S](https://hli.aliyuncs.com/o/config/HaaS_Python/HaaSPython-ESP32-v1.0.1.zip)|-|
|v0.2.0<br>2021-06-15|兼容MicroPython v1.17<br>支持快速连接阿里云物联网云平台（aliyunIoT）<br>扩展Driver、KV、http等功能|不推荐使用|-|

</details>

&emsp;&emsp;
接下来看看如何<font size = 5, color=red>三步快速开启你的HaaS Python之旅</font>吧。

<div align="center">
<img src=../images/HaaSPython_快速开始_overview.png width=70%/>
</div>

## 一、选择你的开发板

&emsp;&emsp;
目前HaaS Python支持如下几种开发板，如果你手上还没有如下开发板，可以从下表按照你的需求进行开发板的选择。

|硬件系列|开发板介绍|开发板特点|开发板快速开始|
|-|-|-|-|
|HaaS EDU K1|[HaaS EDU K1](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5AB3B8A4A88800000001&dataId=800C5AB3B8A4A888)|蓝牙/Wi-Fi<br>内置OLED与多款传感器<br>物联网学习首选|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup)|
|HaaS506|[煜瑛 HaaS506](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CF06E76DC612F00000001&dataId=800CF06E76DC612F)|4G CAT.1|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/HaaS506_startup)|
|HaaS200|[翼数 HaaS200](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C8F815768381600000001&dataId=800C8F8157683816)|蓝牙/Wi-Fi|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/HaaS200_startup)|
|ESP32开发板|[乐鑫 ESP32_DevKitC](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C9562896F994200000001&dataId=800C9562896F9942)<br>[安信可 NodeMCU-32S](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB)<br>[01Studio pyWiFi-ESP32](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C55C67883087B00000001&dataId=800C55C67883087B)|蓝牙/Wi-Fi|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/ESP32_startup)|
|ESP32-S3开发板|[安信可 NodeMCU-ESP-S3-32S-Kit](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C860464C2A53800000001&dataId=800C860464C2A538)|本地AI能力<br>蓝牙/Wi-Fi<br>|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/ESP-S3-12K-Kit_startup)|
|ESP32-C3开发板|[安信可 NodeMCU-ESP-C3-32S-Kit](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CC0B0F588AC3100000001&dataId=800CC0B0F588AC31)|RISC-V <br> 蓝牙/Wi-Fi|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/ESP-C3-32S-Kit_startup)|
|M5StackCore2|[明栈 M5StackCore2](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A)|ESP32芯片<br>带触摸屏<br>支持摄像头与云端一体AI|[链接](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17074837.0.0.66e960b1YtzRuv#/Python/docs/zh-CN/startup/M5StackCore2_startup)|

## 二、安装HaaS-Studio开发工具

&emsp;&emsp;
下载 VS Code 安装包并进行安装，安装完 VS Code之后，请按照下图中数字的指示步骤完成haas-studio插件的安装。

<div align="center">
<img src=../images/1_安装haas_studio_插件.png width=80%/>
</div>

HaaS Studio安装完毕后，在“快速开始”可以看到其全部功能，如下图所示。

<div align="center">
<img src=../images/HaaS_Studio_fast_startup.png width=80%/>
</div>


</div>

## 三、运行Hello World程序
### 3.1 下载HaaS Python固件并烧录

&emsp;&emsp;
请通过本文前面”HaaS Python版本历史“章节中的“固件下载链接”下载适用于你手头开发板的固件，推荐使用最新版本的固件。

&emsp;&emsp;
固件下载完成后，通过HaaS Studio对固件进行烧录。下图是HaaS EDU K1烧录的过程。

<div align="center">
<img src=../images/HaaS_Studio_HaaSEDUK1_fw_download.png width=80%/>
</div>

> 不同开发板详细固件烧录流程请参考上文“开发板快速开始”链接中“固件烧录”章节的说明。

### 3.2 创建并运行第一个HaaS Python程序

&emsp;&emsp;
HaaS Python固件烧录完成后，在HaaS Studio的的“快速开始”页面点击“创建项目”后选择你的开发板中“Hello World”程序（下图是以HaaS EDU K1为例），点击“立即创建”即可创建一个本地工程。

<div align="center">
<img src=../images/HaaS_Studio_HaaSEDUK1_helloworld.png width=80%/>
</div>

&emsp;&emsp;
待工程创建完毕后点击“部署运行”按钮，将程序推送到开发板中即可体验Hello World程序。

> 不同开发板部署运行的详细流程请参考上文“开发板快速开始”链接中“Hello World例程”章节的说明。

&emsp;&emsp;
体验完Hello World例程后，建议你进入[HaaS Python创意案例专区](https://haas.iot.aliyun.com/solution)，找一个和你创意接近的案例或你感兴趣的案例，学学怎样用HaaS Python完成一个云端一体全链路物联网应用开发过程。


