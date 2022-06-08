# 智能水族箱

## 1、简介

### 1.1、背景
&emsp;&emsp;
大多数人家里的阳台上都会养一些绿色的植物，用来美化家，来“养眼”，但是很多人往往养的植物都活不长，因为经常会忘记给植物浇水，植物就长不好甚至会因此而“渴死”。那我们这套HaaS自动浇水养植系统，就可以来解决因你忘记浇水而导致植物死亡的问题。

&emsp;&emsp;
本课程就手把手教开发者如何自制一款自动浇水养植系统。

&emsp;&emsp;
本课程的理念：希望开发者学习完本课程以后，可以模仿本案例，打造出属于自己的自动浇水养植系统，放在家中的植物上，检测植物土壤的湿度，进行实时的控制其湿度。

### 1.2、准备
&emsp;&emsp;
本案例打造需要如下硬件：
* [NodeMCU-32S](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB)一台<br>
* [土壤湿度检测传感器模块](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CCF5AD5CBBCEA00000001&dataId=800CCF5AD5CBBCEA&s=)一个<br>
* [继电器relay控制模块](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CF588B273F26100000001&dataId=800CF588B273F261&s=)一个<br>
* mini小水泵一台<br>
* 杜邦线若干<br>
* Micro-USB数据线一条<br>

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/ESP32_auto_watering_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知GPIO0连接土壤湿度检测传感器的DO作为输入，GPIO2与继电器控制模块IN作为输出，控制继电器的通断，其他就是3V3和5V的电源和GND的连接方式。

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
<img src=./../../../images/auto_water_新建产品页面.png width=40%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“自动浇水养植系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/auto_water_产品列表页.png width=80%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/auto_water_产品详情页面.png width=50%/>
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
<img src=./../../../images/auto_water_管理设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/auto_water_添加设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/auto_water_添加设备.png width=50%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/auto_water_完成添加设备.png width=50%/>
</div>

<br>
<br>

&emsp;&emsp;
下面是设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<br>

<div align="center">
<img src=./../../../images/auto_water_设备信息.png width=80%/>
</div>

#### 2.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### 2.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“小水泵开关“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/auto_water_设备详细信息.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 3、设备端开发

### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“智能水族箱”案例点击“立即创建”即可。

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

import utime   # 延时函数在utime库中
from driver import GPIO
import relay,soil_moisture

relayDev = None
humiDev = None
relayStatus = None

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

    wlan.active(True)                  #激活
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

#...

def start_watering():
    relayDev.trigger()

def stop_watering():
    relayDev.untrigger()

if __name__ == '__main__':
    curstatus = None
    laststatus = None
    relay_init()
    humi_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    while True:
        humidity = humiDev.moistureDetect()
        if humidity == 0:
            curstatus = 0
            if curstatus != laststatus:
                stop_watering()
                relayStatus = 0
                report_event()
                print("关闭小水泵")
                laststatus = curstatus
        elif humidity == 1:
            curstatus = 1
            if curstatus != laststatus:
                start_watering()
                relayStatus = 1
                report_event()
                print("打开小水泵")
                laststatus = curstatus
        utime.sleep(1)

```

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 继电器relay和土壤湿度传感器的初始化
```python
def relay_init():
    global relayDev

    gpioDev = GPIO()
    gpioDev.open("relay")
    relayDev = relay.Relay(gpioDev,1)

def humi_init():
    global humiDev

    gpioObj = GPIO()
    gpioObj.open("humidify")
    humiDev = soil_moisture.SoilMoisture(gpioObj)
```

* 云端控制浇水
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global relayStatus
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "onoff" in payload.keys():
        value = payload["onff"]
        if value == 1:
            start_watering()
            relayStatus = 1
            print("打开小水泵")
            report_event()
        elif value == 0:
            stop_watering()
            relayStatus = 0
            print("关闭小水泵")
            report_event()
        else:
            print("无效参数")
```

* 自动浇水控制
```python
 while True:
    humidity = humiDev.moistureDetect()
    if humidity == 0:
        curstatus = 0
        if curstatus != laststatus:
            stop_watering()
            relayStatus = 0
            report_event()
            print("关闭小水泵")
            laststatus = curstatus
    elif humidity == 1:
        curstatus = 1
        if curstatus != laststatus:
            start_watering()
            relayStatus = 1
            report_event()
            print("打开小水泵")
            laststatus = curstatus
    utime.sleep(1)
```

<br>

## 4、运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “关闭小水泵”说明继电器通断关，小水泵关闭
* “打开小水泵”说明继电器通断开，小水泵打开

```log
mount fs
 ==== python execute bootpy ====
enable OneMinuteOnCloud
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
关闭小水泵
打开小水泵
```

### 4.1、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/自动养植浇水物联网监控设备状态及属性.png width=80%/>
</div>

> 思考：本案例使用了HaaS传感器积木中的土壤湿度传感器和继电器通断控制小水泵，实现自动植物浇花，那有在家里养小金鱼的开发者，可以通过HaaS传感器积木库中选择合适的传感器，来给鱼缸自动加水等等，然后自己优化好，欢迎留言交流～
