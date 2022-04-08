# 声控灯
&emsp;&emsp;
声控灯是通过声音控制开灯的技术，适用于使用声音控制开灯的场景，特别是楼宇、住宅楼的公共场合，例如楼梯间、走廊、大厅等场所，它可减少人工开关电灯的麻烦，解放您的双手，可避免您的双手接触公共开关，降低接触细菌的可能，同时把灯的状态上报至云端，灯的管理者可实时查看灯的状态信息。

<div align="center">
<img src=./../../../images/sound_led/声控灯_步骤简介.png width=90%/>
</div>

## 简介
&emsp;&emsp;
本案例介绍的声控灯是通过声音控制开灯一个解决方案，具备使用简单、开发成本低、节能环保等特点。本案例为了更好玩使用了RGB LED，每次开灯颜色都会变化，并把灯开关的状态上报至云端，云端可以实时查看灯的状态。

<br>

### 背景知识

&emsp;&emsp;
声音传感器是充当收音的作用，用来接收声波，使话筒内的驻极体薄膜震动导致电容的变化，产生0-5V的微小电压，经过A/D转换被数据采集器接收。
<br>
&emsp;&emsp;
RGB LED是红绿蓝三色混光而成，在光的混色上，具备更多元的特性。
<br>

## 准备
1. ESP32开发板一套<br>
2. 声音传感器一个<br>
3. RGB LED一个<br>
4. 母对母杜邦连接线若干<br>
   
涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！
<br>


| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| ESP32开发板 | 1套 | [ESP32开发板一套](https://haas.iot.aliyun.com/solution/detail/hardware?spm=a2cpu.b17028610.0.0.6c2360b1yDodxD&versionId=800C9562896F994200000001&dataId=800C9562896F9942)|
| RGB LED | 1个 | [RGB LED](https://detail.tmall.com/item.htm?spm=a1z10.5-b-s.w4011-16538328951.139.53cd410bJVsY2E&id=548418198148&rn=27fcce6fba3a11a94b9607b415ef397e&abbucket=7) |
| 声音传感器 | 1个 | [声音传感器一个](https://detail.tmall.com/item.htm?spm=a1z10.5-b-s.w4011-16538328951.434.53cd410bJVsY2E&id=520364372142&rn=27fcce6fba3a11a94b9607b415ef397e&abbucket=7) |
| 母对母杜邦线 | 7根 | [母对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh+K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440213) 请选用10cm长即可 |

### 硬件连线图

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/sound_led/声控灯_硬件连线图.png width=90%/>
</div>
<br>

## 物联网平台开发

### 开通公共实例

&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_物联网平台.png width=90%/>
</div>

开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>


### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_新建产品.png
 width=90%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。如下图所示
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_完成创建产品.png width=90%/>
</div>

### 创建产品属性（物模型）

&emsp;&emsp;
点击前往定义物模型后，调整至下面页面，如下图所示。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_尚未添加任何功能.png width=90%/>
</div>

&emsp;&emsp;
点击编辑草稿，导入物模型文件[点击](https://iotx-haas-linkin.oss-cn-shanghai.aliyuncs.com/o/config/solution/soundLed/model.zip)下载物模型文件，直接导入后选择发布上线，如下图所示。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_快速导入物模型.png width=90%/>
</div>
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_导入完成.png width=90%/>
</div>
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_发布物模型.png width=90%/>
</div>
<br>

### 创建云端设备（获取三元组）

&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台_设备入口.png width=90%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，如下图所示。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_添加设备.png width=90%/>
</div>
设备添加完成后，如下图所示。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_设备列表.png width=90%/>
</div>
<br>

##### **获取设备三元组**

&emsp;&emsp;
如下图所示，在设备列表中点击“查看”按钮，就可以看到设备的三元组信息。

&emsp;&emsp;
设备三元组是指产品Key(productKey)、设备名称(DeviceName)、设备秘钥(DeviceSecret)。

&emsp;&emsp;
三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。设备三元组在后面设备端开发时会用到，请注意保存。
<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台开发_设备详情.png width=90%/>
</div>
<br>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“sound_led”）及工作区路径之后，硬件类型选择nodemcu32s，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/sound_led/声控灯_创建工程.png width=80%/>
</div>

&emsp;&emsp;
将[声控灯代码](./code/)文件中的所有脚本进行复制到“sound_led”工程根目录中，然后进行如下设定完成设备端代码的开发。

<div align="center">
<img src=./../../../images/sound_led/声控灯_代码介绍.png width=90%/>
</div>

> Python脚本的详细说明请参考脚本内嵌的文字注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改sound_led工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
把“获取设备的三元组”章节中获取的三元组信息填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

3. **修改LED灯颜色**

&emsp;&emsp;
在sound_led工程里main.py，set_color_reference函数里，修改gLedColor值即可达到修改灯颜色目标。

```python
# RGD灯颜色设置
gLedColor = 0xff0000
```

4. **调整开灯的声音阈值**

&emsp;&emsp;
在sound_led工程里main.py，sound_thread()函数里，修改gDecibelValue值即可达到调整开灯灵敏度，代码里的值是根据工作环境定义的经验值，开发者可根据自身情况定制修改。

5. **调整开灯的时长**

&emsp;&emsp;
在sound_led工程里main.py，sound_thread()函数里，修改led_open()函数下面的utime.sleep_ms(6000)来控制每次亮灯时间(1000代表1秒钟)。

```python
# 声音采集线程
# 声音采集线程
def sound_thread():
    global gOpenLed
    print('启动声音采集线程')
    decibelValue = 0
    while True:
        # 采集声音
        decibelValue = get_decibel()
        # 判断分贝
        if decibelValue > gDecibelValue and gOpenLed == False:
            print("声波转换后的值%s"% (decibelValue))
            gOpenLed = True
            # 开灯
            led_open(set_color_reference())
            # 亮灯6秒
            utime.sleep_ms(6000)
            # 关灯
            led_close()
            gOpenLed = False
        # 监听获取声音的频率，间隔不能太长，延迟大会导致不精确
        utime.sleep_ms(10)
```


## 运行程序

&emsp;&emsp;
推送此脚本到设备之后，串口会周期性的打印日志，设备上通过串口打印的信息如下图所示。
```
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.3.226
NTP start
NTP done
establish tcp connection with server(host='a1HOXQ1AMTs.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
启动声音采集线程
声波转换后的值1540
开灯
属性上报成功
关灯
属性上报成功
```
<br>

## 云端查看
进入阿里云官网，用阿里云账号[登录物联网平台](https://iot.console.aliyun.com/devices/)查看状态

<div align="center">
<img src=./../../../images/sound_led/声控灯_物联网平台_灯关闭状态.png width=90%/>
</div>

<br>

到此为止，声控灯案例就已经完成了。如果想学习更多实验，请参考[HaaS案例实践详解](https://gitee.com/haasedu/haasedu/tree/release_2.0)。

## 后记

&emsp;&emsp;
开发者可以尝试把多个设备的灯状态一起上报，使用IoT Studio制作一个控制面板显示出来。
