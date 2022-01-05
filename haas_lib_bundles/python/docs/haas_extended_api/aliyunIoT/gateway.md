# aliyunIoT 网关类接口

-  模块功能  
Gateway是aliyunIoT模块的一个实例类，提供端侧网关能力，支持为不具备联网能力的设备接入到物联网云端平台。使用时，由网关建立子设备与物联网平台之间的连接，包括把子设备拓扑上报到云端、协助子设备登录到云端、把子设备的数据上传到云端以及把云端的数据或控制命令转发到子设备。

> 注意，需要确保Wi-FI连接成功

请使用下面的代码导入aliyunIoT模块：

```python
from aliyunIoT import Gateway
```

请使用下面的代码连接Wi-Fi: essid和passwd请填写为实际的路由器名称及密码。

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

直到sta_if.isconnected()返回 True 则表明网络连接成功


## gateway - 初始化

-  函数功能：  
对象初始化，初始化物联网平台Gateway类，获取Gateway实例。

-  注意事项:  
确保wifi连接成功。

-  函数原型：
> Gateway()

-  参数说明：  
无

-  返回值说明:  
成功：Gateway实例，失败：None。

-  函数示例  

```python
from aliyunIoT import Gateway
aliyunIoT_gateway = Gateway()
```

## connect - 连接物联网平台

-  函数功能：  
连接物联网平台

-  注意事项:  
确保Wi-FI连接成功并且已经创建gateway对象。

-  函数原型：
> connect(data)

-  参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 上报模块版本号需要的信息 |

-  data参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 否 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

-  返回值：  
成功返回0， 其他表示失败。

