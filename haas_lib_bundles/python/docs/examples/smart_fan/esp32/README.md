# 智能小风扇
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/smart_fan/三步导学.png width=70%/>
</div>

## 简介
&emsp;&emsp;
在日常生活中，我们经常需要一些通过风扇降温的场景，比如说机房，电脑主机等等。通常情况下，我们都是人为的去打开或者关闭风扇，调节档位进行降温操作，操作起来不是非常便捷。

&emsp;&emsp;
本方案通过一个温度传感器实时的采集当前温度，然后依据温度值，动态的去调节风扇的转速和开关，同时我们可以通过手机app实时的控制风扇转速所对应的温档位，实现对风扇的远程控制，既能方便我们控制风扇，也能有效的节能，绿色环保。

&emsp;&emsp;


## 准备
&emsp;&emsp;
本案例打造需要如下硬件：
* ESP32一台
* 风扇电机模块
* AHT21B 温度传感器
* 杜邦线若干 
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/smart_fan/接线图.png width=80%/>
</div>



## 物联网平台开发
### 开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/smart_fan/开通公共实例.png
 width=92%/>
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
<img src=./../../../images/smart_fan/创建产品.png
 width=92%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（如果创建的产品品类为标准品类，可以选择标准品类），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/smart_fan/新建产品.png width=92%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“智能小风扇”的产品，如下图所示。

<div align="center">
<img src=./../../../images/smart_fan/查看产品.png width=92%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/smart_fan/物模型.png width=92%/>
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
<img src=./../../../images/smart_fan/管理设备.png width=92%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/smart_fan/添加设备入口.png width=92%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/smart_fan/添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/smart_fan/完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性详细信息

<div align="center">
<img src=./../../../images/smart_fan/物模型数据定义.png width=92%/>
</div>

<br>

##### **获取设备三元组**

<div align="center">
<img src=./../../../images/smart_fan/查看三元组.png width=92%/>
</div>
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/smart_fan/三元组信息.png width=60%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“当前温度“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/smart_fan/上报数据查看.png width=92%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

## 创建解决方案

&emsp;&emsp;
点击下图中的"快速开始"按键(<img src=./../../../images/smart_fan/HaaS_Studio_创建工程按钮.png width=8%/>)会弹出HaaS Studio的欢迎页面，请点击“创建项目”按钮。

