# 燃气检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/2_燃气检测_步骤概述.jpg width=70%/>
</div>

## 简介
&emsp;&emsp;
天然气是国人目前家庭里边烹饪和取暖所用的主要燃料，其主要成分是甲烷（CH4），同时还含有少量的丙烷、乙烷、丁烷等其他少量气体。天然气本身无色无味，密度比空气小。如果发生天然气泄漏，其在空气中浓度达到5%～15%时，遇到明火就会爆炸。如今天然气普及，每年由于因使用不当、人为因素（忘记关闭开关）造成的燃气泄漏爆炸事故接连不断。

&emsp;&emsp;
本节内容就以此为背景教开发者如何一步一步打造一个燃气泄漏检测系统。

## 准备
&emsp;&emsp;
完成本节内容的学习需要如下硬件：
1. ESP32一套
2. MQ2燃气传感器一个
3. RGB LED一个（或红色LED一个）
4. 连接线若干

### 硬件连线图

&emsp;&emsp;
NODEMCU32S硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/2_燃气检测_ESP32_MQ2连线.png width=70%/>
</div>
<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| DAT | G36 | 燃气传感器信号测试点位 |
| VCC | 3.3V | 燃气传感器供电点位 |
| GND | GND | 燃气传感器地点位 |
| GND | GND | LED传感器地点位 |
| B | G25 | LED传感器Blue点位 |
| G | G26 | LED传感器Green点位 |
| R | G27 | LED传感器Red点位 |

## 一分钟上云体验

- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-燃气检测系统（JavaScript）.png width=40%/>
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
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。开通后点击“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。

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
<img src=./../../../images/2_新建燃气检测设备.png
 width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“燃气检测系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气检测系统_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/2_燃气检测系统_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择导入物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择本地文件导入[燃气检测系统物模型](./link_platform/gas_detector_model.zip)。

<div align="center">
<img src=./../../../images/2_燃气检测系统_快速导入物模型.png width=100%/>
</div>

&emsp;&emsp;
物模型导入成功后可以看到网页出现了我们刚刚导入的物模型属性。其中alarmLight代表的是燃气等的报警状态，1处于报警状态，0代表没有报警；gasVoltage代表MQ2燃气传感器检测到的电压值。


<div align="center">
<img src=./../../../images/2_燃气检测系统_发布物模型.png width=100%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”燃气检测系统“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/2_燃气检测系统_产品页_管理设备.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/2_燃气检测系统_添加设备入口.png width=100%/>
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
<img src=./../../../images/2_燃气检测系统设备详情.png width=100%/>
</div>

<br>

#### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/2_燃气检测系统_设备三元组.png width=50%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“燃气检测电压值“，”报警灯“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/2_燃气检测系统_设备物模型数据.png width=100%/>
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
下载《[ESP32 gas_detector示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
gas_detector
├── app.json         # 板级配置
└── app.js           # 应用代码
└── gas.js           # 燃气检测传感器驱动
```

### 修改wifi ssid和password

&emsp;&emsp;
修改gas_detector工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

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
修改gas_detector工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

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
postProps is called
gasVoltage is 98
alarmLight is 1
postProps is called
gasVoltage is 95
alarmLight is 1
postProps is called
gasVoltage is 97
alarmLight is 1
postProps is called
gasVoltage is 511
alarmLight is 0
```


<br>


### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_燃气检测系统_设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板周围的燃气浓度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

<br>

### 物联网平台控制报警灯状态
<br>
&emsp;&emsp;
物联网设备上线之后，可以通过”监控运维“中的"在线调试"功能进行调试，详细操作步骤见下图：
<div align="center">
<img src=./../../../images/1_燃气检测系统_物联网平台在线调试功能.png width=100%/>
</div>

&emsp;&emsp;
此产品的物模型属性中，"燃气检测电压值"设置的是只读，也就是说智能从设备端读取，不支持设置此状态到设备端，所以点开"燃气超标标志"后面的”调试“之后，里边只有获取的选项。”报警灯“设置的是可读可写，所以点开”报警灯“后面的”调试“之后，里边有”获取“、”设置”和“设置期望值”三个选项。
这里可以选择打开报警灯之后点击“设置”进行报警灯功能的调试。

<div align="center">
<img src=./../../../images/1_燃气检测系统_云端打开报警灯.png width=100%/>
</div>

&emsp;&emsp;
此时查看设备端LED是否已经打开，打开成功则代表控制报警灯成功。
<br>
<br>
## 物联网应用开发

&emsp;&emsp;
通过本节课程的学习，读者可以搭建一个在手机上面可以随时随地查看燃气监控系统状态的应用。

<br>

### 燃气监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何完成燃气监控系统移动端应用程序的开发。

<div align="center">
<img src=./../../../images/1_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

#### **新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“燃气监控报警系统”。
<div align="center">
<img src=./../../../images/2_燃气监控报警系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

### **新建“移动应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个移动应用，命名为“燃气实时监控”。
<div align="center">
<img src=./../../../images/2_燃气监控系统_移动应用创建.png width=80%/>
</div>

创建完移动应用之后系统会自动跳转到”新建页面“，这里选择”创建页面“完成应用页面的创建，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气监控系统_移动应用_新建页面.png width=80%/>
</div>

&emsp;&emsp;
移动端应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/2_燃气监控系统_移动应用_页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

#### **页面设计**
&emsp;&emsp;
这里我们用到3个组件:
* 实时曲线
用于显示温湿度的历史变化曲线及实时数据
* 指示灯
显示和控制空调和加湿器的当前状态
* 天气
用于显示目的地天气

&emsp;&emsp;
将三个组件拖到中间的画布区，适当调整组件布局，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气监控系统_实时监控页面设计.png width=80%/>
</div>

<br>

#### **关联产品和设备**
&emsp;&emsp;
此时回到”燃气监控系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/2_燃气监控系统_关联产品和设备.png width=80%/>
</div>


&emsp;&emsp;
关联产品的过程如下：
<div align="center">
<img src=./../../../images/2_燃气监控系统_关联产品.png width=80%/>
</div>

&emsp;&emsp;
关联设备的过程如下：
<div align="center">
<img src=./../../../images/2_燃气监控系统_关联设备.png width=80%/>
</div>
&emsp;&emsp;
产品和设备关联完毕之后，就可以将把组件和设备的属性关联起来了。

<br>

#### **关联数据源**
&emsp;&emsp;
关联数据源分为如下3个步骤，每个步骤的截图如下：
* 关联产品
* 关联设备
* 关联属性

&emsp;&emsp;
具体操作步骤如下：
1. 选中”开关“组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/2_燃气监控系统_指示灯_配置数据源.png width=80%/>
</div>

1. 选择目标产品
<div align="center">
<img src=./../../../images/2_燃气监控系统_指示灯_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/2_燃气监控系统_开关_配置数据源_设备.png width=80%/>
</div>


4. 选择“报警灯”属性
<div align="center">
<img src=./../../../images/2_燃气监控系统_指示灯_配置数据源_报警灯.png width=80%/>
</div>

&emsp;&emsp;
选择好产品、设备和属性之后，需要修改指示灯大小及”开始报警“/”停止报警“时指示灯的颜色，如下图所示。
<div align="center">
<img src=./../../../images/2_燃气监控系统_指示灯_配置数据源_报警灯颜色.png width=80%/>
</div>


&emsp;&emsp;
同样的方式为”实时曲线“设置为目标设备的”燃气检测电压“，并显示最近半小时的数据，如下图所示。
<div align="center">
<img src=./../../../images/2_燃气监控系统_实时曲线设定.png width=80%/>
</div>

&emsp;&emsp;
选中”天气“组件，可以设定要显示的是什么位置的天气，本节课程中显示默认地址即可，读者也可以设定自己所在地的信息，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气监控系统_位置信息设定.png width=80%/>
</div>

<br>

#### **业务逻辑开发**
&emsp;&emsp;
业务逻辑的主要目的是为了让用户设定物联网设备端的行为逻辑，常规物联网系统都是在设备端固化行为逻辑，出厂之后如果需要修改设备的行为，则需要进行OTA升级。本节课程则向读者介绍如何通过IoT Studio完成业务逻辑的开发。

&emsp;&emsp;
新建一条名为“燃气浓度超过阈值报警”的规则。
<div align="center">
<img src=./../../../images/2_燃气检测系统_创建业务逻辑.png width=80%/>
</div>

&emsp;&emsp;
系统会自动进入到业务逻辑编辑页面，如下图所示，点击左侧的“节点”按钮，则可以看到所有可用的节点选项。右侧红框是如何设计一个业务逻辑的介绍。

<div align="center">
<img src=./../../../images/2_燃气监控系统_高浓度报警.png width=80%/>
</div>

1. 选择目标节点

&emsp;&emsp;
此逻辑需要通过“燃气检测设备”上报的“燃气测量电压”当超过阈值的时候打开报警灯，否则关闭报警灯。所以需要如下4个节点：
* 设备触发节点
* 条件判断节点
* 开启报警灯节点
* 关闭报警灯节点

&emsp;&emsp;
分别从左侧拖动“设备触发”，“条件判断”和2个“燃气检测系统”4个节点到右侧的业务 逻辑编辑框。

<div align="center">
<img src=./../../../images/2_燃气监控系统_节点列表.png width=80%/>
</div>

2. 建立节点间的关联关系

&emsp;&emsp;
按照预设的逻辑建立，如下图所示（在节点的一侧按下鼠标拖动到另一节点的一侧即可建立连线关系）。
<div align="center">
<img src=./../../../images/2_燃气监控系统_节点逻辑关系建立.png width=80%/>
</div>

3. 业务逻辑编辑
* 设备触发节点

&emsp;&emsp;
此设备触发选择“燃气检测设备”的“燃气检测电压”属性即可，如下图所示（和前面“组件”设定类似，同样是鼠标选中第节点，在右侧的配置选项中进行配置）。
<div align="center">
<img src=./../../../images/2_燃气监控系统_设备触发节点配置.png width=80%/>
</div>

* 条件判断节点

&emsp;&emsp;
此处我们设定为当传感器测量到的电压值高于900mV则开始报警。设定步骤如下。

<div align="center">
<img src=./../../../images/2_燃气监控系统_比较节点配置.png width=80%/>
</div>

* 设备节点行为设定

&emsp;&emsp;
分别为设备节点设定开启报警灯和关闭报警灯的行为，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气监控系统_设备节点行为设定.png width=80%/>
</div>


* 业务逻辑保存和部署

&emsp;&emsp;
依此点击右上角的“保存”和“部署”按钮，即可将此业务逻辑设定完毕。
<br>


<br>

#### **预览和发布上线**

&emsp;&emsp;
业务逻辑设定完毕之后，可以在“燃气监控报警系统”页面编辑页面点击“预览”按钮进行预览，如下图所示。

<div align="center">
<img src=./../../../images/2_燃气监控系统_预览.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下所示，同时可以扫描二维码在手机上面查看实际运行效果。

<div align="center">
<img src=./../../../images/2_燃气监控报警系统效果.gif width=80%/>
</div>

&emsp;&emsp;
此时查看设备上面的报警灯的状态会同步和移动端应用的报警灯状态同步显示。
<br>

&emsp;&emsp;
到此为止，燃气检测系统的案例就已经完成了。如果想要学习燃气检测系统更详细的操作步骤，请参考“[燃气检测系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/2-%E6%99%BA%E8%83%BD%E7%94%9F%E6%B4%BB/%E5%9C%BA%E6%99%AF1-%E5%AE%88%E6%8A%A4%E5%AE%B6%E5%BA%AD%E5%81%A5%E5%BA%B7/%E7%87%83%E6%B0%94%E6%B3%84%E6%BC%8F%E6%A3%80%E6%B5%8B/README.md)”中的说明。
