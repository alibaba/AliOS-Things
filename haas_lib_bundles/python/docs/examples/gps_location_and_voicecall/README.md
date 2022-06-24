# GPS定位和紧急拨号

&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../images/gps_location_and_voicecall/steps.png width=80%/>
</div>


## 1、简介

&emsp;&emsp;
在日常生活中，我们经常需要定位学生的位置，并提供紧急情况下的一键拨号功能。 本案例使用HaaS506芯片，通过gps模块进行定位，同时模拟手机通话功能。
HaaS506主控板上有三个按钮，RST重启按钮、BOOT下载模式按钮、KEY1自定义按钮。KEY1按钮可以自定义功能。本案例使用KEY1键来实现拨打电话与挂断电话。并在通话时向云端发送当前位置信息。

### 1.1、准备
&emsp;&emsp;
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
GNSS定位模块HT2828Z3G5L|1
SIM卡|1
杜邦线|若干

### 1.2、硬件图

<div align="center">
<img src=./../../images/gps_location_and_voicecall/gps1.png width=80%/>
</div>
 

## 2、物联网平台开发

&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../images/5_3_开通公共实例.png
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 2.1、创建云端产品

  * 登录[物联网平台控制台](https://iot.console.aliyun.com/lk/summary/new)
  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/1.png width=80%/>
  </div>
 
  * 在实例概览页面，找到对应的实例，单击实例进入实例详情页面（ **注意 在中国地域，目前仅华东2（上海）地域开通了公共实例服务**）。在左侧导航栏，选择设备管理 > 产品，单击创建产品，并输入产品信息，如下图所示
  
  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/2.png width=80%/>
  </div>

  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/3.png width=80%/>
  </div>
 
 - 添加设备
  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/4.png width=80%/>
  </div>
 
**注意：设备名称需要输入IMEI号，代码需要使用设备名称IMEI进行动态注册**

  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/5.png width=80%/>
  </div>
 
 点击确定
  <div align="center">
  <img src=./../../images/gps_location_and_voicecall/6.png width=80%/>
  </div>
 
 - 打开动态注册，要连接云平台必须打开。在这个界面也可以查看需要复制的产品信息。
  <div align="center">
  <img src=./images\7.png width=80%/>
  </div>
 

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
物模型可以导入或手动添加，[导入物模型文件](./link_platform/vehicle_location.zip)，以下为手动添加的操作流程。

 - 选择产品**功能定义**--**编辑草稿**
  <div align="center">
<img src=./../../images/gps_location_and_voicecall/gps2.png width=80%/>
</div>
 
 - **添加自定义功能**
设置**标识符**、**数据类型**、**读写类型**参数，标识符要与代码保持一致。点击**确定**。
 **发布上线**，点击**确定**。
  <div align="center">
<img src=./../../images/gps_location_and_voicecall/gps3.png width=80%/>
</div>
 
  <div align="center">
<img src=./../../images/gps_location_and_voicecall/gps4.png width=80%/>
</div>
 
 ### 2.3、开启动态注册功能

<div align="center">
<img src=./../../images/gps_location_and_voicecall/gps5.png width=80%/>
</div>
 
## 3、设备端开发

### 3.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保HaaS506开发环境已经搭建完毕。详情请参考[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/startup/HaaS506_startup)的说明。
<br>

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“GPS定位和紧急拨号”案例点击“立即创建”即可。

<div align="center">
<img src=./../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

### 3.3、修改代码
1. **修改修改电话号码和物联网平台的产品信息**

&emsp;&emsp;
修改工程里main.py中以下代码段

```python

###############################
# 更改产品信息
productKey = "xxxxxxxx"
productSecret = "xxxxxxxx"
# 更改电话号码
# 需要发拨打的目标电话号码，长度为11的字符串格式
tele_phone = 'xxxxxxxxxxx'
###############################

```

## 4、运行结果

&emsp;&emsp;
由于GPS模块连接TTL口进行通信，需要使用USB端口读取log，在设备管理器中找到port5对应串口

<div align="center">
<img src=./../../images/gps_location_and_voicecall/gps7.png width=80%/>
</div>
 
&emsp;&emsp;
串口调试工具log，运行后自动联网，连接阿里云平台，打印‘-----start-----’

1. 第一次按下‘key1’按钮，打印‘=== call ===’，持续打印当前设备经纬度，并拨打电话
2. 再次按下‘key1’按钮，打印‘----callend----’，挂断电话
3. 再次按下又能拨打电话，如此循环

<div align="center">
<img src=./../../images/gps_location_and_voicecall/gps8.png width=80%/>
</div>
<br>
 
