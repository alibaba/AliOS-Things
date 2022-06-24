# 噪音检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/声音检测系统_步骤概述.png width=70%/>
</div>

## 1、简介

### 1.1、背景
&emsp;&emsp;
当今社会，噪音污染已近影响到了人们日常工作生活的方方面面，办公室的噪音污染会影响员工的工作效率，生活中的噪音污染会影响大家的睡眠和情绪，从而影响我们的身心健康。

&emsp;&emsp;
噪音检测系统目的是检测办公室和生活环境的噪音，当噪音超过设定的峰值时，发出警报。 根据报警提示，办公室里的人可以通过降低打电话，讨论的声音，或者主动到会议室去，以避免打扰别人；家里的人也可以据此作出相应的对策，比如关门窗，联系物业或者警察等。
&emsp;&emsp;
本课程介绍如何打造一个室内噪声检测报警系统。

&emsp;&emsp;
噪声标准
1. 比较安静： 30～40 分贝
2. 影响睡眠和休息：50分贝以上
3. 影响工作效率： 70分贝以上
4. 严重影响听力或导致其他疾病： 长期生活在90分贝以上环境

&emsp;&emsp;
为了检测室内环境中声音的大小，本场景的示意图如下图所示，场景设计的行为如下：
1. 通过声音传感器测量环境声音大小
2. 当声音低于90分贝时，根据不同的声音大小亮不同颜色的灯：
   * 小于40分贝亮绿色 
   * 50～70分贝亮蓝色
   * 大于70分贝亮红色
3. 当声音大于90分贝时打开蜂鸣器

### 1.2、准备
&emsp;&emsp;
本案例只需要如下硬件：
* ESP32一台
* MK002508声音传感器一个
* RGB三色LED灯一个
* 蜂鸣器一个
* 杜邦线若干
* Micro-USB数据线一条

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/stm32-噪声检测硬件连线图.png width=80%/>
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
<img src=./../../../images/声音检测装置_新建产品页面.png
 width=80%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“声音检测装置”的产品，如下图所示。

<div align="center">
<img src=./../../../images/声音检测装置_产品列表页.png width=100%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/声音检测装置_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符,并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录,否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型.png width=100%/>
</div>

&emsp;&emsp;
然后按照下图分别选择声音分贝值、点亮绿灯，点亮红灯，点亮蓝灯和开蜂鸣器属性添加。
* 声音分贝值
<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型1.png width=50%/>
</div>
* 点亮红灯:
<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型2.png width=50%/>
</div>
* 点亮绿灯:
<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型3.png width=50%/>
</div>
* 点亮蓝灯:
<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型4.png width=50%/>
</div>
* 开蜂鸣器:
<div align="center">
<img src=./../../../images/声音检测装置_创建自定义物理模型5.png width=50%/>
</div>

&emsp;&emsp;
产品及其物模型创建完成后，此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中：

<div align="center">
<img src=./../../../images/声音检测装置_发布物理模型.png width=100%/>
</div>
<br>

### 2.3、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/声音检测装置_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/声音检测装置_添加设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/声音检测装置_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/声音检测装置_完成添加设备.png width=40%/>
</div>

<div align="center">
<img src=./../../../images/声音检测装置_查看设备.png width=100%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/声音检测装置_设备详细信息.png width=100%/>
</div>

<br>

##### 2.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/声音检测装置_三元组信息.png width=60%/>
</div>

<br>

#### 2.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“声音分贝值“，“点亮红灯”等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/声音检测装置_设备物模型数据.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

#### 2.4.3、**云端物模型**
&emsp;&emsp;
在此产品的物模型中有四个bool类型的属性，命名为"点亮红灯"， "点亮绿灯"， “点亮蓝灯” 和 “开蜂鸣器”，这四个属性的ture和false分别对应是否点亮红灯，绿灯，蓝灯和是否开蜂鸣器。

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
<img src=./../../../images/声音检测装置_创建场景联动规则.png width=80%/>
</div>

&emsp;&emsp;
点击“创建规则”之后，填入场景规则名称，这里我们先创建第一个“声音低于40分贝点亮绿灯”的规则。

<div align="center">
<img src=./../../../images/声音检测装置_创建场景联动规则_声音低于40分贝点亮绿灯.png width=40%/>
</div>

&emsp;&emsp;
规则创建完成后，等待几秒钟系统会自动跳转到规则编辑页面，这里需要配置如下两步完整一整条规则的创建：
1. 设置“触发器”为声音检测装置的“声音分贝值"属性低于40分贝时发此规则的执行，如下图所示

<div align="center">
<img src=./../../../images/声音检测_声音低于40分贝_触发器设定.png width=80%/>
</div>

2. 设置“执行动作"为控制设备打开”点亮绿灯“属性。
<div align="center">
<img src=./../../../images/声音检测_声音低于40分贝_执行器设定.png width=80%/>
</div>

