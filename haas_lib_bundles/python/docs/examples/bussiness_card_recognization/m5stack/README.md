# 名片识别
&emsp;&emsp;
名片识别是生活中一种非常常见的场景，将纸质名片转为电子版本，存放在电子设备里面。

## 背景知识
&emsp;&emsp;
名片识别也是OCR识别领域中的一种，通过深度学习训练出能够识别名片的模型，有的系统使用边缘设备进行识别，有的使用云端进行识别。本章中使用HaaS云端积木中名片识别功能进行识别，在识别到正常名片后显示在屏幕上。

<br>

## 准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    一根

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

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
3. 推送脚本到M5Stack Core2并运行名片识别；

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
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“bussiness_card_recognization”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。(将下图的项目名字改为“bussiness_card_recognization”)

<div align="center">
<img src=./../../../images/3_2_车牌识别_工程创建.png width=80%/>
</div>


&emsp;&emsp;
将[名片识别代码](./code/)文件下的所有脚本进行复制到“bussiness_card_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改license_plate_recognization工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

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
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，日志中有识别到的详细信息，并且在屏幕上显示红色name，num和email的字样, 未识别到的信息会默认为unknown， 由于目前还不支持中文显示，所以name显示为空。

<div align="center">
<img src=./../../../images/名片识别/名片识别结果显示.png width=60%/>
</div>

```log
Wi-Fi is connected
IP: 192.168.3.25
NTP start
NTP done
establish tcp connection with server(host='a1kJJvGN9ko.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected

get response time : 58
{'msg_id': 629991413, 'service_id': 'hli_async_service', 'params': '{"ext":"{\\"addresses\\":[],\\"cellPhoneNumbers\\":[\\"Tel:18912345678\\"],\\"companies\\":[],\\"departments\\":[],\\"emails\\":[\\"wwang128@126.com\\"],\\"name\\":\\"\u738b\u4e8c\u5c0f\\",\\"officePhoneNumbers\\":[],\\"result\\":\\"success\\",\\"titles\\":[]}","commandName":"recognizeBusinessCardReply","commandType":"haas.faas","argInt":1}', 'code': 0, 'params_len': 308}
name :王二小
phoneNumbers :
['Tel:18912345678']
email_list:
['wwang128@126.com']
```
