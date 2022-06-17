# WORST-CASE
## 1、简介

### 1.1、背景知识
&emsp;&emsp;
开发者在进行硬件选型时通常会遇到这样的问题，即硬件板卡能支持多少外围设备的扩展？系统资源又能支持几路线程的稳定运行？为帮助开发者更为高效的选择合适的物联网硬件设备，我们将针对不同的物联网开发板分别提供了worst case测试案例。本篇文档为esp32s开发板的worst case

### 1.2、准备
&emsp;&emsp;
本案例打造需要如下硬件：

- ESP32开发板一台
- mfrc522 RFID传感器模块一个
- RFID卡片若干
- 磁控传感器
- Servo舵机一个
- 电机转数传感器
- 步进电机及驱动板
- 光敏电阻传感器
- 温湿度传感器
- GPS
- mpu6050（3轴加速度+3轴陀螺仪）

&emsp;&emsp;
ESP32各开发板和外围传感器硬件接线请参考下表。

<table>
    <tr>
        <td align="center">硬件</td>
        <td align="center">步进电机驱动板</td>
        <td align="center">mfrc522 RFID传感器</td>
        <td align="center">磁控传感器</td>
        <td align="center">Servo舵机</td>
        <td align="center">电机转数传感器</td>
        <td align="center">光敏电阻传感器</td>
        <td align="center">温湿度传感器</td>
        <td align="center">mpu6050</td>
        <td align="center">GPS</td>
        <td align="center">ESP32开发板</td>
        <td align="center">电源转接板</td>
    </tr>
    <tr>
        <td align="center" rowspan="16">端口标识</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
        <td align="center">GND</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">3.3V</td>
    </tr>
    <tr>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">VCC</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">5V</td>
    </tr>
    <tr>
        <td align="center">IN1</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P04</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">IN2</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P00</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">IN3</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P02</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">IN4</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P15</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">RST</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P25</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">SPI</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">SPI3</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">DO</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P13</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">PWM</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P27</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">OUT</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">P26</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">AO</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">ADC0</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">I2C</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">I2C0</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">I2C</td>
        <td align="center">-</td>
        <td align="center">I2C0</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">-</td>
        <td align="center">UART</td>
        <td align="center">UART2</td>
        <td align="center">-</td>
    </tr>
    <tr>
        <td align="center" rowspan="1">硬件说明</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C4A7B94BEA8CA00000001&dataId=800C4A7B94BEA8CA" target="_blank">步进电机驱动器详情</td>
        <td align="center" >-</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C4C5091EC2DE500000001&dataId=800C4C5091EC2DE5" target="_blank">磁控开关详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5A3F64C2F88300000001&dataId=800C5A3F64C2F883&s=" target="_blank">舵机详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C24C1F05E458A00000001&dataId=800C24C1F05E458A" target="_blank">电机转数传感器详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C57784852F32C00000002&dataId=800C57784852F32C" target="_blank">光敏电阻传感详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CD7F0F2F57FF800000002&dataId=800CD7F0F2F57FF8&s=" target="_blank">温湿度传感器详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C38A51341E22F00000001&dataId=800C38A51341E22F" target="_blank">MPU6050（加速度计&陀螺仪）详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C7C08112B0A0700000002&dataId=800C7C08112B0A07&s=" target="_blank">GPS模块详情</td>
        <td> <a align="center" href="https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB" target="_blank">ESP32开发板详情</td>
        <td align="center" >-</td>
    </tr>
</table>

&emsp;&emsp;
下图是以NodeMCU-32S为例的硬件连线图：
<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_硬件连线图.png
 width=100%/>
</div>

## 2、物联网平台开发

&emsp;&emsp;
整个过程包含以下4个步骤：

1. 开通公共实例
2. 创建产品（设备模型）
3. 定义产品功能（物模型）
4. 创建设备及获取三元组

### 2.1、开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。开通后点击“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_物联网平台.png
 width=100%/>
</div>

