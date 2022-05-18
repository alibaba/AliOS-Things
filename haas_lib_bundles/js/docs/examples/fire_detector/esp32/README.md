# 火焰检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/4_fire_detector_步骤概述.jpg width=60%/>
</div>

## 简介

&emsp;&emsp;
自从第一次工业革命开始到现代，化石燃料一直是很重要的能源之一，而煤在化石燃料中占比较高。煤转化成能量需要燃烧，不管是第一次工业革命的内燃机还是现在仍然占比很高的火力发电，都需要大量的烧煤。现代的火力发电系统会持续对炉膛中煤的燃烧情况进行监控，防止出现燃煤熄灭的情况。
本场景就是针对这个场景对火焰进行实时的监控，如果出现火焰熄灭的情况，则进行灯光报警。

### 背景知识

&emsp;&emsp;
火力发电系统中所用的火焰监控的核心在于如何精准的检测炉膛中的火焰强度，目前市场上已经有很多的传感器可以对火焰强度进行测量。
目前市面上用的最多的火焰传感器，即红外接收二极管，对火焰特别敏感。它对火焰产生的红外线非常敏感，当火焰亮度越大时，发出的红外线越多，火焰传感器管脚间的阻抗变小；当火焰变小时，发出的红外线变少，火焰传感器管脚间的阻抗变大。
本节选用的火焰传感器外观如下：
<div align="center">
<img src=./../../../images/4_火焰传感器外观图.png width=40%/>
</div>
<br>

## 准备

&emsp;&emsp;
硬件器材：
1. ESP32开发板一套
2. 火焰传感器一个
3. 连接线若干

&emsp;&emsp;
NODEMCU32S硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/4_ESP32_火焰传感器连线图.jpg width=90%/>
</div>
<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| A0 | G36 | 火焰传感器信号测试点位 |
| VCC | 3.3V | 火焰传感器供电点位 |
| GND | GND | 火焰传感器地点位 |


## 一分钟上云体验
- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-火焰检测系统（JavaScript）.png width=40%/>
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

<br>

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，创建云端产品的网址：https://iot.console.aliyun.com/product

&emsp;&emsp;
点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/4_新建火焰检测设备.png
 width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“火焰检测系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/4_火焰检测系统_产品列表页.png width=100%/>
</div>


<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/4_火焰检测系统_产品详情页面.png width=100%/>
</div>



