# IoTGateway-物联网Gateway功能模块

* 类功能：  
阿里云物联网平台支持多种设备接入类型，分别是“直连设备”，“网关设备”和“网关子设备”。本文主要介绍“网关设备”和“网关子设备”借助iot-gateway模块快速接入阿里云物联网平台的使用规范和示例。更多详情请参考《[阿里云物联网平台-网关与子设备](https://help.aliyun.com/document_detail/73734.html)》。

## gateway - 创建IoTGateway实例
* 函数功能：  
初始化物联网平台Gateway类，创建Gateway实例。iot实例默认开启TLS加密。

* 函数原型：
> var gateway = iot.gateway({
                                 productKey: productKey,
                                 deviceName: deviceName,
                                 deviceSecret: deviceSecret });

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 网关的prodctKey |
| deviceName | String | 是 | 网关的deviceName |
| deviceSecret | String | 是 | 网关的deviceSecret |
| region | String | 否 | 阿里云region，默认值：**cn-shanghai**。阿里云region详细说明见[阿里云物联网平台 - 地域和可用区](https://help.aliyun.com/document_detail/40654.html?spm=a2c4g.11174283.6.1017.62551668ZlYWAE&parentId=30520) |
| keepaliveSec | Number | 否 | 心跳包报文时间间隔，默认值60秒 |

* 返回值：
Object类型，IoTGateway实例，为空代表创建失败。IoTGateway实例的异步事件描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| 'connect' | Function | 是 | 当 iot gateway 连接到iot平台时，触发connect事件。 |
| 'disconnect' | Function | 是 | 当连接断开时，触发'disconnect'事件。 |
| 'reconnect' | Function | 是 | 重连成功事件。 |
| 'message' | Function | 是 | 收到云端message事件。 |

## subscribe - 订阅服务端topic
* 函数功能：  
订阅来自服务端的某个topic，当服务端推送该topic消息时，设备端会收到该topic消息通知。

* 函数原型：
> gateway.subscribe(Object options);

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
> gateway.unsubscribe(topic);

* 参数说明：  
String类型，topic的字符串。

* 返回值：
无

## publish - 上报topic消息
* 函数功能：  
上报topic消息。

* 函数原型：
> gateway.publish({ topic: topic, payload: 'xxx', qos: 1 });

* 参数说明：  
Object类型，上报topic消息的数据格式，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| topic | String | 是 | topic string类型。 |
| payload | String | 是 | user defined payload string. |
| qos | Number | 是 | qos值 |

* 返回值：
无

## addTopo - 添加子设备
* 函数功能：  
为gateway 添加子设备。

* 函数原型：
> gateway.addTopo(subdev);

* 参数说明：  
Object类型，其属性描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 物联网平台上注册的子设备的productKey |
| deviceName | String | 是 | 物联网平台上注册的子设备的deviceName |
| deviceSecret | String | 是 | 物联网平台上注册的子设备的deviceSecre |

* 返回值：
无

## getTopo - 获取网关拓扑
* 函数功能：  
获取云端存储的网关拓扑，云端返回的异步结果会在on('message', xx)消息中体现。

* 函数原型：
> gateway.getTopo(function cb(topo){});

* 参数说明：  
Function类型，其参数topo的属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| code | Number | 是 | code id |
| data | String | 是 | event payload data |
| message | String | 是 | event payload string |

* 返回值：
无

## removeTopo - 删除子设备
* 函数功能：  
从网关拓扑删除子设备。

* 函数原型：
> gateway.removeTopo({productKey: '', deviceName: '', deviceSecret: ''})

* 参数说明：  
Object类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 物联网平台上注册的productKey |
| deviceName | String | 是 | 物联网平台上注册的deviceName |
| deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |

* 返回值：
无

## registerSubDevice - 动态注册子设备
* 函数功能：  
动态注册子设备，免白方式；云端不需要提前创建该设备。

* 函数原型：
> gateway.registerSubDevice({productKey: '', deviceName: ''})

* 参数说明：  
Object类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 物联网平台上注册的productKey |
| deviceName | String | 是 | 物联网平台上注册的deviceName |

* 返回值：
无

## login - 登陆子设备
* 函数功能：  
子设备登录物联网云端平台。

* 函数原型：
> gateway.login({productKey: '', deviceName: '', deviceSecret: ''})

* 参数说明：  
Object类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 物联网平台上注册的productKey |
| deviceName | String | 是 | 物联网平台上注册的deviceName |
| deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |

* 返回值：
无

## logout - 登出子设备
* 函数功能：  
子设备登出物联网云端平台。

* 函数原型：
> gateway.logout({productKey: '', deviceName: '', deviceSecret: ''})

* 参数说明：  
Object类型，其属性有，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| productKey | String | 是 | 物联网平台上注册的productKey |
| deviceName | String | 是 | 物联网平台上注册的deviceName |
| deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |

* 返回值：
无

## getNtpTime - 获取NTP时间
* 函数功能：  
从云端获取NTP时间。

* 函数原型：
> gateway.getNtpTime(function(time) {})

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

## 使用实例
* 网关通过三元组上云示例<br>
-- [haaseeduk1 网关通过三元组上云示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/iot/iot-gateway/sanyuanzu)<br>

* 网关通过白名单动态注册上云示例<br> 
-- [haaseeduk1 网关通过白名单动态注册上云示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/iot/iot-gateway/wl-dynreg)<br>

注：HaaS100/ESP32等其他硬件的HTTP示例可完全复用以上haaseduk1示例<br>

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

