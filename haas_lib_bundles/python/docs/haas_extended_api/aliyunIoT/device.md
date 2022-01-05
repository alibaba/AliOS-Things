# aliyunIoT 设备类接口


-  模块功能  
AliyunIoT模块可以帮助设备厂商将设备安全地接入到阿里云IoT物联网平台，继而让设备可以被阿里云IoT物联网平台进行管理。设备有三种形态，分别为直连设备（device），网关设备（gateway），网关子设备，其中网关子设备隶属于直连设备，唯一区别是没有联网能力，可被网关设备绑定为子设备。

阿里云物联网平台中涉及三个基本概念如下:
-  模块功能属性(prop)  
云端/设备端数据互通的通道，云端通过改变prop值，反馈到设备端，设备端修改prop值，保存到云端；
-  事件(event)  
设备端到云端的事件通知；
-  服务(service)  
云端到设备端的服务调用；

> 注意，需要确保Wi-FI连接成功

请使用下面的代码连接Wi-FI:
```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
if not sta_if.isconnected():
  print('connecting to network...')
  sta_if.connect('essid', 'passwd')
  utime.sleep_ms(500)
  while not sta_if.isconnected():
    utime.sleep_ms(500)
  print('connect succeed! ', sta_if.ifconfig())
else:
  print('network is connected.', sta_if.ifconfig())
```

直到sta_if.isconnected()返回 True 则表明网络连接成功。

## device() - 初始化

-  函数功能：  
device初始化，初始化物联网平台Device类，获取Device实例
-  注意事项:  
确保wifi连接成功
-  函数原型：
> device()

-  参数说明：  
无
-  返回值说明:  
成功：Device实例, 失败：None
-  函数示例  

```python
from aliyunIoT import Device

aliyunIoT_device  = Device()
```


## register - 子设备注册

-  函数功能:  
子设备动态注册
-  注意事项：  
确保wifi连接成功。
确保物联网平台产品、设备创建成功 。
- 函数原型:  
> register(data, cb)


-  参数说明:

    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 设备信息 |
    | cb | func | 是 | 注册回调函数 |

-  data参数说明:

    | 参数 | 类型 | 必选参数？ | 说明 |
    | ---- | ---- | ---- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 是 | 物联网平台上注册的productSecret |
    | deviceSecret | String | 否 | 物联网平台上注册的deviceSecre |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

-  返回值说明：  
成功返回0， 其他表示失败

-  使用示例:  