&emsp;&emsp;
点击保存之后”温度过高打开空调“的场景联动规则就创建好了。
&emsp;&emsp;
用相同的方式创建另外三条场景联动规则：
* "50-70分贝点亮蓝色"，如下图所示

<div align="center">
<img src=./../../../images/声音检测_声音在50分贝到70分贝之间_点亮蓝灯.png 
width=80%/>
</div>
* ”大于70分贝点亮红色“，如下图所示

<div align="center">
<img src=./../../../images/声音检测_声音高于70分贝_点亮红灯.png width=80%/>
</div>

* "持续90分贝以上开蜂鸣器“，如下图所示

<div align="center">
<img src=./../../../images/声音检测_声音持续高于90分贝_开蜂鸣器.png width=80%/>
</div>

&emsp;&emsp;
创建完这几条规则之后，在”场景联动“页面中点击规则后面的启动按钮，则所有的规则都会运行起来。
<div align="center">
<img src=./../../../images/声音检测_开启场景联动规则.png width=80%/>
</div>

## 3、设备端开发

### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“噪音检测系统”案例点击“立即创建”即可。
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

3. **修改设备端上报声音信息所用标识符**

&emsp;&emsp;
main.py中下面的代码实现的是上传声音大小到云端的功能。其中SoundDecibelValue便是声音值上报时所用的标识符。
```python
# 上传声音大小到物联网平台
def upload_voice():
    global device

    while True:
        data = get_voice()                      # 读取声音大小信息
        # 生成上报到物联网平台的属性值字串
        prop = ujson.dumps({
            'SoundDecibelValue': data
            })
        print('uploading data: ', prop)

        upload_data = {'params': prop}
        # 上传声音信息到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)
```
&emsp;&emsp;
确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/声音检测_noise_detecor_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* 新增四个变量用来控制led和蜂鸣器对应的GPIO
```python
# LED 蜂鸣器控制变量
redledon = 0
blueledon = 0
greenledon =0 
buzzeron =0
```

* 新增函数处理云端信息并控制GPIO的接口
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global redledon, blueledon, greenledon, buzzeron, voice_value

    payload = ujson.loads(request['params'])

    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "light_up_green_led" in payload.keys():
        greenledon = payload["light_up_green_led"]
        if (greenledon):
            print("点亮绿灯")

    if "light_up_blue_led" in payload.keys():
        blueledon = payload["light_up_blue_led"]
        if (blueledon):
            print("点亮蓝灯")

    if "light_up_red_led" in payload.keys():
        redledon = payload["light_up_red_led"]
        if (redledon):
            print("点亮红灯")

    if "switch_on_buzzer" in payload.keys():
        buzzeron = payload["switch_on_buzzer"]
        if (buzzeron):
            print("打开蜂鸣器")

    redled.write(redledon)           # 控制红灯
    blueled.write(blueledon)         # 控制蓝灯
    greenled.write(greenledon)       # 控制绿灯
    buzzer.write(buzzeron)           # 控制蜂蜜器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'light_up_green_led': greenledon,
        'light_up_blue_led': blueledon,
        'light_up_red_led':redledon,
        'switch_on_buzzer':buzzeron,
    })

    upload_data = {'params': prop}
    # 上报led和蜂鸣器属性到云端
    device.postProps(upload_data)
```

<br>

## 3、运行结果

### 3.1、本地运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “物联网平台连接成功” 代表成功连接到物联网平台
* "uploading data:"之后跟着的为设备端向云端发布的声音大小属性信息，其中SoundDecibelValue后面的数值为声音大小之，单位：分贝。

* 设备端解析到云端送下来的控制指令包含“{"light_up_green_led":1}”之后会打印"点亮绿灯"并点亮绿色LED
* 设备端解析到云端送下来的控制指令包含"{"light_up_blue_led":1}"之后会打印"点亮蓝灯"并点亮蓝色LED
* 设备端解析到云端送下来的控制指令包含"{"light_up_red_led":1}"之后会打印"点亮红灯"并点亮红色LED
* 设备端解析到云端送下来的控制指令包含"{"switch_on_buzzer":1}"之后会打印"打开蜂鸣器"并打开蜂鸣器


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
('192.168.43.45', '255.255.255.0', '192.168.43.1', '192.168.43.1')
establish tcp connection with server(host='gv4cyhcq5aA.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
uploading data:  {"SoundDecibelValue": 0}
点亮绿灯
uploading data:  {"SoundDecibelValue": 0}
点亮绿灯
uploading data:  {"SoundDecibelValue": 0}
点亮绿灯
uploading data:  {"SoundDecibelValue": 10300}
点亮红灯
打开蜂鸣器
```
&emsp;&emsp;
开发板实际效果如下：
* 在声音低于40分贝时，可以看到日志中出现"点亮绿灯"的字样，伴随着绿色LED灯亮起
* 在声音在50和70分贝之间时，可以看到日志中出现“点亮蓝灯”的字样，伴随着蓝色LED灯亮起

