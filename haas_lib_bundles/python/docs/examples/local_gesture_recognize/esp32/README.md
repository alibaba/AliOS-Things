# 本地动态手势识别

PAJ7620 本地手势识别传感器接入案例。本案例也包含将手势识别结果上报到物联网平台的设备物模型中的功能，以便后续云侧的业务处理。

# 背景知识

PAJ7620 模块内部带有一个光学阵列式传感器，其内部算法能够将各种手部动作在其光学阵列中的探测和变化情况，识别为不同的动态手势动作。与拍照在线识别相比，本动态手势识别方案不需要联网，且识别的是动态手势，适合在手势控制需要位移或外形转换的场景中使用，目前支持的手势类型有：

* 上划
* 下划
* 左划
* 右划
* 靠近
* 远离
* 顺时针
* 逆时针
* 挥手

# 流程

<div align="center">
<img src=./../../../images/local_gesture_recognize/流程.png width=90%/>
</div>

# 准备

## 硬件

1. 任何支持 I2C 接口的开发板1套，本案例以 ESP32 乐鑫开发板作为演示
2. PAJ7620U2 手势识别模块1个
3. 杜邦线若干，或面包板+跳线若干

涉及到的硬件购买链接如下，仅供参考，本文作者不负责商家发货的品质保障等问题！

| 名称                        | 数量 | 参考链接                                         |
| --------------------------- | ---- | ------------------------------------------------ |
| PAJ7620U2手势识别传感器模块 | 1    | https://item.taobao.com/item.htm?id=618595129787 |
| ESP32-WROOM-32E             | 1    | https://item.taobao.com/item.htm?id=542143157571 |
| 蜂鸣器（高电平触发）        | 1    | https://detail.tmall.com/item.htm?id=41251333522 |
| 杜邦线或面包板              | 若干 | https://detail.tmall.com/item.htm?id=16513870165 |

硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/local_gesture_recognize/硬件连接图.png width=90%/>
</div>

## 物联网平台

本案例包括将手势识别结果上报物模型功能，所以需要开阿里云通物联网平台。
如果您已经熟悉阿里云物联网平台的基本操作，请通过以下「快速操作指引」完成物联网产品和设备的准备，并跳过「详细操作步骤」章节：

### 快速操作指引

1. [注册阿里云账号](https://account.aliyun.com/login/login.htm) 并登录 [阿里云物联网平台控制台](https://iot.console.aliyun.com/)；
2. [创建产品](https://iot.console.aliyun.com/product)；
3. 编辑产品物模型，导入 [物模型描述文件](https://hli.aliyuncs.com/o/config/HaaSPythonExamples/local_gesture_recognize/model.zip)；
4. [创建设备](https://iot.console.aliyun.com/devices) 并查看设备密钥，

如您不熟悉物联网平台的基本操作，请按以下图示，依次完成准备过程：

### 详细操作步骤

> 如已按照「快速操作指引」完成准备，请跳过此章节。

1. [注册阿里云账号](https://account.aliyun.com/login/login.htm) 并登录 [阿里云物联网平台控制台](https://iot.console.aliyun.com/)：

   <div align="center">
   <img src=./../../../images/local_gesture_recognize/物联网平台.png width=90%/>
   </div>

2. 进入产品管理页面，创建产品：

   <div align="center">
   	<img src=./../../../images/local_gesture_recognize/新建产品入口.png width=90%/>
   	<br/>
     <br/>
     <img src=./../../../images/local_gesture_recognize/新建产品过程.png width=90%/>
   </div>

3. 修改产品物模型：

   <div align="center">
   	<img src=./../../../images/local_gesture_recognize/前往定义物模型.png width=90%/>
   </div>

4. 导入 [物模型描述文件](https://hli.aliyuncs.com/o/config/HaaSPythonExamples/local_gesture_recognize/model.zip)：

   <div align="center">
   	<img src=./../../../images/local_gesture_recognize/功能定义.png width=90%/>
   	<br/>
     <br/>
     <img src=./../../../images/local_gesture_recognize/快速导入.png width=90%/>
     <br/>
     <br/>
     <img src=./../../../images/local_gesture_recognize/导入完成.png width=90%/>
   </div>

5. 发布物模型：

   <div align="center">
   	<img src=./../../../images/local_gesture_recognize/发布物模型.png width=90%/>
   </div>

6. 进入设备管理页面添加设备：

   <div align="center">
   	<img src=./../../../images/local_gesture_recognize/添加设备.png width=90%/>
   </div>

7. 查看设备详情：

   <div align="center">
     <img src=./../../../images/local_gesture_recognize/设备列表.png width=90%/>
   <br/><br/>
     <img src=./../../../images/local_gesture_recognize/设备详情.png width=90%/>
   </div>

8. 查看并记录设备证书：

   <div align="center">
     <img src=./../../../images/local_gesture_recognize/设备证书.png width=90%/>
   </div>

# 设备端开发

1. 开发环境

请确保 ESP32 开发环境已经搭建完毕。详见 [ESP32快速开始](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/startup/ESP32_startup)。

2. 创建解决方案

如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“gesture”）及工作区路径之后，硬件类型选择 nodemcu32s，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/local_gesture_recognize/创建工程.png width=60%/>
</div>

3. 将 [手势识别](./code/) 文件夹中的所有脚本复制到“gesture”工程根目录中。程序代码的功能说明请参考脚本内嵌注释。

4. 修改 main.py 文件顶部的信息，包括设备接入密钥、 WiFi 名称和密码（请注意，名称和密码都需要放在''符号中间）

```python
######################
#### 请修改此处信息 ####
PRODUCT_KEY = '这里填写产品PK'
DEVICE_NAME = '这里填入设备名称DN'
DEVICE_SECRET = '这里填入设备密钥DS'
WIFI_SSID = 'WiFi名称'
WIFI_PWD = 'WiFi密码'
######################
```

# 查看结果

在开发板上部署运行脚本，查看结果。
当网络连接或物联网平台连接中或连接失败时，会打印等待中或出错的日志；
当手势识别模块初始化失败时，蜂鸣器会有连续 3 声短鸣提示；
初始化过程一切正常，蜂鸣器会有 1 声长鸣提示，表示手势识别已就绪。

## 查看日志

在PAJ7620U2模块前方10~20cm范围内做出手势，查看日志输出结果：

```log
Connecting to WiFi...
Waiting for WiFi connection...
Waiting for WiFi connection...
('192.168.3.49', '255.255.255.0', '192.168.3.1', '192.168.3.1')
WiFi Connected
Connecting to IoT LinkPlatform...
establish tcp connection with server(host='*****.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
Waiting for IoT Platform connection...
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
ON_CONNECT:  {'code': 0}
IoT Platform connected
Initializing gesture recognizer...
Done
LEFT
DOWN
RIGHT
UP
BACKWARD
FORWARD
CLOCKWISE
ANTICLOCKWISE
WAVE
```

## 查看物模型信息

进入阿里云物联网平台，打开设备的物模型数据，查看上报到物模型的手势识别结果和上报的手势识别事件：

<div align="center">
<img src=./../../../images/local_gesture_recognize/物模型数据.png width=90%/>
</div>
