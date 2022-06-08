# 运动心率检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_步骤概述.png width=70%/>
</div>

## 1、简介
&emsp;&emsp;
当今社会，人们经常通过跑步等运动方式来健身。适量的运动可以提高免疫力，促进身心健康。 然而，无论是跑步，还是其他的健身运动，如果运动过量就会对身体造成损害，甚至导致休克、猝死等严重后果。

&emsp;&emsp;
心率是判断运动是否过量的重要指标，心率检测系统使用方便的设备，可以实时检测心率，动态的在网页上显示心率值和心电图，共运动者参考，并在心率过快（超过设定的阀值）时通过报警提醒运动者注意防护。

&emsp;&emsp;
本课程介绍如何打造一个心率检测系统。

&emsp;&emsp;
心率是人每分钟心跳的次数，一般人在安静的情况下为 60～100次每分钟。运动情况下人的心跳会加速，此时称为运动心率，一般为100～150次每分钟，如果超过150次/分钟，人会出现胸闷，心悸情况需要停下休息。

&emsp;&emsp;
一般人运动时心率的控制可参考以下公式：
* （220─现在年龄）×0.8=最大运动心率
* （220─现在年龄）×0.6=最小运动心率

&emsp;&emsp;
为了检测心率之，本场景的示意图如下图所示，场景设计的行为如下：
1. 通过心率传感器测量心率的大小
2. 当心率在控制范围内时绿灯按照心率值闪烁
3. 当心率低于最小运动心率时蓝灯按照心率值闪烁
4. 当心率高于最大运动心率时红灯按照心率值闪烁并打开蜂鸣器发出警报

### 1.1、准备
&emsp;&emsp;
本案例需要如下硬件：
* ESP32一台
* MK002508心率传感器一个
* RGB三色LED灯一个
* Micro-USB数据线一条
* 蜂鸣器一个
* 杜邦线若干

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_硬件连线图.png width=80%/>
</div>

<br>

## 2、物联网平台开发
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的4个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）
4. 多设备场景联动设定

<br>

### 2.1、创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”，节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_新建产品页面.png
 width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“心率检测装置”的产品，如下图所示。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_产品列表页.png width=100%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符,并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录,否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_创建自定义物理模型.png width=100%/>
</div>

&emsp;&emsp;
然后按照下图分别选择“心率大小”和“蜂鸣器”属性添加。
* 心率大小
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_创建自定义物理模型1.png width=50%/>
</div>

* 蜂鸣器
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_创建自定义物理模型2.png width=50%/>
</div>

&emsp;&emsp;
产品及其物模型创建完成后，此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中.

<br>

### 2.3、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_添加设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_完成添加设备.png width=40%/>
</div>

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_查看设备.png width=100%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_设备详细信息.png width=100%/>
</div>

<br>

#### 2.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_三元组信息.png width=60%/>
</div>

<br>

#### 2.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“心率大小”属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_设备物模型数据.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

#### 2.4.3、**云端物模型**
&emsp;&emsp;
在此产品的物模型中有1个bool类型的属性，命名为"蜂鸣器"，代表是否打开蜂鸣器。

#### 2.4.4、**多设备场景联动设定**
&emsp;&emsp;
控制逻辑有两种实现方式：
1. 使用物联网平台的规则引擎功能
2. 使用IoT Studio的业务逻辑功能

&emsp;&emsp;
本节接下来就开始介绍使用物联网平台的规则引擎来完成此控制逻辑的设定。

&emsp;&emsp;
按照下图所示的步骤创建场景联动的规则：
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_创建场景联动规则.png width=80%/>
</div>

&emsp;&emsp;
点击“创建规则”之后，填入场景规则名称，这里我们先创建第一个“心率过快开蜂鸣器”的规则。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_创建场景联动规则_心率过快开蜂鸣器.png width=40%/>
</div>

