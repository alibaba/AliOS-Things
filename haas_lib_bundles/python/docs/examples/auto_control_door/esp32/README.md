# 自动感应门
&emsp;&emsp;
下图是本案例的四步导学。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门-导览.png width=80%/>
</div>

## 简介
### 背景
&emsp;&emsp;
目前很多写字楼、酒店等入口都采用自动感应的方式开门、关门，当有人靠近时，门就会自动打开，远离时会自动关门；有时候也需要将自动门设置常开状态，比如需要多次进出大门搬卸货物的时候，不需要让门关闭；或者夜晚、放假时，需要将自动门设置常闭状态，不允许任何人进入，除非人为设置解锁为正常状态。本案例采用ESP32开发板结合红外感应、舵机以及阿里云物联网平台，只需4步，就可以搭建简单的应用场景来模拟自动感应门的功能。

### 准备

1. ESP32开发板         一套
2. SG90舵机           一个
3. 红外避障传感器       一个
4. 连接线             若干

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| ESP32开发板 | 1套 | [ESP32开发板一套](https://haas.iot.aliyun.com/solution/detail/hardware?spm=a2cpu.b17028610.0.0.6c2360b1yDodxD&versionId=800C9562896F994200000001&dataId=800C9562896F9942)|
| SG90角度舵机 | 1个 | [舵机一个](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.737613fdaUg7NB&id=41248626148&ns=1&abbucket=9&skuId=4167665919053) |
| 红外避障传感器 | 1个 | [红外避障传感器一个](https://detail.tmall.com/item.htm?id=40844279989&ali_refid=a3_430582_1006:1109448581:N:10dd5ZFgNCY07xv6IBZLPW%20Fztb96tqA:cd36f5dbea54931aaabadac652033624&ali_trackid=1_cd36f5dbea54931aaabadac652033624&spm=a230r.1.14.6) |
| 母对母杜邦线 | 若干 | [母对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh+K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440213) 请选用10cm长即可 |
| 公对母杜邦线 | 若干 | [公对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh+K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440213) 请选用10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_接线图.png width=1000%/>
</div>
<br>

## 云端开发

### 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。注册完成后登录官网。如果您已经有阿里云账号，直接登录即可。
<br>

### 创建产品、设备
&emsp;&emsp;
进入[阿里云IoT控制台](http://iot.console.aliyun.com/)，进入公共实例，创建产品和设备。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_物联网平台.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_新建产品.png width=100%/>
</div>
&emsp;&emsp;
接着进到设备管理模块，选择创建设备，输入自己的设备唯一标识，创建完后可以在设备详情页面查看设备证书，即设备的三元组信息(产品ProductKey、设备名称DeviceName、设备密钥DeviceSecret)
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_添加设备.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_设备详情.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_设备证书.png width=100%/>
</div>

&emsp;&emsp;
创建完产品、设备后开始创建产品的物模型，有关物模型的介绍说明，请到阿里云物联网平台查看。本案例创建了2个属性，一个是设备上报累计人员数量“total_person_count”，另一个是云端设置设备门禁状态属性“door_status”。具体操作流程如下，也可以参考后面流程下载物模型定义文件后直接导入上线。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_创建产品物模型1.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_创建产品物模型2.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_创建产品物模型3.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_创建产品物模型4.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_创建产品物模型5.png width=100%/>
</div>

&emsp;&emsp;
通过点击[下载](https://iotx-haas-linkin.oss-cn-shanghai.aliyuncs.com/o/config/solution/autoControlDoor/auto_control_door.zip)物模型文件，直接导入后选择发布上线。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门禁_物联网平台开发_快速导入.png width=100%/>
</div>

### IoT Stuido创建移动应用
&emsp;&emsp;
进到IoT Studio控制台[首页](https://studio.iot.aliyun.com/)，选择项目管理，新建空白项目，比如这里创建的项目名称为“自动门禁管控”。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_创建项目1.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_创建项目2.png width=100%/>
</div>
&emsp;&emsp;
点击“自动门禁管控”项目，进到项目配置，关联要使用的产品和设备
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_关联产品设备1.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_关联产品设备2.png width=100%/>
</div>
&emsp;&emsp;
接着，选择“移动应用”，新建一个移动应用。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_新建移动应用.png width=100%/>
</div>
&emsp;&emsp;
进入应用编辑，选择组件->基础组件->图表->实时曲线
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_添加实时曲线组件.png width=100%/>
</div>
&emsp;&emsp;
选择要检测的产品设备，以及设备上报的人员统计数量属性
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_数据源配置_选择产品.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_数据源配置_选择设备.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_数据源配置_选择属性.png width=100%/>
</div>
&emsp;&emsp;
接着，继续添加单选组件，选择”样式“->“配置静态数据源”。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_添加单选组件.png width=100%/>
</div>
&emsp;&emsp;
这里修改组件名称为“门禁状态设置”，同时设置配置数据源为“静态数据”，配置如下：
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_配置静态数据源.png width=100%/>
</div>
详细静态数据内容，与上面物模型定义的“门禁状态”保持一致

```json
[
  {
    "label": "正常",
    "value": "1"
  },
  {
    "label": "常闭",
    "value": "-1"
  },
  {
    "label": "常开",
    "value": "1"
  }
]
```

&emsp;&emsp;
接着切换到交互，新增交互，事件为“值改变”，动作为”设置设备属性“，之后配置设备关联要操作的产品、设备。可以点击设置设备上的门禁状态“door_status”的值，用来配置门禁常开、常闭及正常状态。常闭状态时，即使红外感应到有人接近，舵机也不会转动操作开门；常开状态时，即使没人，舵机也不回转动操作关门。此交互表示在该页面选中某个值时，会自动向设备发送选中属性的值。比如选择”常开“，会自动发送一条消息到设备上，设置”door_status=1“。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_设置门禁状态交互.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_单选配置产品设备属性.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_单选配置设备属性.png width=100%/>
</div>
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_单选配置设备属性值.png width=100%/>
</div>


<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_修改默认选中.png width=100%/>
</div>

&emsp;&emsp;
之后点击页面右上角的保存->预览
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_IoTStudio_保存预览.png width=100%/>
</div>


## 设备端开发

### 开发环境准备
&emsp;&emsp;
在进行下一步之前请确保ESP32中已经烧录了HaaS Python固件并且其开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“auto_control_door”）及工作区路径之后，硬件类型选择"nodemcu32s"，点击”立即创建“，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_HaaSStudio_创建工程.png width=100%/>
</div>


&emsp;&emsp;
将[自动感应门管控代码](./code/)文件下的所有脚本进行复制到“auto_control_door”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改auto_control_door工程里main.py中SSID和PWD的值为开发者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接开发者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
按照创建产品、设备步骤获取三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```
&emsp;&emsp;
导入代码后的目录结构及更新Wi-Fi和三元组位置如下截图
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_HaaSStudio_代码目录和参数替换.png width=100%/>
</div>

&emsp;&emsp;
最后点击VSCode中HaaS Studio工程底部的部署运行或串口，选择对应的端口编号烧录程序。详细烧录方法请参考：[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。烧录后串口执行log如下：

```log
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: xxx.xxx.xxx.xxx
NTP start...
NTP done
establish tcp connection with server(host='xxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
init ir...
ir inited!
init servo...
buzzer inited!
open the door
object detected, count = 1
累计人员数量上报成功
object detected, count = 2
累计人员数量上报成功
object detected, count = 3
累计人员数量上报成功
object detected, count = 4
累计人员数量上报成功
no object detected
close the door
no object detected
no object detected
object detected, count = 5
累计人员数量上报成功
object detected, count = 6
累计人员数量上报成功
door_status set value : 1                
object detected, count = 7
```

## 端云连调
### 云端查看设备上报结果
&emsp;&emsp; 
此时如果用物体遮挡红外感应器，舵机则会立即转动90度，隔5秒后会自动再转回原位置。同时可以看到云端实时曲线展示截止当前累计人员流量曲线图。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_HaaSStudio_云端预览设置.png width=100%/>
</div>

&emsp;&emsp; 
接着通过云端设置状态为“常开”，也可以手机扫码设置，此时在VSCode可以看到如下log显示，同时舵机会自动转动90度。之后如果用物体遮挡红外，舵机不会再转动，物体远离红外，舵机也不会再转回原位置。

```log
object detected, count = 26
累计人员数量上报成功
door_status set value : 1                #手机端页面设置“常开”状态时收到的指令日志
object detected, count = 27
```

&emsp;&emsp;
扫描二维码后，开启设备进行拍摄，此时在手机端就可以看到如下监控曲线，同时也可以选中门禁状态，会自动向设备端发送状态设置，日志和上面的PC页面设置时一样。这里的二维码有效期24小时，如需发布长期使用，请在阿里云[域名注册](https://wanwang.aliyun.com/)申请独立域名后在Iot Studio配置才可。
<div align="center">
<img src=./../../../images/auto_control_door/自动感应门_手机端效果.png width=60%/>
</div>

## 后记
&emsp;&emsp;
笔者这里实现了软件和硬件的开发，实际产品还需要结构设计，读者完成此案例之后可以尝试设计此自动开门系统的结构设计（可以用3D打印或亚克力外壳裁剪拼接）。