&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择导入物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择本地文件[火焰检测系统物模型](./link_platform/）fire_detector_model.zip进行导入。

<div align="center">
<img src=./../../../images/4_火焰检测系统_发布物模型.png width=100%/>
</div>


&emsp;&emsp;
物模型导入成功后可以看到网页出现了我们刚刚导入的物模型属性。其中alarmState代表的是火焰的报警状态，1处于报警状态，0代表没有报警；fireVoltage代表火焰传感器检测到的电压值。


<div align="center">
<img src=./../../../images/4_火焰检测系统_物模型.png width=100%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后，就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”火焰检测系统“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/4_火焰检测系统_产品页_管理设备.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/4_火焰传感器_添加设备入口.png width=100%/>
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
<img src=./../../../images/4_火焰检测系统_设备详情.png width=100%/>
</div>

<br>

### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/4_火焰检测系统_设备三元组.png width=50%/>
</div>

<br>

### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“火焰检测电压“，”报警状态“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/4_火焰检测系统_设备物模型数据.png width=100%/>
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
下载《[ESP32 fire_detector示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
fire_detector
├── app.json         # 板级配置
└── app.js           # 应用代码
└── fire_mk002354.js # 火焰检测传感器驱动
```

### 修改wifi ssid和password

&emsp;&emsp;
修改fire_detector工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

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
修改fire_detector工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

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

```log
wifi connected
create IotDeviceClient
iot platform connected
>> fireVoltage is 35, alarm_on is 1
>> fireVoltage is 511, alarm_on is 0
...
```

> 打火机打着火后，远离和靠近火焰传感器的二极管，查看设备端日志的量测值是否有变化。
> 请务必注意安全！！！

<br>


## 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_火焰检测系统_设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板周围的火焰强度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

<br>

## 物联网平台控制报警灯状态
&emsp;&emsp;
物联网设备上线之后，可以通过”监控运维“中的"在线调试"功能进行调试，详细操作步骤见下图：
<div align="center">
<img src=./../../../images/1_火焰检测系统_物联网平台在线调试功能.png width=100%/>
</div>

&emsp;&emsp;
此产品的物模型属性中，"火焰检测电压值"设置的是只读，也就是说智能从设备端读取，不支持设置此状态到设备端，所以点开"火焰检测电压"后面的”调试“之后，里边只有获取的选项。”报警状态“设置的是可读可写，所以点开”报警状态“后面的”调试“之后，里边有”获取“、”设置”和“设置期望值”三个选项。
这里可以选择打开报警状态之后点击“设置”进行报警灯功能的调试。

<div align="center">
<img src=./../../../images/1_火焰检测系统_云端打开报警灯.png width=100%/>
</div>

&emsp;&emsp;
此时查看设备端LED是否已经打开，打开成功则代表控制报警灯成功。

<br>

## 物联网应用开发

### 火焰亮度监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何完成火焰监控系统web端应用程序的开发。

<div align="center">
<img src=./../../../images/1_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

### **新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“火焰监控报警系统”。
<div align="center">
<img src=./../../../images/4_火焰监控报警系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

### **新建“web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个web应用，命名为“火焰实时监控”。
<div align="center">
<img src=./../../../images/4_火焰监控系统_web应用创建.png width=80%/>
</div>


&emsp;&emsp;
web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/4_火焰监控系统_移动应用_页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

### **页面设计**
&emsp;&emsp;
这里我们用到3个组件:
* 实时曲线
用于显示火焰亮度的历史变化曲线及实时数据
* 指示灯
显示和控制空调和火焰的当前报警状态
* 设备地图
用于显示设备所在位置

将三个组件拖到中间的画布区，适当调整组件布局，如下图所示。

<div align="center">
<img src=./../../../images/2_火焰监控系统_实时监控页面设计.png width=80%/>
</div>

<br>

### **关联产品和设备**
&emsp;&emsp;
此时回到”火焰监控系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/4_火焰监控系统_关联产品和设备.png width=80%/>
</div>


&emsp;&emsp;
关联产品和设备的过程如下，选中左下角的“关联产品的同时关联其下所有设备”之后 ，该产品下创建的所有的产品都会被关联到这个项目中。
<div align="center">
<img src=./../../../images/4_火焰监控系统_关联产品和设备.png width=80%/>
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
1. 选中”指示灯“组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/4_火焰监控系统_指示灯_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/4_火焰监控系统_指示灯_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/4_火焰监控系统_开关_配置数据源_设备.png width=80%/>
</div>


4. 选择“报警灯”属性
<div align="center">
<img src=./../../../images/4_火焰监控系统_指示灯_配置数据源_报警灯.png width=80%/>
</div>

&emsp;&emsp;
选择好产品、设备和属性之后，需要修改指示灯大小及其展示样式（设置为图片），并且分别为”开始报警“/”停止报警“两种状态上传图片，如下图所示。图片位于[开启报警](../../../images/4_火焰熄灭示意图.png)和[关闭报警](../../../images/4_火焰燃烧示意图.png)。
<div align="center">
<img src=./../../../images/4_火焰监控系统_指示灯_配置数据源_报警灯图片.png width=80%/>
</div>


&emsp;&emsp;
同样的方式为”实时曲线“设置为目标设备的”火焰检测电压“，并显示最近半小时的数据，如下图所示。
<div align="center">
<img src=./../../../images/4_火焰监控系统_实时曲线设定.png width=80%/>
</div>

&emsp;&emsp;
选中”地图“组件，点击“编辑设备地图”按钮，如下图所示。

<div align="center">
<img src=./../../../images/4_火焰监控系统_地图_编辑入口.png width=80%/>
</div>

&emsp;&emsp;
点击“选择产品”按钮 ，选中“火焰检测系统”，如下图所示。如果此时设备已经上线，则地图会自动跳到设备当前所在位置。
<div align="center">
<img src=./../../../images/4_火焰监控系统_地图_选择产品.png width=80%/>
</div>

<br>

### **业务逻辑开发**
&emsp;&emsp;
业务逻辑的主要目的是为了让用户设定物联网设备端的行为逻辑，常规物联网系统都是在设备端固化行为逻辑，出厂之后如果需要修改设备的行为，则需要进行OTA升级。本节课程则向读者介绍如何通过IoT Studio完成业务逻辑的开发。

&emsp;&emsp;
新建一条名为“火焰亮度监控报警设定”的规则。
<div align="center">
<img src=./../../../images/4_火焰检测系统_创建业务逻辑.png width=80%/>
</div>

&emsp;&emsp;
系统会自动进入到业务逻辑编辑页面，如下图所示，点击左侧的“节点”按钮，则可以看到所有可用的节点选项。右侧红框是如何设计一个业务逻辑的介绍。

<div align="center">
<img src=./../../../images/4_火焰监控报警.png width=80%/>
</div>

1. 选择目标节点

&emsp;&emsp;
此逻辑需要通过“火焰检测系统”上报的“火焰检测电压”当低于高于电压阈值的时候打开报警灯，否则关闭报警灯。所以需要如下4个节点：
* 设备触发节点
* 条件判断节点
* 开启报警灯节点
* 关闭报警灯节点

&emsp;&emsp;
分别从左侧拖动“设备触发”，“条件判断”和2个“火焰检测系统”4个节点到右侧的业务逻辑编辑框。

<div align="center">
<img src=./../../../images/4_火焰监控系统_节点列表.png width=80%/>
</div>

2. 建立节点间的关联关系

&emsp;&emsp;
按照预设的逻辑建立，如下图所示（在节点的一侧按下鼠标拖动到另一节点的一侧即可建立连线关系）。
<div align="center">
<img src=./../../../images/4_火焰监控系统_节点逻辑关系建立.png width=80%/>
</div>

1. 业务逻辑编辑
* 设备触发节点

&emsp;&emsp;
此设备触发选择“火焰检测设备”的“火焰检测电压”属性即可，如下图所示（和前面“组件”设定类似，同样是鼠标选中第节点，在右侧的配置选项中进行配置）。
<div align="center">
<img src=./../../../images/4_火焰监控系统_设备触发节点配置.png width=80%/>
</div>

* 条件判断节点

&emsp;&emsp;
此处我们设定为当传感器测量到的电压值高于1000mV则开始报警。设定步骤如下。
> 在没有检测到火焰的时候传感器输出高电平，在检测到火焰之后，输出电压会下降

<div align="center">
<img src=./../../../images/4_火焰监控系统_比较节点配置.png width=80%/>
</div>

* 设备节点行为设定

&emsp;&emsp;
分别为设备节点设定开启报警灯和关闭报警灯的行为，如下图所示。

<div align="center">
<img src=./../../../images/4_火焰监控系统_设备节点行为设定.png width=80%/>
</div>


* 业务逻辑保存和部署

&emsp;&emsp;
依此点击右上角的“保存”和“部署”按钮，即可将此业务逻辑设定完毕。
<br>

<br>

### **预览和发布上线**

&emsp;&emsp;
业务逻辑设定完毕之后，可以在“火焰监控报警系统”页面编辑页面点击“预览”按钮进行预览，如下图所示。

<div align="center">
<img src=./../../../images/4_火焰监控系统_预览.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下所示，同时可以扫描二维码在手机上面查看实际运行效果。

<div align="center">
<img src=./../../../images/4_火焰监控报警系统效果.gif width=80%/>
</div>

&emsp;&emsp;
此时查看设备上面的报警灯的状态会同步和web应用的报警灯状态同步显示。

<br>

&emsp;&emsp;
这样整个火焰检测系统的趣味案例就完成了。如果想要看整个案例更详细的操作步骤，请参考“[火焰检测系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/4-%E6%99%BA%E6%85%A7%E5%B7%A5%E4%B8%9A/%E5%9C%BA%E6%99%AF1-%E7%81%AB%E7%84%B0%E7%9B%91%E6%8E%A7%E6%8A%A5%E8%AD%A6%E7%B3%BB%E7%BB%9F/README.md)”中的说明。

<br>
