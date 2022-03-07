# HaaS Python

## HaaS Python快速开始

&emsp;&emsp;
对于初学者，我们建议您从开发板快速开始入门。
* [ESP32快速开始](docs/startup/ESP32_startup.md)
* [HaaS100快速开始](docs/startup/HaaS100_startup.md)
* [HaaS EDU K1快速开始](docs/startup/HaaS_EDU_K1_startup.md)
* [HaaS506快速开始](docs/startup/HaaS506_startup.md)

&emsp;&emsp;
快速入门完成之后，建议您进入HaaS Python官网中[趣味案例专区](https://haas.iot.aliyun.com/solution)，趣味对应的markdown文档链接如下：
* ESP32开发板
  * [起夜灯](./docs/examples/human_detector/esp32/README.md)
  * [燃气检测系统](./docs/examples/gas_detector/esp32/README.md)
  * [甲醛检测系统](./docs/examples/hcho_detector/esp32/README.md)
  * [花卉养殖](./docs/examples/floriculture/esp32/README.md)
  * [火焰检测](./docs/examples/fire_detector/esp32/README.md)
  * [智慧路灯](./docs/examples/smart_public_lighting/esp32/README.md)
  * [车辆定位](./docs/examples/vehicle_location/esp32/README.md)

&emsp;&emsp;
如果您想更详细的了解如何从浅到深完成一个完整的物联网应用的开发，建议您进入我们的[学习中心](https://haas.iot.aliyun.com/learning)进行学习。


&emsp;&emsp;
如果您想看HaaS Python都提供哪些库及其API说明，请点击如下链接查看。
* MicroPython标准API

|库名称|库说明|超链接|
|:-:|:-|:-:|
|math|数学函数|[点我查看说明](docs/standard_api/math.md)|
|ubinascii|二进制/ASCII转换|[点我查看说明](docs/standard_api/ubinascii.md)|
|ucollections|容器|[点我查看说明](docs/standard_api/ucollections.md)|
|uhashlib|散列算法|[点我查看说明](docs/standard_api/uhashlib.md)|
|uio|输入/输出流|[点我查看说明](docs/standard_api/uio.md)|
|ujson|JSON编解码|[点我查看说明](docs/standard_api/json.md)|
|uos|基础系统操作模块|[点我查看说明](docs/standard_api/uos.md)|
|usocket|socket模块|[点我查看说明](docs/standard_api/usocket.md)|
|ustruct|打包和解压原始数据类型|[点我查看说明](docs/standard_api/ustruct.md)|
|sys|系统相关函数|[点我查看说明](docs/standard_api/sys.md)|
|utime|时间相关函数|[点我查看说明](docs/standard_api/utime.md)|
|uzlib|zlib解压缩|[点我查看说明](docs/standard_api/uzlib.md)|
|_thread|底层多线程|[点我查看说明](docs/standard_api/_thread.md)|
|bluetooth|蓝牙BLE功能|[点我查看说明](docs/standard_api/bluetooth.md)|

* HaaS Python扩展API

|库名称|库说明|超链接|
|:-|:-|:-:|
|aliyunIoT Device|物联网平台设备类功能|[点我查看说明](docs/haas_extended_api/aliyunIoT/device.md)|
|aliyunIoT Gateway|物联网平台网关类功能|[点我查看说明](docs/haas_extended_api/aliyunIoT/gateway.md)|
|kv|键值对存储系统|[点我查看说明](docs/haas_extended_api/kv.md)|
|OSS|阿里云存储服务|[点我查看说明](docs/haas_extended_api/oss.md)|
|audio| 音频录制和播放|[点我查看说明](docs/haas_extended_api/audio.md)|
|bleNetConfig|蓝牙配网功能|[点我查看说明](docs/haas_extended_api/bleNetConfig.md)|
|http|HTTP模块|[点我查看说明](docs/haas_extended_api/http.md)|
|modbus|Modbus协议|[点我查看说明](docs/haas_extended_api/modbus.md)|
|ulog|log分级打印模块|[点我查看说明](docs/haas_extended_api/ulog.md)|
|OTA|Python轻应用在线更新|[点我查看说明](docs/haas_extended_api/ota.md)|
|ucamera| 摄像头功能模块|[点我查看说明](docs/haas_extended_api/ucamera.md)|
|AI|人工智能功能模块|[点我查看说明](docs/haas_extended_api/AI.md)|

* HaaS Python硬件积木驱动库

|硬件积木名称|功能|Python驱动库|
|:-|:-|:-:|
|aht21b|温湿度传感器库|[库文件](./libraries/aht21b)|
|sht3x|温湿度传感器库|[库文件](./libraries/sht3x)|
|si7006|温湿度传感器库|[库文件](./libraries/si7006)|
|cht8305|温湿度传感器库|[库文件](./libraries/cht8305)|
|ir|人体感应传感器|[库文件](./libraries/ir)|
|dcmotor|直流电机|[库文件](./libraries/dcmotor)|
|fire|火焰传感器|[库文件](./libraries/fire)|
|ap3216c|光照传感器|[库文件](./libraries/ap3216c)|
|mq2|燃气/烟雾传感器|[库文件](./libraries/mq2)|
|photoresistor|光敏传感器|[库文件](./libraries/photoresistor)|
|hcho|甲醛传感器|[库文件](./libraries/hcho)|
|gnss|GPS传感器|[库文件](./libraries/gnss)|

更多关于硬件积木的介绍、API使用文档及对应的案例文档请参考[链接](https://gitee.com/haasedu/haasedu/blob/release_2.0/%E6%8B%93%E5%B1%95%E7%9F%A5%E8%AF%86/HaaS%E7%A1%AC%E4%BB%B6%E7%A7%AF%E6%9C%A8%E5%BA%93/%E4%BC%A0%E6%84%9F%E5%99%A8%E7%A1%AC%E4%BB%B6%E7%A7%AF%E6%9C%A8/README.md#/haasedu/haasedu/blob/master/%E6%8B%93%E5%B1%95%E7%9F%A5%E8%AF%86/HaaS%E7%A1%AC%E4%BB%B6%E7%A7%AF%E6%9C%A8%E5%BA%93/%E4%BC%A0%E6%84%9F%E5%99%A8%E7%A1%AC%E4%BB%B6%E7%A7%AF%E6%9C%A8/%E6%B8%A9%E6%B9%BF%E5%BA%A6%E4%BC%A0%E6%84%9F%E5%99%A8_SHT3X-DIS.md)。

## HaaS Python背景

&emsp;&emsp;
Python在PC端取得的巨大成功是众所周知的。IoT场景如何借力Python生态，如何通过Python这种简单、高效的脚本化语言赋能硬件资源受限的IoT设备研发？这两个问题被IoT行业的头部玩家持续关注，但始终都未能找到一个完美的解决方案。

&emsp;&emsp;
2014年MicroPython诞生，MicroPython是Python3编程语言的精简高效实现，包含了Python基础语法特性和部分最常用的Python库，可以满足IoT应用开发的基本需求。MicroPython最低内存占用只有16K，具备了广泛应用在硬件资源受限的IoT设备上的条件。基于MicroPython使用各种复杂的外设功能只需要少量的代码。正因如此，MicroPython诞生之后，意法半导体、TI、乐鑫等物联网芯片方案的头部厂商积极跟进，完成了芯片层的适配。自此 ，MicroPython硬件生态初具雏形。到现如今，越来越多的芯片原生支持MicroPython，随着树莓派、荔枝派、HaaS等国内外知名开发板宣布全面支持MicroPython，数以十万计的开发者开始学习和使用MicroPython。

&emsp;&emsp;
MicroPython开源项目中主要包含四部分内容：轻量级的Python解释器、开发应用必备的基础库、设备基础能力的适配接口标准以及MicroPython在各种芯片上的硬件适配实现，这四块内容基本可以覆盖大多数IoT设备的最基础开发的需要。但是，IoT场景的需求复杂多样，想要真正实现快速定制和零门槛开发，还需要基于MicroPython官方版本做更多的扩展。HaaS Python轻应用（简称Python轻应用）对MicroPython 做了大量易用性的改造和高级能力的补齐，是目前MicroPython相关开源工程中很受行业认可的长期维护项目。

&emsp;&emsp;
HaaS Python是阿里云IoT团队最新研发的一套低代码编程框架，兼容MicroPython编程规范，依托HaaS平台软硬件积木提供AI、支付、蓝牙配网、云连接、UI等物联网场景常用的能力，从而解决了物联网应用开发难的问题。有了Python轻应用框架，物联网编程不再局限于专业软件开发人员，一般的技术员也可以快速实现复杂的物联网需求。

### HaaS Python特点
1. 更好的解决数据上云问题

&emsp;&emsp;
物联网中最常用的场景是数据上云及远程设备控制，针对这点，Python轻应用提供了简单易用支持Python应用快速移植的硬件访问接口Driver库及包括Socket，HTTP，HTTPS，Websocket，MQTT及aliyunIoT（连接阿里云物联网平台的SDK）在内的多种网络功能。以下是基于Python轻应用框架实现远程控制开发板LED灯的例子。可以看出只需要数十行代码(以ESP32平台为例)就可以完成物联网设备数据上云及远程控制的功能。


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

2. 丰富的垂直解决方案

&emsp;&emsp;
HaaS云端解决方案中心包含大量软硬一体应用案例，覆盖连云、控端、AI、UI等IoT设备常见应用。开发者可以基于这些案例快速定制物联网产品。

1. 背靠阿里生态，不止有端，还有云

&emsp;&emsp;
Python轻应用不仅仅是一个端上开发框架，它还背靠阿里生态，提供强大的云端能力。除了提供物联网常用的设备管理、远程控制、数据报表、异常预警等能力以外，还提供了支付宝支付能力、天猫精灵语音识别和语义理解能力、达摩院数视觉算法能力及高德地图能力等。

4. 易上手也易精通

&emsp;&emsp;
积木化编程接口+跨平台IDE，快速搭建您的物联网应用并一键热部署。精简掉了传统C语言编译、烧录固件等繁琐复杂的调试环节，使得Python轻应用容易上手也容易精通。

5. 完善的技术保障和支持体系

&emsp;&emsp;
HaaS开发者钉钉群提供7*24小时技术支持。HaaS供需平台，帮HaaS开发者实现商业链路闭环。每月一次Python轻应用开发者月会，开发者提出的需求和建议通过阿里内部任务系统跟进，并通过HaaS微发布向开发者反馈最新进展。Python轻应用的开发团队还提供了一套详细的编程文档，其中包括如何在不同的HaaS开发板上完成快速上手的文章、最新编程接口的详细介绍及参考用法（其中包括Python轻应用在物联网场景的垂直使用案例），基于这套文档及技术支持体系，开发者只需要一两天就可以学会Python轻应用。


## 了解更多
&emsp;&emsp;
关于HaaS，如果您想了解更多，请访问[HaaS 官网首页](http://haas.iot.aliyun.com/)

&emsp;&emsp;
如果您对HaaS有任何疑问或建议，也欢迎进群提给我们。
<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/imgs/HaaS社区.png  width=100%/>
</div>
