# 紫外线检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
紫外线是阳光光线中的一部分。适当紫外线强度的照射有助于人体合成维生素D，然而，过量的紫外线暴晒，会对人体造成损害，诱发皮肤疾病。

&emsp;&emsp;
本案例的紫外线检测系统可以实时检测紫外线光强度, 并在紫外线强度过高时提醒作出相应的防护。

&emsp;&emsp;
紫外线的强度等级分为0-15级别，通常，夜间的紫外线指数为0，热带、高原地区、晴天时的紫外线指数为15。按照对人体的影响，在紫外线强度等级的基础上，有以下划分：
1. 紫外线强度为0、1、2时，表示紫外线量最小，这个量对人体基本上没有影响
2. 紫外线强度为3或4时，表示紫外线量是比较低的，对人体的影响也是比较小的
3. 紫外线强度为5和6时，表示紫外线量是中等强度，对人体皮肤也有中等强度的伤害影响
4. 紫外线强度为7、8、9时，表示有较强的紫外线照射强度，这时，对人体的影响就比较大，需要采取相应的防护措施
5. 紫外线强度大于10时，表示紫外线照射量非常强，对人体有最大的影响，必须采取防护措施

&emsp;&emsp;
本案例通过紫外线传感器测量环境紫外线强度，并根据不同的紫外线强度点亮不同颜色的LED灯或者打开蜂鸣器：
   * 紫外线强度低于或等于4点亮绿灯
   * 紫外线强度在5和6之间点亮蓝灯
   * 紫外线强度在7和9之间点亮红灯
   * 紫外线强度大于9开蜂鸣器

## 准备
&emsp;&emsp;
本案例需要如下硬件：
* ESP32开发板一台
* 紫外线传感器一个
* RGB三色LED灯一个
* 杜邦线若干
* Micro-USB数据线一条
* 蜂鸣器一个

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_硬件连线图.png width=80%/>
</div>

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
 width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择自定义品类，节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_新建产品页面.png
 width=40%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“紫外线检测系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。
> 物模型中的“地理位置”属性本案例后面的内容没有用到，读者可忽略。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择直接新建方式来创建此产品需要的物模型信息，点击上图中的”编辑草稿“按钮。然后如下图再点击“添加自定义功能”。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_编辑草稿.png width=80%/>
</div>

&emsp;&emsp;
如下图添加一个“红色LED”的功能。其中标识符为redledSwith, 0表示LED灯关闭，1表示LED灯开启。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加自定义功能01.png width=40%/>
</div>
&emsp;&emsp;
用同样的方式分别添加“蓝色LED”、“绿色LED”、“蜂鸣器”  和 “紫外线强度”功能，标识符分别为 blueSwitch, greeSwitch，buzzer 和 uvlevel

蓝色LED:
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加自定义功能02.png width=40%/>
</div>

绿色LED:
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加自定义功能03.png width=40%/>
</div>

蜂鸣器:
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加自定义功能04.png width=40%/>
</div>

紫外线强度:
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加自定义功能05.png width=60%/>
</div>

&emsp;&emsp;
添加完属性之后，点击左下角的“发布上线”按钮 ，根据系统提示进行确认即可，如下图所示：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_属性发布上线.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_属性发布上线_确认.png width=40%/>
</div>

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。
<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_管理设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加设备入口.png width=100%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备详细信息.png width=100%/>
</div>

<br>

##### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_三元组信息.png width=60%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“紫外线强度“，”蜂鸣器“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备物模型数据.png width=100%/>
</div>
<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

## 创建解决方案

&emsp;&emsp;
点击下图中的"快速开始"按键(<img src=./../../../images/ultraviolet_detector/紫外线检测系统_HaaS_Studio_创建工程按钮.png width=8%/>)会弹出HaaS Studio的欢迎页面，请点击“创建项目”按钮。

