# 物体识别系统
&emsp;&emsp;
本案例是确定要识别的物体区域后，通过模拟按键通知摄像头抓拍区域图片并上传至云端，云端通过HaaS增值服务对抓拍的图片进行识别，最后给出图片中的都有哪些物品，同时给出识别物品名称和可信度，通常使用在装修设计、图片检查上辅助操作者确定物品是否齐全。


<div align="center">
<img src=./../../../images/detect_object/物体识别_步骤指引.png width=90%/>
</div>

## 背景知识

&emsp;&emsp;
HaaS增值服务：集多种人工智能算法为一体的云端服务，通过设备端上传图片到云端，云端对图片进行识别，并把结果返回给设备的服务，目前已涵盖图片识别、文字识别、物品识别、人型识别等多个类别。


## 场景功能拆解

```python
实验整体步骤如下：
1. 连接硬件
2. HaaS云端积木平台创建设备
3. 设备端开发和运行
4. 处理云端识别结果
```
## 准备
```python
1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    一根
4. 公对母杜邦线               若干
```
涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |
| 公对母杜邦线 | 若干 | [公对母杜邦线](https://detail.tmall.com/item.htm?spm=a230r.1.14.22.b10c6663PCvNOq&id=41254478179&ns=1&abbucket=7&skuId=3211140814458) 请选用10cm长即可 |


&emsp;&emsp;
连线示意图如下图所示：
<div align="center">
<img src=./../../../images/detect_object/物体识别_实物连线.png width=30%/>
</div>

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/detect_object/物体识别_设备连线.png width=90%/>
</div>


<br>

## 云端平台功能开通
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用 **对象存储OSS** 和 **视觉智能开发平台**。因此，整个云端步骤分为下面两步。
```python
1. 注册阿里云账号并登录阿里云账号；
2. 登录HaaS云端积木控制台创建设备；
```

## 注册阿里云账号

&emsp;&emsp;
进入阿里云官网，如已有阿里云账号可跳过这步，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。
<br>

## HaaS云端积木平台创建设备

1. **登录HaaS云端积木平台**

&emsp;&emsp;
进入阿里云官网，用阿里云账号[登录HaaS云端组件控制台](https://haas.iot.aliyun.com/welcome) 。
<div align="center">
<img src=./../../../images/detect_object/AI_HaaS官网.png width=90%/>
</div>
<br>

2. **创建设备**

&emsp;&emsp;
在左侧导航栏设备管理中，可以看到产品和设备选项，点击“产品”，可以看到平台默认创建了“haas_正式游客产品”这个产品；
所以开发者只需要创建设备即可。点击“批量添加”， 添加方式为“自动生成”， 申请数量可填为“1”个，即可完成新设备的创建。
<div align="center">
<img src=./../../../images/detect_object/AI_创建设备.png width=60%/>
</div>
<div align="center">
<img src=./../../../images/detect_object/AI_创建设备_设备信息.png width=60%/>
</div>
<br>

3. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”，就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/detect_object/AI_查看设备信息.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/detect_object/AI_设备三元组.png width=80%/>
</div>
<br>

&emsp;&emsp;
请复制该处的三元组信息，后面设备端开发需要使用。


## 设备端开发

1. **开发环境**

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[M5Stack Core2开发环境](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1exaiyq#/Python/docs/zh-CN/startup/M5StackCore2_startup)说明。
<br>

2. **创建解决方案**

&emsp;&emsp;
如下图所示，打开VSCode后，新建一个基于helloworld的python工程，设定好工程名称（“detect_object”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/detect_object/物体识别_创建工程.png width=90%/>
</div>


3. **代码准备**

&emsp;&emsp;
将[物体识别识别代码](./code/)文件下的所有脚本进行复制到“detect_object”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


3.1. **修改路由器名称及密码**

&emsp;&emsp;
修改detect_object工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

3.2. **修改设备的三元组信息**

&emsp;&emsp;
按照[获取设备的三元组]获取三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```
3.3. **运行结果**

&emsp;&emsp;
推送此脚本到M5Stack之后，在网络连接成功后，屏幕上将打印出IP地址和"NTP Done"。如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
模拟按钮动作，可通过公对公杜邦线模拟，先把一头插入GPIO27，然后把另一头插入GND后，马上拔出，即启动抓拍并上传云端，识别成功后串口会有提示。

&emsp;&emsp;
物体识别识别成功后，在屏幕上显示红色Object Detected的字样。

&emsp;&emsp;
串口会周期性的打印如下日志。
```log
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.xxx.xxx
NTP start
NTP done
establish tcp connection with server(host='${Your-ProductKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com',port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp,fd=54
link platform connected
启动按钮监控线程
按下拍照按钮
[  24.210]<E>AIOT_UPLOAD MQTT Upload file(detectObject.jpg) ID(HPmY8VNEXj2K6skS3t3S010200) success
{'ext': '{"elements":[{"boxes":[33,134,111,238],"score":0.3,"type":"potted plant"},{"boxes":[33,134,111,238],"score":0.208,"type":"flower"}]"result":"success"}','commandName': 'detectObjectReply','commandType': 'haas.faas','argInt': 1}
detect: potted plant
识别结束
get response time : 1234
按下拍照按钮
[  27.100]<E>AIOT_UPLOAD MQTT Upload file(detectObject.jpg) ID(OLnlY33oqKZZgf7yu6gX010200) success
get response time : 458
{'ext': '{"elements":[{"boxes":[48,86,178,111],"score":0.378,"type":"keyboard"}],"result":"success"}','commandName': 'detectObjectReply','commandType': 'haas.faas','argInt': 1}
detect: keyboard
识别结束
```

<div align="center">
<img src=./../../../images/detect_object/物体识别_识别结果.png width=90%/>
</div>

## 后记

&emsp;&emsp;
物体识别的精准度跟摄像头强相关，后续会考虑优化抓拍图片清晰度来提高识别精准度。