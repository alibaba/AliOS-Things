# 久坐提醒系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/3_久坐提醒_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
身边很多从事办公室的白领，经常会听他们说：腰椎不行了，有点难受，要不就颈椎也不舒服，这些常见的现象不可忽视，它会对人们后面的生活产生很多负面的影响，所以我们想到能不能有这么一个设备，它会定期提醒人们不要坐太久。其实久坐提醒不是一个新鲜事，市面上也有许许多多关于久坐提醒的工具神器，但是，今天我们HaaS团队就手把手教长期在办公室久坐着的你亲手打造一款属于自己的久坐提醒设备，当你长时间在工位上坐着，它会通过钉钉提醒你，让你一段时间去活动一下筋骨，走动走动，这样让我们上班的同时身体也变得更健康。

&emsp;&emsp;
久坐提醒设备是通过人体红外监测周围区域是否有人移动，本系统有两种情况会出现：漏提醒和多提醒，这个可以根据实际情况做一些调整即可
1. 漏提醒：当没在红外区域内检测到动（休息睡觉等），就不会触发提醒动作，这个会出现漏提醒
2. 多提醒：当离开红外区域时间太短（小于设置窗口时间），检测不到人员离开（比如上厕所时间小于设置窗口时间），这个时候不会认为你有运动，到时间还是会提醒你，就会给人感觉多提醒了

## 准备
&emsp;&emsp;
本案例只需要如下硬件：
* ESP32一台
* IR人体红外传感器一个
* SSD1306显示屏一个
* 杜邦线若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/1_ESP32_sedentary_remind_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知如果要模拟控制空调打开，则需要打开P27（蓝色LED）；如需模拟控制加湿器打开，则需要打开P26（绿色LED）。

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

开通物联网平台功能之后，需要完成下面的4个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）
4. 多设备场景联动设定

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
<img src=./../../../images/1_sr_新建产品页面.png
 width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“久坐提醒器”的产品，如下图所示。

<div align="center">
<img src=./../../../images/1_sr_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/1_sr_产品详情页面.png width=100%/>
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
<img src=./../../../images/1_sr_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_sr_添加设备入口.png width=100%/>
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
<img src=./../../../images/1_sr_设备详细信息.png width=100%/>
</div>

<br>

##### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的”提醒休息“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/1_sr_设备物模型数据.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

&emsp;&emsp;
至此，久坐提醒器云端的产品、设备以及物模型就全部创建完成了，三元组信息也拿到，接下去开始进行设备端的开发

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

## 创建解决方案

&emsp;&emsp;
点击下图中的"快速开始"按键(<img src=./../../../images/1_HaaS_Studio_创建工程按钮.png width=8%/>)会弹出HaaS Studio的欢迎页面，请点击“创建项目”按钮。

&emsp;&emsp;
在随后弹框中，设定好项目名称（“sedentary_remind”）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/1_创建_sr_工程_ESP32.png width=40%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖sedentary_remind工程目录下的原有文件。其中main.py脚本的内容如下图所示：
```python
#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import I2C,GPIO,TIMER
import utime
from ssd1306 import SSD1306_I2C
import ir
import time

temptimerObj = 0
time_dict = {}

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
    oled_init()
    #########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    #########
    ir_check_init()
    timer_init()
    while True:
        if has_rest == 1:
            has_rest = 0
            report_event(1)
            print('report rest')
        if has_leve == 1:
            has_leve = 0
            report_event(0)
            print('report leve')
        utime.sleep(1)

```

&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改sedentary_remind工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改sedentary_remind工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

1. **修改设备端上报提醒休息信息标识符**

&emsp;&emsp;
sedentary_remind工程里main.py中下面的代码实现的是上传提醒休息到云端的功能。其中call_rst消息提醒上报时所用的标识符。
```python
# 上传休息提醒状态到物联网平台
def report_event(data):
    upload_data = {'params': ujson.dumps({
        'call_rst': data,
        })
    }
    device.postProps(upload_data)
```
&emsp;&emsp;
确保代码中的call_rst标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/1_ESP32_sr_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 人体红外区域判断是否有人的两个参数，可以根据实际情况调整
```python
check_max_time1 = (90) #单位s ,用于判断当前区域没有人的时间阈值 (90*timer_interval_1000ms)
check_max_time2 = (45*60) #单位s,用于判断人在当前区域的最大时间阈值 (45*60*timer_interval_1000ms)
CHECK_NOPERSON_MAX_TIME = check_max_time1 #超过这个值，说明人已经离开改红外区域
CHECK_HASPERSON_MAX_TIME = check_max_time2 #超过这个值，提醒休息
```

