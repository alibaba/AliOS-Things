# IoTDevice-物联网Device功能模块

* 类功能：  
阿里云物联网平台支持多种设备接入类型，分别是“直连设备”，“网关设备”和“网关子设备”。本文主要介绍“直连设备”（iot-device模块）快速接入阿里云物联网平台的使用规范和示例。更多详情请参考《[阿里云物联网平台-设备接入](https://help.aliyun.com/document_detail/145493.html)》文档。

## device - 创建IoTDevice实例
* 函数功能：  
创建IoTDevice实例，同时会开始尝试连接阿里云IoT平台。iot实例默认开启TLS加密。

* 函数原型：
> var iotdev = iot.device({ productKey: productKey, deviceName: deviceName, deviceSecret: deviceSecret });

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 设备的prodctKey |
| deviceName | String | 是 | 设备的deviceName |
| deviceSecret | String | 是 | 设备的deviceSecret |
| region | String | 否 | 阿里云region，默认值：**cn-shanghai**。阿里云region详细说明见[阿里云物联网平台 - 地域和可用区](https://help.aliyun.com/document_detail/40654.html?spm=a2c4g.11174283.6.1017.62551668ZlYWAE&parentId=30520) |
| keepaliveSec | Number | 否 | 心跳包报文时间间隔，默认值60秒 |

* 返回值：
Object类型，IoTDevice实例，为空代表创建失败。IoTDevice实例的异步事件描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| 'connect' | Function | 是 | 当 iot client 连接到iot平台时，触发connect事件。 |
| 'disconnect' | Function | 是 | 当连接断开时，触发'disconnect'事件。 |
| 'reconnect' | Function | 是 | 重连成功事件。 |
| 'message' | Function | 是 | 收到云端message事件。 |

## subscribe - 订阅服务端topic
* 函数功能：  
订阅来自服务端的某个topic，当服务端推送该topic消息时，设备端会收到该topic消息通知。

* 函数原型：
> iotdev.subscribe(Object options);

* 参数说明：
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | topic string, json类型。 |
| qos | Number | 是 | qos值 |

* 返回值：
无

## unsubscribe - 取消订阅服务端topic
* 函数功能：  
取消订阅来自服务端的某个topic，当服务端推送该topic消息时，设备端不会收到该topic消息通知。

* 函数原型：
> iotdev.unsubscribe(topic);

* 参数说明：  
String类型，topic的字符串。

* 返回值：
无

## publish - 上报topic消息
* 函数功能：  
上报topic消息。

* 函数原型：
> iotdev.publish({ topic: topic, payload: 'xxx', qos: 1 });

* 参数说明：  
Object类型，上报topic消息的数据格式，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | topic string类型。 |
| payload | String | 是 | user defined payload string. |
| qos | Number | 是 | qos值 |

* 返回值：
无

## onProps - 监听云端属性下发
* 函数功能：  
监听云端属性下发。

* 函数原型：
>  iotdev.onProps(function(properity) {})

* 参数说明：  
Function类型，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | Function | 是 | 收到云端属性下发的回调函数。 |

其中回调函数function的参数properity类型描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| msg_id | Number | message id |
| params_len | Number | parameter length |
| params | String | parameter |

* 返回值：
无

## postEvent - 上报设备属性
* 函数功能：  
上报设备属性。

* 函数原型：
> iotdev.postEvent({ id: 'ErrorMessage', params: jsonstring });

* 参数说明：  
Object类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | Number | 是 | event id |
| params | String | 是 | event payload string |

* 返回值：
无

## onService - 监听云端设置服务事件
* 函数功能：  
监听云端设置服务事件。

* 函数原型：
> iotdev.onService(function(service) {})

* 参数说明：  
Function类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | Function | 是 | 收到云端设置服务事件的回调函数。 |

其中回调函数function的参数service类型描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| msg_id | Number | 是 | message id |
| service_id | Number | 是 | service id |
| params_len | Number | 是 | parameter length |
| params | String | 是 | parameter |

* 返回值：
无

## getNtpTime - 获取NTP时间
* 函数功能：  
从云端获取NTP时间。

* 函数原型：
> iotdev.getNtpTime(function(time) {})

* 参数说明：  
Function类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | Function | 是 | 收到云端NTP时间的回调函数。 |

其中回调函数function的参数time类型描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| hour | Number | 是 | 小时 |
| minute | Number | 是 | 分钟 |
| second | Number | 是 | 秒 |
| msecond | Number | 是 | 微秒 |

* 返回值：
无

## end - 关闭IoTDevice实例
* 函数功能：  
关闭IoTDevice实例，释放资源。

* 函数原型：
> iotdev.end(function() {})

* 参数说明：  
Function类型，关闭IoTDevice实例的回调函数。 |

* 返回值：
无

## 使用实例
* 通过三元组上云示例<br>
-- [haaseeduk1 通过三元组上云示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/iot/iot-device/sanyuanzu)<br>
-- [haas600 通过三元组上云示例](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600-EC600S/advance/iot-sensor)<br>
-- [ESP32 通过三元组上云示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/iot/iot-device/sanyuanzu)<br>

* 通过白名单动态注册上云示例<br>
-- [haaseeduk1 通过白名单动态注册上云示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/iot/iot-device/wl-dynreg)<br>

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