&emsp;&emsp;
在随后弹框中，设定好项目名称（“smart_fan”）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/smart_fan/ide_for_project.png width=80%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖smart_fan工程目录下的原有文件。其中main.py脚本的内容如下图所示：
```python
#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    main.py
@Author     :    guoliang.wgl
@version    :    1.0
@Description:    smart_fan案例 - 智能控制小风扇
                 board.json - 硬件资源配置文件
'''

from fan import Fan
from aht21b import AHT21B
from driver import PWM, I2C
import time
from aliyunIoT import Device  # iot组件是连接阿里云物联网平台的组件
import json

# 物联网平台连接标志位
iot_connected = False
wlan = None
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
# 物联网设备实例
device = None
# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"
# 警报开关以及时间段控制（大于等于alarm_start 或者小于等于alarm_end ）
gear1_temp = 22
gear2_temp = 27
gear3_temp = 32
FLAG_CUR_TEMP = "cur_temp"
FLAG_GEAR1 = "gear1"
FLAG_GEAR2 = "gear2"
FLAG_GEAR3 = "gear3"
cur_gear = 0


# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    global wlan
    wifi_connected = False
    wlan.active(True)  #激活界面
    wlan.scan()  #扫描接入点
    #print("start to connect ", wifiSsid)
    # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    wlan.connect(wifiSsid, wifiPassword)
    while True:
        wifi_connected = wlan.isconnected()  # 获取Wi-Fi连接路由器的状态信息
        if wifi_connected:  # Wi-Fi连接成功则退出while循环
            break
        else:
            time.sleep(0.5)
            print("wifi_connected:", wifi_connected)
    ifconfig = wlan.ifconfig()  #获取接口的IP/netmask/gw/DNS地址
    print(ifconfig)
    time.sleep(0.5)


# 物联网平台连接成功的回调函数
def on_connect(data):
    global iot_connected
    iot_connected = True


# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global FLAG_GEAR1, FLAG_GEAR2, FLAG_GEAR3, gear1_temp, gear2_temp, gear3_temp
    try:
        props = eval(request['params'])
        if FLAG_GEAR1 in props.keys():
            gear1_temp = props[FLAG_GEAR1]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR1, gear1_temp))
        elif FLAG_GEAR2 in props.keys():
            gear2_temp = props[FLAG_GEAR2]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR2, gear2_temp))
        elif FLAG_GEAR3 in props.keys():
            gear3_temp = props[FLAG_GEAR3]
            print('on_props:  name is {},value is  {}'.format(
                FLAG_GEAR3, gear3_temp))
        post_default_value()
    except Exception as e:
        print(e)


def post_props(data):
    global device
    if isinstance(data, dict):
        data = {'params': json.dumps(data)}
    ret = device.postProps(data)
    return ret


def connect_lk(productKey, deviceName, deviceSecret):
    global device, iot_connected
    key_info = {
        'region': 'cn-shanghai',
        'productKey': productKey,
        'deviceName': deviceName,
        'deviceSecret': deviceSecret,
        'keepaliveSec': 60
    }
    # 将三元组信息设置到iot组件中
    device = Device()
    # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
    device.on(Device.ON_CONNECT, on_connect)
    # 配置收到云端属性控制指令的回调函数
    # 如果收到物联网平台发送的属性控制消息，则调用on_props函数
    device.on(Device.ON_PROPS, on_props)
    # 启动连接阿里云物联网平台过程
    device.connect(key_info)
    # 等待设备成功连接到物联网平台
    while True:
        if iot_connected:
            print('物联网平台连接成功')
            break
        else:
            print('sleep for 1 s')
            time.sleep(1)
    time.sleep(2)


def post_default_value():
    global FLAG_GEAR1, FLAG_GEAR2, FLAG_GEAR3, gear1_temp, gear2_temp, gear3_temp
    value = {FLAG_GEAR1: gear1_temp}
    post_props(value)
    value = {FLAG_GEAR2: gear2_temp}
    post_props(value)
    value = {FLAG_GEAR3: gear3_temp}
    post_props(value)


def upload_temp(temp):
    value = {FLAG_CUR_TEMP: temp}
    post_props(value)


if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)  #创建WLAN对象
    # 请替换物联网平台申请到的产品和设备信息
    # global productKey, deviceName, deviceSecret ,on_request, on_play
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    post_default_value()
    # 初始化风扇控制pwm
    pwmObj = PWM()
    pwmObj.open("fan")
    fan = Fan(pwmObj)
    fan.control(0)
    # 初始化温度传感器
    i2c = I2C()
    i2c.open('aht21b')
    aht = AHT21B(i2c)
    while True:
        temp = aht.getTemperature()
        print('cur temp is {}'.format(temp))
        upload_temp(temp)
        if temp <= gear1_temp and cur_gear != 0:
            cur_gear = 0
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear1_temp and temp <= gear2_temp and cur_gear != 1:
            cur_gear = 1
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear2_temp and temp <= gear3_temp and cur_gear != 2:
            cur_gear = 2
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))
        elif temp > gear3_temp and cur_gear != 3:
            cur_gear = 3
            fan.control(cur_gear)
            print('fan change to gear {}'.format(cur_gear))

```

&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改smart_fan工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

3. **确保设备端物模型和云端定义的一致**

&emsp;&emsp;
smart_fan工程里main.py中下面的代码定义的是物模型需要用到的三个props的标识符
```python
FLAG_CUR_TEMP = "cur_temp"
FLAG_GEAR1 = "gear1"
FLAG_GEAR2 = "gear2"
FLAG_GEAR3 = "gear3"

```
&emsp;&emsp;
确保所有标识符和物联网产品的物模型中属性标识符是一样的，同时对应的数据类型和数据取值范围是一样的，如下图所示：
<div align="center">
<img src=./../../../images/smart_fan/物模型数据定义.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 初始化网络上报prop的默认值到物联网平台
```python
wlan = network.WLAN(network.STA_IF)  #创建WLAN对象
# 请替换物联网平台申请到的产品和设备信息
# global productKey, deviceName, deviceSecret ,on_request, on_play
get_wifi_status()
connect_lk(productKey, deviceName, deviceSecret)
post_default_value()
```

* 风扇初始化
```python
pwmObj = PWM()
pwmObj.open("fan")
fan = Fan(pwmObj)
fan.control(0)
```

* 温度传感器初始化
```python
i2c = I2C()
i2c.open('aht21b')
aht = AHT21B(i2c)
```