&emsp;&emsp;
规则创建完成后，等待几秒钟系统会自动跳转到规则编辑页面，这里需要配置如下两步完整一整条规则的创建：
1. 设置“触发器”为心率检测系统的“心率大小"属性高于150时触发此规则的执行，如下图所示

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_心率过快_触发器设定.png width=80%/>
</div>

2. 设置“执行动作"为控制设备打开”蜂鸣器“属性
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_心率过快_执行器设定.png width=80%/>
</div>

&emsp;&emsp;
点击保存之后”心率过快开蜂鸣器“的场景联动规则就创建好了。

&emsp;&emsp;
创建完规则之后，在”场景联动“页面中点击规则后面的启动按钮，则设定的规则就会运行起来。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测装置_开启场景联动规则.png width=80%/>
</div>

## 3、设备端开发

### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“运动心率检测系统”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

3. **修改设备端上报心率大小所用标识符**

&emsp;&emsp;
main.py中下面的代码实现的是上传心率大小到云端的功能。其中HeartRate便是心率大小上报时所用的标识符。
```python
# 上传心率到物联网平台
def upload_heartbeat():
    global device, heartbeatdev
    while True:
        # 生成上报到物联网平台的属性值
        value = getBPM()
        prop = ujson.dumps({
            'HeartRate': value
            })
        print('uploading data: ', prop)
        # 上传心率值到物联网平台
        upload_data = {'params': prop}
        device.postProps(upload_data)
        utime.sleep(2)
```
&emsp;&emsp;
确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测_heartrate_detector_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 新增一个变量用来控制蜂鸣器对应的GPIO
```python
# 蜂鸣器控制变量
buzzeron =0
```

* 新增函数处理云端信息并控制GPIO的接口
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global buzzeron
    payload = ujson.loads(request['params'])

    # 获取dict状态字段
    if "Buzzer" in payload.keys():
        buzzeron = payload["Buzzer"]
        if(buzzeron):
            print("打开蜂鸣器")
    buzzerobj.write(buzzeron)      # 控制蜂蜜器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'Buzzer': buzzeron,
    })
    upload_data = {'params': prop}
    # 上报蜂鸣器属性到云端
    device.postProps(upload_data)
