# 花卉养植系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/3_花卉养植系统_步骤概述.jpg width=70%/>
</div>

## 简介
&emsp;&emsp;
我们国家的现代农业和过去相比已经有了长足的进步,其中花卉养植监控系统也是高效农业的一个重要组成部分。

&emsp;&emsp;
花卉养植监控系统是一种可以改变植物生长环境、为植物生长创造最佳条件、避免外界四季变化和恶劣气候对其影响的场所。它以采光覆盖材料作为全部或部分结构材料，可在冬季或其他不适宜露地植物生长的季节栽培植物。花卉养植监控系统生产以达到调节产期，促进生长发育，防治病虫害及提高质量、产量等为目的。而花卉养植监控系统设施的关键技术是环境控制，该技术的最终目标是提高控制与作业精度。

&emsp;&emsp;
本章课程就以山茶花养植为例介绍如何打造一个最适合茶花生长的花卉养植系统。

&emsp;&emsp;
按照茶花的生长习性，最适宜茶花的生长环境因素主要有3个：
1. 温度：20~28度
2. 湿度：~70%
3. 光照：不能太强

&emsp;&emsp;
为了制造适合茶花生长的环境，本场景的示意图如下图所示，场景设计的行为如下：
1. 通过温度传感器测量环境温度，并在温度过高时打开空调进行降低环境温度
2. 通过湿度传感器测量环境湿度，并在湿度过低的时候控制加湿器进行加湿操作

## 准备
&emsp;&emsp;
本案例只需要如下硬件：
* ESP32一台
* SHT3X-DIS温湿度传感器一个
* RGB三色LED灯一个
* 杜邦线若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
NODEMCU32S硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/1_ESP32_SHT3X_DIS_RGB_LED_节点图.jpg width=80%/>
</div>

<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| VCC | 3.3V | SHT3X传感器供电点位 |
| GND | GND | SHT3X传感器地点位 |
| SCL | G22 | SHT3X传感器SCL点位 |
| SDA | G21 | SHT3X传感器SDA点位 |
| GND | GND | LED传感器地点位 |
| B | G25 | LED传感器Blue点位 |
| G | G26 | LED传感器Green点位 |
| R | G27 | LED传感器Red点位 |

&emsp;&emsp;
需要连接两颗LED模拟控制空调和加湿器的开关的功能此处就借助于ESP32上面外接的蓝色和绿色LED灯，其中：

* 蓝色LED模拟空调的开关<br>
* 绿色LED模拟加湿器的开关<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知如果要模拟控制空调打开，则需要打开P27（蓝色LED）；如需模拟控制加湿器打开，则需要打开P26（绿色LED）。

## 一分钟上云体验
- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-花卉养殖（JavaScript）.png width=40%/>
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
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“标准品类”（如果创建的产品品类为非标品类，可以选择自定义品类），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/1_新建产品页面.png
 width=100%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“温湿度检测装置”的产品，如下图所示。

<div align="center">
<img src=./../../../images/1_产品列表页.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。
> 物模型中的“地理位置”属性本案例后面的内容没有用到，读者可忽略。

<div align="center">
<img src=./../../../images/1_产品详情页面.png width=100%/>
</div>

&emsp;&emsp;
因为在创建产品的过程中选择了标准的产品品类，这里会出现标准品类中自带的物模型设定，包含“当前温度”、“当前湿度”和“地理位置”信息。这三条都适合属性信息，其标识符、数据类型、数据定义及该属性支持的操作都可以在这个页面看到。

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
如果在创建产品的时候没有选择标准的品类，则需要在这个地方新增物模型的属性或服务。详情请参考[物联网平台](../../../拓展知识/物联网平台/README.md)中的内容。

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/1_管理设备入口.png width=100%/>
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
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/1_设备详细信息.png width=100%/>
</div>

<br>

##### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“当前温度“，”当前湿度“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/1_设备物模型数据.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

#### **云端物模型**
&emsp;&emsp;
在此产品的物模型中新增两个bool类型的属性，命名为"airconditioner"和"humidifier"分别用来控制空调和加湿器。
* 当需要打开空调时，云端将“airconditioner”的属性值设置为True；需要关闭空调时，将其设置为False。
* 当需要打开加湿器时，云端将“humidifier”的属性值设置为True；需要关闭空调时，将其设置为False。

&emsp;&emsp;
添加物模型属性的过程下所述：
1. 在产品的“功能定义”标签页中点击"编辑草稿"，如果没有“编辑草稿”的按钮，说明此产品的物模型已经发布，需要点击右上角的“取消发布”按钮才能添加或删除属性。
<div align="center">
<img src=./../../../images/3_添加物模型属性_编辑草稿.png width=80%/>
</div>

