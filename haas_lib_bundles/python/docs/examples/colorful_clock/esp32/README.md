# 炫彩时钟表

## 1、简介
&emsp;&emsp;
当今社会到处彰显自己的个性，来代表我和别人是不同的，那今天咱们就来说说和咱们生活、工作、甚至生命都息息相关的东西——时间，我们获取时间的方式有手机、手表、pad、电脑、户外各种信息屏等。那今天HaaS给大家带来另外一台不一样的时钟设备——炫彩时钟表。

&emsp;&emsp;
本课程就手把手教开发者如何自制一款炫彩时钟表。

&emsp;&emsp;
本课程的理念：希望开发者学习完本课程以后，可以模仿本案例，打造出属于自己的炫彩DIY时钟表，可以摆放在你的办公桌、书桌等等的地方。

### 1.1、准备
&emsp;&emsp;
本案例打造需要如下硬件：
* [NodeMCU-32S](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB)一台<br>
* [WS2812]()<br>
* 杜邦线若干<br>
* Micro-USB数据线一条<br>

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/ESP32_colorful_clock_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知WS2812 RGB灯模块环可以通过I/O控制其RGB状态。

<br>

## 2、物联网平台开发
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png width=80%/>
</div>

开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 2.1、创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（如果创建的产品品类为标准品类，可以选择标准品类），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/colorful_clock_新建产品页面.png width=40%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“炫彩时钟表”的产品，如下图所示。

<div align="center">
<img src=./../../../images/colorful_clock_产品列表页.png width=80%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/colorful_clock_产品详情页面.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 2.3、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/colorful_clock_管理设备入口.png width=90%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/colorful_clock_添加设备入口.png width=90%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/colorful_clock_添加设备.png width=40%/>
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
<img src=./../../../images/colorful_clock_设备详细信息.png width=80%/>
</div>

<br>

#### 2.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### 2.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“开关表设备“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/colorful_clock_设备属性信息.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 3、设备端开发

### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“炫彩时钟表”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
其中main.py脚本的内容如下图所示：
```python
#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

import ws2812
import utime
from driver import GPIO

ws2812Dev = None
ws2812status = None

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

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False

    wlan.active(True)                  #激活界面
    wlan.scan()                        #扫描接入点
    #print("start to connect ", wifiSsid)
    wlan.connect(wifiSsid, wifiPassword)       # 连接到指定路由器名称和密码

    while True:
        wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:                     # Wi-Fi连接成功则退出while循环
            break
        else:
            utime.sleep(0.5)
            print("wifi_connected:", wifi_connected)

    ifconfig = wlan.ifconfig()                    #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    utime.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True

# ....

def ws2812_init():
    global ws2812Dev

    gpioObj=GPIO()
    gpioObj.open("ws2812")
    ws2812Dev = ws2812.WS2812(gpioObj,24)

def ws2812_close():
    ws2812Dev.close()

if __name__ == '__main__':
    ws2812_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    while True:
        timeArray = utime.localtime()
        if ws2812status == 1:
            ws2812Dev.timeShow(0,100,0,timeArray[3],timeArray[4],timeArray[5])
        utime.sleep(1)

```

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 初始化WS2812驱动
```python
##初始化ws2812驱动
def ws2812_init():
    global ws2812Dev

    gpioObj=GPIO()
    gpioObj.open("ws2812")
    ws2812Dev = ws2812.WS2812(gpioObj,24)
    print("ws2812 inited!")

##关闭ws2812
def ws2812_close():
    ws2812Dev.close()
```

* WS2812显示时钟
```python
if __name__ == '__main__':
##....
while True:
    timeArray = utime.localtime()
    if ws2812status == 1:
        ws2812Dev.timeShow(0,100,0,timeArray[3],timeArray[4],timeArray[5])
    utime.sleep(1)
```

* 云端控制WS2812开关
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global ws2812status
    payload = ujson.loads(request['params'])
    if "onoff" in payload.keys():
        value = payload["onff"]
        if value == 1:
            print("打开时钟")
            ws2812status = 1
        elif value == 0:
            print("关闭时钟")
            ws2812_close()
            ws2812status = 0
```

<br>

## 4、运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* ws2812 inited!”说明ws2812模块初始化完毕
* “打开时钟”说明炫彩时钟表被打开
* “关闭时钟”说明炫彩时钟表被关闭

```log
mount fs
 ==== python execute bootpy ====
enable OneMinuteOnCloud
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ws2812 inited!
打开时钟
关闭时钟

```

### 4.1、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/炫彩时钟物联网监控设备状态及属性.png width=80%/>
</div>

> 思考：本案例使用了HaaS传感器积木中的WS2812 24颗RGB灯环，实现一个简单漂亮的时钟，开发者可以通过增加HaaS传感器积木，让本案例更加丰富，比如增加数码管和温湿度传感器，增加年月日和当前空气的温湿度显示等等，欢迎留言交流～