&emsp;&emsp;
在随后弹框中，设定好项目名称（“uvdetector）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_创建uvdetector工程_ESP32.png width=80%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖uvdetector工程目录下的原有文件。其中main.py脚本的内容如下图所示：

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_uvdetector工程_ESP32.png width=80%/>
</div>


&emsp;&emsp;
然后对main.py里边的内容后需要完成以下三处代码修改。
> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**
&emsp;&emsp;
修改uvdetector工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的wlan.connect(wifiSsid, wifiPassword)语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改nvdetector工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：
```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```

3. **修改设备端上报温湿度信息所用标识符**

&emsp;&emsp;
uvdetector工程里main.py中下面的代码实现的是上传紫外线强度值到云端的功能。其中uvlevel便是紫外线强度上报时所用的标识符。
```python
# 上传紫外线等级信息到物联网平台
def upload_uvlevel():
    global device

    while True:
        data = get_uvlevel()                      # 读取紫外线等级
        # 生成上报到物联网平台的属性值字串
        prop = ujson.dumps({
            'uvlevel': data
            })
        print('uploading data: ', prop)

        upload_data = {'params': prop}
        # 上传紫外线等级值到物联网平台
        device.postProps(upload_data)
        utime.sleep(2)
```
&emsp;&emsp;
确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_属性标识符修改.png
 width=80%/>
</div>

&emsp;&emsp;
<b>下面是本案例重点代码段落讲解：</b>

* 定义4个变量用来控制LED和蜂鸣器对应的GPIO
```python
# LED灯和蜂鸣器状态
redledon = 0
greenledon = 0
blueledon = 0
buzzeron = 0
```

* 定义函数处理云端信息并控制GPIO的接口
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global redledon, greenledon, blueledon, buzzeron, redleddev, blueleddev, greenleddev, buzzerdev

    payload = ujson.loads(request['params'])

    if "redledSwith" in payload.keys():
        redledon = payload["redledSwith"]
        if (redledon):
            print("点亮红色LED")

    if "blueledSwitch" in payload.keys():
        blueledon = payload["blueledSwitch"]
        if (blueledon):
            print("点亮蓝色LED")

    if "greenledSwitch" in payload.keys():
        greenledon = payload["greenledSwitch"]
        if (greenledon):
            print("点亮绿色LED")

    if "buzzer" in payload.keys():
        buzzeron = payload["buzzer"]
        if (buzzeron):
            print("打开蜂鸣器")

    redleddev.write(redledon)                  # 控制红灯开关
    blueleddev.write(blueledon)                # 控制蓝灯开关
    greenleddev.write(greenledon)              # 控制绿灯开关
    buzzerdev.write(buzzeron)                  # 控制蜂鸣器

    # 要将更改后的状态同步上报到云平台
    prop = ujson.dumps({
        'redledSwith': redledon,
        'blueledSwitch': blueledon,
        'greenledSwitch': greenledon,
        'buzzer': buzzeron,
    })

    upload_data = {'params': prop}
    # 上报LED和蜂鸣器属性到云端
    device.postProps(upload_data)