2. 点击“添加自定义功能”按钮，如下图所示：
<div align="center">
<img src=./../../../images/3_花卉养植_添加空调和加湿器属性.png width=80%/>
</div>

 3. 添加属性名称、标识符、类型等信息，如下图所示：
* 添加“airconditioner”属性
<div align="center">
<img src=./../../../images/3_花卉养植_添加控制空调属性.png width=40%/>
</div>

* 添加“humidifier”属性
<div align="center">
<img src=./../../../images/3_花卉养植_添加控制加湿器属性.png width=40%/>
</div>

&emsp;&emsp;
添加完属性之后，点击左下角的“发布上线”按钮 ，根据系统提示进行确认即可，如下图所示：
<div align="center">
<img src=./../../../images/3_花卉养植_属性发布上线.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/3_花卉养植_属性发布上线_确认.png width=40%/>
</div>

#### **多设备场景联动设定**
&emsp;&emsp;
控制逻辑有两种实现方式：
1. 使用物联网平台的规则引擎功能
2. 使用IoT Studio的业务逻辑功能

&emsp;&emsp;
本节接下来就开始介绍使用物联网平台的规则引擎来完成此控制逻辑的设定。

&emsp;&emsp;
按照下图所示的步骤创建场景联动的规则：
<div align="center">
<img src=./../../../images/3_花卉养植_创建场景联动规则.png width=80%/>
</div>

&emsp;&emsp;
点击“创建规则”之后，填入场景规则名称，这里我们先创建第一个“温度过高开启空调”的规则。

<div align="center">
<img src=./../../../images/3_花卉养植_创建场景联动规则_温度过高开启空调.png width=40%/>
</div>

&emsp;&emsp;
规则创建完成后，等待几秒钟系统会自动跳转到规则编辑页面，这里需要配置如下两步完整一整条规则的创建：
1. 设置“触发器”为温湿度监控器的“当前温度"属性高于茶花最适宜生长的温度28度触发此规则的执行，如下图所示

<div align="center">
<img src=./../../../images/3_花卉养植_温度过高开启空调_触发器设定.png width=80%/>
</div>

2. 设置“执行动作"为控制设备打开”空调开关“属性
<div align="center">
<img src=./../../../images/3_花卉养植_温度过高开启空调_执行器设定.png width=80%/>
</div>

&emsp;&emsp;
点击保存之后”温度过高打开空调“的场景联动规则就创建好了。
&emsp;&emsp;
用相同的方式创建另外三条场景联动规则：
* "温度合适关闭空调"，如下图所示

<div align="center">
<img src=./../../../images/3_花卉养植_创建场景联动规则_温度合适关闭空调.png width=80%/>
</div>
* ”湿度适合关闭加湿器“，如下图所示

<div align="center">
<img src=./../../../images/3_花卉养植_创建场景联动规则_湿度适合关闭加湿器.png width=80%/>
</div>

* "湿度过低打开加湿器“，如下图所示

<div align="center">
<img src=./../../../images/3_花卉养植_创建场景联动规则_湿度过低开启加湿器.png width=80%/>
</div>

