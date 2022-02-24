# 人体红外测温系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/3_人体红外测温系统_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
2020年1月全球新冠疫情发生并开始传播，但是2年时间已经都过去了，疫情还是不断反复无常，丝毫没有减退的迹象，并且越演愈烈。马上又一个春节来临，春节对于中国人来说是一个亲友团聚的日子，亲友团聚意味着大家要从五湖四海赶到一块进行相聚，对于现在的疫情现状，相聚意味着有传播的风险。基于这个背景，我们HaaS团队打造了这一套人体红外测温系统的学习系统，你只要按照下面一步一步操作，就可以打造出属于自己的一套防疫测温系统，当亲友来访时，只要对手腕测一下温度，就知道有没有发烧发热了。

&emsp;&emsp;
人体红外测温系统是一套实时监测来访人员的体温，监测到异常体温时，设备会发出蜂鸣器报警以及相应的钉钉推送，主人可以很快了解到来访人员的情况，真正可以做到春节期间健康串门。

&emsp;&emsp;
本章课程就以新冠疫情这个为背景介绍如何打造一个春节期间可以进行健康串门的人体测温系统。

&emsp;&emsp;
根据人体的各个部位表皮温度会有一些差异，建议以额头或手背中的其中一个部位进行统一测温，这样保证测量参考温度更有效和准确，并且距离不要超过1m，在一个闷热的环境待的太久也不宜马上测量温度，可以等15分钟到半小时再测量：
1. 手背温度：33~35度
2. 测量距离：小于1m

## 准备
&emsp;&emsp;
本案例打造需要如下硬件：
* ESP32一台
* MLX90614人体红外温度传感器一个
* SSD1306显示屏一个
* 蜂鸣器一个
* 杜邦线若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/1_ESP32_huma_ir_temp_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知oled和人体红外温度模块是I2C总线连接，蜂鸣器i/o与muc通过pwm连接。

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
<img src=./../../../images/1_hit_新建产品页面.png width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“温度计”的产品，如下图所示。

<div align="center">
<img src=./../../../images/1_hit_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/1_hit_产品详情页面.png width=100%/>
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
<img src=./../../../images/1_hit_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_hit_添加设备入口.png width=100%/>
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
<img src=./../../../images/1_hit_设备详细信息.png width=100%/>
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
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“人体温度“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/1_hit_设备详细信息.png width=100%/>
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
在随后弹框中，设定好项目名称（“ir_temperature”）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/1_创建ir_temperature工程_ESP32.png width=80%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖ir_temperature工程目录下的原有文件。其中main.py脚本的内容如下图所示：
```python
#########
#########
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson
##################

from driver import I2C,PWM,TIMER
import utime
from ssd1306 import SSD1306_I2C
import buzzer
import mlx90614
import time

INVALUDE_TEMP = 1037 # 失效测量值
LOWER_LEVEL = 30 # 最小有效测温阈值 【30 60】
HIGH_LEVEL = 60 # 最大有效测温阈值
ABNORMAL_TEMP = 35 # 异常温度设定阈值  【30 ... 60】

E_CAL_TEMP_INVALID = 1
E_CAL_TEMP_FINISH = 2
E_CAL_TEMP_HIGH = 3
E_CAL_TEMPING = 4
E_CAL_TEMP_START = 5

timer_interval_100ms = 100 #ms
cal_temp_timer = 100  #15s = (150*timer_interval_100ms)
high_timer = 50  #10s = (100*timer_interval_100ms)
TEMP_CAL_TIMER = (cal_temp_timer) #有效测量时间
TEMP_CAL_TIMER_HIGH = (high_timer) #异常温度，有效测量时间

timer_interval_1000ms = 1000
stay_long = 5  # 5s (5*timer_interval_1000ms)
DISPLAY_STAY_LONG = (stay_long) # 测量出有效温度后 屏幕持续显示时间

mlx90614Dev = 0
oled = 0
object_temp = 0.00
ambient_temp = 0.00

cal_hightemp_cnt = 0
cal_temp_cnt = 0
last_temp = 0
valid_temp = 0
blink = 0
display_time_cnt = 0
event_display = 0
start_cal_temp = 1
time_is_start = 0
displaytime_flag = 0

obj_buzzer = 0
pwmObj = 0

temptimerObj = 0
oledtimerObj = 0
displaytimerObj = 0

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
    mlx9061_init()
    oled_init()
    ##########
    wlan = network.WLAN(network.STA_IF)   #创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    ##########
    buzzer_init()
    timer_init()
```

&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改ir_temperature工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改ir_temperature工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

3. **修改设备端上报温湿度信息所用标识符**

&emsp;&emsp;
humiture工程里main.py中下面的代码实现的是上传温湿度值到云端的功能。其中object_temp人体异常温度上报时所用的标识符。
```python
# 上传测到的人体异常温度信息到物联网平台
def report_event():
    upload_data = {'params': ujson.dumps({
        'object_temp': object_temp,
    })
    }
    device.postProps(upload_data)
```
&emsp;&emsp;
确保object_temp标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/1_ESP32_ir_temp_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 人体红外测温过程参数设置
```python
ABNORMAL_TEMP = 35 # 异常温度设定阈值  【30 ... 60】
TEMP_CAL_TIMER = (cal_temp_timer) #有效测量时间
TEMP_CAL_TIMER_HIGH = (high_timer) #异常温度，有效测量时间
```

