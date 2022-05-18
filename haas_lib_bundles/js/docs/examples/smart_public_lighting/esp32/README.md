# 智慧路灯系统
&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/4_智慧路灯_步骤概述.jpg width=60%/>
</div>

## 简介

&emsp;&emsp;
随着城市化不断扩大，城市的灯光秀越来越多，让我们居住的城市生活变得五彩缤纷，灯火辉煌，让城市显得越来越繁华。但是，我们也会经常发现有些路边的灯，在天很黑了不会自动打开，或者到清晨，天很亮了，马路边的灯也不会自动熄灭，只会在固定时间统一开关控制，这样既不人性化也不环保。于是现在很多地方的路灯会根据周围环境亮度自动开启或关闭路边灯光，那这样的路灯就非常人性化，同时也非常环保，这才是真正的城市智慧路灯。

### 背景知识

&emsp;&emsp;
本章课程打造的智慧路灯控制系统是当检测到周围环境亮度变暗的时候，系统会自动打开灯，当周围环境亮度变亮的时候，系统就会自动关闭灯。本智慧路灯控制系统默认设置一个亮度阈值，当亮度值超过阈值，关灯，当亮度值低于阈值，开灯，当然，这个阈值可以根据每个人对光的亮度感觉不同而调整。

## 准备
&emsp;&emsp;
硬件器材：
1. ESP32开发一套
2. 光敏电阻传感器一个
3. 连接线若干

硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/4_ESP32_热敏电阻传感器连线图.jpg width=85%/>
</div>
<br>

&emsp;&emsp;
M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| A0 | G36 | 光敏电阻传感器信号测试点位 |
| VCC | 3.3V | 光敏电阻传感器供电点位 |
| GND | GND | 光敏电阻传感器地点位 |
| LED | G27 | 控制LED灯引脚 |

## 一分钟上云体验

- 打开“支付宝”扫描下图二维码

<div align="center">
<img src=../../../images/案例-智慧路灯系统（JavaScript）.png width=40%/>
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

### 云端创建产品
1. **创建产品**

&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/2_3_创建产品.jpg
 width=40%/>
</div>

&emsp;&emsp;
这里创建了一个名称为“智慧路灯”的产品。
<div align="center">
<img src=./../../../images/5_1_新建智慧路灯产品.jpg
 width=40%/>
</div>

&emsp;&emsp;
点击确认，就可以在产品列表中出现智慧路灯。

<div align="center">
<img src=./../../../images/5_1_完成产品创建.jpg
 width=40%/>
</div>

&emsp;&emsp;
查看产品详情。
<div align="center">
<img src=./../../../images/5_1_产品功能定义.jpg
 width=40%/>
</div>

&emsp;&emsp;
编辑草稿，选择自定义功能。
<div align="center">
<img src=./../../../images/5_1_自定义产品物模型定义.jpg
 width=40%/>
</div>

&emsp;&emsp;
如下图设置对应产品的物模型，分别设置光强度电压值和灯开关两个模型。
<div align="center">
<img src=./../../../images/5_1_2产品物模型.jpg
 width=40%/>
 <img src=./../../../images/5_1_3产品物模型onoff.jpg
 width=40%/>
</div>

&emsp;&emsp;
在产品详情中增加产品的功能，可以看到刚才设置的光强度电压值和灯开关两个功能属性，点击发布上线。
<div align="center">
<img src=./../../../images/5_1_3完成物模型设置.jpg
 width=80%/>
</div>

&emsp;&emsp;
这样，整个智慧路灯产品在物联网平台也创建好了。

1. **设备管理**

&emsp;&emsp;
选中最左边栏设备管理中的设备，然后添加设备，
<div align="center">
<img src=./../../../images/2_3_添加设备.jpg
 width=40%/>
</div>

&emsp;&emsp;
选择的产品为刚刚创建的智慧路灯，设备名设置为smartlight,点击确认，
<div align="center">
<img src=./../../../images/5_1_智慧路灯设备.jpg
 width=40%/>
</div>

&emsp;&emsp;
进入设备详情，获取设备三元组信息，
<div align="center">
<img src=./../../../images/5_1_设备三元组信息.jpg
 width=40%/>
</div>

&emsp;&emsp;
将设备三元组信息一键拷贝出来，在设备开发中会用到。
<div align="center">
<img src=./../../../images/2_3_设备证书.jpg
 width=40%/>
</div>

4. **设备引擎**

&emsp;&emsp;
如何通过光感自适应控制灯的亮灭呢？可以在物联网平台上通过引擎规则实现，本课程使用的光敏电阻传感器测的光强度电压值越高，说明周围的环境越暗；反之，测得的光强度电压值越低，说明周围的环境越亮。
<div align="center">
<img src=./../../../images/5_1_规则引擎1.jpg
 width=70%/>
</div>

&emsp;&emsp;
当光强度电压值大于1000，说明周围的环境变暗，执行开灯的指令
<div align="center">
<img src=./../../../images/5_1_3规则引擎2.jpg
 width=70%/>
</div>

&emsp;&emsp;
当光强度电压值小于1000，说明周围的环境变亮，执行关灯的指令
<div align="center">
<img src=./../../../images/5_1_3规则引擎3.jpg
 width=70%/>
</div>

&emsp;&emsp;
规则引擎创建完成以后，启动引擎即可。
<div align="center">
<img src=./../../../images/5_1_3规则引擎4.jpg
 width=70%/>
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
下载《[ESP32 smart_public_lighting示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)》，示例代码目录如下所示：
```bash
smart_public_lighting
├── app.json         # 板级配置
└── app.js           # 应用代码
└── photoresistor.js # 光敏电阻传感器驱动
```

### 修改wifi ssid和password

&emsp;&emsp;
修改smart_public_lighting工程里app.js中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

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
修改smart_public_lighting工程里app.js中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

```javascript
var iotOptions = {
    productKey: "请填写您在阿里云物联网平台申请的产品PK",
    deviceName: "请填写您在阿里云物联网平台申请的产品DN",
    deviceSecret: "请填写您在阿里云物联网平台申请的产品DS",
    region: 'cn-shanghai',
    keepaliveSec: 60
  };
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
postProps is called
photoresistorVoltage is 98
led onoff is 1
postProps is called
photoresistorVoltage is 95
led onoff is 1
postProps is called
photoresistorVoltage is 97
led onoff is 1
postProps is called
photoresistorVoltage is 511
led onoff is 0
postProps is called
photoresistorVoltage is 511
led onoff is 0
postProps is called
photoresistorVoltage is 144
led onoff is 1
postProps is called
```

> 当光强小于450的时候，打开esp32开发板上的LED灯，当光强度大于450的时候，关闭esp32开发板上的LED灯。（LED等需要外接到GPIO 27脚上）


<br>



&emsp;&emsp;
到此为止，智慧路灯系统案例就已经完成了。如果想要学习智慧路灯案例更详细的操作步骤，请参考“[智慧路灯系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/5-%E6%99%BA%E6%85%A7%E5%9F%8E%E5%B8%82/%E5%9C%BA%E6%99%AF1-%E6%99%BA%E6%85%A7%E8%B7%AF%E7%81%AF/README.md)”中的说明。
