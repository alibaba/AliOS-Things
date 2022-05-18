# 智能皂液器
&emsp;&emsp;
下图是本案例除硬件连线外的4步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/soap_dispenser/智能皂液器系统_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
智能皂液器是一种非接触式的皂液投放设备，由微控制器、红外传感器、液位检测传感器、舵机等关键部件构成，其特点是非接触，定量出液，液量数据实时监控，欠液提醒等。可被广泛应用于酒店，机场，办公园区等公共卫生间。

### 模块介绍
&emsp;&emsp;
红外传感器具有一对红外发射与接收管，发射管发射一定频率的红外线，当遇到遮挡时，红外线反射回来被接收管接收，探测距离可通过电位器设置阈值。据此可知传感器前方是否有遮挡。该类型的传感器可被用于，避障车，自动门等众多场景。
<div align="center">
<img src=./../../../images/soap_dispenser/红外传感器.png width=50%/>
</div>
&emsp;&emsp;
水位监测传感器主要用于测量水位深度的传感器，该传感器测量范围在0cm-4cm，通过使用一系列五个连接到系统接地的裸露走线来工作。在每两个接地迹线之间交错插入一个感测迹线，五个接地迹线和总共五个感测迹线。
<div align="center">
<img src=./../../../images/soap_dispenser//ext_水位监测_定义尺寸.png  width=50%/>
</div>

## 准备
硬件列表
* ESP32开发板一套  
* 红外传感器一个  
* 水位传感器一个  
* 舵机一个  
* 杜邦连接线若干  

ESP32各开发板和红外传感器、水位传感器、舵机之间的硬件连线
<table>
    <tr>
        <td align="center">硬件</td>
        <td align="center">红外传感器</td>
        <td align="center">水位传感器</td>
        <td align="center">舵机</td>
        <td align="center">乐鑫ESP32开发板</td>
        <td align="center">安信可NODEMCU-32开发板</td>
        <td align="center">01Studio pyWIFI-ESP32开发板</td>
    </tr>
    <tr>
        <td align="center" rowspan="6">端口标识</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
    </tr>
    <tr>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">3.3V</td>
        <td align="center">3.3V</td>
        <td align="center">3.3V</td>
    </tr>    
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">VCC</td>
        <td align="center">5V</td>
        <td align="center">5V</td>
        <td align="center">5V</td>
    </tr>
    <tr>
        <td align="center">OUT</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P12</td>
        <td align="center">P12</td>
        <td align="center">P12</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">AO</td>
        <td align="center">-</td>
        <td align="center">ADC0</td>
        <td align="center">ADC0</td>
        <td align="center">ADC0</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">PWM</td>
        <td align="center">P27</td>
        <td align="center">P27</td>
        <td align="center">P27</td>
    </tr>
    <tr>
        <td align="center" rowspan="1">硬件说明</td>
        <td align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C48B2CEA2F29A00000001&dataId=800C48B2CEA2F29A" target="_blank">硬件积木说明</td>
        <td align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CA7848F143B5B00000001&dataId=800CA7848F143B5B&s=" target="_blank">硬件积木说明</td>
        <td align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5A3F64C2F88300000001&dataId=800C5A3F64C2F883" target="_blank">硬件积木说明</td>
        <td align="center" colspan="3" href="https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b17028610.0.0.1a3f60b12fN6Lx#/Python/docs/zh-CN/startup/ESP32_startup" target="_blank">快速开始详细端口定义</td>
    </tr>
</table>


### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/soap_dispenser/智能皂液器连线图.png width=90%/>
</div>
<br>

<br>

## 物联网平台开发
### 开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_创建产品.png
 width=80%/>
</div>

&emsp;&emsp;
在新建产品页面按下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/soap_dispenser/皂液器设备.png width=50%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“皂液器”的产品，如下图所示。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_产品列表页.png width=80%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_产品详情页面.png width=80%/>
</div>

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_创建自定义物模型1.png width=60%/>
</div>

&emsp;&emsp;
然后按照下图选择属性添加。其中“标识符”是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。这里标识符soap_status代表皂液器中剩余皂液量，数据类型为字符串。
<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_创建自定义物模型2.jpeg width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚刚创建的物模型属性。此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中了，之后便可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_产品页_管理设备.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_添加设备入口.png width=100%/>
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
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<br>

#### **获取设备三元组**
&emsp;&emsp;
如图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_设备三元组.png width=50%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
点击上图中“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“皂液量“属性值就会显示设备最新的属性信息。

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>

## 设备端开发
### 开发环境
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于hellworld的python工程，设定好工程名称（“soap_demo”）及工作区路径之后，硬件类型选择nodemcu32s，解决方案选择“helloworld”，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/soap_dispenser/创建soap工程.png width=60%/>
</div>

### 案例代码
&emsp;&emsp;
将[皂液器系统](./code/)文件下所有脚本代码复制后，覆盖刚创建的“soap_demo”工程根目录下的文件。

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
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息。

3. **修改设备端上报数据所用标识符**

&emsp;&emsp;
main.py中下面的代码功能是将液位传感器检测到的皂液剩余量上传到云端的功能。其中Soap_volume便是数据上报云端所用的标识符。

```python
    # 生成上报到物联网平台的属性值字串
    # 此处的属性标识符"soap_status"必须和物联网平台的属性一致
    upload_data = {'params': ujson.dumps({'soap_status': soap_status})}
    # 上传数据到物联网平台
    device.postProps(upload_data)
```

<br>

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到ESP32之后并运行，串口会周期性的打印如下日志：

```log
==== python execute from /data/pyamp/main.py ====
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.43.89', '255.255.255.0', '192.168.43.1', '192.168.43.1')
establish tcp connection with server(host='h47gOUih0Ox.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
hand is here .....

hand is here .....

hand is here .....

hand is here .....

hand is here .....
```

### 物联网平台端设备信息查看

&emsp;&emsp;
设备系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/soap_dispenser/皂液器系统_设备三元组.png width=80%/>
</div>

&emsp;&emsp;
此时如果改变液位传感器插入液体中的深度，物联网平台的物模型数据会更新为设备上报的最新的属性值。通过点击查看数据，可以看到一段时间监测到的皂液量数值。

