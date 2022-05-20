# mqtt-消息队列遥测传输协议模块

* 类功能：  
MQTT（Message Queuing Telemetry Transport，消息队列遥测传输协议）是符合OASIS标准，基于发布/订阅范式的消息协议。它工作在 TCP/IP协议族上，是为轻量级IoT应用场景设计的发布/订阅型消息协议，具有轻量级、高效、可靠、安全等优点，这些特点使它适用范围非常广泛。

## createClient - 创建MQTTClient 实例
* 函数功能：  
创建一个 MQTTClient 实例（客户端），创建时会连接 MQTT 服务端。

* 函数原型：
> var client = mqtt.createClient(Object options)

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| host | String | 是 | 要连接的MQTT 服务端的地址，可以是ip地址，也可以是域名 |
| port | Number | 否 | 要连接的MQTT 服务端的端口号，默认为1883 |
| clientId | String | 否 | 设置clientId，默认为系统随机生成 |
| username | String | 否 | 设置username |
| password | String | 否 | 设置password |
| keepalive_interval | Number | 否 | 设置MQTT心跳时间，单位秒，默认60 |
| success | Function | 否 | 调用成功的回调函数 |
| fail | Function | 否 | 调用失败的回调函数 |

* 返回值：
MQTTClient实例，为空代表创建失败。
**异步事件**：
   - 'connect'：当iot client 连接到iot平台时，触发connect事件。
   - 'disconnect'：当连接断开时，触发'disconnect'事件。
   - 'reconnect': 重连成功事件。
   - 'message'：收到云端message事件，message格式如下。

| 参数 | 类型 | 必选参数？|说明 |
| --- | --- | :---:|--- |
| topic | String |是 | 接收到数据的topic |
| message | ArrayBuffer | 是 |接收到的数据 |

   - 'error'：异常事件。
   - 'close'：链接关闭事件。

## subscribe - 向服务端订阅一个topic
* 函数功能：  
向服务端订阅一个topic。

* 函数原型：
> client.subscribe(Object options)

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | 要订阅的topic |
| qos | Number | 否 | QoS订阅等级，默认为0 |
| success | Function | 否 | 订阅成功时的回调函数 |
| fail | Function | 否 | 订阅失败时的回调函数 |

* 返回值：
整型，0表示成功，其他值表示失败。

## unsubscribe - 向服务端取消订阅一个topic
* 函数功能：  
向服务端取消订阅一个topic。

* 函数原型：
> client.unsubscribe(Object options)

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | 要取消订阅的topic |

* 返回值：
整型，0表示成功，其他值表示失败。

## publish - 向指定topic发布消息
* 函数功能：  
向指定topic发布消息。

* 函数原型：
> client.publish(Object options)

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | 要发布消息的topic |
| message | String 或ArrayBuffer | 是 | 要发送的数据 |
| qos | Number | 否 | QoS订阅等级，默认为0 |
| success | Function | 否 | 发布消息成功时的回调函数 |
| fail | Function | 否 | 发布消息失败时的回调函数 |

* 返回值：
整型，0表示成功，其他值表示失败。

## close - 关闭MQTT Client实例
* 函数功能：  
关闭MQTT client实例。

* 函数原型：
> client.close()

* 参数说明：  
无

* 返回值：
整型，0表示成功，其他值表示失败。

## 使用实例
* 测试test.mosquitto.org服务器<br>
-- [haaseduk1 wifi测试mqtt服务器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/mqtt)<br>
-- [ESP32 wifi测试mqtt服务器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/mqtt)<br>
-- [haase100以太网测试mqtt服务器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/mqtt)<br>
-- [haas600 cellular测试mqtt服务器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/mqtt)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|是||
|HaaS EDU K1|是||
|HaaS600-EC100Y|是||
|HaaS600-EC600S|是||
|HaaS510|是||
|HaaS531|是||
|ESP32|是||

<br>

