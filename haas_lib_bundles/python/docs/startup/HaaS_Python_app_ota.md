# HaaS Python在线更新

## 原理
HaaS在线更新即用户借助VS Code的HaaS Studio插件通过无线的方式（OTA）给开发板更新HaaS应用程序。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_overview.png width=100%/>
</div>

目前如下HaaS硬件可以支持本功能，可通过相应板子的快速开始文档查看并升级到最新版本。

| **开发板** | **固件版本** |
| --- | --- |
| [HaaS EDU K1](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5AB3B8A4A88800000001&dataId=800C5AB3B8A4A888) | 2.1.0及以上版本 |
| [HaaS 200](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C8F815768381600000001&dataId=800C8F8157683816) | 2.1.0及以上版本 |
| [NodeMCU-32S](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB) | 2.1.0及以上版本 |
| [ESP32乐鑫开发板](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C9562896F994200000001&dataId=800C9562896F9942) | 2.1.0及以上版本 |
| [pyWiFi-ESP32](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C55C67883087B00000001&dataId=800C55C67883087B) | 2.1.0及以上版本 |
| [M5Stack Core2](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C84FAF561DF6A00000001&dataId=800C84FAF561DF6A) | 2.1.0及以上版本 |

## 准备

- PC工具：Visual Studio Code和HaaS Studio插件V2.1.0以上版本。
- HaaS智能硬件：已烧录最新版HaaS固件（V2.1.0版本及以上）。
- 路由器：要求可正常访问互联网。

## 操作步骤
请按照如下步骤说明进行在线更新的操作。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_steps.png width=100%/>
</div>

### 创建工程
通过HaaS Studio基于helloworld创建名为“HaaS_OTA”工程。
### 设备联网
HaaS在线更新需要开发板接入互联网，不同的平台的接入方式请参考如下：
#### ESP32系列（NodeMCU32S、ESP32乐鑫、pyWiFi-ESP32、M5StackCore2）
复制如下代码到"HaaS_OTA"工程中的main.py中，并修改源码中的wifiSsid & wifiPassword为自己的路由器热点信息，再通过“本地更新”方式将示例代码推送到硬件中执行，等待设备联网成功（打印“Wi-Fi connected”）。
```python
import network                   # Wi-Fi功能所在库
import utime                     # 延时API所在组件

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的Wi-Fi名字"
wifiPassword = "请输入您的Wi-Fi密码"

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
if not sta_if.isconnected():
  print('connecting to network...')
  sta_if.connect(wifiSsid, wifiPassword)
  utime.sleep_ms(500)
  while not sta_if.isconnected():
    utime.sleep_ms(500)
print('Wi-Fi connected', sta_if.ifconfig())
```
#### HaaS EDU K1和HaaS200
复制如下代码到"HaaS_OTA"工程中的main.py中，并修改源码中的wifiSsid & wifiPassword为自己的路由器热点信息，再通过“本地更新”方式将示例代码推送到硬件中执行，等待设备联网成功（打印“Wi-Fi connected”）。
```python
# -*- coding: UTF-8 -*-
import netmgr as nm              # Wi-Fi功能所在库
import utime                     # 延时API所在组件                # ir人体红外传感器类

# Wi-Fi SSID和Password设置
wifiSsid = "请输入您的Wi-Fi名字"
wifiPassword = "请输入您的Wi-Fi密码"


# 等待Wi-Fi成功连接到路由器
def get_wifi_status():
    nm.init()
    wifi_connected = nm.getStatus()

    print("start to connect " + wifiSsid)
    if wifi_connected != 5:
        nm.connect(wifiSsid, wifiPassword)       # 连接到指定的路由器（路由器名称为wifiSsid, 密码为：wifiPassword）
    
    while True :
        if wifi_connected == 5:               # nm.getStatus()返回5代表连线成功
            break
        else:
            wifi_connected = nm.getStatus() # 获取Wi-Fi连接路由器的状态信息
            utime.sleep(0.5)
            print("wifi_connected: " + str(wifi_connected))

    print("Wi-Fi connected")
    print('DeviceIP:' + nm.getInfo()['ip'])  # 打印Wi-Fi的IP地址信息


if __name__ == '__main__' :
    get_wifi_status()
```

### 获取Token
当设备**联网**成功之后，HaaS硬件会向阿里云物联网平台请求设备Token，该Token是设备可正常使用在线更新能力的唯一凭证，请大家务必仔细保存好该Token，避免泄漏给无关人员。

您可以通过以下方式来获取设备Token，复制此Token备用。

- 查看日志：当设备激活后，我们可以通过串口日志读取Token信息。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_get_tocken.png width=100%/>
</div>

### 执行在线更新过程
请先切到当前工程到要推送的目标工程下。

#### 首次更新
首次对目标设备进行在线更新操作请参考如下示意图中步骤1～步骤5的操作。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_step1_2.png width=100%/>
</div>


#### 再次更新
之后如果对同一目标设备进行在线更新操作请参考如下示意图中步骤1～步骤2的操作。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_redo.png width=100%/>
</div>


### 查看结果
更新成功之后，设备会自动重启，从串口log中就可以看到新的应用已经生效了。

<div align="center">
<img src=https://hli.aliyuncs.com/haas-static/haasapi/Python/docs/zh-CN/images/haas_studio_app_ota_result.png width=100%/>
</div>