```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志，其中：

* “物联网平台连接成功”代表成功连接到物联网平台。
* “uploading data:  {"uvlevel": 0}” 代表在向云平台上传紫外线强度等级(uvlevel)，等级值为0。

```log
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
uv inited!
LED inited!
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
establish tcp connection with server(host='gv4cPr38bfE.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
uploading data:  {"uvlevel": 0}
```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从“未激活”状态变为“上线”，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板紫外线强度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 物联网平台控制LED灯和蜂鸣器
&emsp;&emsp;
物联网设备上线之后，可以通过”监控运维“中的"在线调试"功能进行调试，详细操作步骤见下图：

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_物联网平台在线调试功能.png width=50%/>
</div>

&emsp;&emsp;
此产品的物模型属性中，"紫外线强度"设置的是只读，也就是说智能从设备端读取，不支持设置此状态到设备端，所以点开"紫外线强度"后面的”调试“之后，里边只有获取的选项。三个LED和蜂鸣器设置的是可读可写，所以点开”红色LED“后面的”调试“之后，里边有”获取“、”设置”和“设置期望值”三个选项。
这里可以选择打开"绿色lED"之后点击“设置”进行绿色LED功能的调试。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_云端打开绿色LED灯.png width=80%/>
</div>

&emsp;&emsp;
此时查看设备端绿色LED是否已经打开，打开成功则代表控制成功。

## 物联网应用开发

### 紫外线强度监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何完成火焰监控系统web端应用程序的开发。

<div align="center">
<img src=./../../../images/1_物联网应用开发的一般流程.png width=50%/>
</div>

<br>

### **新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“紫外线强度监控系统”。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

### **新建“web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个web应用，命名为“紫外线实时监控”。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_web应用创建.png width=80%/>
</div>


&emsp;&emsp;
web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_Web页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

### **页面设计**
&emsp;&emsp;
这里我们用到3种组件:
* 实时曲线一个
用于紫外线强度的历史变化曲线及实时数据
* 指示灯三个
显示和控制LED灯
* 开关一个
显示和控制蜂鸣器

将组件拖到中间的画布区，适当调整组件布局，添加文字，如下图所示。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_实时监控页面设计.png width=80%/>
</div>

<br>

### **关联产品和设备**
&emsp;&emsp;
此时回到”火焰监控系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_关联产品和设备.png width=80%/>
</div>


&emsp;&emsp;
关联产品和设备的过程如下，选中左下角的“关联产品的同时关联其下所有设备”之后 ，该产品下创建的所有的产品都会被关联到这个项目中。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_关联产品和设备01.png width=80%/>
</div>

&emsp;&emsp;
产品和设备关联完毕之后，就可以将把组件和设备的属性关联起来了。

<br>

### **关联数据源**
&emsp;&emsp;
关联数据源分为如下3个步骤，每个步骤的截图如下：
* 关联产品
* 关联设备
* 关联属性

&emsp;&emsp;
具体操作步骤如下：
1. 选中”红色指示灯“组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_红色指示灯_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_红色指示灯_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_红色指示灯_选择设备.png width=80%/>
</div>


4. 选择“红色LED”属性
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_红色指示灯_配置数据源_红色LED.png width=80%/>
</div>

&emsp;&emsp;
选择好产品、设备和属性之后，需要修改指示灯大小及其展示样式（设置为颜色），并且分别为”红色“/”灰色“两种颜色，如下图所示：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_红色指示灯_配置数据源_颜色.png width=80%/>
</div>


&emsp;&emsp;
同样的方式为“蓝色LED”, "绿色LED", "蜂鸣器"，和 ”实时曲线“设置，如下图所示。

蓝色LED:

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_蓝色指示灯_配置数据源.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_蓝色指示灯_配置数据源_颜色.png width=80%/>
</div>


绿色LED:

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_绿色指示灯_配置数据源.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_绿色指示灯_配置数据源_颜色.png width=80%/>
</div>

实时曲线:

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_实时曲线_配置数据源.png width=80%/>
</div>

<br>

### **业务逻辑开发**
&emsp;&emsp;
业务逻辑的主要目的是为了让用户设定物联网设备端的行为逻辑，常规物联网系统都是在设备端固化行为逻辑，出厂之后如果需要修改设备的行为，则需要进行OTA升级。本节课程则向读者介绍如何通过IoT Studio完成业务逻辑的开发。

&emsp;&emsp;
如下图，新建一条名为“紫外线监控报警设定01”的规则，用于紫外线强度<=4时只点亮绿灯
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线监控系统_创建业务逻辑.png width=80%/>
</div>

&emsp;&emsp;
系统会自动进入到业务逻辑编辑页面，如下图所示，点击左侧的“节点”按钮，则可以看到所有可用的节点选项。右侧红框是如何设计一个业务逻辑的介绍。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线监控报警.png width=80%/>
</div>

1. 选择目标节点

&emsp;&emsp;
此逻辑需要根据“紫外线检测系统”上报的“紫外线强度”不同的强度等级值作出打开不同颜色的灯和蜂鸣器的操作。所以需要如下3个节点：
* 设备触发节点
* 条件判断节点
* 开启LED或者蜂鸣器节点

&emsp;&emsp;
分别从左侧拖动“设备触发”，1个“条件判断”和1个“紫外线检测系统”3个节点到右侧的业务逻辑编辑框。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_节点列表.png width=80%/>
</div>

1. 建立节点间的关联关系

&emsp;&emsp;
按照预设的逻辑建立，如下图所示（在节点的一侧按下鼠标拖动到另一节点的一侧即可建立连线关系）。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_节点逻辑关系建立.png width=80%/>
</div>

1. 业务逻辑编辑
* 设备触发节点

&emsp;&emsp;
此设备触发选择“uvdetectordev”的“紫外线强度”属性即可，如下图所示（和前面“组件”设定类似，同样是鼠标选中第节点，在右侧的配置选项中进行配置）。
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备触发节点配置.png width=80%/>
</div>

* 条件判断节点

&emsp;&emsp;
此处我们将条件判断分别设定为： 紫外线强度<=4 只点亮绿灯

条件判断由一个条件构成：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置.png width=80%/>
</div>

* 设备节点行为设定：

分别为设备节点设定点亮和关闭灯以及开关蜂鸣器的行为，如下图所示。

只点亮绿灯：
<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备节点行为设定01.png width=80%/>
</div>


用同样的方法新建：

1. 名为“紫外线监控报警设定02”的规则，用于紫外线强度<=6 并且 >=5 时只点亮蓝灯
2. 名为“紫外线监控报警设定03”的规则，用于紫外线强度<=9 并且 >=7 时只点亮红灯
3. 名为“紫外线监控报警设定04”的规则，用于紫外线强度 >9 时 只点亮红灯 并且打开蜂鸣器

“紫外线监控报警设定02”如下图：

    设备触发：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备触发节点配置02.png width=80%/>
</div>


     条件判断由两个条件”与“构成：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置02-1.png width=80%/>
</div>

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置02-2.png width=80%/>
</div>

    设备行为：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备节点行为设定02.png width=80%/>
</div>

“紫外线监控报警设定03”如下图：

    设备触发：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备触发节点配置03.png width=80%/>
</div>

    条件判断由两个条件”与“构成：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置03-1.png width=80%/>
</div>

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置03-2.png width=80%/>
</div>

    设备行为：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备节点行为设定03.png width=80%/>
</div>



“紫外线监控报警设定04”如下图：

    设备触发：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备触发节点配置04.png width=80%/>
</div>

    条件判断由1个条件构成：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_比较节点配置04.png width=80%/>
</div>

    设备行为：

<div align="center">
    <img src=./../../../images/ultraviolet_detector/紫外线检测系统_设备节点行为设定04.png width=80%/>
</div>


* 业务逻辑保存和部署

&emsp;&emsp;
依此点击右上角的“保存”和“部署”按钮，即可将此业务逻辑设定完毕。
<br>

<br>

### **预览和发布上线**

&emsp;&emsp;
业务逻辑设定完毕之后，可以在“紫外线强度检测系统”页面编辑页面点击“预览”按钮进行预览，如下图所示。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统_预览.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下所示，同时可以扫描二维码在手机上面查看实际运行效果。

<div align="center">
<img src=./../../../images/ultraviolet_detector/紫外线检测系统效果.png width=80%/>
</div>

&emsp;&emsp;
此时查看设备上面的报警灯的状态会同步和web应用的报警灯状态同步显示。

<br>

&emsp;&emsp;
这样整个紫外线检测系统的趣味案例就完成了。
