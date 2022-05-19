# 雷达避障系统

&emsp;&emsp;
下图是本案例除硬件连线外的 3 步导学，每个步骤中实现的功能请参考图中的说明。

<div align="center">
<img src=./../../../images/雷达避障系统/步骤概述.jpg width=35%/>
</div>

## 简介

&emsp;&emsp;
近年来，随着智能汽车、智能机器人的发展，雷达避障技术得到了较快的发展。避障雷达是防撞设备的辅助装置，可以根据需要对避障距离和避障范围进行调整。避障雷达通常安装在物体可移动方向的前部位置，用于探测物体移动方向前方障碍物横截面与自身的距离，由此来判断是否让物体减速或停止移动，从而避免发生碰撞。

### 背景知识

&emsp;&emsp;
本系统的核心在于使用超声波传感器测量与障碍物之间的距离，当距离小于阈值时，发出报警声，并上报物联网平台，提示采取相应措施，以避免发生碰撞。原理图如下所示：

<div align="center">
<img src=./../../../images/雷达避障系统/原理图.jpg width=35%/>
</div>

## 准备

&emsp;&emsp;
本案例需要使用如下硬件：
* ESP32开发板一个
* HC-SR04超声波测距传感器一个
* 蜂鸣器一个
* 杜邦线若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
board.json配置如下，其中buzzer为蜂鸣器的IO配置，trig和echo是HC-SR04超声波测距传感器的IO配置：
```json
{
  "name": "esp32",
  "version": "1.0.0",
  "io": {
    "buzzer": {
      "type": "GPIO",
      "port": 27,
      "dir": "output",
      "pull": "pullup"
    },
    "trig": {
      "type": "GPIO",
      "port": 25,
      "dir": "output",
      "pull": "pullup"
    },
    "echo": {
      "type": "GPIO",
      "port": 26,
      "dir": "input",
      "pull": "pullup"
    }
  },
  "debugLevel": "ERROR",
  "repl": "disable"
}
```

&emsp;&emsp;
对应硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/雷达避障系统/ESP32_连线.jpg width=80%/>
</div>
<br>

## 物联网平台开发

### 开通公共实例

&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东 2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的 3 个步骤完成云端设备的创建：

1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品

&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **`创建产品`** 按钮，如下图所示。

<div align="center">
<img src=./../../../images/雷达避障系统/雷达避障系统_物联网平台开发_空产品页.png
 width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA 标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。

<div align="center">
<img src=./../../../images/雷达避障系统/雷达避障系统_物联网平台开发_新建产品.png width=80%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“雷达避障系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/雷达避障系统/雷达避障系统_物联网平台开发_产品列表页.jpg width=80%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
在“产品”页面，点击刚才创建的“雷达避障系统”产品右边的查看按钮，然后点击 “功能定义” 菜单，开始定义产品功能。首先下载[物模型文件](./link_platform/model.zip)。
<div align="center">
<img src=./../../../images/雷达避障系统/功能定义.jpg width=80%>
</div>

&emsp;&emsp;
在“功能定义”页面点击“编辑草稿”，点击“快速导入”，点击“点击上传”。
<div align="center">
<img src=./../../../images/雷达避障系统/雷达避障系统_物联网平台开发_快速导入.png width=80%>
</div>

&emsp;&emsp;
点击“上传物模型”，选择刚刚下载的物模型文件model.zip进行上传。

<div align="center">
<img src=./../../../images/485环境监测/485环境监测-定义产品功能3.png width=30%>
</div>

&emsp;&emsp;
物模型成功以后，效果如下图，点击“发布上线”。

<div align="center">
<img src=./../../../images/雷达避障系统/雷达避障系统_物联网平台开发_导入完成.png width=80%>
</div>

<br>

### 为产品创建云端设备
&emsp;&emsp;
在产品列表页面中，点击”雷达避障系统“对应的“管理设备”按钮，进入设备管理页面。

<div align="center">
<img src=./../../../images/雷达避障系统/创建云端设备1.jpg
 width=80%/>
</div>

&emsp;&emsp;
点击“添加设备”按钮。

<div align="center">
<img src=./../../../images/雷达避障系统/创建云端设备2.jpg width=80%/>
</div>

&emsp;&emsp;
在弹框中填入DeviceName信息，然后点击“确认”、点击“完成”完成设备添加.
<div align="center">
<img src=./../../../images/雷达避障系统/创建云端设备3.jpg width=30%/>
</div>

&emsp;&emsp;
创建完云端设备以后，点击“设备管理” -> “设备” 菜单可以看到刚才创建的设备，点击设备对应的“查看”按钮进入设备详情页面。在详情页点击“查看” 按钮获取设备三元组。 设备三元组信息需要填写到设备端代码中。