* 在声音高于70分贝时，可以看到日志中出现"点亮红灯"的字样，伴随着红色LED灯亮起
* 在湿度高于90分贝时，可以看到日志中出现"点亮红灯" “打开蜂鸣器”的字样，伴随着红色LED灯亮起，蜂鸣器打开

### 3.2、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/声音检测设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板周围声音发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 4、物联网应用开发

&emsp;&emsp;
如果能够将噪音数据通过网页的形式直观的展现出来，用户就可以通过查看噪音的当前和历史数据，识别出噪音的规律，有助于得出有效的管控方法和应对措施。同时相关人员还可以远距离的查看数据，得出直观的了解和认识。

<br>

### 4.1、中控大屏目标
&emsp;&emsp;
本节课程主要目标是设计一个总控大屏的web应用，能实时的查看如下信息：
1. 过去3小时的声音变化实时信息
2. LED灯和蜂鸣器实时状态显示
3. 手动控制LED灯和蜂鸣器的开关状态

<br>

### 4.2、声音监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何花卉养植系统中控大屏的web应用程序的开发。

<div align="center">
<img src=./../../../images/声音检测_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

#### 4.2.1、**新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“声音监控系统”。
<div align="center">
<img src=./../../../images/声音监控系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

#### 4.2.2、**新建“Web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个Web应用，命名为“室内声音大小实时监控”。
<div align="center">
<img src=./../../../images/声音监控系统_室内声音大小实时监控.png width=80%/>
</div>

&emsp;&emsp;
Web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/声音监控系统_室内声音大小实时监控_页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

#### 4.2.3、**页面设计**
&emsp;&emsp;
这里我们用到3种组件:
* 1个实时曲线
用于显示声音的历史变化曲线及实时数据
* 3个指示灯
显示和控制LED的当前状态
* 1个开关
显示和控制蜂鸣器的状态


&emsp;&emsp;
将3种组件拖到中间的画布区，适当调整组件布局。然后拖动文字组件，为组件添加文字注释，如下图所示。

<div align="center">
<img src=./../../../images/声音监控系统_室内声音大小实时监控_组件排列.png width=80%/>
</div>

<br>

#### 4.2.4、**关联产品和设备**
&emsp;&emsp;
此时回到”声音监控系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/声音监控系统_关联产品和设备.png width=80%/>
</div>

&emsp;&emsp;
关联产品的过程如下：
<div align="center">
<img src=./../../../images/声音监控系统_关联产品.png width=80%/>
</div>

&emsp;&emsp;
关联设备的过程如下：
<div align="center">
<img src=./../../../images/声音监控系统_关联设备.png width=80%/>
</div>
&emsp;&emsp;
产品和设备关联完毕之后，就可以将把组件和设备的属性关联起来了。

<br>

#### 4.2.5、**关联数据源**
&emsp;&emsp;
关联数据源分为如下3个步骤，每个步骤的截图如下：
* 关联产品
* 关联设备
* 关联属性

&emsp;&emsp;
具体操作步骤如下：
1. 选中”开关“组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/声音监控系统_开关_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/声音监控系统_开关_配置数据源_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/声音监控系统_开关_配置数据源_选择设备.png width=80%/>
</div>


4. 选择“开蜂鸣器”属性
<div align="center">
<img src=./../../../images/声音监控系统_开关_配置数据源_选择属性.png width=80%/>
</div>

&emsp;&emsp;
同样的方式为另三个”指示灯“组件分别选择选择相同设备的“点亮绿灯”，“点亮蓝灯”和“点亮红灯”属性。

&emsp;&emsp;
接下来需要为”实时曲线“组件设定数据源。同样点击该组件后，点击右边的配置数据源。因为本场景中的声音大小测量和LED灯/蜂鸣器的控制都是通过一个物联网设备完成的，所以本页中要选择”单设备多属性“。然后选择好本场景创建的产品，如下图所示。
<div align="center">
<img src=./../../../images/声音监控系统_实时曲线_选择产品.png width=80%/>
</div>

&emsp;&emsp;
选择相同的设备，如下图所示。
<div align="center">
<img src=./../../../images/声音监控系统_实时曲线_选择设备.png width=80%/>
</div>

&emsp;&emsp;
本场景显示的是茶花生长环境的温度信息和湿度信息，所以”数据项“中要选择这两项属性。
<div align="center">
<img src=./../../../images/声音监控系统_实时曲线_选择属性.png width=80%/>
</div>

&emsp;&emsp;
最后选择要显示的实时温湿度数据的时间长度，这里选择3小时，如下图所示。
<div align="center">
<img src=./../../../images/声音监控系统_实时曲线_选择数据时间短.png width=80%/>
</div>

<br>

#### 4.2.6、**发布上线**
<div align="center">
<img src=./../../../images/声音监控系统_IoT_Studio_预览和发布.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下图所示：

<div align="center">
<img src=./../../../images/声音监控系统_运行效果.png width=80%/>
</div>

<br>

&emsp;&emsp;
这样我们就完成了一个声音监控系统从设备声音大小测量，设备控制、物联网云平台开发及物联网应用开发全链路的开发。
