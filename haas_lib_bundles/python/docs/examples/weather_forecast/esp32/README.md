# 基于GPS定位的HaaS天气预报系统
&emsp;&emsp;
天气预报是对某一地点当前和未来的大气层的状态进行预测，目前天气预报主要是收集气温、湿度、风速、气压数据，然后利用气象学来确定未来空气变化。由于大气过程是多变的，所以天气预报有一定误差的。本案例利用GPS定位获取设备位置，然后将位置信息传到云端，由云端获取近几天的天气状况返回给设备端展示。

&emsp;&emsp;
本案例导学步骤如下图所示。
<div align="center">
<img src=./../../../images/weather_forecast/天气预报-步骤简介.png width=80%/>
</div>

## 1、简介
### 1.1、背景知识
&emsp;&emsp;
天气预报，可以在设备上实时查看当前和未来几天的天气状况，及时作出相应调整，比如将设备放在偏远的山上，就可以通过设备自身的定位实时获取该区域最近几天的天气状况，以便提醒是否需要带雨伞上山。

&emsp;&emsp;
本案例使用的HT2828Z3G5L GPS传感器是一款高性能的面向车载组合导航领域的定位G-MOUSE，系统包含同时支持北斗和GPS的高性能卫星接收机芯片。具备全方位功能，能满足专业定位的严格要求。同时该传感器体积小巧，可以装置在汽车内部任何位置，低功耗，能适应个人用户的需要。需要注意GPS传感器需要放到空旷区域，办公室内等需要将设备放在露天阳台或窗户外面才能获取到定位信息。

### 1.2、场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
2. 开通HaaS Cloud服务
3. 设备端开发
4. 设备运行

### 1.3、准备

