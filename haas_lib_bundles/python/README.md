# HaaS Python

## HaaS Python背景

&emsp;&emsp;
Python在PC端取得的巨大成功是众所周知的。IoT场景如何借力Python生态，如何通过Python这种简单、高效的脚本化语言赋能硬件资源受限的IoT设备研发？这两个问题被IoT行业的头部玩家持续关注，但始终都未能找到一个完美的解决方案。

&emsp;&emsp;
2014年MicroPython诞生，MicroPython是Python3编程语言的精简高效实现，包含了Python基础语法特性和部分最常用的Python库，可以满足IoT应用开发的基本需求。MicroPython最低内存占用只有16K，具备了广泛应用在硬件资源受限的IoT设备上的条件。基于MicroPython使用各种复杂的外设功能只需要少量的代码。正因如此，MicroPython诞生之后，意法半导体、TI、乐鑫等物联网芯片方案的头部厂商积极跟进，完成了芯片层的适配。自此 ，MicroPython硬件生态初具雏形。到现如今，越来越多的芯片原生支持MicroPython，随着树莓派、荔枝派、HaaS等国内外知名开发板宣布全面支持MicroPython，数以十万计的开发者开始学习和使用MicroPython。

&emsp;&emsp;
MicroPython开源项目中主要包含四部分内容：轻量级的Python解释器、开发应用必备的基础库、设备基础能力的适配接口标准以及MicroPython在各种芯片上的硬件适配实现，这四块内容基本可以覆盖大多数IoT设备的最基础开发的需要。但是，IoT场景的需求复杂多样，想要真正实现快速定制和零门槛开发，还需要基于MicroPython官方版本做更多的扩展。HaaS Python轻应用（简称Python轻应用）对MicroPython 做了大量易用性的改造和高级能力的补齐，是目前MicroPython相关开源工程中很受行业认可的长期维护项目。

&emsp;&emsp;
HaaS Python是阿里云IoT团队最新研发的一套低代码编程框架，兼容MicroPython编程规范，依托HaaS平台软硬件积木提供AI、支付、蓝牙配网、云连接、UI等物联网场景常用的能力，从而解决了物联网应用开发难的问题。有了Python轻应用框架，物联网编程不再局限于专业软件开发人员，一般的技术员也可以快速实现复杂的物联网需求。

## HaaS Python特点
1. 更好的解决数据上云问题

