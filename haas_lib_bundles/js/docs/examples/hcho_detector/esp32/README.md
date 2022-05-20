# 甲醛浓度检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/2_甲醛检测系统_步骤概述.jpg width=70%/>
</div>

## 简介
&emsp;&emsp;
甲醛(化学式HCHO或CH2O)是一种有特殊刺激气味的气体，对人的眼睛和鼻子有强烈的刺激作用。若空气中甲醛浓度过高，比如新装修的房子、新买的汽车等，可引起中毒反应，严重的可致癌。甲醛是空气中的杀手之一，需要时刻提防。

### 背景知识
&emsp;&emsp;
本系统的核心在于如何精准的检测家庭中的甲醛浓度。现在市面上有很多种甲醛检测的仪器，大多企业在用的都是通过电化学的方法来检测。
<div align="center">
<img src=./../../../images/2_电化学甲醛传感器.jpeg width=30%/>
</div>
&emsp;&emsp;
电化学甲醛传感器对比其它方式来检测的的抗干扰能力强，响应专一，灵敏度高，测量结果精确，检测下限低，恢复-响应特性佳。

&emsp;&emsp;
本节选用的是HOCO甲醛传感器，可精确测量空气中的甲醛浓度，并能抑制干扰气体，具有稳定性高、抗干扰气体能力强等特点。分辨率高达0.01ppm，支持3.3~6V宽电压输入，具备良好的兼容性，并且使用寿命长达2年。 简单易用的Gravity接口、宽输入电压、支持模拟电压或者串口输出，几乎可兼容所有的主控器。

<div align="center">
<img src=./../../../images/2_DFROBOT甲醛传感器.jpeg width=30%/>
</div>

> 甲醛传感器在使用之前需要预热5分钟以上。


## 准备
1. ESP32开发板一套
2. HCHO甲醛传感器一个
3. 杜邦连接线若干

### 硬件连线图
&emsp;&emsp;
NODEMCU32S硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/2_甲醛检测_ESP32_连线.jpg width=90%/>
</div>
<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| S | G36 | 甲醛传感器信号测试点位 |
| VCC | 3.3V | 甲醛传感器供电点位 |
| GND | GND | 甲醛传感器地点位 |

## 一分钟上云体验

- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-甲醛浓度检测系统（JavaScript）.png width=40%/>
</div>

- 在完成上面的“硬件连线”步骤之后，点击“体验案例”按钮，即可立即体验本案例的实际运行效果。
<div align="center">
<img src=../../../images/1_一分钟上云_step2.png width=80%/>
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
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/2_新建甲醛检测设备.png width=50%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“甲醛检测”的产品，如下图所示。

<div align="center">
<img src=./../../../images/2_甲醛检测系统_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/2_甲醛检测系统_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/2_甲醛检测_创建自定义物模型1.png width=100%/>
</div>

然后按照下图选择甲醛浓度属性添加。
<div align="center">
<img src=./../../../images/2_甲醛检测_创建自定义物模型2.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚刚创建的物模型属性。其中HCHO代表甲醛传感器检测到的浓度值，数据类型为double浮点型，单位为ppm。此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中。


<div align="center">
<img src=./../../../images/2_甲醛检测_发布物模型.png width=100%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”甲醛检测“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/2_甲醛检测_产品页_管理设备.png width=100%/>
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

<div align="center">
<img src=./../../../images/2_甲醛检测设备详情.png width=100%/>
</div>

<br>

#### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_设备三元组_马赛克.png width=50%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“甲醛浓度“属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/2_甲醛检测设备详情.png width=100%/>
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
下载《[ESP32 hcho_detector示例](https://gitee.com/alios-things/amp/tree/master/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
hcho_detector
├── app.json         # 板级配置
└── app.js           # 应用代码
└── hcho.js          # 甲醛检测传感器驱动
```

### 修改wifi ssid和password

&emsp;&emsp;
修改hcho_detector工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

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
修改hcho_detector工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

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
hchoVoltage is 0.43 ppm
hchoVoltage is 0.43 ppm
hchoVoltage is 0.43 ppm
hchoVoltage is 0.43 ppm
```

<br>


### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/2_甲醛检测_设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板周围的甲醛浓度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。通过点击查看数据，可以看到一段时间监测到的甲醛浓度值。
<div align="center">
<img src=./../../../images/2_甲醛检测_查看数据.png width=100%/>
</div>

<br>

&emsp;&emsp;
到此为止，甲醛检测系统的案例就已经完成了。如果想要学习甲醛检测系统更详细的操作步骤，请参考“[甲醛检测系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/2-%E6%99%BA%E8%83%BD%E7%94%9F%E6%B4%BB/%E5%9C%BA%E6%99%AF1-%E5%AE%88%E6%8A%A4%E5%AE%B6%E5%BA%AD%E5%81%A5%E5%BA%B7/%E7%94%B2%E9%86%9B%E6%A3%80%E6%B5%8B/README.md)”中的说明。
