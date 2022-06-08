# 超声测距仪

本案例通过一个超声测距传感器测量距离，测量值显示在液晶屏幕上，同时上送到云端。本案例是手持式超声测距仪的原型。

## 简介
## 1.1、背景知识

由于超声波指向性强，能量消耗缓慢，在介质中传播的距离较远，因而超声波经常用于距离的测量，如测距仪和物位测量仪等都可以通过超声波来实现。利用超声波检测往往比较迅速、方便、计算简单、易于做到实时控制，并且在测量精度方面能达到工业实用的要求，因此在移动机器人研制上也得到了广泛的应用。
<br>

### 1.2、准备

本案例需要准备如下硬件：
1. M5Stack Core2开发板一套<br>
2. URM37超声测距传感器一个<br>
3. 母对母杜邦连接线若干<br>

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！
<br>


| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发板 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| URM37超声测距传感器 | 1 | [URM37超声测距传感器](https://wiki.dfrobot.com.cn/_SKU_SEN0001_URM37V5.0%E8%B6%85%E5%A3%B0%E6%B3%A2%E6%B5%8B%E8%B7%9D%E4%BC%A0%E6%84%9F%E5%99%A8) |
| 母对母杜邦线 | 4 | [母对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh+K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440213) 请选用10cm长即可 |

硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/ultrasonic_ranging/hardware_connection.png width=100%/>
</div>
<br>

## 2、物联网平台开发

对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_物联网平台.png width=90%/>
</div>

开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 2.1、创建云端产品

点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/ultrasonic_ranging/create.png
 width=100%/>
</div>

选择之后，点击“确认”按钮，即可完成产品创建。如下图所示
<div align="center">
<img src=./../../../images/ultrasonic_ranging/create_1.png width=100%/>
</div>

### 2.3、创建产品属性（物模型）

按照如下步骤定义物模型。完成后点击“发布上线”。
<div align="center">
<img src=./../../../images/ultrasonic_ranging/product.png width=100%/>
</div>
<br>
<div align="center">
<img src=./../../../images/ultrasonic_ranging/product_1.png width=100%/>
</div>
<br>
<div align="center">
<img src=./../../../images/ultrasonic_ranging/product_2.png width=75%/>
</div>

### 2.3、创建云端设备（获取三元组）

按照如下步骤添加设备。
<div align="center">
<img src=./../../../images/ultrasonic_ranging/device.png width=100%/>
</div>
<br>
<div align="center">
<img src=./../../../images/ultrasonic_ranging/device_1.png width=75%/>
</div>

**获取设备三元组**

如下图所示，在设备列表中点击“查看”按钮，就可以看到设备的三元组信息。

设备三元组是指产品Key(productKey)、设备名称(DeviceName)、设备秘钥(DeviceSecret)。

三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。设备三元组在后面设备端开发时会用到，请注意保存。
<div align="center">
<img src=./../../../images/ultrasonic_ranging/triple.png width=100%/>
</div>
<br>

## 3、设备端开发

### 3.1、开发环境

在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 3.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“超声测距仪”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

### 3.3、修改路由器名称及密码

修改此工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

### 3.4、修改设备的三元组信息

请参考“获取设备的三元组步骤”，把获取的三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

### 4、运行结果

#### 4.2、本地运行效果查看
液晶屏幕上实时显示距离测量值，单位为厘米。
<div align="center">
<img src=./../../../images/ultrasonic_ranging/display.png width=50%/>
</div>

#### 4.2、云端查看

进入阿里云官网，用阿里云账号[登录物联网平台](https://iot.console.aliyun.com/devices/)查看状态

<div align="center">
<img src=./../../../images/ultrasonic_ranging/result.png width=100%/>
</div>

<br>

到此为止，超声测距仪案例就已经完成了。如果想学习更多实验，请参考[HaaS案例实践详解](https://gitee.com/haasedu/haasedu/tree/release_2.0)。

## 5、后记

本案例中使用的URM37超声波测距传感器还拥有舵机控制和温度采集功能，后续可尝试把这些功能加入案例中。