-  函数示例：  
其中产品密钥、设备名称、设备密钥是在[物联网平台](https://iot.console.aliyun.com/product)上申请的三元组。

```python
from aliyunIoT import Gateway

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
aliyunIoT_gateway = Gateway()
ret = aliyunIoT_gateway.connect(key_info)
```

## on - 事件回调

-  函数功能：  
注册Gateway的回调函数，在相应事件触发时开始时触发

-  注意事项:  
确保wifi连接成功。
确保aliyunIoT连接成功。

-  函数原型：
> gateway.on(event_type, cb)

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
| ON_TOPOGET | 获取topo |
| ON_TOPOADD | add topo成功 |
| ON_TOPOREMOVE | 删除topo成功 |
| ON_LOGIN | 登陆成功 |
| ON_LOGOUT | 退出成功 |
| ON_SUBREGISTER | 子设备绑定 |

-  函数示例：  

```python
from aliyunIoT import Gateway

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
aliyunIoT_gateway = Gateway()

aliyunIoT_gateway.on(Gateway.ON_CONNECT, on_lk_connect)
ret = aliyunIoT_gateway.connect(key_info)
```

## getGatewayHandle

-  函数功能：  
获得gateway实例句柄

-  注意事项:  
确保Wi-FI连接成功并且已经创建getway对象。

-  函数原型：  
> getGatewayHandle()

-  参数说明：  
无

-  返回值说明:  
成功：gateway实例, 失败：None。

-  函数示例：  

```python
from aliyunIoT import Gateway

aliyunIoT_gateway = Gateway()
ret = aliyunIoT_gateway.getGatewayHandle()
```

## getNtpTime

-  函数功能：  
从物联网平台获得网络时间。

-  注意事项:  
确保Wi-FI连接成功并且已经创建gateway对象。

-  函数原型：
> getNtpTime(cb(data))

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | cb | func | 是 | 注册回调函数 |

-  返回值说明:
成功：Gateway实例, 失败：None

-  参数说明:

    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 上报模块版本号需要的信息 |

-  data参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | year | String | 是 | 年 |
    | month | String | 是 | 月 |
    | day | String | 是 | 日 |
    | second | String | 是 | 秒 |
    | msecond | String | 是 | 毫秒 |
    | timestamp | int | 是 | 时间戳 |

-  函数示例：  

```python
from aliyunIoT import Gateway

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
aliyunIoT_gateway = Gateway()
ret = aliyunIoT_gateway.connect(key_info)
def on_getntptime(data):
    print('%d-%d-%d %d:%d:%d:%d'%(data['year'],data['month'],\
        data['day'],data['hour'],\
        data['minute'],data['second'],data['msecond']))
    print('timestamp:', data['timestamp'])

ret = aliyunIoT_gateway.getNtpTime(on_getntptime)
```


## addTopo - 添加拓扑

-  函数功能：  
获得gateway实例句柄。

-  注意事项:  
确保Wi-FI连接成功并且已经创建gateway对象。

-  函数原型：  
> addTopo(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | dictionary | 是 | 子设备信息 |

-  subdev_info 参数说明:
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 否 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

-  返回值说明:
成功：gateway实例, 失败：None。

-  函数示例：  

```python
from aliyunIoT import Gateway

productKey = "网关设备产品密钥"
deviceName = "网关设备名称"
deviceSecret = "网关设备密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}
subdev_info = [
  {
      'productKey': '子设备产品密钥',
      'deviceName': '子设备名字',
      'deviceSecret': '子设备密钥',
  },
]
aliyunIoT_gateway = Gateway()
ret = aliyunIoT_gateway.connect(key_info)

def on_topo_add(data):
  print('on_topo_add')

aliyunIoT_gateway.on(Gateway.ON_TOPOADD, on_topo_add)
ret = aliyunIoT_gateway.addTopo(subdev_info)
```


## getTopo - 获取拓扑

-  函数功能：  
获得gateway拓扑。

-  注意事项:
确保Wi-FI连接成功。

-  函数原型：  
> addTopo(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数？ | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | dictionary | 是 | 子设备信息 |

-  subdev_info 参数说明:
    subdev_info参数同上

-  返回值说明:
成功：gateway实例, 失败：None。

-  函数示例：  

```python
from aliyunIoT import Gateway

productKey = "网关设备产品密钥"
deviceName = "网关设备名称"
deviceSecret = "网关设备密钥"
key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}
subdev_info = [
  {
      'productKey': '子设备产品密钥',
      'deviceName': '子设备名字',
      'deviceSecret': '子设备密钥',
  },
]
aliyunIoT_gateway = Gateway()
ret = aliyunIoT_gateway.connect(key_info)

def on_topo_get(data):
  print('enter on_topo_get')

aliyunIoT_gateway.on(Gateway.ON_TOPOGET, on_topo_get)
ret = aliyunIoT_gateway.getTopo()
```

## login - 子设备登陆

-  函数功能：  
子设备登陆。

-  注意事项:
确保Wi-FI连接成功。

-  函数原型：  
> login(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |

-  subdev_info 参数说明: subdev_info参数同上

-  返回值说明:
成功：0, 失败：非0整数
-  函数示例：  

```python
def on_login(data):
  print('enter on_login')

aliyunIoT_gateway.on(Gateway.ON_LOGIN, on_login)
ret = aliyunIoT_gateway.login(subdev_info)
```


## registerSubDevice - 子设备动态注册


-  函数功能：  

Topic订阅

-  注意事项:
确保Wi-FI连接成功。
-  函数原型：  
> registerSubDevice(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | tpoic信息 |

-  subdev_info参数说明:subdev_info参数同上

-  返回值说明:
成功：0, 失败：非0整数
-  函数示例：  

```python
subdev_register_info = [
    {
        'productKey': '子设备3产品密钥',
        'deviceName': '子设备3名字',
        'deviceSecret': '子设备3密钥',
    },
    {
        'productKey': '子设备4产品密钥',
        'deviceName': '子设备4名字',
        'deviceSecret': '子设备4密钥',
    },
]

def on_subregister(data):
  print('enter on_subregister')

aliyunIoT_gateway.on(Gateway.ON_SUBREGISTER, on_subregister)
ret = aliyunIoT_gateway.registerSubDevice(subdev_register_info)
```
​

## subscribe - 订阅


-  函数功能：  
Topic订阅
-  注意事项:
确保Wi-FI连接成功。

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
-  函数示例：  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
def cb_lk_subcribe(data):
  print('enter cb_lk_subcribe')
aliyunIoT_gateway.on(Gateway.ON_SUBCRIBE, cb_lk_subcribe)

ret = aliyunIoT_gateway.subscribe(topic_test_info)
```
测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。
## unsubscribe - 取消订阅

- 函数功能：
取消订阅
-  注意事项:
确保Wi-FI连接成功。
确保已经subscribe。
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

-  函数示例：  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
ret = aliyunIoT_gateway.unsubscribe(topic_test_info)
```
测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。

## publish - 发布

-  函数功能：  
发布消息到指定topic
-  注意事项:
    确保Wi-FI连接成功并且已经创建ƒ对象。
    确保已经subscribe。
-  函数原型：  
> publish(options)

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
-  函数示例：  

```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}
ret = aliyunIoT_gateway.publish(topic_test_info)
```
测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。
​

## logout - 子设备退出


-  函数功能：  
子设备退出。
-  注意事项:
确保Wi-FI连接成功。
-  函数原型：  
> logout(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |

-  subdev_info 参数说明:

subdev_info参数同上

-  返回值说明:
成功：0, 失败：非0整数
-  函数示例：  

```python
def on_logout(data):
  print('enter on_logout')

aliyunIoT_gateway.on(Gateway.ON_LOGOUT, on_logout)
ret = aliyunIoT_gateway.logout(subdev_info)
```


## removetopo - 删除子设备


-  函数功能：  
从网络拓扑中删除子设备。
-  注意事项:
确保Wi-FI连接成功。
-  函数原型：  
> removetopo(subdev_info)

-  参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |

-  subdev_info 参数说明: subdev_info参数同上

-  返回值说明:
成功：0, 失败：非0整数
-  函数示例：  

```python
def on_removetopo(data):
  print('enter on_removetopo')

aliyunIoT_gateway.on(Gateway.ON_LOGOUT, on_removetopo)
ret = aliyunIoT_gateway.removetopo(subdev_info)
```
​

## end - 结束
-  函数功能：  
close对象

-  注意事项: 无
-  函数原型：  
> end()

-  参数说明：  
无

-  返回值说明:  
成功：0，失败：非0整数。

-  函数示例：  

```python
ret = aliyunIoT_gateway.end()
```
