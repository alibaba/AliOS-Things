# 水果识别系统
&emsp;&emsp;
现在很多农场里边使用摘采机器人识别水果进行水果摘采，盒马超市也使用自动识别称来识别水果种类自动计费。

## 背景知识
&emsp;&emsp;
水果的种类繁多，识别种类越多，需要越多的水果图片数据进行模型训练。本章通过使用串口摄像头采集水果照片上传至云端进行识别。目前HaaS云端积木的水果检测识别功能可以检测图像中的所有水果，并返回水果的名称和置信度。水果识别可以识别60种常见的水果和16种坚果。比如：八月瓜、百香果、菠萝、菠萝蜜、草莓、橙子、脆瓜、甘蔗、桂圆、哈蜜瓜、海底椰、海红果、红毛丹、火龙果、桔子、蓝莓、梨、荔枝、李子、榴莲、蔓越莓、芒果、梅子、猕猴桃、木瓜、柠檬、牛油果、欧李、枇杷、苹果、葡萄、茄瓜、桑葚、沙果、沙棘果、山楂、山竹、蛇皮果、石榴、柿子、树莓、桃、桃胶、西瓜、西梅、仙人掌果、香瓜、香蕉、橡子、小番茄、杏子、雪莲果、杨梅、洋蒲桃、杨桃、椰子、银杏果、樱桃、柚子、枣、南瓜子、夏威夷果、巴旦木、开心果、杏仁、松子、板栗、核桃、榛子、白果、碧根果、腰果、花生、葡萄干、葵花子、西瓜子等。

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
3. HaaS云端积木平台创建设备
4. 设备端开发
5. 设备运行

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
3. 推送脚本到M5Stack Core2并运行水果识别；

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
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“fruits_recognization”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/3_2_水果识别_工程创建.png width=100%/>
</div>


&emsp;&emsp;
将[水果识别代码](./code/)文件下的所有脚本进行复制到“fruits_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改fruits_recognization工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

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
在网络连接成功后，屏幕上将打印出IP地址和时间戳，时间显示当前北京时间，如果在网络连接成功后，显示的时间是(2000, 1, 1, 0, 1, 47, 5, 1)表示NTP同步不成功，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且识别到水果后在屏幕上显示红色XXXX Detected的字样。
```log
I (71364) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.3.26
NTP start
NTP done
establish tcp connection with server(host='a1kJJvGN9ko.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
capture time : 214
image time : 151
{'ext': '{"fruitList":[{"box":[117.39963,37.268333,267.16675,202.39557],"name":"\u82f9\u679c","score":0.78409535}],"result":"success"}', 'commandName': 'detectFruitsReply', 'commandType': 'haas.faas', 'argInt': 1}
detect: Apple
get response time : 413
Fruits Detected!!!
```
