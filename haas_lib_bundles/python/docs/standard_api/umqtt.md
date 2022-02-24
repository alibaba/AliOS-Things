# umqtt - MQTT客户端
* 模块功能:  
MQTT客户端功能 - 连线、断线、发布消息、订阅主题、KeepAlive等功能。

MQTT协议采用订阅者/发布者模式，协议中定义了消息服务质量（Quality of Service），它保证了在不同的网络环境下消息传递的可靠性。
MQTT协议设计了QoS0、QoS1和QoS2 3个QoS的级别。
* QoS0 - 最多传输一次，如果当时客户端不可用，则会丢失该消息，适用于不重要的消息传输
* QoS1 - 最少传输一次， 适用于对可靠性有要求，但对重复度没有要求的消息传输
* QoS2 - 只传输一次， 适用于有可靠性要求，也不允许发生重复的消息传输

关于MQTT协议详细的介绍，请参考[HaaSEDU项目-扩展知识-物联网通信协议](https://gitee.com/haasedu/haasedu/tree/release_2.0/%E6%8B%93%E5%B1%95%E7%9F%A5%E8%AF%86/%E7%89%A9%E8%81%94%E7%BD%91%E9%80%9A%E4%BF%A1%E5%8D%8F%E8%AE%AE#mqtt%E5%8D%8F%E8%AE%AE%E4%BB%8B%E7%BB%8D)。


## MQTTClient - 构建MQTT连接对象

* 函数原型：
> MQTTClient(client_id, server, port=0, user=None, password=None, keepalive=0, ssl=False, ssl_params={},reconn=True,version=4)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| client_id | string | Y | 客户端ID，客户端唯一标识 |
| server | string | Y | 服务器地址，IP地址或域名 |
| port | int | N | 服务器端口，默认是1883 |
| user | string | N | 登录服务器上所用用户名|
| password | string | N | 用户密码 |
| keepalive | int | N | MQTT客户端keepalive超时时间，单位：秒。 默认为0，范围：[60, 1200]|
| ssl | bool | N | 是否使能 SSL/TLS |
| ssl_params | string | N | SSL/TLS连线配置参数 |
| reconn | bool | N | 是否允许MQTT库自动重连，默认值：True，开启自动重连|
| version | int | N | 要使用的mqtt版本，version=3代表使用MQTTv3.1，默认version=4，使用MQTTv3.1.1 |

* 返回值：  
构建的MQTT客户端对象。

## set_callback(callback) - 设置MQTT消息回调函数

* 函数原型：
> MQTTClient.set_callback(callback)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| callback | 函数指针 | 是 | 在收到服务端发送的MQTT消息时候由MQTT库回调用户设置的callback函数 |

callback函数说明
 * 函数原型：
> function(topic, message)

 * 函数参数： 

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | string | 是 | 该消息所属的Topic |
| message | string | 是 | MQTT Broker发布的消息内容 |

* 返回值：   
无

## error_register_cb - 注册异常回调函数

* 函数原型：
> MQTTClient.error_register_cb(callback)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| callback | 函数指针 | 是 | umqtt内部发生异常时通过回调函数通知应用程序错误信息，如果在`MQTTClient中开启了自动重连标志，则此回调机制不会生效。` |


* 返回值：  
无

## set_last_will - 设置要发送给服务器的遗嘱

* 函数原型：
> MQTTClient.set_last_will(topic, message, retain=False, qos=0)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|topic|string|是|遗嘱消息的目标主题|
|message|string|是|遗嘱消息的内容|
|retain|bool|否|Broker是否一直保留该消息，默认False|
|qos|int|否|消息的QoS类型，有效类型：0或1，默认为0|

* 返回值：  
无

## connect - 和服务器连接

* 函数原型：
> MQTTClient.connect(clean_session=True)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|clean_session|bool|否|客户端类型标志，默认为False <br>  如果为True，MQTT Broker会在客户端断开连接时删除该客户端相关的所有信息。 <br> 如果为False，客户端断开连接时，订阅信息和排队消息都会被MQTT Broker保留。|

* 返回值：  
成功返回0，失败则抛出异常

## ping - 发送ping包

* 函数原型：
> MQTTClient.ping()

* 返回值：  
无

> 当keepalive不为0且在一定时间内核MQTT Broker没有消息交互，MQTT会主动向服务器发送ping包，检测和MQTT Broker的连连线状态。


## publish - 发布消息

* 函数原型：
> MQTTClient.publish(topic, msg, retain=False, qos=0)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|topic|string|是|遗嘱消息的目标主题|
|message|string|是|遗嘱消息的内容|
|retain|bool|否|Broker是否一直保留该消息，默认False <br> MQTT服务器只会为每一个Topic保存MQTT Client发送的最后一条retain为True的消息|
|qos|int|否|消息的QoS类型，有效类型：0或1，默认为0|

* 返回值：  
无

## subscribe - 订阅MQTT主题
* 函数原型：
> MQTTClient.subscribe(topic, qos)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|topic|string|是|遗嘱消息的目标主题|
|qos|int|否|消息的QoS类型，有效类型：0或1，默认为0|

* 返回值：    
无

## get_mqttsta - 获取MQTT的连接状态

* 函数原型：
> MQTTClient.get_mqttsta()

* 参数说明：  
无

* 返回值

0：连接成功<br>
1：连接中<br>
2：服务端连接关闭<br>
-1：连接异常

> 调用disconnect之后，再调用get_mqttsta返回-1

## wait_msg - 等待服务器的消息
* 函数原型：
> MQTTClient.wait_msg()

* 参数说明：  
无

* 返回值：  
无

> 服务器向已注册主题发送的消息将会被发送到通过set_callback设置的回调函数；其它类型的消息则在MQTT模块内部处理。

## check_msg - 检查服务器是否有待处理消息
* 函数原型：
> MQTTClient.check_msg()

* 参数说明：  
无

* 返回值：  
无

> check_msg和wait_msg的差别在于，check_msg是非阻塞模式，调用之后立即返回；而wait_msg为阻塞模式，直到有消息才返回。

## disconnect - 和服务器断开连接

* 函数原型：
> MQTTClient.disconnect()

* 参数说明：  
无

* 返回值
无

## close - 关闭MQTT连接

* 函数原型：
> MQTTClient.close()

* 参数说明：  
无

* 返回值
无

> close只释放socket资源，disconnect则会释放包含线程在内的MQTTClient所占用的资源

* 示例：

```python

# ESP32平台umqtt案例

import utime
from umqtt.simple import MQTTClient    # MQTT库
import network                         # Wi-Fi功能所在库

# Wi-Fi SSID和Password设置

wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"


wlan = None                            # 网络连接对象

client = None                          # MQTT客户端对象

# 等待Wi-Fi成功连接到路由器
def wait_for_wifi_connection():
    global wlan
    wifi_connected = False

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    wifi_connected = wlan.isconnected()    # 获取Wi-Fi连接路由器的状态信息
    if not wifi_connected: 
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

# 处理来自MQTT Broker的MQTT消息
def sub_cb(topic, msg):
    global client
    print("From Topic:", topic, " Message:", msg)           # 打印来自MQTT Broker的消息
    if topic == b'HaaSRegister':                            # 如果来自名为“HaaSRegister”的Topic，则发布消息给名为“HaaSUser”的Topic
        client.publish("HaaSUser", "Nice to meet you, HaaS Python User")

    if topic == b'HaaSUser':                                # 如果来自名为“HaaSUser”的Topic，则发布消息给名为“HaaSRegister”的Topic
        client.publish("HaaSRegister", "Hi, HaaS Python, I am coming")

if __name__ == '__main__':

    wait_for_wifi_connection()                                    # 等待Wi-Fi连接成功

    client = MQTTClient("HaaSPython", "broker-cn.emqx.io", 1883)  # 创建MQTT对象

    client.set_callback(sub_cb)                                   # 设置MQTT callback
    client.connect()                                              # 连接MQTT Broker

    client.subscribe("HaaSRegister")                              # 订阅来自"HaaSRegister"的消息
    client.subscribe("HaaSUser")                                  # 订阅来自"HaaSUser"的消息

    client.publish("HaaSRegister", "Hi, HaaS Python")             # 向"HaaSRegister"发送消息

    while True:
        client.check_msg()                                        # 检查是否有来自MQTT Broker的消息
        utime.sleep(1)

```

* 示例输出：

```python
...
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.0.118', '255.255.255.0', '192.168.0.1', '192.168.0.1')
Topic: b'HaaSRegister'  Message: b'Hi, HaaS Python!'
Topic: b'HaaSUser'  Message: b'Nice to meet you, HaaS Python User!'
Topic: b'HaaSRegister'  Message: b'Hi, HaaS Python, I am coming!'
Topic: b'HaaSUser'  Message: b'Nice to meet you, HaaS Python User!'
...
```