<div align="center">
<img src=./../../../images/雷达避障系统/创建云端设备4.jpg width=80%/>
</div>
<br>

## 超声波测距开发
&emsp;&emsp;
HC-SR04超声波传感器使用声纳来确定物体的距离，其工作原理类似于蝙蝠。测量范围从2厘米到450厘米，HC-SR04超声波传感器上配有超声波发射器和接收器两个模块。

<br>

&emsp;&emsp;
超声波测距传感器是数字接口类型，其基本工作过程如下:
* 发射器（触发引脚-Trig），发送信号 一段高频声音<br>
* 当信号遇到一个物体时，它会被反射<br>
* 然后发射器（回声引脚-Echo）接收该信号<br>

<div align="center">
<img src=./../../../images/雷达避障系统/ext_超声波测距_定义尺寸.png  width=20%/>
</div>

<br>

&emsp;&emsp;
引脚定义
* VCC：DC-5V<br>
* Trig：触发(输入)<br>
* Echo：回声(输出)<br>
* GND：地

<br>

### HCSR04(trigObj,echoObj) - 创建超声波传感器对象
<br>

* 函数原型

> hcsr04Obj = HCSR04(trigObj,echoObj)

* 参数说明

|  参数   | 类型  | 必选参数？ | 说明                                                                  |
| :-----: | :---: | :--------: | :-------------------------------------------------------------------- |
| trigObj | GPIO  |     是     | GPIO配置输出，触发引脚，调用此函数前需确保trigObj对象已经处于open状态 |
| echoObj | GPIO  |     是     | GPIO配置输入，测量引脚，调用此函数前需确保echoObj对象已经处于open状态 |

* 返回值

> HCSR04对象成功，返回HCSR04对象；HCSR04对象创建失败，抛出Exception

* 示例代码
```python
import hcsr04
from driver import GPIO

print("Testing HCSR04 ...")
echoDev = GPIO()
echoDev.open("echo")

trigDev = GPIO()
trigDev.open("trig")

hcsr04Dev = hcsr04.HCSR04(trigDev,echoDev)
```

* 输出
```python
Testing HCSR04 ...
```
</br>

### measureDistance() - 获取超声波测量的距离
<br>

* 函数功能：

> 获取超声波测量的距离

* 函数原型：

> HCSR04.measureDistance()

* 参数说明：
> 无

* 返回值：
> 返回值为cm单位的距离值

* 示例：

```python
import hcsr04
from driver import GPIO
import utime

print("Testing HCSR04 ...")
echoDev = GPIO()
echoDev.open("echo")

trigDev = GPIO()
trigDev.open("trig")

hcsr04Dev = hcsr04.HCSR04(trigDev,echoDev)
while True:             # 无限循环
    distance = disDev.measureDistance()
    print(str(distance)+' CM')
    utime.sleep(1)      # 打印完之后休眠1秒
```

* 输出
```python
Testing HCSR04 ...
34 CM
492 CM
```

</br>

## 蜂鸣器报警开发
&emsp;&emsp;
蜂鸣器是一种一体化结构的电子讯响器，采用直流电压供电，广泛应用于计算机、打印机、复印机、报警器、电子玩具、汽车电子设备、电话机、定时器等电子产品中作发声器件。蜂鸣器主要分为压电式蜂鸣器和电磁式蜂鸣器两种类型。此有源蜂鸣器模块采用压电式，高电平触发原理，即当I/O口输入高电平时，蜂鸣器发声，具体外观接口如下图：

<div align="center">
<img src=./../../../images/雷达避障系统/ext_蜂鸣器_外观图.jpg  width=20%/>
</div>

<br>

### GPIO() - 创建蜂鸣器GPIO对象
<br>

* 函数原型

> GPIO.GPIO()

* 参数说明

> 无

* 返回值

> GPIO对象创建成功，返回GPIO对象；GPIO对象创建失败，抛出ENOMEN异常。

* 示例代码
```python
from driver import GPIO
buzzerdev = GPIO()
```

</br>

### open() - 打开蜂鸣器GPIO设备
<br>

* 函数功能

> 打开GPIO设备节点，并根据节点的配置信息初始化GPIO。

* 函数原型：

> GPIO.open(nodeName)

* 参数说明

| 属性 | 类型   | 必填 | 描述                                           |
| ---- | ------ | ---- | ---------------------------------------------- |
| node | String | 是   | 待操作的GPIO设备节点，定义在board.json文件中。 |

* 示例代码
```python
from driver import GPIO
buzzerdev = GPIO()
buzzerdev.open('buzzer')
print("buzzer inited!")
```

### write() - 打开/关闭蜂鸣器报警
</br>

* 函数原型：  
> GPIO.write(value)

* 参数说明

