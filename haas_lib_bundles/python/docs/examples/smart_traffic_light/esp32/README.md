# 智能红绿灯
&emsp;&emsp;
本案例导学步骤如下图所示。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_案例步骤.png width=80%/>
</div>

## 1、简介
### 1.1、背景
&emsp;&emsp;
红绿灯是交通路口用来指挥车辆、行人的信号灯，如果能根据车流量自动调整交通红绿灯的时长，可以缓解交通压力、提高每个路口的通行效率。本案例是结合之前提供的“车辆拥堵检测案例”的方案识别是否发生了车辆拥堵，再结合RGB传感器，控制红灯及绿灯的时长周期，最后使用IoT Studio搭建手机端远程控制的方式实现简易的智能红绿灯方案。

### 1.2、设计方案
&emsp;&emsp;
通过摄像头拍摄红绿灯路口的车辆，发送云端进行识别，如果连续几次拍照识别的车辆数量大于设定的阈值“warningVehicleNum”，则调整下次红绿灯的时长。具体方案和参数设置如下：

&emsp;&emsp;
1、初始红灯60秒、黄灯3秒、绿灯30秒，每4次拍摄为一组

&emsp;&emsp;
2、摄像头3秒拍摄1次,连续4次拍摄的车辆数都是大于M量车,判断当前是红灯或黄灯,则将接下来的绿灯加10秒,下一次的红灯减去10秒,如果当前就是绿灯,则不处理,继续下一组拍摄

&emsp;&emsp;
3、如果有一次拍摄不满足大于M量车,则下一次灯的周期恢复为原始设置周期

&emsp;&emsp;
4、如果云端有强制设置红绿灯时间周期或者强制恢复本地默认周期,则以云端为准
要求红灯最低只能减到30秒,绿灯最多60秒,其中一个达到限制点后,不再继续调整