1. M5StackCore2开发板       一套
2. GPS传感器                一个
3. 杜邦线                   若干
4. XH2.54间距端子连接线      一根

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5StackCore2开发版 | 1 | [M5StackCore2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5StackCore2开发套件自带 |
| GPS传感器 | 1 | [HT2828Z3G5L 型号的GPS传感器](https://haas.iot.aliyun.com/solution/detail/hardware?spm=a2cpu.b17028610.0.0.17a360b1XpfMzG&versionId=800C7C08112B0A0700000002&dataId=800C7C08112B0A07) |
| 公对母杜邦线 | 6根 | [公对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh%20K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440218) 请选用15cm长即可 |
| 端子连接线 | 1根 | [XH2.54间距端子连接线](https://item.taobao.com/item.htm?id=625846591396&ali_refid=a3_420434_1006:1341550160:N:QrMAkLy4PaPVs3hJP9YtZBerfi2%2BAYZj:91b083f3f611239e41b7a30c9f29ab43&ali_trackid=1_91b083f3f611239e41b7a30c9f29ab43&spm=a230r.1.0.0) 请选用6P 10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/weather_forecast/天气预报-硬件连线图.png width=100%/>
</div>
<br>

&emsp;&emsp;
本案例只需要接GPS的GND、VCC和TX端口，其中GPS的TX口必须接M5StackCore2的UART2_RXD口，该接口即为board.json文件中配置的gnss的port。

## 2、云端平台功能开通
&emsp;&emsp;
云端的步骤分为：
1. 注册阿里云账号并登录阿里云账号；
2. 在HaaS Cloud控制台创建产品、设备；

### 2.1、注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/register/qr_register.htm?oauth_callback=https%3A%2F%2Fwww.aliyun.com) 。如果您已经有阿里云账号，请忽略该步骤，直接进入下一步。

<br>

### 2.2、开通HaaS Cloud服务

1. **登录HaaS官网**

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/) 。
<div align="center">
<img src=./../../../images/AI_HaaS官网.png width=100%/>
</div>

<br>

2. **进入HaaS Cloud控制台**

&emsp;&emsp;
如**上图**所示，点击右上角的“控制台”，进入HaaS Cloud的控制台页面。

<div align="center">
<img src=./../../../images/AI_HaaS控制台.png width=100%/>
</div>

<br>


3. **创建设备**

&emsp;&emsp;
在左侧导航栏设备管理中，可以看到产品和设备选项，点击“产品”，可以看到平台默认创建了“haas_正式游客产品”这个产品；
所以开发者只需要创建设备即可。点击“批量添加”， 添加方式为“自动生成”， 申请数量可填为“1”个，即可完成新设备的创建。
<div align="center">
<img src=./../../../images/AI_创建设备.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/AI_创建设备_设备信息.png width=100%/>
</div>
<br>

4. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”，就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/AI_查看设备信息.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/AI_设备三元组.png width=100%/>
</div>
<br>

&emsp;&emsp;
完成以上步骤表示开通了HaaS Cloud服务，您可以拿到生成的三元组进行设备端开发，通过该三元组认证连云。

## 3、设备端开发

### 3.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 3.2、创建项目

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”，此处选择"M5Stack-Core2"，再从右侧的案例中选择“天气预报系统”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

> Python脚本的详细说明请参考脚本内嵌的文字注释
1. **修改路由器名称及密码**

&emsp;&emsp;
修改main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
将上面步骤获取的三元组信息，填入main.py中：

```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-DeviceName"
deviceSecret  = "Your-DeviceSecret"
```

## 4、运行

&emsp;&emsp;
烧录代码到设备上之后，在网络连接成功时，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。之后串口会打印如下日志，并且获取到GPS的经纬度坐标后在屏幕上显示最近几天的日期、温度等数据，如下所示，其中经纬度日志打印如下

```log
The gnss infor 纬度-%d 经度-%d 海拔-%d [120.0748, 'E'] [30.13433, 'N'] 26.2
```

&emsp;&emsp;
获取到的预测天气信息日志如下：其中date表示日期，daytemp表示白天最高温度，nighttemp表示夜晚最低温度。
```log
date=2022-06-09,daytemp=32,nighttemp=22
```

详细的log如下：

```log
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.xxx.xxx
NTP start
NTP done
gnss init...
establish tcp connection with server(host='productKey.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=55
link platform connected
The gnss infor 纬度-%d 经度-%d 海拔-%d [120.0748, 'E'] [30.13433, 'N'] 26.2
latitude: 30.13433, longitude: 120.0748
get response time : 16
casts:[{"nightpower": "4", "week": "4", "daytemp": "32", "nightwind": "东", "dayweather": "阴", "nighttemp": "22", "daywind": "东", "date": "2022-06-09", "daypower": "4", "nightweather": "阴"}, {"nightpower": "4", "week": "5", "daytemp": "26", "nightwind": "东", "dayweather": "小雨", "nighttemp": "21", "daywind": "东", "date": "2022-06-10", "daypower": "4", "nightweather": "小雨"}, {"nightpower": "5", "week": "6", "daytemp": "27", "nightwind": "东", "dayweather": "小雨", "nighttemp": "21", "daywind": "东", "date": "2022-06-11", "daypower": "5", "nightweather": "小雨"}, {"nightpower": "≤3", "week": "7", "daytemp": "24", "nightwind": "东", "dayweather": "中雨", "nighttemp": "20", "daywind": "东", "date": "2022-06-12", "daypower": "≤3", "nightweather": "小雨-中雨"}]
date=2022-06-09,daytemp=32,nighttemp=22
date=2022-06-10,daytemp=26,nighttemp=21
date=2022-06-11,daytemp=27,nighttemp=21
date=2022-06-12,daytemp=24,nighttemp=20
```

&emsp;&emsp;
设备端运行效果如下图：
<div align="center">
<img src=./../../../images/weather_forecast/天气预报-运行结果.png width=100%/>
</div>

&emsp;&emsp;
设备上目前只显示日期、温度数据，实际云端下发的天气信息也会包含风力、晴天或阴天等信息，感兴趣的话，可以在cb_lk_service方法中自行解析云端返回的结果进行展示