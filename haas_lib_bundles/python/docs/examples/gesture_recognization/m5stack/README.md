# 手势识别系统
&emsp;&emsp;
手势识别在用户交互中越来越广泛的使用，比如通过手势进行隔空点赞，通过手势控制设备等，已成为一种较为基础的功能。


## 背景知识
&emsp;&emsp;
手势识别，用摄像机或摄像头采集含有手势的图像或视频流，并自动在图像中检测和跟踪手势，属于机器视觉中重要的一个应用。本章中使用阿里云视觉智能开放平台的静态手势识别功能来进行识别，目前支持的手势类型有：
* ok：确认
* palm：手掌
* left：握拳且大拇指向左
* right：握拳且大拇指向右
* good：点赞（即握拳且大拇指向上）
* mute：噤声（将食指放在嘴上即被识别为噤声）
* down：握拳且大拇指向下

## 准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    一根

涉及到的硬件购买链接如下，仅供参考，本文作者不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/COM_M5Stack硬件连线图.png width=90%/>
</div>
<br>

## 云端平台功能开通
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用 **对象存储OSS** 和 **视觉智能开发平台**。


&emsp;&emsp;
因此，整个步骤分为：
1. 注册阿里云账号并登录阿里云账号；
2. 在HaaS官网中的云端积木控制台创建设备；
3. 推送脚本到M5Stack Core2并运行手势识别；

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。

<br>

## HaaS云端积木平台

1. **登录HaaS官网**

&emsp;&emsp;
进入阿里云官网，[HaaS官网](https://haas.iot.aliyun.com/) 。
<div align="center">
<img src=./../../../images/AI_HaaS官网.png width=80%/>
</div>

<br>

2. **进入HaaS云端积木控制台**
&emsp;&emsp;  
如**上图**所示，点击右上角的“控制台”，进入HaaS云端积木的控制台页面。

<div align="center">
<img src=./../../../images/AI_HaaS控制台.png width=80%/>
</div>

<br>


3. **创建设备**

&emsp;&emsp;
在左侧导航栏设备管理中，可以看到产品和设备选项，点击“产品”，可以看到平台默认创建了“haas_正式游客产品”这个产品；
所以开发者只需要创建设备即可。点击“批量添加”， 添加方式为“自动生成”， 申请数量可填为“1”个，即可完成新设备的创建。
<div align="center">
<img src=./../../../images/AI_创建设备.png width=60%/>
</div>
<div align="center">
<img src=./../../../images/AI_创建设备_设备信息.png width=60%/>
</div>
<br>

4. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”,就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/AI_查看设备信息.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/AI_设备三元组.png width=80%/>
</div>
<br>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“gesture_recognization”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。(将下图的项目名字改为“gesture_recognization”)

<div align="center">
<img src=./../../../images/2_4_人脸比对_工程创建.png width=60%/>
</div>

&emsp;&emsp;
将[手势识别](./code/)文件下的所有脚本进行复制到“gesture_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。

> Python脚本的详细说明请参考脚本内嵌的文字注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改gesture_recognization工程里main.py中SSID和PWD的值为开发者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接开发者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
按照[获取设备的三元组]获取三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```


## 运行结果
### 本地查看

&emsp;&emsp;
在网络连接成功后，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且在识别到正确手势后会在屏幕上显示红色“Gesture: good”的字样(这里good表示“good：点赞（即握拳且大拇指向上）”的手势)。
<div align="center">
<img src=./../../../images/手势识别/静态手势识别.gif width=60%/>
</div>

```log

Wi-Fi is connecting...
Wi-Fi is connected
IP: 172.16.2.55
NTP start
NTP done
establish tcp connection with server(host='a1kJJvGN9ko.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected

upload--->{'qos': 1, 'payload': '{"version": "1.0", "id": 1, "params": {"ext": "{\\"fileName\\": \\"gesture.jpg\\", \\"fileId\\": \\"jrxlXxCYhEusz6sXJKSa010200\\", \\"filePosition\\": \\"lp\\"}", "eventType": "haas.faas", "eventName": "handGesture", "argInt": 1}}', 'topic': '/sys/a1kJJvGN9ko/3xG1QN7Q0wLxVHfCsZP5/thing/event/hli_event/post'}
get response time : 39
download <----{'msg_id': 1087807190, 'service_id': 'hli_async_service', 'params': '{"ext":"{\\"height\\":127,\\"result\\":\\"success\\",\\"score\\":0.9921875,\\"type\\":\\"good\\",\\"width\\":73,\\"x\\":146,\\"y\\":94}","commandName":"handGestureReply","commandType":"haas.faas","argInt":1}', 'code': 0, 'params_len': 189}
recognize hand gesture : good

```