### 2.2、创建产品（设备模型）
&emsp;&emsp;
进入[公共实例控制台](https://iot.console.aliyun.com/lk/summary/new)，点击“创建产品”按钮，即可进入[新建产品页面](https://iot.console.aliyun.com/product)。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_空产品页.png
 width=100%/>
</div>

&emsp;&emsp;
进入[新建产品页面](https://iot.console.aliyun.com/product)，设定“产品名称”，这里我们命名为“**WORST-CASE**”，读者也可以根据自己的喜好来命名。在“所属品类”中，选择“自定义品类”。

&emsp;&emsp;
产品的节点类型选择“直连设备”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。开发者可根据自己的需求在“产品描述”页面添加针对此产品的描述。

&emsp;&emsp;
对于 ESP32 等搭载 Wi-Fi 的设备而言，联网方式选择“Wi-Fi”。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_新建产品.png
 width=100%/>
</div>

&emsp;&emsp;
点击“确认”按钮，即可完成产品创建。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_完成创建产品.png
 width=100%/>
</div>

&emsp;&emsp;
点击“前往定义物模型”

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_尚未添加任何功能.png
 width=100%/>
</div>

### 2.3、定义产品功能（物模型）
&emsp;&emsp;
进入功能定义，点击编辑草稿，选择添加自定义功能，依次添加“地理位置”“加速度”“角速度”“入侵”“告警”五个功能项，如下所示：

&emsp;&emsp;
生成后的效果如下：

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_添加功能.png
 width=100%/>
</div>

&emsp;&emsp;
定义好物模型后，需要发布物模型上线，并发布产品，以使变更生效。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_发布物模型.png
 width=100%/>
</div>

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_发布产品.png
 width=100%/>
</div>

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

### 2.4、创建设备及获取三元组
&emsp;&emsp;
点击左侧栏中“设备“，在筛选框中选择要添加设备的产品，点击“添加设备”。这里这里我们命名为“**ability_test**”，开发者也可以根据自己的喜好来命名。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_添加设备.png
 width=100%/>
</div>

&emsp;&emsp;
开发者也可以选择“批量添加”，一次性添加多个设备，并生成随机的DeviceName。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_批量添加.png
 width=100%/>
</div>

&emsp;&emsp;
生成的设备如下。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_设备列表.png
 width=100%/>
</div>

&emsp;&emsp;
点击前往“查看”按钮，就可以看到此设备的详细信息了。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_设备详情.png
 width=100%/>
</div>

&emsp;&emsp;
点击右上角的“查看”按钮，就能看到设备的三元组信息了。
三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_设备证书.png
 width=100%/>
</div>

&emsp;&emsp;
再次前往物联网平台的设备信息页面，若设备运行正确，此时应该可以看到设备名右侧的状态由“未激活”变为“在线”。
选中“实时刷新”，可以看到数据实时从设备上报到物联网平台。设备上云成功。

<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_物模型数据.png
 width=100%/>
</div>

## 3、规则引擎

### 3.1、场景联动设定

&emsp;&emsp;
点击规则引擎->场景联动，点击创建规则。
<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_创建规则0.png width=100%/>
</div>

&emsp;&emsp;
进行规则设定，如下所示，云端检测到“入侵”，那么发送告警使能信息到设备通知设备打开告警。
<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_编辑规则0.png width=100%/>
</div>

&emsp;&emsp;
点击规则引擎->场景联动，点击创建规则。
<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_创建规则1.png width=100%/>
</div>

&emsp;&emsp;
进行规则设定，如下所示，云端未检测到“入侵”，那么就发送告警解除信息到设备通知设备关闭告警。
<div align="center">
<img src=./../../../images/worst_case/esp32s/ESP32_WORST_CASE_编辑规则1.png width=100%/>
</div>

## 4、设备端开发

### 4.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 4.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“WORST-CASE”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

1. **填写Wi-Fi名称及密码**

&emsp;&emsp;
在main.py中，填写可用的Wi-Fi名称及密码。

``` python
# wifi连接的的ssid和pwd定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

2. **修改设备端三元组**

&emsp;&emsp;
在main.py中，填写创建的设备三元组信息。关于设备三元组的获取，请参考[创建设备及获取三元组](./README.md "创建设备及获取三元组")中的步骤。

``` python
# 三元组信息
productKey     = "产品key"
deviceName     = "设备名称"
deviceSecret   = "设备密钥"
```
