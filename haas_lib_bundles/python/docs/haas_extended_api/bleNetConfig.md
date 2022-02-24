# bleNetConfig - 蓝牙配网
* 模块功能：  
通过蓝牙传输WiFi信息，配置设备连接到无线AP。

* 注意事项：
  * 该功能开启后会影响原有的蓝牙功能
  * 该功能目前未实现数据加密，WiFi配网信息均以明文传输

* 使用指南：  
该功能可以使用“HaaS小程序“的配网工具进行配网，开发者也可以在其他平台自行实现相关的功能，数据交互满足通信格式即可。
![图片](https://img.alicdn.com/imgextra/i2/O1CN01M2kfiM1T1yn8rAQgS_!!6000000002323-0-tps-918-1481.jpg)

* 通信格式：  
设备接收到的配网指令为以下格式的字符串

```
'{
"cmd": "WiFiCon",
    "param": {
    "ssid": "xxxxxxxx",
    "pswd": "xxxxxxxx",
    "timeout": xxxxxx
    }
}'
```

其中，"param"内容可以视为入参，其格式如下：

| 参数    | 类型   | 必选参数？ | 说明                                   |
| ------- | ------ | :--------: | -------------------------------------- |
| ssid    | string |     是     | WiFi名，可以是路由器，也可以是手机热点 |
| pswd    | string |    密码    |
| timeout | string |     否     | 默认5秒，超时未连接会返回失败          |


设备在接收到数据后，会将其解析成Json格式，并尝试连接对应的AP。
设备连接成功后，根据连接状态的不同，会主动回复消息：

```
// 连接成功  返回 ifconfig 参数
'{
"cmd": "WiFiCon",
    "ret": {
    "state": "STAT_GOT_IP",
    "ifconfig": ["192.168.3.232", "255.255.255.0", "192.168.3.1", "192.168.3.1"]
    }
}'

// 连接失败  未找到指定AP
'{
"cmd": "WiFiCon",
    "ret": {
    "state": "STAT_NO_AP_FOUND",
    }
}'

// 连接失败  连接超时
'{
"cmd": "WiFiCon",
    "ret": {
    "state": "STAT_CONNECT_TIMEOUT",
    }
}'
```

## start - 启动配网

* 函数功能：  
启用配网功能，并作为服务一直运行在后台。

* 注意事项：
  * 此方法会影响原有的蓝牙功能。
  * 当此函数发起后，会进行蓝牙广播，设备可以被发现为"esp-node"。

* 函数原型：
> bleNetConfig.start()

* 返回值：  
成功返回0，失败时会抛出异常。

* 示例： 

```python
import bleNetConfig
bleNetConfig.start()
```

## getBleStatus - 获取蓝牙配网的状态信息

* 函数功能：  
该方法返回蓝牙配网状态。

* 函数原型：
> bleNetConfig.getBleStatus()

* 注意事项：
  * 该方法只关心配网过程中蓝牙交互的状态。
  * WLAN有关的状态，开发者可以通过调用 bleNetConfig.getWiFiStatus() 方法获取。

* 返回值：  
根据配网状态的不同，分别返回以下状态：

| 常量定义                       | 常量值 | 常量说明                   |
| ------------------------------ | ------ | -------------------------- |
| bleNetConfig.BLE_CONNECTED     | 0      | 设备蓝牙已连接             |
| bleNetConfig.BLE_DISCONNECTED  | 1      | 设备蓝牙已断开             |
| bleNetConfig.BLE_COMMINICATING | 2      | 设备已正在进行蓝牙数据交互 |

* 示例： 

```python
import bleNetConfig

try:
    bleNetConfig.start()
except Exception as e:
    raise e

print(bleNetConfig.getBleStatus())
```

## getWLAN - 获取用于连接的WLAN对象

* 函数功能：  
该方法返回蓝牙配网底层使用的 network.WLAN 对象。
该方法同 network.WLAN(network.STA_IF) 的返回相同，获取的是同一个全局对象。

* 函数原型：
> bleNetConfig.getWLAN()

* 返回值：  
network.WLAN 对象

* 示例： 

```python
import bleNetConfig

try:
    bleNetConfig.start()
except Exception as e:
    raise e

wlan = bleNetConfig.getWLAN()
print(wlan.status())
```

## getWiFiStatus - 获取蓝牙配网的状态信息

* 函数功能：  
该方法返回WiFi连接状态。

* 函数原型：
> bleNetConfig.getWiFiStatus()

* 返回值：  
根据网络连接状态的不同，分别返回以下状态：

| 常量定义                     | 常量值 | 常量说明               |
| ---------------------------- | ------ | ---------------------- |
| bleNetConfig.WIFI_IDLE       | 1000   | 无连接/无活动          |
| bleNetConfig.WIFI_CONNECTING | 1001   | WiFi连接中             |
| bleNetConfig.WIFI_GOT_IP     | 1010   | WiFi连接成功并获取到IP |

* 示例： 

```python
import bleNetConfig

try:
    bleNetConfig.start()
except Exception as e:
    raise e

print(bleNetConfig.getWiFiStatus())
```

## getWiFiConfig - 获取WiFi网络配置信息

* 函数功能：  
获取IP级网络接口参数：IP 地址、子网掩码、网关和 DNS服务器。

* 函数原型：
> bleNetConfig.getWiFiConfig()

* 返回值：  
此方法返回一个包含 IP 地址、子网掩码、网关和 DNS服务器 的 4 元组。

* 示例： 

```python
import bleNetConfig

if(bleNetConfig.getWiFiStatus() == bleNetConfig.WIFI_GOT_IP):
    print(bleNetConfig.getWiFiConfig())
```

* 输出：
```
('192.168.0.4', '255.255.255.0', '192.168.0.1', '8.8.8.8')
```

## stop - 停止配网

* 函数功能： 
启用配网功能。

* 注意事项：  
  * 此方法会关闭蓝牙服务。

* 函数原型：

> bleNetConfig.stop()

* 返回值：   
成功返回0，失败时会抛出异常。

* 示例：

```python
import bleNetConfig
bleNetConfig.stop()
```

## 使用实例
* 代码：

```python
import bleNetConfig
import time

print('Start BLE Net Config.')

bleNetConfig.start()

while(bleNetConfig.getWiFiStatus() != bleNetConfig.WIFI_GOT_IP):
    bleStatus = bleNetConfig.getBleStatus()
    if(bleStatus == bleNetConfig.BLE_CONNECTED):
        print('BLE_CONNECTED')
    elif(bleStatus == bleNetConfig.BLE_COMMINICATING):
        print('BLE_COMMINICATING')
    elif(bleStatus == bleNetConfig.BLE_DISCONNECTED):
        print('BLE_DISCONNECTED')

    time.sleep(1)
    # 在此时打开小程序进行连接

print('Wi-Fi connected.')

bleNetConfig.stop()
```

* 输出

```
Start BLE Net Config.
BLE_DISCONNECTED
BLE_DISCONNECTED
BLE_DISCONNECTED
BLE_CONNECTED
BLE_COMMINICATING
BLE_CONNECTED
Wi-Fi connected.
```