| 属性  | 类型 | 必填 | 描述                                 |
| ----- | ---- | ---- | ------------------------------------ |
| value | int  | 是   | GPIO输出电平，0为低电平，1为高电平。 |

* 返回值：  
成功：0；失败：故障码。

* 示例代码：

```python
from driver import GPIO
import utime

buzzerdev = GPIO()
buzzerdev.open('buzzer')

buzzerdev.write(1) # 打开蜂鸣器报警
utime.sleep(3)     # 休眠3秒
buzzerdev.write(0) # 关闭蜂鸣器报警
```

## 工程创建与推送
### 开发环境
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于hellworld的python工程，设定好工程名称（``obstacle_avoidance``）及工作区路径之后，硬件类型选择ESP32，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/雷达避障系统/创建工程.png width=40%/>
</div>

### 案例代码
&emsp;&emsp;
将[雷达避障系统](./code/)文件下所有脚本代码复制后，覆盖刚创建的``obstacle_avoidance``工程根目录下的文件。

1. **修改路由器名称及密码**

&emsp;&emsp;
修改obstacle_avoidance工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改obstacle_avoidance工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

```python
# 三元组信息
productKey = "请填写您的productKey"
deviceName = "请填写您的deviceName"
deviceSecret = "请填写您的deviceSecret"
```

3. **main.py关键代码说明**

&emsp;&emsp;
ALARM_DISTANCE为安全距离，当障碍物离超声波探头的距离小于安全距离时，会上传距离至物联网平台，同时蜂鸣器会发出报警声，ALARM_DISTANCE可由用户自行配置：
```python
def obstacle_detector():
    global disDev, echoDev, trigDev, buzzerdev

    while True:             # 无限循环
        distance = disDev.measureDistance()
        print('distance = ', distance)

        if(distance < ALARM_DISTANCE):
            buzzerdev.write(1)
            upload_data = {'params': ujson.dumps({
                'distance': distance,
            })
            }
            # 上传状态到物联网平台
            device.postProps(upload_data)
        else:
            buzzerdev.write(0)

        utime.sleep(1)      # 打印完之后休眠1秒

    echoDev.close()
    trigDev.close()

if __name__ == '__main__':
    wlan = network.WLAN(network.STA_IF)  # 创建WLAN对象
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)
    buzzer_init()
    obstacle_avoidance_init()
    obstacle_detector()
```

## 推送脚本
**推送步骤**

&emsp;&emsp;
推送此工程到 ESP32 设备上，推送步骤请参照[ESP32推送步骤](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1E1WgYn#/Python/docs/zh-CN/startup/ESP32_startup?id=%e6%8e%a8%e9%80%81%e8%84%9a%e6%9c%ac%e5%88%b0%e8%ae%be%e5%a4%87)执行。

**推送脚本时的日志**

&emsp;&emsp;
推送脚本过程中的日志输出如下所示。

```log
==== wait hardware reset!!! ====
==== hardware reset OK!!! ====
=== start upload... ===
=== wait python process interrupt ===
upload process...1%
upload process...10%
upload process...20%
xxxxxxx
upload process...90%
upload process...91%
upload process...92%
upload process...94%
upload process...94%
upload process...96%
upload process...98%
upload process...99%
upload process...100%
=== upload progress end ===
```

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到ESP32之后并运行，将遮挡物放在HC-SR04超声波发射接口前并逐步向发射头挪进，以模拟靠近障碍物的情况，当离障碍物的距离小于安全距离（安全距离可通过main.py中的ALARM_DISTANCE进行设置）时，会上报距离至物联网平台，同时蜂鸣器会发出警报声。串口会周期性的打印如下日志：

```log
mount fs
 ==== python execute bootpy ====
enable OneMinuteOnCloud
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
buzzer inited!
distance =  4.845
distance =  5.151
distance =  5.151
distance =  5.134
distance =  5.151
distance =  5.032
distance =  5.151
distance =  5.1
distance =  5.151
distance =  5.032
distance =  5.151
distance =  5.151
distance =  5.151
distance =  4.151
distance =  4.151
distance =  5.151
distance =  4.743
distance =  5.151
distance =  5.151
distance =  5.032
distance =  5.151
distance =  5.151
```
当距离（distance =  xxxx）小于用户定义的安全距离时，蜂鸣器就会发出报警声。

<br>

## 云端查看

&emsp;&emsp;
进入阿里云官网，用阿里云账号[登录物联网平台](https://iot.console.aliyun.com/devices/)查看设备物模型数据。

<div align="center">
<img src=./../../../images/雷达避障系统/距离识别结果记录.jpg width=80%/>
</div>

<br>

&emsp;&emsp;
到此为止，雷达避障检测系统案例就已经完成了。如果想学习更多实验，请参考[HaaS 案例实践详解](https://gitee.com/haasedu/haasedu/tree/release_2.0)。
