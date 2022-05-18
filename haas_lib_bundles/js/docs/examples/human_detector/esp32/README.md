# 人体检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/2_起夜灯_步骤概述.jpg width=60%/>
</div>

## 简介

&emsp;&emsp;
**红外线**是一种人类肉眼看不到的光，虽然看不见，但他有一个显著的特性就是具有热效应，即所有高于绝对零度的物质都会产生红外线。人体热释电红外传感器就是一种能探测人体红外光谱变化的传感器，他能检测人发射的红外线，并转化为电信号输出。

&emsp;&emsp;
使用一个控制器来接收人体热释电红外传感器的信号，通过这个信号来控制灯的开启和关闭，就可以制作一个实用的起夜灯。

<div align="center">
<img src=./../../../images/2_起夜灯_起夜灯原理.png width=60%/>
</div>

<br>

## 准备

1. ESP32开发板          一套
2. 人体热释电红外传感器   一个
3. 连接线               若干

### 硬件连线图
<div align="center">
<img src=./../../../images/2_2_起夜灯硬件接线图_esp32.jpg width=85%/>
</div>

<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| D | G36 | 人体热释电红外传感器信号测试点位 |
| VCC | 3.3V | 人体热释电红外传感器供电点位 |
| GND | GND | 人体热释电红外传感器地点位 |
| LED | G27 | 控制LED灯引脚 |

## 一分钟上云体验

- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-起夜灯（JavaScript）.png width=40%/>
</div>

- 在完成上面的“硬件连线”步骤之后，点击“体验案例”按钮，即可立即体验本案例的实际运行效果。
<div align="center">
<img src=../../../images/1_一分钟上云_step2.png width=80%/>
</div>

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

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png
 width=60%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/2_2_新建起夜灯设备.png
 width=60%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“起夜灯”的产品，如下图所示。

<div align="center">
<img src=./../../../images/2_2_起夜灯_产品列表页.png width=60%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/2_2_起夜灯_产品详情页面.png width=60%/>
</div>

&emsp;&emsp;
其中标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择直接新建方式来创建此产品需要的物模型信息，点击上图中的”编辑草稿“按钮。然后如下图再点击“添加自定义功能”。

<div align="center">
<img src=./../../../images/2_2_起夜灯_编辑草稿.png width=60%/>
</div>

&emsp;&emsp;
如下图添加一个“LED开关”的默认功能。其中标识符为LEDSwith, 0表示LED灯关闭，1表示LED开启。
<div align="center">
<img src=./../../../images/2_2_起夜灯_添加自定义功能.png width=60%/>
</div>

点击确定后回到前一个页面，再点击“发布上线”

<div align="center">
<img src=./../../../images/2_2_起夜灯_发布物模型.png
width=60%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”起夜灯“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/2_2_起夜灯_产品页_管理设备.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/2_2_起夜灯_添加设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/2_2_起夜灯_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/2_2_起夜灯_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/2_2_起夜灯_设备详情.png width=100%/>
</div>

<br>

### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击deviceSecret后面的“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/2_2_起夜灯_设备三元组.png width=50%/>
</div>

<br>

### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“LED开关“，属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/2_2_起夜灯_设备物模型数据.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。
<br>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保以下工作已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32-quick-start.md)的说明。
<br>
&emsp;&emsp;
(1) 烧录支持HaaS-JS开发的"ESP32 HaaS固件"。
<br>
&emsp;&emsp;
(2) 已安装ESP32 USB串口驱动。
<br>
&emsp;&emsp;
(2) 已下载HaaS-JS轻应用推送工具amp。
<br>

### 下载示例代码

&emsp;&emsp;
下载《[ESP32 human_detector示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
gas_detector
├── app.json         # 板级配置
└── app.js           # 应用代码
```

### 修改wifi ssid和password

&emsp;&emsp;
修改human_detector工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```javascript
var wifiOptions = {
    ssid: "请填写您的路由器名称",
    password: "请填写您的路由器密码",
    bssid: '',
    timeout_ms: 18000
  };
```

### 修改设备端三元组

&emsp;&emsp;
修改human_detector工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

```javascript
var iotOptions = {
    productKey: "请填写您在阿里云物联网平台申请的产品PK",
    deviceName: "请填写您在阿里云物联网平台申请的产品DN",
    deviceSecret: "请填写您在阿里云物联网平台申请的产品DS"});
    region: 'cn-shanghai',
    keepaliveSec: 60
  }
```
<br>

### 推送脚本至设备
&emsp;&emsp;
 注意: ESP32在Windows环境下执行amp-win工具下载JS脚本之前，需要先把ESP32的GPIO_0拉高（和3.3v脚短接），这样才能正常推送。Mac环境下则无此问题，不需该步骤。

&emsp;&emsp;
ESP32板子GPIO_0 硬件连线如下表所示：
|板子|板级pin脚|说明|
|-----|----|----|
| NODEMCU-32 | P0 | GPIO_0对应管脚 |
| M5StackCore2 | G0 | GPIO_0对应管脚 |

&emsp;&emsp;
执行以下amp命令。
> ./amp-clitool/dist/amp-macos/amp serialput ./fire_detector /dev/cu.wchusbserial537A003351 -m quickjs -e 0 -b 115200

&emsp;&emsp;
注意：amp工具路径和ESP32的串口ID请以实际为准。
<br>

## 运行结果
### 本地查看
&emsp;&emsp;
JavaScript脚本推送到ESP32之后 ，会自动运行，运行过程中日志如下。其中：

```bash
wifi connected
create IotDeviceClient
iot platform connected
human state is :0
human state is :1
human is here .....
postProps is called
human state is :0
postProps is called
human state is :1
human is here .....
postProps is called
human state is :0
postProps is called
```


<br>


### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/2_2_起夜灯_设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板周围有人经过，起夜灯就会开启，物联网平台的物模型数据会更新为设备上报的最新的属性值。

<br>

&emsp;&emsp;
到此为止，起夜灯数据上云的案例就已经完成了。如果想要学习起夜灯案例更详细的操作步骤，请参考“[起夜灯系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/2-%E6%99%BA%E8%83%BD%E7%94%9F%E6%B4%BB/%E5%9C%BA%E6%99%AF2-%E8%B5%B7%E5%A4%9C%E7%81%AF/README.md)”中的说明。