* 新增函数用于判断红外区域信息
```python
def irq_handler(data):
    global tick_time,total_time,gpioirDev,first_move,has_rest,has_leve,ir_event

    if first_move == 0:
        first_move = 1

    ret = gpioirDev.read()
    if ret == 1:
        print('person come')
        if tick_time >= CHECK_NOPERSON_MAX_TIME:
            print("person has left ",tick_time)
            total_time = 0
            tick_time = 0
            has_leve = 1
            ir_event = PERSON_LEAVE
            return
        else:
            total_time += tick_time
            print("check person move ",tick_time,total_time)
            tick_time = 0
            ir_event = PERSON_MOVE
    elif ret == 0:
        total_time += tick_time
        print('check person nomove ',tick_time,total_time)
        tick_time = 0
        ir_event = PERSON_NOMOVE

    if total_time >= CHECK_HASPERSON_MAX_TIME:
        has_rest = 1
        total_time = 0
        print('you need has rest,because you work too long')
```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* "person come"说明人员在移动。
* "check person nomove“说明监控区域人员在未移动。
* ”report rest“上报久坐提醒信息到云端

```log
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ssd1306 inited!
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.0.166', '255.255.255.0', '192.168.0.1', '192.168.0.1')
establish tcp connection with server(host='a1iDYXz7nCT.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
sleep for 1 s
sleep for 1 s
sleep for 1 s
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
sleep for 1 s
物联网平台连接成功
sleep for 2s
person come
check person nomove  0 0
person come
check person move  10 10
...

you need has rest,because you work too long
report rest
```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_sr_温湿度监控设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果有久坐提醒的信息和人员离开的信息都会上报，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 钉钉消息提醒

### 添加钉钉机器人

&emsp;&emsp;
在钉钉上创建一个群组并进入群设置->智能群助手->添加机器人->自定义。首先选择“智能群助手”：
<div align="center">
<img src=./../../../images/1_dd_智能助手设置.png width=100%/>
</div>

&emsp;&emsp;
然后进入“机器人管理”，选择点击“自定义”
<div align="center">
<img src=./../../../images/1_dd_机器人管理.png width=100%/>
</div>

&emsp;&emsp;
开始配置机器人，如下图所示：
<div align="center">
<img src=./../../../images/1_dd_机器人配置.png width=100%/>
</div>

&emsp;&emsp;
点击完成，在下一个界面中，会展现出一个webhook地址，把这个地址拷贝下来，后面会用到。
<div align="center">
<img src=./../../../images/1_dd_机器人hook获取.png width=100%/>
</div>

### IoT Studio设置

&emsp;&emsp;
首先从物联网平台的功能页面，点击相关产品，进入IoT Studio产品页，开通服务后（首次访问）选择应用开发->业务服务->新建一个测试用的业务服务
<div align="center">
<img src=./../../../images/1_dd_studio_新建项目.png width=100%/>
</div>

&emsp;&emsp;
给服务起个名字，如果下面所属项目没内容，可以从链接进去新建一个项目

<div align="center">
<img src=./../../../images/1_dd_studio_新建应用.png width=100%/>
</div>

&emsp;&emsp;
如果下面所属项目没内容，点击上面的“新建项目”按钮，创建如下：
<div align="center">
<img src=./../../../images/1_dd_studio_新建项目2.png width=100%/>
</div>


&emsp;&emsp;
进入开发界面后，点击下图红框中的部分，进入拖拽式开发界面
<div align="center">
<img src=./../../../images/1_dd_studio_拖拽开发.png width=100%/>
</div>

&emsp;&emsp;
拖一个设备触发节点到右边的画布上
<div align="center">
<img src=./../../../images/1_dd_studio_设备触发.png width=100%/>
</div>

&emsp;&emsp;
再拖一个钉钉机器人上来
<div align="center">
<img src=./../../../images/1_dd_studio_机器人触发.png width=100%/>
</div>

&emsp;&emsp;
点击红框中的小正方形，即可拉出一条线，接到右边红框中的小正方形，把两块积木连接起来，相当于是说数据从左边流向右边
<div align="center">
<img src=./../../../images/1_dd_studio_机器人设备连接.png width=100%/>
</div>

&emsp;&emsp;
然后开始配置设备触发和钉钉机器人信息，注意（配置信息是在上面物联网平台创建产品和设备的时候存在，第一次使用的时候需要进行关联添加）
<div align="center">
<img src=./../../../images/1_dd_studio_设备配置.png width=100%/>
</div>

&emsp;&emsp;
第一次使用的时候需要进行如下产品设备的关联添加：
<div align="center">
<img src=./../../../images/1_dd_studio_关联添加1.png width=100%/>
</div>

<div align="center">
<img src=./../../../images/1_dd_studio_关联添加2.png width=100%/>
</div>

### 钉钉机器人配置
&emsp;&emsp;
点击钉钉机器人节点，并将你的钉钉机器人webhook地址填入其中，其余选项按下图中配置即可，注意之前配置机器人的关键词为“设备”，因为这里的通知消息里带有“设备”两字。

<div align="center">
<img src=./../../../images/1_dd_studio_机器人配置.png width=100%/>
</div>

&emsp;&emsp;
到这里时，你已经完成了全部的配置，可以部署和运行了，依次点击下图中的部署和发布。
<div align="center">
<img src=./../../../images/1_dd_studio_部署运行.png width=100%/>
</div>

运行后，就可以通过对设备上下电，触发消息，实现你的钉钉消息通知了！
