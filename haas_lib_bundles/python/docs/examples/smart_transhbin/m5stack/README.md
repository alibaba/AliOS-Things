# 智能垃圾桶
&emsp;&emsp;
本系统在物体接近后，通过红外避障传感器检测到物体，然后启动摄像头抓拍图片上传至云端，云端通过HaaS增值服务对抓拍的图片进行识别，并将垃圾类别、物品名称和可信度等信息返回给设备端，设备端根据返回的垃圾类别，自动打开对应类别的垃圾桶，待投放者离开后，再关上垃圾桶。

下图是本案例的四步导学。

<div align="center">
<img src=./../../../images/smart_trashbin/步骤指引.png width=80%/>
</div>



## 背景知识

&emsp;&emsp;
红外避障传感器：利用物体的反射性质，在一定范围内，如果没有障碍物，发射出去的红外线，距离越远越弱，如果有障碍物，红外线遇到障碍物就会被反射，传感器接收头接收到这一信号，就可以确定正前方有物体。

&emsp;&emsp;
HaaS增值服务：集多种人工智能算法为一体的云端服务，通过设备端上传图片到云端，云端对图片进行识别，并把结果返回给设备的服务，目前已涵盖图片识别、文字识别、物品识别、人型识别等多个类别。详情请参考:[HaaS AI API说明文档](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.0.0.0.582260b1ysbMcT#/Python/docs/zh-CN/haas_extended_api/cloudAI)

&emsp;&emsp;
垃圾分类目前分为可回收垃圾、有害垃圾、厨余垃圾(湿垃圾)、其他垃圾(干垃圾)。

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
2. HaaS云端积木平台创建设备
3. 设备端开发
4. 设备运行

## 准备

1. [M5Stack Core2开发板](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A) 一套
2. M5Stack Unit CAM摄像头 一个
3. 连接线 一根
4. [红外避障传感器](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C48B2CEA2F29A00000001&dataId=800C48B2CEA2F29A) 一个
5. [SG90舵机](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5A3F64C2F88300000001&dataId=800C5A3F64C2F883) 一个
6. 公对母、公对公杜邦线 若干

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |
| 红外避障传感器 | 1 | [红外避障传感器](https://detail.tmall.com/item.htm?id=40844279989&ali_refid=a3_430582_1006:1109448581:N:10dd5ZFgNCY07xv6IBZLPW%20Fztb96tqA:cd36f5dbea54931aaabadac652033624&ali_trackid=1_cd36f5dbea54931aaabadac652033624&spm=a230r.1.14.6)  |
| 公对母杜邦线 | 若干 | [杜邦线](https://detail.tmall.com/item.htm?spm=a230r.1.14.22.b10c6663PCvNOq&id=41254478179&ns=1&abbucket=7&skuId=3211140814458) 请选用10cm长即可 |
| SG90角度舵机 | 1 | [SG90舵机](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.737613fdaUg7NB&id=41248626148&ns=1&abbucket=9&skuId=4167665919053) |

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/smart_trashbin/接线图.png width=80%/>
</div>

<br>

## 云端平台功能开通
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用 **对象存储OSS** 和 **视觉智能开发平台**。因此，整个云端步骤分为下面两步。
```python
1. 注册阿里云账号并登录；
2. 登录HaaS云端积木控制台创建设备；
```

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，如已有阿里云账号可跳过这步，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。

<br>

## HaaS云端积木平台创建设备

1. **登录HaaS云端积木平台**

&emsp;&emsp;
进入阿里云官网，用阿里云账号[登录HaaS云端组件控制台](https://haas.iot.aliyun.com/welcome) 。

<div align="center">
<img src=./../../../images/classifying_rubbish/AI_HaaS官网.png width=90%/>
</div>
<br>


2. **创建设备**

&emsp;&emsp;
在左侧导航栏设备管理中，可以看到产品和设备选项，点击“产品”，可以看到平台默认创建了“haas_正式游客产品”这个产品；
所以开发者只需要创建设备即可。点击“批量添加”， 添加方式为“自动生成”， 申请数量可填为“1”个，即可完成新设备的创建。

<div align="center">
<img src=./../../../images/classifying_rubbish/AI_创建设备.png width=60%/>
</div>
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_创建设备_设备信息.png width=60%/>
</div>
<br>

3. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”,就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_查看设备信息.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_设备三元组.png width=80%/>
</div>
<br>

&emsp;&emsp;
请复制该处的三元组信息，后面设备端开发需要使用。

## 设备端开发

1. **开发环境**

&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

2. **创建解决方案**

&emsp;&emsp;
如下图所示，打开VSCode之后在新建一个基于helloworld的python工程，设定好工程名称（“smart_trashbin”）及工作区路径之后，硬件类型选择M5Stack Core2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_创建工程.png width=80%/>
</div>


1. **代码准备**

&emsp;&emsp;
将[智能垃圾桶代码](./code/)目录下的所有脚本进行复制到“smart_trashbin”工程根目录中，然后进行如下设定完成设备端代码的开发。

> Python脚本的详细说明请参考脚本内嵌的文字注释


3.1. **修改路由器名称及密码**

&emsp;&emsp;
修改smart_trashbin工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

3.2. **修改设备的三元组信息**

&emsp;&emsp;
请参考“获取设备的三元组步骤”，把获取的三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

3.3. **运行结果**

&emsp;&emsp;
推送此脚本到M5Stack Core2之后，网络连接成功后，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
把要识别的垃圾放到大约距离红外传感器1-10cm处，当传感器上两个灯都亮起时，表明检测到物体开始垃圾分类识别，识别成功后在屏幕上显示红色XXXX Detected的字样，XXXX是四种垃圾分类的一种，样例log如下。在检测成功后，会根据识别的垃圾类型，打开对应类别的垃圾桶。

&emsp;&emsp;
推送此脚本到M5StackCore2之后，通过PC端串口调试工具可以看到设备端打印如下日志。日志的含义为:
* “link platform connected” 代表成功连接到云端平台。
* "upload--->"之后跟着的为设备端向云端发送的请求信息，其中"eventName"为"classifyingRubbish"，表示需要云端进行垃圾分类识别。
* "download <----"之后跟着的是收到云端回复的识别结果信息：
    - result：识别是否成功。（success：识别成功，failed：识别失败）
    - elements：数组元素说明：
      - category：垃圾类别。包括可回收垃圾、干垃圾、湿垃圾、有害垃圾。
      - categoryScore：识别出的垃圾类别的置信度。
      - rubbish：物品名称
      - RubbishScore：物品名称的置信度。
* detected: Recyclable 表示当前检测到的垃圾是可回收垃圾。
* open the Recyclable bin 表示将要打开可回收垃圾桶盖。

```log
I (71364) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.xxx.xxx
NTP start
NTP done
E (61287) ACTIVATION_REPORT: activation success

establish tcp connection with server(host='${Your-ProductKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
IR detector ...
启动红外检测线程
有物体进入
upload--->{'qos': 1, 'payload': '{"version": "1.0", "id": 1, "params": {"ext": "{\\"fileName\\": \\"test.jpg\\", \\"fileId\\": \\"xxxxx\\", \\"filePosition\\": \\"lp\\"}", "eventType": "haas.faas", "eventName": "classifyingRubbish", "argInt": 1}}', 'topic': '/sys/xxxxx/xxxxx/thing/event/hli_event/post'}
download <----{'msg_id': 2136609361, 'service_id': 'hli_async_service', 'params': '{"ext":"{\\"elements\\":[{\\"category\\":\\"Recyclable\\",\\"categoryScore\\":0.9868,\\"rubbish\\":\\"\u706f\u7ba1\\",\\"rubbishScore\\":0.9868}],\\"result\\":\\"success\\",\\"sensitive\\":false}","commandName":"classifyingRubbishReply","commandTyperecognize time : 2378
":"haas.faas","argInt":1}', 'code': 0, 'params_len': 245}
detect: Recyclable
detected: Recyclable
open the Recyclable bin
```

<div align="center">
<img src=./../../../images/smart_trashbin/test.jpg width=80%/>
</div>

&emsp;&emsp;
动手改造一下手边的小垃圾桶，通过舵机控制垃圾桶盖的开合，实现自动打开对应类别垃圾桶的效果。

<div align="center">
<img src=./../../../images/smart_trashbin/演示.gif width=80%/>
</div>
