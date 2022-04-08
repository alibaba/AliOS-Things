# 智能刷卡门禁系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/3_智能刷卡门禁系统_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
我们每天进小区人行通道的时候，都会带一张卡或者一个小扣子，进小区以前往往要把小扣子靠近一下闸机，然后门才开，那这一套闸机系统是怎么实现的呢？
本章课程将一步步教会大家如何去学习并且打造一套DIY智能刷卡门禁系统。

&emsp;&emsp;
小区管理系统中会提前设置好哪些RFID卡是有效的，然后将这些RFID卡分发给小区的业主，但是如果拿着不是系统录入的卡，那么RFID卡是刷不开门禁系统的。

## 准备
&emsp;&emsp;
本案例打造需要如下硬件：
* ESP32一台
* mfrc522 RFID传感器模块一个
* 蜂鸣器一个
* Servo舵机一个
* 杜邦线若干
* RFID卡片若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/1_ESP32_rfid_opendoor_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知舵机的信号线线是连接PWM，RFID识别模块是SPI与MCU连接，蜂鸣器I/O与MCU的GPIO连接。

<br>

## 物联网平台开发
### 开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（如果创建的产品品类为标准品类，可以选择标准品类），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/1_opendoor_新建产品页面.png width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“智能刷卡机”的产品，如下图所示。

<div align="center">
<img src=./../../../images/1_opendoor_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/1_opendoor_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/1_opendoor_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_opendoor_添加设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/1_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/1_opendoor_设备详细信息.png width=100%/>
</div>

<br>

####  **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“人体温度“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/1_opendoor_设备详细信息.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

## 创建解决方案

&emsp;&emsp;
点击下图中的"快速开始"按键(<img src=./../../../images/1_HaaS_Studio_创建工程按钮.png width=8%/>)会弹出HaaS Studio的欢迎页面，请点击“创建项目”按钮。

&emsp;&emsp;
在随后弹框中，设定好项目名称（“rfid_open_door”）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/1_创建rfid_open_door工程_ESP32.png width=80%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖rfid_open_door工程目录下的原有文件。其中main.py脚本的内容如下图所示：
```python
#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import SPI,GPIO,PWM
import mfrc522
import utime
import servo

rfid_card  = 0
mfrc522Dev = 0
buzzerobj = 0
servoObj = 0

#########
# 物联网平台连接标志位
iot_connected = False
wlan = None

# 三元组信息
productKey = "产品密钥" #需要填入物联网云平台申请到的productKey信息
deviceName = "设备名称" #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "设备密钥" #需要填入物联网云平台申请到的deviceSecret信息

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"

...

if __name__ == '__main__' :
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    servo_init()
    init_buzzer()
    init_mfrc522()
    while True:
        print("\nHold a tag near the reader")
        rfid_card = mfrc522Dev.readCardID()
        buzzer_readcard()
        report_event()

```

&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改rfid_open_door工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改rfid_open_door工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

3. **修改设备端上报RFID卡号信息所用标识符**

&emsp;&emsp;
rfid_open_door工程里main.py中下面的代码实现的是上传RFID卡号到云端的功能。其中“rfid_card”为上报时所用的标识符。
```python
def report_event():
    global rfid_card

    upload_data = {'params': ujson.dumps({
        'rfid_card': str(rfid_card),
    })
    }
    # 上传端测的RFID卡号到物联网平台
    print(upload_data)
    device.postProps(upload_data)
```
&emsp;&emsp;
确保rfid_card标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/1_ESP32_opendoor_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 设备联网调用
```python
 ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
##########
```

* 舵机和RFID识卡模块的初始化
```python
def servo_init():
    global servoObj

    pwmObj = PWM()
    pwmObj.open("servo")
    data_r = {'freq':50, 'duty': 0}
    pwmObj.setOption(data_r)
    print("buzzer inited!")
    servoObj = servo.SERVO(pwmObj)

def init_mfrc522():
    global mfrc522Dev

    spiDev = SPI()
    spiDev.open("mfrc522")

    sda_gpio = GPIO()
    sda_gpio.open("mfrc522_sda")
    reset_gpio = GPIO()
    reset_gpio.open("mfrc522_rst")

    mfrc522Dev = mfrc522.MFRC522(spiDev, sda_gpio, reset_gpio)
    print("mfrc522 inited!")
```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* “Hold a tag near the reader”说明RFID识别模块准备好
* "{'params': '{"rfid_card": "979024648674"}'}"顺利读到RFID卡，并且把卡号“979024648674”上报云端

```log
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('172.16.2.152', '255.255.254.0', '172.16.2.1', '114.114.114.114')
establish tcp connection with server(host='xxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
buzzer inited!
mfrc522 inited!

Hold a tag near the reader
{'params': '{"rfid_card": "979024648674"}'}

Hold a tag near the reader

```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_opendoor_温湿度监控设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果设备端读取到RFID卡号，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 规则引擎

### 场景联动设定

&emsp;&emsp;
点击规则引擎->场景联动，点击创建规则。
<div align="center">
<img src=./../../../images/1_dd_规则创建.png width=100%/>
</div>

&emsp;&emsp;
进行规则设定，如下所示，云端识别到卡号“152915170457”，那么就发送一条开门命令。
<div align="center">
<img src=./../../../images/1_dd_规则参数设定.png width=100%/>
</div>

### 调试

&emsp;&emsp;
ESP32串口会周期性的打印如下日志。其中：
* "{'params': '{"rfid_card": "152915170457"}'}"顺利读到RFID卡，并且把卡号“152915170457”上报云端
* “90 0 打开门”，说明成功执行了一次开门，然后自动关门的动作

```log
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('172.16.2.152', '255.255.254.0', '172.16.2.1', '114.114.114.114')
establish tcp connection with server(host='xxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
buzzer inited!
mfrc522 inited!

Hold a tag near the reader
{'params': '{"rfid_card": "979024648674"}'}

Hold a tag near the reader
{'params': '{"rfid_card": "152915170457"}'}

Hold a tag near the reader
90
0
打开门

```
