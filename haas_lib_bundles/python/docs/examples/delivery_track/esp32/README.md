# 外卖跟踪系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/外卖跟踪系统_步骤概述.png width=80%/>
</div>

## 简介
&emsp;&emsp;
随着人们对外卖的需求日益提高，外卖的配送服务却因为缺少智能化而体验并未很理想，所以本项目设计一个智能外卖箱模块，让顾客可以通过web端随时查看自己的外卖的温度和位置等，从而能够更好的享受外卖服务。

### 背景知识
&emsp;&emsp;
本系统的核心在于如何确定外卖的实时位置和温度，并及时推送到web端。

&emsp;&emsp;
HT2828Z3G5L是一款高性能的面向车载组合导航领域的定位G-MOUSE，系统包含同时支持北斗和GPS的高性能卫星接收机芯片。

BM280是博世出品的气压温度传感器，气压、温度可以通过SPI或I2C读出，在室内导航、无人机和气象站等场景有广泛应用。

利用HT2828Z3G5L提供的坐标信息和BMP280提供的温度信息，将其上报到物联网平台。依靠IoT Studio的 **``Web应用``** 开发工具，可以及时把状态通知到Web端，实现外卖的远程监控。


&emsp;&emsp;
HT2828Z3G5L定位模块可提供1-10HZ的定位更新速率、小于5米的定位精度以及小于0.5Degrees的方向精度。

<div align="center">
<img src=./../../../images/外卖跟踪系统_HT2828Z3G5L模块.png width=50%/>
</div>

&emsp;&emsp;
BMP280气压温度传感器可提供0.01摄氏度的温度测试精度和0.01hPa的气压测试精度。

<div align="center">
<img src=./../../../images/外卖跟踪系统_BMP280传感器.png width=50%/>
</div>

## 准备
ESP32开发板一套

HT2828Z3G5L定位模块一个

BMP280气压温度传感器一个

杜邦连接线若干
### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/外卖跟踪系统_ESP32_连线.png width=90%/>
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
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **``创建产品``** 按钮，如下图所示。
<div align="center">
<img src=./../../../images/外卖跟踪系统_空产品页.png
 width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/外卖跟踪系统_新建产品.png width=70%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“外卖跟踪系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/外卖跟踪系统_产品列表页.png width=80%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/外卖跟踪系统_尚未添加任何功能.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合**数据定义**中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/外卖跟踪系统_创建自定义物模型1.png width=80%/>
</div>

然后按照下图选择外卖跟踪系统属性添加。
<div align="center">
<img src=./../../../images/外卖跟踪系统_添加自定义功能1.png width=50%/>
</div>

<div align="center">
<img src=./../../../images/外卖跟踪系统_添加自定义功能2.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚才创建的物模型属性。其中temperature代表外卖的温度，数据类型为double型；GeoLocation代表外卖的坐标位置，数据类型为struct型。此时点击“确定”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中。功能定义完成后点击 **``发布``** 按钮完成发布


<div align="center">
<img src=./../../../images/外卖跟踪系统_发布物模型.png width=80%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击 **``外卖跟踪系统``** 后的 **``管理设备``**，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/外卖跟踪系统_设备管理.png width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_添加设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称(这里我们填入设备名称delivery_track1)，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/1_添加设备.png width=50%/>
</div>

&emsp;&emsp;
点击 **确认** 后弹出下面的“添加完成“提示框，点击 **``前往查看``** 按钮就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<div align="center">
<img src=./../../../images/外卖跟踪系统_物模型数据.png width=80%/>
</div>

<br>

#### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_设备三元组_马赛克.png width=80%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“外卖温度“和“地理位置”属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/外卖跟踪系统_物模型数据.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>

## 设备端开发
### 开发环境
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 开发原理
    (1)BMP280采集外卖温度，1s向云端上报一次
    (2)HT2828Z3G5L采集外卖位置信息，每1s采集一次，当当前坐标和上一次坐标信息不一样时，向云端上报一次

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于hellworld的python工程，设定好工程名称（``delivery_track``）及工作区路径之后，硬件类型选择ESP32，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/外卖跟踪系统_esp32_1.png width=40%/>
</div>

### 案例代码
&emsp;&emsp;
将[外卖跟踪系统](./code/)文件下所有脚本代码复制后，覆盖刚创建的``delivery_track``工程根目录下的文件。

> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改delivery_track工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

```python
# Wi-Fi SSID和Password设置
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名字"
deviceSecret = "设备密钥"
```
</div>