* 新增函数处理云端信息并控制GPIO的接口
```python
def is_abnormal(obj_temp):
    global cal_hightemp_cnt,cal_temp_cnt,last_temp,valid_temp

    i_obj_temp = int(obj_temp)
    if i_obj_temp == INVALUDE_TEMP :
        return

    if i_obj_temp >= HIGH_LEVEL or i_obj_temp <= LOWER_LEVEL:
        last_temp = i_obj_temp
        cal_hightemp_cnt = 0
        cal_temp_cnt = 0
        return E_CAL_TEMP_INVALID
    else:
        if last_temp >= HIGH_LEVEL or last_temp <= LOWER_LEVEL:
            last_temp = i_obj_temp
            cal_temp_cnt = 1
            cal_hightemp_cnt = 1
            return E_CAL_TEMP_START #开始测温

        if i_obj_temp >= ABNORMAL_TEMP:
            if last_temp < ABNORMAL_TEMP:
                cal_hightemp_cnt = 1 #重新开始测，第一次测到高温
            else:
                cal_hightemp_cnt += 1 #连续测到高温
        else:
            cal_hightemp_cnt = 0  #测到正常温度

    cal_temp_cnt += 1
    last_temp = i_obj_temp
    if cal_hightemp_cnt >= TEMP_CAL_TIMER_HIGH:
        cal_hightemp_cnt = 0   #连续测TEMP_CAL_TIMER_HIGH都是高温，那么认定高温
        valid_temp = obj_temp
        return E_CAL_TEMP_HIGH

    if cal_temp_cnt >= TEMP_CAL_TIMER:
        cal_temp_cnt = 0
        valid_temp = obj_temp
        return E_CAL_TEMP_FINISH #最多连续TEMP_CAL_TIMER，那么结束测温
    return E_CAL_TEMPING
```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* “start”说明开始有效测温
* "high"之后跟着的为设备端采集的环境温度和测到的人体温度，“23.63C | 35.39C”，设定温度阈值35，大于阈值，就上报异常温度，同时蜂鸣器报警。
* "finish"之后跟着的为设备端采集的环境温度和测到的人体温度，23.61C | 32.09C，设定温度阈值35，小于阈值，说明体温正常。


```log
  ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
mlx90614 inited!
ssd1306 inited!
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.0.204', '255.255.255.0', '192.168.0.1', '192.168.0.1')
establish tcp connection with server(host='a1I8HyYFD6K.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
buzzer inited!
>>> display  1 0
display ivalid enent
time stop:
------------------
reload time: 0
start
display  4 0
display  4 0
high
23.63C | 35.39C
------------------
reload time: 0
start
display  4 0
display  4 0
display  4 0
finish
23.61C | 32.09C

```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_hit_温湿度监控设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果设备端测到异常体温，物联网平台的物模型数据会更新为设备上报的最新的属性值。

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
<img src=./../../../images/1_dd_studio_新建应用_ir.png width=100%/>
</div>

&emsp;&emsp;
如果下面所属项目没内容，点击上面的“新建项目”按钮，创建如下：
<div align="center">
<img src=./../../../images/1_dd_studio_新建项目2.png width=100%/>
</div>


&emsp;&emsp;
进入开发界面后，点击下图红框中的部分，进入拖拽式开发界面
<div align="center">
<img src=./../../../images/1_dd_studio_拖拽开发_ir.png width=100%/>
</div>

&emsp;&emsp;
拖一个设备触发节点到右边的画布上
<div align="center">
<img src=./../../../images/1_dd_studio_设备触发_ir.png width=100%/>
</div>

&emsp;&emsp;
再拖一个钉钉机器人上来
<div align="center">
<img src=./../../../images/1_dd_studio_机器人触发_ir.png width=100%/>
</div>

&emsp;&emsp;
点击红框中的小正方形，即可拉出一条线，接到右边红框中的小正方形，把两块积木连接起来，相当于是说数据从左边流向右边
<div align="center">
<img src=./../../../images/1_dd_studio_机器人设备连接_ir.png width=100%/>
</div>

&emsp;&emsp;
然后开始配置设备触发和钉钉机器人信息，注意（配置信息是在上面物联网平台创建产品和设备的时候存在，第一次使用的时候需要进行关联添加）
<div align="center">
<img src=./../../../images/1_dd_studio_设备配置_ir.png width=100%/>
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
<img src=./../../../images/1_dd_studio_机器人配置_ir.png width=100%/>
</div>

&emsp;&emsp;
到这里时，你已经完成了全部的配置，可以部署和运行了，依次点击下图中的部署和发布。
<div align="center">
<img src=./../../../images/1_dd_studio_部署运行_ir.png width=100%/>
</div>

运行后，就可以通过对设备上下电，触发消息，实现你的钉钉消息通知了！