* 依据当前温度调价风扇转速并上传温度到物联网平台
```python
    temp = aht.getTemperature()
    print('cur temp is {}'.format(temp))
    upload_temp(temp)
    if temp <= gear1_temp and cur_gear != 0:
        cur_gear = 0
        fan.control(cur_gear)
        print('fan change to gear {}'.format(cur_gear))
    elif temp > gear1_temp and temp <= gear2_temp and cur_gear != 1:
        cur_gear = 1
        fan.control(cur_gear)
        print('fan change to gear {}'.format(cur_gear))
    elif temp > gear2_temp and temp <= gear3_temp and cur_gear != 2:
        cur_gear = 2
        fan.control(cur_gear)
        print('fan change to gear {}'.format(cur_gear))
    elif temp > gear3_temp and cur_gear != 3:
        cur_gear = 3
        fan.control(cur_gear)
        print('fan change to gear {}'.format(cur_gear))

```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* “on_props”说明收到手机端的props控制指令
* "fan change to gear" 说明温度区间发生变化，调节了风扇档位
```log
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.3.198', '255.255.255.0', '192.168.3.1', '192.168.3.1')
establish tcp connection with server(host='a14ir3ezcgN.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
open AHT21B
cur temp is 24.21
fan change to gear 1
cur temp is 24.24
cur temp is 24.25
cur temp is 24.27
cur temp is 24.29

```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/smart_fan/上报数据查看.png width=92%/>
</div>

&emsp;&emsp;
此时如果手机端更新了属性值设置，物联网平台的物模型数据会更新为设备上报的最新的属性值。



## 物联网应用开发

&emsp;&emsp;
IoT Studio 提供了应用快速开发的能力，可以很方便地与物联网平台进行联动。本节的开发工作也将围绕 IoT Studio展开。

### 新建“普通项目”
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中新建一个空白项目，如下图所示，将此项目命名为“智能小风扇”。

<div align="center">
<img src=./../../../images/smart_fan/IoTStudio_新建应用.png width=92%/>
</div>

<div align="center">
<img src=./../../../images/smart_fan/iot_studio_新建项目.png width=92%/>
</div>



### 关联产品
&emsp;&emsp;
为了使本项目能够获取到目标设备的定位信息，我们首先需要将该项目和我们在前一节创建的产品“智能小风扇”绑定。

&emsp;&emsp;
在项目控制台，点击左侧的“产品”，点击“关联物联网平台产品”。此时可以看见我们创建的“智能小风扇”。点击选中，并勾选“关联产品同时关联其下所有设备”，以便该项目可以访问到所有设备的定位信息。


<div align="center">
<img src=./../../../images/smart_fan/iot_studio_关联物联网产品.png width=92%/>
</div>


### 创建“移动应用”
&emsp;&emsp;
在项目控制台，点击左侧的“主页”，选择项目开发中的移动应用，点击新建按钮。
<div align="center">
<img src=./../../../images/smart_fan/iot_studio_新建移动应用.png width=92%/>
</div>

### 界面开发及交互配置
如下图所示，本应用中一个使用了三个UI组件，分别是文字和数字框和实时曲线。选中相应的组件，拖拽到中间的位置。


<div align="center">
<img src=./../../../images/smart_fan/iot_studio_ui.png width=92%/>
</div>

* 文本编辑

&emsp;&emsp;
    对于文字组件而言，点击并修改文字内容等属性即可

* 配置数据源

<div align="center">
<img src=./../../../images/smart_fan/iot_studio_配置数据.png width=92%/>
</div>

&emsp;&emsp;
    对于数字框和实时曲线组件而言，需要按照上图中的步骤，选中组件，并配置数据源，将该组件和具体的产品，设备，属性进行绑定，物联网产品创建章节中对应的属性，分别对应着这里的数字框组件和温度曲线。




* 添加交互

<div align="center">
<img src=./../../../images/smart_fan/iot_studio_交互设置.png width=92%/>
</div>

&emsp;&emsp;
    对于数字框组件而言，配置了数据源以后，我们还需要定义它的交互行为，如上图所示，选中组件，点击右侧的交互标签，选择新增交互，并设置具体的事件和动作，最后点击配置设备打开如下图中所示的界面。

<div align="center">
<img src=./../../../images/smart_fan/iot_studio_交互_配置设备.png width=92%/>
</div>

&emsp;&emsp;
    选择相应的产品设备和属性后，最后先泽参数来源：组件值

### 预览及发布应用

&emsp;&emsp;
点击右上角按钮预览或发布，即可在手机上使用应用，可以实现随时随地远程控制啦。

<div align="center">
<img src=./../../../images/smart_fan/iot_studio_预览发布.png width=92%/>
</div>