### 1.3、准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. RGB LED                  一个
4. M5Stack GROVE连接线       一条
5. 公对母杜邦连接线            若干     

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| RGB LED | 1个 | [RGB LED](https://detail.tmall.com/item.htm?spm=a1z10.5-b-s.w4011-16538328951.139.53cd410bJVsY2E&id=548418198148&rn=27fcce6fba3a11a94b9607b415ef397e&abbucket=7) |
| M5Stack GROVE 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |
| 公对母杜邦线 | 4根 | [公对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh%20K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440218) 请选用15cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_连接图1.png width=60%/>
<img src=./../../../images/smart_traffic_light/智能红绿灯_连接图2.png width=90%/>
</div>

&emsp;&emsp;
RGB传感器使用请参考[链接](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CDF6C7F03209E00000001&dataId=800CDF6C7F03209E&s=)
<br>

## 2、阿里云IoT物联网平台创建产品、设备
&emsp;&emsp;
在本案例中涉及到云端AI识别功能都聚合在HaaS云端积木中，所以无需单独使用**对象存储OSS**和**视觉智能开发平台**。

### 2.1、注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。注册完成后登录官网。如果您已经有阿里云账号，直接登录即可。
<br>

### 2.1、创建产品、设备
&emsp;&emsp;
进入[阿里云IoT控制台](http://iot.console.aliyun.com/)，创建产品和设备。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_新建产品.png width=100%/>
</div>
<br>
&emsp;&emsp;
这里选择批量创建设备
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_批量添加.png width=100%/>
</div>
<br>

&emsp;&emsp;
导入物模型文件，[点击](https://iotx-haas-linkin.oss-cn-shanghai.aliyuncs.com/o/config/HaaSPythonExamples/smart_traffic_light/model.zip)下载物模型文件，直接导入后选择发布上线。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_快速导入.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_发布物模型.png width=100%/>
</div>
<br>
&emsp;&emsp;
创建完产品、设备后进入HaaS云端积木平台绑定刚创建的产品。

### 2.3、开通HaaS增值服务

1. **登录HaaS官网**

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/) 。
<div align="center">
<img src=./../../../images/smart_traffic_light/AI_HaaS官网.png width=100%/>
</div>
<br>

2. **进入HaaS云端积木控制台**

&emsp;&emsp;
如**上图**所示，点击右上角的“控制台”，进入HaaS云端积木的控制台页面。

<div align="center">
<img src=./../../../images/smart_traffic_light/AI_HaaS控制台.png width=100%/>
</div>


3. **绑定产品**

&emsp;&emsp;
进入“设备管理”，选择“产品”，绑定物联网平台产品，选择刚创建的产品，点击“确定”绑定即可。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_HaaS官网_绑定物联网产品.png width=100%/>
</div>
<br>

4. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”，就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。这里的设备三元组和物联网平台是一样的。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_HaaS控制台_查看三元组.png width=100%/>
</div>
<br>

## 3、设备端开发

### 3.1、开发环境准备
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 3.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“智能红绿灯”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

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
最后点击工程底部的部署运行或串口，选择对应的端口编号烧录程序

## 4、IoT Studio 移动应用搭建
&emsp;&emsp;
如果不需要通过手机端控制红绿灯时长，可以跳过此步骤。

&emsp;&emsp;
进到IoT Studio控制台[首页](https://studio.iot.aliyun.com/)，选择项目管理，新建空白项目，比如这里创建的项目名称为“haas_智能红绿灯项目”。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS新建项目.png width=100%/>
</div>
&emsp;&emsp;
点击项目名称“haas_智能红绿灯项目”，进到项目配置，关联要使用的产品和设备
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS关联产品.png width=100%/>
</div>
&emsp;&emsp;
接着，点击主页，选择“移动应用”，新建一个“空白”的移动应用
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS创建移动应用.png width=100%/>
</div>
&emsp;&emsp;
进入应用编辑，开始配置红灯时长页面，选择组件->基础组件->表单->下拉框 及 基础组件->控制->按钮，将这两个组件分别拖到画布上，如下图：  
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_应用页面配置1.png width=100%/>
</div>
&emsp;&emsp;
接着选中下拉框组件，在右侧“样式->内容配置->列表内容”配置数据源，这里配置的是静态数据源且默认值方式选择“始终列表第一项”，如下图：
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_应用页面配置2.png width=100%/>
</div>
详细的静态内容如下：

```json
[
  {
    "label": "30",
    "value": 30
  },
  {
    "label": "40",
    "value": 40
  },
  {
    "label": "50",
    "value": 50
  },
  {
    "label": "60",
    "value": 60
  }
]
```
&emsp;&emsp;
接着，配置按钮的事件，选中按钮组件，在页面右侧选择“交互->新增交互”，添加按钮的“单击”事件，同时绑定要操作的产品和设备及对应的属性，这里的属性是选择物联网平台创建物模型时的属性参数。绑定后的页面如下图：
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS设置设备属性.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_应用页面配置3.png width=100%/>
</div>


&emsp;&emsp;
同样，绿灯时长配置可以参考上面红灯时长配置方法，且下拉框的静态数据保持一致。
最后增加一个“恢复默认值(红灯60秒,黄灯3秒,绿灯30秒)”的按钮，清除云端下发的红绿灯时长的参数配置。
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS设置设备属性1.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_IS设置设备属性2.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_物联网平台开发_应用页面配置4.png width=100%/>
</div>

&emsp;&emsp;
之后点击页面右上角的保存->预览
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_IOTStudio应用保存预览.png width=60%/>
</div>


## 5、运行结果
### 5.1、设备本地查看

&emsp;&emsp;
在网络连接成功后，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且识别到车辆后在屏幕上显示红色Vehicle Num: xx的字样。“xx”代表当前图片中车辆的数量。根据代码设置的规则，如果连续4次识别的车辆数都超过了设置的阈值“warningVehicleNum”且当前是红灯情况，则会调整下次的红灯时长减去10秒，详细设计方案见文章开始部分的介绍。

&emsp;&emsp;
可以通过如下log，即needChangePeriod为true时，会调整下一次红绿灯的时长，且调整后各自的时长周期在日志“rgb_current_period after change”后打印，如果有云端主动下发，则setting_from_server=True，之后就以云端下发的值为准，不再根据识别的结果调整。
```log
now needChangePeriod is  True
rgb_current_period after change: [50, 3, 40]
```


具体识别结果参考下图(条件不允许情况，可以在PC电脑上找几个红绿灯路口的车辆图片进行拍照识别)：
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_设备识别结果.png width=50%/>
</div>

&emsp;&emsp;
串口日志打印如下：

```log
=== wait python process interrupt ===
upload process...0%
upload process...26%
upload process...43%
upload process...58%
upload process...78%
upload process...97%
=== upload progress end ===
--- Miniterm on /dev/cu.wchusbserial5319013481  115200,8,N,1 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
flash_get_info no:5 offset:0x16000 size:0x1000
128: ota crc cal:0x6514 param:0xffff
ota upg_flag:0xffffcount:0 crc:0xffffboot_type:255 
No OTA upgrade.
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.3.201
NTP start
NTP done
establish tcp connection with server(host='xxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
rgb灯线程
current_time: 1652715678
0
[   6.770]<A>AOS create camera device successfully
当前灯的颜色:0xff0000 亮灯时间: 60s
needChangePeriod changed False
[  15.250]<E>AIOT_UPLOAD MQTT Upload file(test.jpg) ID(F5fbF0lzCJfI70GQuAVy010200) success
detect: 11 vehicle
recognize time : 3142
Vehicle Num:11 !!!
Vehicle Num:11 !!!
now needChangePeriod is  False
false setting_from_server: False setting_resume_default: 0
rgb_current_period change to: [60, 3, 30]
[  29.420]<E>AIOT_UPLOAD MQTT Upload file(test.jpg) ID(292585NGvFO2RbXvCtzr010200) success
Vehicle Num:11 !!!
recognize time : 12871
Vehicle Num:11 !!!
establish tcp connection with server(host='xxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
success to establish tcp, fd=54
Vehicle Num:11 !!!
now needChangePeriod is  True
curGNumber: 0 setting_from_server: False setting_resume_default: 0
rgb_current_period after change: [50, 3, 40]
```

### 5.2、手机端效果
<div align="center">
<img src=./../../../images/smart_traffic_light/智能红绿灯_IOTStudio_手机预览.png width=50%/>
</div>

&emsp;&emsp;
扫描二维码后，开启设备进行拍摄，此时在手机端就选择配置下发到设备。这里的二维码有效期24小时，如需发布长期使用，请在阿里云[域名注册](https://wanwang.aliyun.com/)申请独立域名后在Iot Studio配置才可。读者也可以修改代码，实现自己的控制方案。