<br>

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到ESP32之后并运行。

```log
>>> execfile("/data/pyamp/main.py")
wifi_connected: False
wifi_connected: False
E (6416) OTAPUT:: ===============HaaS token info===============

E (6416) OTAPUT:: HaaS device name: haas_8cce4e9a6c24

E (6426) OTAPUT:: device not activated

E (6426) OTAPUT:: =============================================

E (6436) OTAPUT:: starts connection with AIoT platform
establish mbedtls connection with server(host='a1ul4uS7RfV.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
wifi_connected: False
('172.16.2.125', '255.255.254.0', '172.16.2.1', '114.114.114.114')
success to establish mbedtls connection, fd = 54(cost 0 bytes in total, max used 0 bytes)
E (7886) DYNREGMQ:: AIOT_MQTTEVT_CONNECT
establish tcp connection with server(host='a1awp8q9axG.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=55
物联网平台连接成功
sleep for 2s
gnss inited!
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
temperature =  23.45
[0.0, 'W'] [0.0, 'N'] 0.0
```

<br>

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/外卖跟踪系统_设备状态及属性.png width=80%/>
</div>

<br>

## 物联网应用开发（定位数据可视化）

&emsp;&emsp;
本节中，我们将介绍如何快速实现将外卖的位置和温度信息实时显示在地图中。

&emsp;&emsp;
IoT Studio 提供了应用快速开发的能力，可以很方便地与物联网平台进行联动。本节的开发工作也将围绕 IoT Studio展开。
<br>

### 新建“普通项目”
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中新建一个空白项目，如下图所示，将此项目命名为“外卖跟踪系统”。
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_普通项目.png width=80%/>
<img src=./../../../images/外卖跟踪系统_iotstudio_创建空白项目.png width=80%/>
</div>

<br>

### 关联产品
&emsp;&emsp;
为了使本项目能够获取到目标设备的定位信息，我们首先需要将该项目和我们在前一节创建的产品“外卖跟踪系统”绑定。

&emsp;&emsp;
在项目控制台，点击左侧的“产品”，点击“关联物联网平台产品”。此时可以看见我们创建的“外卖跟踪系统”。点击选中，并勾选“关联产品同时关联其下所有设备”，以便该项目可以访问到所有设备的定位信息。
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_关联产品.png width=80%/>
</div>

### 创建“Web应用”
&emsp;&emsp;
在项目控制台，点击左侧的“主页”，在新项目的首页新建一个Web应用，命名为“外卖监控”。
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_web应用.png width=80%/>
</div>

&emsp;&emsp;
Web应用创建成功后自动跳转到应用界面设计页面。点击左侧栏中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

&emsp;&emsp;
为了能够将设备显示在地图上，我们选用“设备地图”组件。鼠标悬停在该组件上以查看详细信息。
<div align="center">
<img src=./../../../images/外卖跟踪系统_设备地图.png width=80%/>
</div>

&emsp;&emsp;
将“设备地图”组件拖拽至画布，并调整大小。此时，地图上还未能正确显示设备。我们需要点击右侧“编辑设备地图”。
<div align="center">
<img src=./../../../images/外卖跟踪系统_页面布局.png width=80%/>
</div>

&emsp;&emsp;
点击右侧“编辑设备地图”后，我们可以在右侧“选择产品”中，选择我们刚刚和项目绑定的“外卖跟踪系统”,点击确定。
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_选择产品.png width=80%/>
</div>

&emsp;&emsp;
点击确认后，可以看到在地图上显示了设备的坐标信息
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_设备坐标信息.png width=80%/>
</div>

&emsp;&emsp;
配置显示外卖温度信息，点击右侧外卖跟踪系统的星号，点击配置属性，勾选外卖温度，点击确定。
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_设备温度信息.png width=80%/>
</div>

&emsp;&emsp;
在配置完外卖温度显示后，再点击地图上的外卖，即可看到外卖的温度信息
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_显示设备温度信息.png width=80%/>
</div>

&emsp;&emsp;
上面的配置都完成后，可以点击右上角的保存
<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_保存.png width=80%/>
</div>

<br>

​
### 发布上线

&emsp;&emsp;
回到WEB应用页面，可以看到地图上的设备定位已经正常显示，可以进行发布。在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下图所示,预览没问题可以点击右上角的发布了。

<div align="center">
<img src=./../../../images/外卖跟踪系统_iotstudio_预览.png width=100%/>
</div>

<br>

&emsp;&emsp;
到此为止，外卖跟踪系统的案例就已经完成了。