&emsp;&emsp;
创建完这几条规则之后，在”场景联动“页面中点击规则后面的启动按钮，则所有的规则都会运行起来。
<div align="center">
<img src=./../../../images/3_花卉养植_开启场景联动规则.png width=80%/>
</div>

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
下载《[ESP32 floriculture示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
fire_detector
├── app.json      # 板级配置
└── app.js        # 应用代码
└── sht3x.js      # sht3x温湿度传感器驱动
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
Temperature is 30.85 degrees, humidity is 39.51
Temperature is 31.26 degrees, humidity is 44.42
cloud req params is {"airconditioner":0}
关闭空调
cloud req params is {"humidifier":1}
打开加湿器
...
```

<br>
&emsp;&emsp;
开发板实际效果如下：
* 在温度高于阈值时，可以看到日志中出现"打开空调"的字样，伴随着蓝色LED灯亮起
* 在温度低于阈值之后 ，可以看到日志中出现“关闭空调”的字样，伴随着蓝色LED灯熄灭

* 在湿度低于阈值时，可以看到日志中出现"打开加湿器"的字样，伴随着绿色LED灯亮起
* 在湿度高于阈值时，可以看到日志中出现"关闭加湿器"的字样，伴随着绿色LED灯熄灭

<br>

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/1_温湿度监控设备状态及属性.png width=100%/>
</div>

&emsp;&emsp;
此时如果开发板温度和周围湿度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

## 物联网应用开发

&emsp;&emsp;
现代花卉养植的过程中越来越多的需要农业专家的参与，但和养花的农户来说，农业专家的数量是很少的。在茶花生长出现问题的时候，怎样让农业专家远程迅速的完成对茶花异常生长状况的诊断是很重要的一个研究议题。本节要介绍的中控大屏就是将茶花当前生产环境的信息通过网页的形式直观的展现出来，一方面方便农户很方便的查看相关信息并判断是否需要人为干预；另一方面在出现问题的时候可以让专家远程快速的对问题进行诊断。

<br>

### 中控大屏目标
&emsp;&emsp;
本节课程主要目标是设计一个总控大屏的web应用，能实时的查看如下信息：
1. 过去3小时的温湿度实时信息
2. 空调及加湿器状态实时状态显示
3. 手动控制空调或加湿器的开关状态

<br>

### 茶花生长环境监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何花卉养植系统中控大屏的web应用程序的开发。

<div align="center">
<img src=./../../../images/1_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

#### **新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“茶花养植系统”。
<div align="center">
<img src=./../../../images/3_茶花养植系统_创建IoTStudio项目.png width=80%/>
</div>

<br>

#### **新建“Web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个Web应用，命名为“茶花生长环境实时监控”。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控.png width=80%/>
</div>

&emsp;&emsp;
Web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_页面编辑页.png width=80%/>
</div>


&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

#### **页面设计**
&emsp;&emsp;
这里我们用到3个组件:
* 实时曲线
用于显示温湿度的历史变化曲线及实时数据
* 开关
显示和控制空调和加湿器的当前状态
* 图片
空调或加湿器的示意图

&emsp;&emsp;
将三个组件拖到中间的画布区，适当调整组件布局。然后选中图片组件，点击右边的“上传图片”按钮，分别上传空调和加湿器的图片，如下图所示。

<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_图片上传.png width=80%/>
</div>

<br>

#### **关联产品和设备**
&emsp;&emsp;
此时回到”茶花养植系统“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/3_花卉养殖系统_关联产品和设备.png width=80%/>
</div>

&emsp;&emsp;
关联产品的过程如下：
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花养植系统_关联产品.png width=80%/>
</div>

&emsp;&emsp;
关联设备的过程如下：
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_关联设备.png width=80%/>
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
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_开关_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_开关_配置数据源_设备.png width=80%/>
</div>


4. 选择“空调开关”属性
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_开关_配置数据源_空调开关.png width=80%/>
</div>

&emsp;&emsp;
选择好产品、设备和属性之后，可以看到湿度计表盘上面的湿度值已经已经更新为物联网平台设备属性中最新的湿度值。

&emsp;&emsp;
同样的方式为另一个”开关“组件选择相同设备的“加湿器”属性。
接下来需要为”实时曲线“组件设定数据源。同样点击该组件后，点击右边的配置数据源。因为本场景中的温湿度测量和空调/加湿器控制都是通过一个物联网设备完成的，所以本页中要选择”单设备多属性“。然后选择好本场景创建的产品，如下图所示。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_实时曲线_设备选择.png width=80%/>
</div>

&emsp;&emsp;
选择相同的设备，如下图所示。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_实时曲线_选择设备.png width=80%/>
</div>

&emsp;&emsp;
本场景显示的是茶花生长环境的温度信息和湿度信息，所以”数据项“中要选择这两项属性。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_实时曲线_历史数据属性.png width=80%/>
</div>

&emsp;&emsp;
最后选择要显示的实时温湿度数据的时间长度，这里选择3小时，如下图所示。
<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_实时曲线_数据时间段.png width=80%/>
</div>

<br>

#### **发布上线**
<div align="center">
<img src=./../../../images/1_IoT_Studio_HaaS_EDU_K1_预览和发布.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下图所示：

<div align="center">
<img src=./../../../images/3_茶花养植系统_茶花生长环境实时监控_运行效果.png width=80%/>
</div>

<br>

&emsp;&emsp;
这样我们就完成了一个花卉养殖系统从设备端温湿度测量，设备控制、物联网云平台开发及物联网应用开发全链路的开发。如果想要学习花卉养植系统更详细的操作步骤，请参考“[花卉养植系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/3-%E6%99%BA%E6%85%A7%E5%86%9C%E4%B8%9A/%E5%9C%BA%E6%99%AF1-%E8%8A%B1%E5%8D%89%E5%85%BB%E6%A4%8D/README.md)”中的说明。