&emsp;&emsp;
物联网中最常用的场景是数据上云及远程设备控制，针对这点，Python轻应用提供了简单易用支持Python应用快速移植的硬件访问接口Driver库及包括Socket，HTTP，HTTPS，Websocket，MQTT及aliyunIoT（连接阿里云物联网平台的SDK）在内的多种网络功能。以下是基于Python轻应用框架实现远程控制开发板LED灯的例子。可以看出只需要数十行代码(以ESP32平台为例)就可以完成物联网设备数据上云及远程控制的功能。


 <details>
 <summary>数据上云示例代码</summary>

 ```python

 '''
ESP32开发板上board.json配置如下：
 {
  "name": "esp32",
  "version": "1.0.0",
  "io": {
    "led": {
      "type": "GPIO",
      "port": 2,
      "dir": "output",
      "pull": "pullup"
    }
  },
  "debugLevel": "ERROR",
  "repl": "disable"
}
 '''

# -*- coding: UTF-8 -*-

from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件
import network                   # Wi-Fi功能所在库
import ujson                     # json字串解析库
import utime                     # 延时API所在组件
from driver import GPIO          # GPIO类，用于控制微处理器的输入输出功能

gpioledDev = 0

# 物联网平台连接标志位
iot_connected = False
wlan = None
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网设备实例
device = None

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的路由器名称"
wifiPassword = "请输入您的路由器密码"

# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
   global wlan
   wifi_connected = False

   wlan.active(True)                  #激活界面
   wlan.scan()                        #扫描接入点
   #print("start to connect ", wifiSsid)
   wlan.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）

   while True:
       wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
       if wifi_connected:                     # Wi-Fi连接成功则退出while循环
           break
       else:
           utime.sleep(0.5)
           print("wifi_connected:", wifi_connected)

   ifconfig = wlan.ifconfig()                    #获取接口的IP/netmask/gw/DNS地址
   print(ifconfig)
   utime.sleep(0.5)

# 物联网平台连接成功的回调函数
def on_connect(data):
   global iot_connected
   iot_connected = True

# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):

   # {"ledSwitch":1} or {"ledSwitch":0}
   payload = ujson.loads(request['params'])

   # 获取dict状态字段 注意要验证键存在 否则会抛出异常
   if "ledSwitch" in payload.keys():
       print(payload)
       on = payload["ledSwitch"]

       if (on):
           led_control(1)
       else:
           led_control(0)

def connect_lk(productKey, deviceName, deviceSecret):
   global device, iot_connected
   key_info = {
       'region': 'cn-shanghai',
       'productKey': productKey,
       'deviceName': deviceName,
       'deviceSecret': deviceSecret,
       'keepaliveSec': 60
   }
   # 将三元组信息设置到iot组件中
   device = Device()

   # 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
   device.on(Device.ON_CONNECT, on_connect)

   # 配置收到云端属性控制指令的回调函数，如果收到物联网平台发送的属性控制消息，则调用on_props函数
   device.on(Device.ON_PROPS, on_props)

   # 启动连接阿里云物联网平台过程
   device.connect(key_info)

   # 等待设备成功连接到物联网平台
   while(True):
       if iot_connected:
           print('物联网平台连接成功')
           break
       else:
           print('sleep for 1 s')
           utime.sleep(1)
   print('sleep for 2s')
   utime.sleep(2)


def led_control(on):
   global gpioledDev
   if on == 0:
       gpioledDev.write(0)        # GPIO写入0，灭灯
   else:
       gpioledDev.write(1)        # GPIO写入1，亮灯

if __name__ == '__main__' :

   gpioledDev = GPIO()
   gpioledDev.open("led")

   wlan = network.WLAN(network.STA_IF)    #创建WLAN对象

   get_wifi_status()

   connect_lk(productKey, deviceName, deviceSecret)

 ```
 </details>

2. 丰富的垂直解决方案

&emsp;&emsp;
HaaS云端解决方案中心包含大量软硬一体应用案例，覆盖连云、控端、AI、UI等IoT设备常见应用。开发者可以基于这些案例快速定制物联网产品。

3. 背靠阿里生态，不止有端，还有云

&emsp;&emsp;
Python轻应用不仅仅是一个端上开发框架，它还背靠阿里生态，提供强大的云端能力。除了提供物联网常用的设备管理、远程控制、数据报表、异常预警等能力以外，还提供了支付宝支付能力、天猫精灵语音识别和语义理解能力、达摩院数视觉算法能力及高德地图能力等。

4. 易上手也易精通

&emsp;&emsp;
积木化编程接口+跨平台IDE，快速搭建您的物联网应用并一键热部署。精简掉了传统C语言编译、烧录固件等繁琐复杂的调试环节，使得Python轻应用容易上手也容易精通。

5. 完善的技术保障和支持体系

&emsp;&emsp;
HaaS开发者钉钉群提供7*24小时技术支持。HaaS供需平台，帮HaaS开发者实现商业链路闭环。每月一次Python轻应用开发者月会，开发者提出的需求和建议通过阿里内部任务系统跟进，并通过HaaS微发布向开发者反馈最新进展。Python轻应用的开发团队还提供了一套详细的编程文档，其中包括如何在不同的HaaS开发板上完成快速上手的文章、最新编程接口的详细介绍及参考用法（其中包括Python轻应用在物联网场景的垂直使用案例），基于这套文档及技术支持体系，开发者只需要一两天就可以学会Python轻应用。

## HaaS Python快速开始
如何快速开始你的HaaS Python之旅，请参考[链接](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/startup/startup)。

## 了解更多
&emsp;&emsp;
关于HaaS，如果您想了解更多，请访问[HaaS 官网首页](http://haas.iot.aliyun.com/)

&emsp;&emsp;
如果您对HaaS有任何疑问或建议，也欢迎进群提给我们。
<div align="center">
<img src=https://img.alicdn.com/imgextra/i3/O1CN01aYBmpk1kG2REDqZNM_!!6000000004655-2-tps-2240-880.png width=100%/>
</div>