```

<br>

## 4、运行结果

### 4.1、本地运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* "uploading data:"之后跟着的为设备端向云端发布的心率大小属性信息，其中HeartRate后面的数值为心率大小，单位：BPM。

* 设备端解析到云端送下来的控制指令包含“{"Buzzer":1}”之后会打印"打开蜂鸣器"并打开蜂鸣器


```log
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
voice inited!
led inited!
buzzer inited!
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.43.45', '255.255.255.0', '192.168.43.1', '192.168.43.1')
establish tcp connection with server(host='gv4cuThyf0j.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
sleep for 1 s
物联网平台连接成功
sleep for 2s
sleep for 2s before start heartbeatsensor
heartbeat inited!
uploading data:  {"HeartRate": 0}
uploading data:  {"HeartRate": 108.1081}
uploading data:  {"HeartRate": 108.2056}
uploading data:  {"HeartRate": 113.0476}
uploading data:  {"HeartRate": 133.6303}
uploading data:  {"HeartRate": 167.364}
打开蜂鸣器
uploading data:  {"HeartRate": 159.7869}
打开蜂鸣器
uploading data:  {"HeartRate": 96.19239}
uploading data:  {"HeartRate": 94.71192}
uploading data:  {"HeartRate": 95.27591}
uploading data:  {"HeartRate": 94.45101}
uploading data:  {"HeartRate": 94.48819}
uploading data:  {"HeartRate": 94.78673}
uploading data:  {"HeartRate": 94.63722}
uploading data:  {"HeartRate": 95.42744}

```
&emsp;&emsp;
传感器默认返回心率230左右，将手指轻按在心率传感器上后，可测出实际心率。

开发板实际效果如下：
* 在心率高于150BPM时，可以看到日志中出现"打开蜂鸣器"的字样，伴随着蜂鸣器打开，红色LED灯闪烁
* 心率在100和150BPM之间时，绿色LED灯闪烁
* 心率在低于100BPM之间时，蓝色LED灯闪烁


### 4.2、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
将手指放在传感器上，传感器测量心率上报，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 5、物联网应用开发

&emsp;&emsp;
如果能够将心率数据通过网页的形式直观的展现出来，用户就可以通过查看心率的当前和历史数据，识别出心率的规律，有助于得出有效的健身强度管控方法和应对措施。同时相关人员还可以远距离的查看数据，得出直观的了解和认识。

<br>

### 5.1、中控大屏目标
&emsp;&emsp;
本节课程主要目标是设计一个总控大屏的web应用，能实时的查看如下信息：
1. 过去3小时的心率变化实时信息
2. 蜂鸣器实时状态显示

<br>

### 5.2、心率检测监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何心率检测系统中控大屏的web应用程序的开发。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

#### 5.2.1、**新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“心率检测系统”。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

#### 5.2.2、**新建“Web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个Web应用，命名为“心率大小实时监控”。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控.png width=80%/>
</div>

&emsp;&emsp;
Web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

#### 5.2.3、**页面设计**
&emsp;&emsp;
这里我们用到3种组件:
* 1个实时曲线
用于显示心率大小的历史变化曲线及实时数据
* 1个开关
显示和控制蜂鸣器的状态


&emsp;&emsp;
将2种组件拖到中间的画布区，适当调整组件布局。然后拖动文字组件，为组件添加文字注释，如下图所示。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_组件排列.png width=80%/>
</div>

<br>

#### 5.2.4、**关联产品和设备**
&emsp;&emsp;
此时回到”心率检测系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_关联产品和设备.png width=80%/>
</div>

&emsp;&emsp;
关联产品的过程如下：
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_关联产品.png width=80%/>
</div>

&emsp;&emsp;
关联设备的过程如下：
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_关联设备.png width=80%/>
</div>
&emsp;&emsp;
产品和设备关联完毕之后，就可以将把组件和设备的属性关联起来了。

<br>

#### 5.2.5、**关联数据源**
&emsp;&emsp;
关联数据源分为如下3个步骤，每个步骤的截图如下：
* 关联产品
* 关联设备
* 关联属性

&emsp;&emsp;
具体操作步骤如下：
1. 选中”开关“组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_开关_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_开关_配置数据源_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_开关_配置数据源_选择设备.png width=80%/>
</div>


4. 选择“开蜂鸣器”属性
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_开关_配置数据源_选择属性.png width=80%/>
</div>

&emsp;&emsp;
接下来需要为”实时曲线“组件设定数据源。同样点击该组件后，点击右边的配置数据源。因为本场景中的心率大小测量和蜂鸣器的控制都是通过一个物联网设备完成的，所以本页中要选择”单设备多属性“。然后选择好本场景创建的产品，如下图所示。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_实时曲线_配置数据源_选择产品.png width=80%/>
</div>

&emsp;&emsp;
选择相同的设备，如下图所示。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_实时曲线_配置数据源_选择设备.png width=80%/>
</div>

&emsp;&emsp;
本场景显示的是检测的心率大小，所以”数据项“中选择这个属性。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_实时曲线_配置数据源_选择属性.png width=80%/>
</div>

&emsp;&emsp;
最后选择要显示的实时心率大小数据的时间长度，这里选择3小时，如下图所示。
<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_心率大小实时监控_实时曲线_配置数据源_选择数据时间段.png width=80%/>
</div>

<br>

#### 5.2.6、**发布上线**

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_IoT_Studio_预览和发布.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下图所示：

<div align="center">
<img src=./../../../images/heartrate_detector/心率检测系统_运行效果.png width=80%/>
</div>

<br>

&emsp;&emsp;
这样我们就完成了一个心率检测系统从设备心率检测、设备控制、物联网云平台开发及物联网应用开发全链路的开发。