```python
from aliyunIoT import Device

productKey = "产品key"
deviceName = "设备名称"
productSecret = "产品密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'productSecret': deviceSecret ,
    'keepaliveSec': 60
}
def cb_lk_register(data):
  global g_register_flag
  g_register_flag = True

aliyunIoT_gateway = Device()
ret = aliyunIoT_device.connect(key_info)
ret = aliyunIoT_device.register(dev_register_info, cb_lk_register)
```
其中产品key、设备名称、产品密钥是在[物联网平台](https://iot.console.aliyun.com/product)上申请的三元组。

## connect - 连接物联网平台

-  函数功能：  
连接物联网平台

-  注意事项:  
确保Wi-FI连接成功。

-  函数原型：
> connect(data)

-  参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 设备模型 |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 否 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

-  返回值说明：  
成功返回0， 其他表示失败

-  函数示例  

```python
from aliyunIoT import Device

productKey = "产品key"
deviceName = "设备名称"
deviceSecret = "设备密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}
aliyunIoT_gateway = Device()
ret = aliyunIoT_device.connect(key_info)
```


## on  - 回调注册

-  函数功能：  
注册device的回调函数，在相应事件触发时开始时触发

-  注意事项:  
  - 确保wifi连接成功
  - 确保aliyunIoT连接成功

-  函数原型：
> device.on(event_type, cb)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | event | int | 是 | 注册的事件类型 |
    | cb | fuction | 是 | 监听事件的回调函数 |


- 事件类型：
  | 事件 | 说明 |
  | --- | --- |
  | ON_CONNECT | 连接成功 |
  | ON_DISCONNECT | 断开连接 |
  | ON_SERVICE | service信息 |
  | ON_PROPS | 收到服务端的props消息 |
  | ON_CLOSE | 对象关闭 |
  | ON_ERROR | 异常 |
  | ON_SUBCRIBE | sub成功 |



-  函数示例  

```python
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}
def on_lk_connect(data):
    print('on_lk_connect')

aliyunIoT_deivce = Device()

aliyunIoT_deivce.on(Device.ON_CONNECT, on_lk_connect)
ret = aliyunIoT_device.connect(key_info)
```


## getDeviceHandle - 获取句柄


-  函数功能：  
获得Device实例句柄

-  注意事项:  
确保Wi-FI连接成功。

-  函数原型：
> getDeviceHandle()

-  参数说明：  
无

-  返回值说明:  
成功：Device实例, 失败：None

-  函数示例  

```python
from aliyunIoT import Device
aliyunIoT_deivce = Device()

ret = aliyunIoT_deivce.getDeviceHandle()
```


## getNtpTime - NTP对时

-  函数功能：  
从物联网平台获得网络时间

-  注意事项:  
确保Wi-FI连接成功。

-  函数原型：
> getNtpTime(cb(data))

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | cb | func | 是 | 注册回调函数 |

-  返回值说明:
成功：Device实例, 失败：None
-  参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 上报模块版本号需要的信息 |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | year | String | 是 | 年 |
    | month | String | 是 | 月e |
    | day | String | 是 | 日 |
    | second | String | 否 | 秒 |
    | msecond | String | 否 | 毫秒 |
    | timestamp | int | 否 | 时间戳 |

-  函数示例  

```python
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

aliyunIoT_deivce = Device()
ret = aliyunIoT_device.connect(key_info)

def on_getntptime(data):
  print('%d-%d-%d %d:%d:%d:%d'%(data['year'],data['month'],\
          data['day'],data['hour'],\
          data['minute'],data['second'],data['msecond']))
  print('timestamp:', data['timestamp'])

ret = aliyunIoT_device.getNtpTime(on_getntptime)
```


## postProps - 上报属性

-  函数功能：  
上报设备属性

-  注意事项:  
确保Wi-FI连接成功。

-  函数原型：
> postProps(data)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | payload |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | params | String | 是 | payload |

-  返回值说明:  
成功：0, 失败：非0整数

-  函数示例  

```python
value =  {'test_prop' : 100}
data = {'params': json.dumps(value)}
ret = aliyunIoT_device.postProps(data)
```

## postEvent - 上报事件

-  函数功能：  
上报事件

-  注意事项:  
确保Wi-FI连接成功。

-  函数原型：
> postEvent(data)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | payload |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | id | String | 是 | 事件 |
    | params | String | 是 | payload |

-  返回值说明:  
成功：0, 失败：非0整数

-  函数示例  

```python
value =  {
    'test_event' : 100
}
param_str = json.dumps(value)
data = {
    'id': 'EventTest' ,
    'params': param_str
}
ret = aliyunIoT_device.postEvent(data)
```

## subscribe - 订阅

-  函数功能：  
Topic订阅

-  注意事项:  
确保Wi-FI连接成功并且已经创建deveice对象。

-  函数原型：
> subscribe(options)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

-  返回值说明:  
成功：0, 失败：非0整数

-  函数示例  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
def cb_lk_subcribe(data):
  print('enter cb_lk_subcribe')
aliyunIoT_device.on(Device.ON_SUBCRIBE, cb_lk_subcribe)

ret = aliyunIoT_device.subscribe(topic_test_info)
```
测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。

## unsubscribe - 取消订阅

- 函数功能：  
取消订阅

-  注意事项:  
确保Wi-FI连接成功。
确保已经subscribe过。

-  函数原型：
> unsubscribe(options)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

-  返回值说明:  
成功：0, 失败：非0整数

-  函数示例  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
ret = aliyunIoT_device.unsubscribe(topic_test_info)
```
测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。

## publish - 发布

-  函数功能：  
发布消息到指定topic

-  注意事项:  
确保Wi-FI连接成功并且已经创建对象。
确保已经subscribe成功。

-  函数原型:  
> publish(options)

-  参数说明：  
成功：Device实例, 失败：None
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |

-  data参数说明:  
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

-  返回值说明:  
成功：0, 失败：非0整数

-  函数示例：  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
ret = aliyunIoT_device.publish(topic_test_info)
```

测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。
​

## end - 结束

-  函数功能：  
close对象

-  函数原型：
> end()

-  参数说明：无
-  返回值说明:
成功：0, 失败：非0整数
-  函数示例
```python
ret = aliyunIoT_device.end()
```


