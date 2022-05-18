# 车型识别系统
&emsp;&emsp;
车型识别是识别汽车图片（完整或部件图片）的类型，目前有小轿车、多用途汽车、SUV等类别。

&emsp;&emsp;
本案例导学步骤如下图所示。
<div align="center">
<img src=./../../../images/vehicle_type_recognization/车型识别_步骤.png width=80%/>
</div>

## 背景知识
&emsp;&emsp;
根据拍摄照片，快速识别图片中车辆的品牌型号，提供针对性的信息和服务，可用于相册管理、图片分类打标签、电子汽车说明书、一键拍照租车等场景。以及在监控高速路闸口、停车场出入口的进出车辆，识别详细车型信息，结合车牌、车辆属性对车辆身份进行校验，形成车辆画像。本案例采用M5Stack Core2开发板，结合摄像头实时拍照，将图片发送到HaaS云端进行识别，并返回识别的车型。

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
2. 开通HaaS增值服务
3. 设备端开发
4. 设备运行

## 准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    若干

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
<img src=./../../../images/COM_M5Stack硬件连线图.png width=60%/>
</div>
<br>

## 云端平台功能开通
&emsp;&emsp;
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用 **对象存储OSS** 和 **视觉智能开发平台**。案例整个步骤分为：
1. 注册阿里云账号并登录阿里云账号；
2. 在HaaS官网中的云端积木控制台创建设备；
3. 推送脚本到M5Stack Core2并运行车型识别；

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。如果您已经有阿里云账号，请忽略该步骤，直接进入下一步。

<br>

## 开通HaaS增值服务

1. **登录HaaS官网**

&emsp;&emsp;
进入[HaaS官网](https://haas.iot.aliyun.com/) 。
<div align="center">
<img src=./../../../images/AI_HaaS官网.png width=100%/>
</div>

<br>

2. **进入HaaS云端积木控制台**

&emsp;&emsp;
如**上图**所示，点击右上角的“控制台”，进入HaaS云端积木的控制台页面。

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
完成以上步骤表示开通了HaaS增值服务，您可以拿到生成的三元组进行设备端开发，连接云端进行识别。

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定工程名称（“vehicle_type_recognization”）及工作区路径之后，硬件类型选择M5Stack Core2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/vehicle_type_recognization/车型识别_HaaSStudio_创建工程.png width=100%/>
</div>


&emsp;&emsp;
将[车型识别代码](./code/)文件下的所有脚本进行复制到“vehicle_type_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。具体目录和要修改的参数设置如下图：

<div align="center">
<img src=./../../../images/vehicle_type_recognization/车型识别_HaaSStudio_工程目录及参数修改.png width=100%/>
</div>

> Python脚本的详细说明请参考脚本内嵌的文字注释
1. **修改路由器名称及密码**

&emsp;&emsp;
修改vehicle_type_recognization工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

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
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

## 运行
### 烧录代码

&emsp;&emsp;
烧录代码到设备上之后，在网络连接成功时，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。之后串口会周期性的打印如下日志，并且识别到车型后在屏幕上显示红色XXXX Detected的字样。具体log如下：

```log
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: xxx.xxx.xxx.xxx
NTP start
NTP done
establish tcp connection with server(host='a1mdXrPhrZM.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
{'ext': '{"items":[{"name":"car","score":0.0339},{"name":"MPV_mian","score":0.0152},{"name":"others","score":0.0209},{"name":"SUV","score":0.93}],"result":"success"}', 'commandName': 'recognizeVehicleReply', 'commandType': 'haas.faas', 'argInt': 1}
detect: SUV
get response time : 2157
Vehicle Detected!!!
Vehicle Detected!!!
Vehicle Detected!!!
get response time : 383
Vehicle Detected!!!
{'ext': '{"items":[{"name":"car","score":0.0199},{"name":"MPV_mian","score":0.0153},{"name":"others","score":0.0255},{"name":"SUV","score":0.9393}],"result":"success"}', 'commandName': 'recognizeVehicleReply', 'commandType': 'haas.faas', 'argInt': 1}
detect: SUV
Vehicle Detected!!!
Vehicle Detected!!!
get response time : 367
```

&emsp;&emsp;
设备显示识别中...
<div align="center">
<img src=./../../../images/vehicle_type_recognization/车型识别_设备识别中.png width=60%/>
</div>

&emsp;&emsp;
识别成功
<div align="center">
<img src=./../../../images/vehicle_type_recognization/车型识别_设备识别结果.png width=60%/>
</div>

&emsp;&emsp;
目前代码中设置的识别可信度超过85%认为识别成功，可以自行修改代码中的可信度参数，具体修改如下：

```log
def cb_lk_service(data):
    global g_lk_service, g_score, detected, name
    # dev.publish(compare_reply)
    # print(data)
    #resp = json.loads(data)
    if data != None:
        params = data['params']
        params_dict = json.loads(params)
        print(params_dict)
        ext = params_dict['ext']
        ext_dict = json.loads(ext)
        result = ext_dict['result']
        if result == 'success':
            i = 0
            item_list = ext_dict['items']
            detected = False
            while (i < len(item_list)) :
                g_score = item_list[i]['score']
                name = item_list[i]['name']
                # 这里可以修改识别的可信度，目前设置返回可信度大于85%才认为识别正确
                if g_score > 0.85 and name != 'others':
                    print('detect: ' + name)
                    detected = True
                    break
                i += 1
            if detected == False:
                print('do not detect!')
        else:
            print('do not detect!')
            detected = False
    g_lk_service = True
```
