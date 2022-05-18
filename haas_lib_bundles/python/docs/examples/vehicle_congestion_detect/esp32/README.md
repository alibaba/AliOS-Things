# 车辆拥堵检测
&emsp;&emsp;
本案例导学步骤如下图所示。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_案例步骤.png width=80%/>
</div>

## 简介
### 背景
&emsp;&emsp;
交通拥堵一直是全世界大中小城市面临的一大难题，目前交通路口一般是通过安装很多摄像头拍摄路口状况，再通过识别算法判断是否发生交通阻塞。本案例是通过使用HaaS Python对摄像头图像进行采集，调用HaaS云端积木能力对图片中的车辆进行识别，根据识别出的车辆数量判断是否发生拥堵并借助IoT Studio搭建手机端监控显示。

### 准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    一条

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/COM_M5Stack硬件连线图.png width=60%/>
</div>
<br>

## 阿里云IoT物联网平台创建产品、设备
&emsp;&emsp;
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用**对象存储OSS**和**视觉智能开发平台**。

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。注册完成后登录官网。如果您已经有阿里云账号，直接登录即可。
<br>

## 创建产品、设备
&emsp;&emsp;
进入[阿里云IoT控制台](http://iot.console.aliyun.com/)，创建产品和设备。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_物联网平台开发_新建产品.png width=100%/>
</div>
<br>
&emsp;&emsp;
这里选择批量创建设备
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_物联网平台开发_批量添加.png width=100%/>
</div>
<br>

&emsp;&emsp;
导入物模型文件[点击](https://iotx-haas-linkin.oss-cn-shanghai.aliyuncs.com/o/config/solution/vehicleCongestion/Detect/model.zip)下载物模型文件，直接导入后选择发布上线。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_物联网平台开发_快速导入.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_物联网平台开发_发布物模型.png width=100%/>
</div>
<br>
&emsp;&emsp;
创建完产品、设备后进入HaaS云端积木平台绑定刚创建的产品。

## 开通HaaS增值服务

1. **登录HaaS官网**

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/) 。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/AI_HaaS官网.png width=100%/>
</div>

<br>

2. **进入HaaS云端积木控制台**

&emsp;&emsp;
如**上图**所示，点击右上角的“控制台”，进入HaaS云端积木的控制台页面。

<div align="center">
<img src=./../../../images/vehicle_congestion_detect/AI_HaaS控制台.png width=100%/>
</div>

<br>


3. **绑定产品**

&emsp;&emsp;
进入“设备管理”，选择“产品”，绑定物联网平台产品，选择刚创建的产品，点击“确定”绑定即可。如果您当前是游客用户，可在我的->个人信息查看账号状态，进行申请转为正式用户后再进行此步骤。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_HaaS官网_绑定物联网产品.png width=100%/>
</div>
<br>

4. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”，就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_HaaS控制台_查看三元组.png width=100%/>
</div>
<br>

## 设备端开发

### 开发环境准备
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“vehicle_congestion_detect”）及工作区路径之后，硬件类型选择m5stackcore2，点击”立即创建“，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_HaaSStudio_创建工程.png width=30%/>
</div>


&emsp;&emsp;
将[车辆拥堵检测代码](./code/)文件下的所有脚本进行复制到“vehicle_congestion_detect”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改fruits_recognization工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
按照[获取设备的三元组]获取三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```
&emsp;&emsp;
最后点击Iot Studio工程底部的部署运行或串口，选择对应的端口编号烧录程序
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_部署运行代码.png width=70%/>
</div>

## IoT Studio 移动应用搭建
&emsp;&emsp;
如果只想在设备上看到识别结果，可以跳过此步骤。该步骤是为了让检测结果发送到云端并在手机侧展示。

&emsp;&emsp;
进到IoT Studio控制台[首页](https://studio.iot.aliyun.com/)，选择项目管理，新建空白项目，比如这里创建的项目名称为“AI识别”。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio创建项目.png width=100%/>
</div>
&emsp;&emsp;
点击“AI识别”项目，进到项目配置
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio关联产品设备1.png width=100%/>
</div>
&emsp;&emsp;
关联要使用的产品和设备
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio关联产品设备2.png width=100%/>
</div>
&emsp;&emsp;
接着，选择“移动应用”，新建一个移动应用
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio创建移动应用.png width=100%/>
</div>
&emsp;&emsp;
进入应用编辑，选择组件->基础组件->图表->实时曲线
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio添加移动应用组件.png width=100%/>
</div>
&emsp;&emsp;
选择要检测的产品设备，以及设备上报的车辆数量属性
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio组件绑定设备属性.png width=100%/>
</div>
&emsp;&emsp;
之后点击页面右上角的保存->预览
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio应用保存预览.png width=100%/>
</div>


## 运行结果
### 设备本地查看

&emsp;&emsp;
在网络连接成功后，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且识别到水果后在屏幕上显示红色Vehicle Num: xx的字样。“xx”代表当前图片中车辆的数量。具体识别结果参考下图：
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_设备识别结果.png width=50%/>
</div>

&emsp;&emsp;
串口日志打印如下：

```log
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: xxx.xxx.xxx.xxx
NTP start
NTP done
establish tcp connection with server(host='***.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
{'ext': '{"result":"failed"}', 'commandName': 'recognizeLogoReply', 'commandType': 'haas.faas', 'argInt': 1}
do not detect!
车辆数量上报成功
get response time : 838
{'ext': '{"elements":[{"boxes":[{"bottom":144,"left":143,"right":190,"top":107}],"score":0.90478516,"typeName":"vehicle"},{"boxes":[{"bottom":125,"left":119,"right":165,"top":81}],"score":0.89990234,"typeName":"vehicle"},{"boxes":[{"bottom":119,"left":171,"right":211,"top":85}],"score":0.8989258,"typeName":"vehicle"},{"boxes":[{"bottom":136,"left":200,"right":249,"top":101}],"score":0.8964844,"typeName":"vehicle"},{"boxes":[{"bottom":154,"left":84,"right":132,"top":113}],"score":0.89501953,"typeName":"vehicle"},{"boxes":[{"bottom":168,"left":24,"right":67,"top":119}],"score":0.89501953,"typeName":"vehicle"},{"boxes":[{"bottom":126,"left":240,"right":286,"top":93}],"score":0.8886719,"typeName":"vehicle"},{"boxes":[{"bottom":105,"left":95,"right":123,"top":78}],"score":0.8232422,"typeName":"vehicle"},{"boxes":[{"bottom":131,"left":71,"right":107,"top":100}],"score":0.7128906,"typeName":"vehicle"},{"boxes":[{"bottom":90,"left":80,"right":109,"top":68}],"score":0.49121094,"typeName":"vehicle"},{"boxes":[{"bottom":109,"left":47,"right":87,"top":71}],"score":0.47265625,"typeName":"vehicle"}],"result":"success"}', 'commandName': 'recognizeLogoReply', 'commandType': 'haas.faas', 'argInt': 1}
detect: 11vehicle
车辆数量上报成功
Vehicle Num:11 !!!
get response time : 940
{'ext': '{"elements":[{"boxes":[{"bottom":153,"left":79,"right":129,"top":111}],"score":0.90283203,"typeName":"vehicle"},{"boxes":[{"bottom":121,"left":239,"right":284,"top":88}],"score":0.90283203,"typeName":"vehicle"},{"boxes":[{"bottom":115,"left":167,"right":209,"top":80}],"score":0.89501953,"typeName":"vehicle"},{"boxes":[{"bottom":141,"left":140,"right":187,"top":104}],"score":0.89160156,"typeName":"vehicle"},{"boxes":[{"bottom":122,"left":114,"right":161,"top":79}],"score":0.8671875,"typeName":"vehicle"},{"boxes":[{"bottom":133,"left":194,"right":247,"top":98}],"score":0.85009766,"typeName":"vehicle"},{"boxes":[{"bottom":167,"left":20,"right":62,"top":117}],"score":0.81640625,"typeName":"vehicle"},{"boxes":[{"bottom":129,"left":66,"right":105,"top":97}],"score":0.78808594,"typeName":"vehicle"},{"boxes":[{"bottom":107,"left":41,"right":81,"top":69}],"score":0.7470703,"typeName":"vehicle"},{"boxes":[{"bottom":102,"left":90,"right":118,"top":76}],"score":0.59472656,"typeName":"vehicle"},{"boxes":[{"bottom":78,"left":20,"right":45,"top":52}],"score":0.57958984,"typeName":"vehicle"},{"boxes":[{"bottom":61,"left":42,"right":70,"top":35}],"score":0.4020996,"typeName":"vehicle"},{"boxes":[{"bottom":88,"left":30,"right":58,"top":64}],"score":0.39501953,"typeName":"vehicle"}],"result":"success"}', 'commandName': 'recognizeLogoReply', 'commandType': 'haas.faas', 'argInt': 1}
detect: 13vehicle
车辆数量上报成功
Vehicle Num:13 !!!
get response time : 599
{'ext': '{"result":"failed"}', 'commandName': 'recognizeLogoReply', 'commandType': 'haas.faas', 'argInt': 1}
do not detect!
车辆数量上报成功
get response time : 549
```

### PC和手机端效果
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio_PC预览.png width=80%/>
</div>

&emsp;&emsp;
扫描二维码后，开启设备进行拍摄，此时在手机端就可以看到如下监控曲线。这里的二维码有效期24小时，如需发布长期使用，请在阿里云[域名注册](https://wanwang.aliyun.com/)申请独立域名后在Iot Studio配置才可。
<div align="center">
<img src=./../../../images/vehicle_congestion_detect/车辆拥堵检测_IOTStudio_手机扫码查看实时数据.png width=50%/>
</div>
