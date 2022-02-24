# 刷脸门禁系统
&emsp;&emsp;
刷脸门禁系统在家庭、公司、酒店等很多场景中均有广泛使用，无接触式刷脸考勤能有效防止代打卡等作弊行为，已成为疫情防控场景下企业考勤的首选方案。酒店住客刷脸自助办理入住，将现场授权获取的人脸照片，与身份证提取的照片进行1:1比对，确保住客身份的真实性的同时，减少了人工核查和服务成本的同时可向住客提供更便捷的入住体验。


## 背景知识
&emsp;&emsp;
人脸识别，是基于人的脸部特征信息进行身份识别的一种生物识别技术。用摄像机或摄像头采集含有人脸的图像或视频流，并自动在图像中检测和跟踪人脸，属于机器视觉中重要的一个应用，它包含了目标检测及人脸识别两个关键步骤。本章中使用阿里云视觉智能开放平台的人脸识别比对1:1功能来进行识别。

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
3. 新建人脸比对样本；
4. 推送脚本到M5Stack Core2并运行人脸比对；

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

5. **新建人脸比对样本**

&emsp;&emsp;
在刷脸门禁系统（人脸比对）案例中，需要将摄像头实时拍摄的照片和样本进行比较，以确认是否为同一个人。所以在这里需要上传一张人脸的照片用来做人脸比对的样本。
点击“人脸比对”，再点“新建样本”。
<div align="center">
<img src=./../../../images/AI_创建人脸比对样本.png width=80%/>
</div>
设备名称，请选择前面创建的设备；样本名称可自定义，特别需要注意的是，样本照片需要满足提示中的要求，比如图片大小不能超过3M.
<div align="center">
<img src=./../../../images/AI_上传样本.png width=80%/>
</div>
<br>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“face_recognization”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/2_4_人脸比对_工程创建.png width=100%/>
</div>

&emsp;&emsp;
将[人脸比对代码](./code/)文件下的所有脚本进行复制到“face_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。

> Python脚本的详细说明请参考脚本内嵌的文字注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改face_recognization工程里main.py中SSID和PWD的值为开发者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

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
在网络连接成功后，屏幕上将打印出IP地址和时间戳，时间显示当前北京时间，如果在网络连接成功后，显示的时间是(2000, 1, 1, 0, 1, 47, 5, 1)表示NTP同步不成功，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且在识别到人脸后会在屏幕上显示红色Recognize successfully!!!的字样。
```log
I (71364) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
wifi is connecting...
wifi is connecting...
wifi is connecting...
wifi is connecting...
wifi is connecting...
wifi is connected
IP: 192.168.43.168
NTP start
NTP done

error code: InvalidImage.NotFoundFace
error code: InvalidImage.NotFoundFace
error code: InvalidImage.NotFoundFace
error code: InvalidImage.NotFoundFace
{'h': 64.0, 'confidence': 73.23015, 'x': 64.0, 'y': 74.0, 'w': 131.0}
Recognize successfully!!!
Recognize successfully!!!
Recognize successfully!!!
```
