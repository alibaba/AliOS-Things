# aliyunIoT 设备类接口

- 模块功能

AliyunIoT模块可以帮助设备厂商将设备安全地接入到阿里云IoT物联网平台，继而让设备可以被阿里云IoT物联网平台进行管理。设备有三种形态，分别为直连设备（device），网关设备（gateway），网关子设备，其中网关子设备隶属于直连设备，唯一区别是没有联网能力，可被网关设备绑定为子设备。

阿里云物联网平台中涉及三个基本概念如下，

- 模块功能属性(prop)
云端/设备端数据互通的通道，云端通过改变prop值，反馈到设备端，设备端修改prop值，保存到云端；
- 事件(event)
设备端到云端的事件通知；
- 服务(service)
云端到设备端的服务调用；
> 注意，需要确保网络连接成功，下面是连接Wi-Fi的示例代码。
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
  print('connect succeed! ', sta_if.ifconfig())
else:
  print('network is connected.', sta_if.ifconfig())
```

- 输出

```python
connecting to network...
connect succeed!  ('192.168.170.30', '255.255.255.0', '192.168.170.26', '192.168.170.26')
```

> 在示例以及输出结果中对敏感数据做了替换处理。

## device - 初始化

- 函数功能：device初始化，初始化物联网平台Device类，获取Device实例。
- 注意事项：请替换[物联网平台](https://iot.console.aliyun.com/product)申请到的产品和设备信息。确保网络以及物联网平台连接成功
- 函数原型：
> device()
- 参数说明：无
- 返回值说明：
成功，device实例；失败，None
- 函数示例
```python
from aliyunIoT import Device     # iot组件是连接阿里云物联网平台的组件

device = Device()
if device is None:
    print("create device error")
else:
    print("create device succeed!")
```

- 输出

```python
create device succeed!
```
## register - 设备动态注册

- 函数功能：设备动态注册，用于一型一密设备获取deviceSecret。
- 注意事项：首先确保网络连接成功。确保物联网平台产品、设备创建成功 。
- 函数原型
> register(data, cb)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 设备信息 |
    | cb | func | 是 | 注册回调函数 |

- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 是 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

- 返回值说明：成功，0； 失败，非0整数
- 使用示例

```python
# 请首先添加连接WIFI连接代码

from aliyunIoT import Device

def cb_lk_register(data):
  global g_register_flag
  print("device regiter succeed !")
  g_register_flag = True

productKey = "产品key"
deviceName = "设备名称"
productSecret = "产品密钥"

dev_register_info = {
    'productKey': productKey,
    'deviceName': deviceName,
    'productSecret': productSecret
}

aliyunIoT_device = Device()
aliyunIoT_device.register(dev_register_info, cb_lk_register)
```

其中产品key、设备名称、产品密钥是在[物联网平台](https://iot.console.aliyun.com/product)上申请的三元组。

- 输出
```shell
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
success to establish mbedtls connection, fd = 54(cost 0 bytes in total, max used 0 bytes)
device regiter succeed !
```
## connect - 连接物联网平台
- 函数功能：连接物联网平台。
- 注意事项：确保网络连接成功。
- 函数原型
> connect(data)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 设备模型 |
- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

- 返回值说明：成功返回0， 其他表示失败
- 函数示例

```python
# 请首先添加连接Wi-Fi连接代码

from aliyunIoT import Device
# 三元组信息
productKey = "请输入您的productKey"  #需要填入物联网云平台申请到的productKey信息
deviceName = "请输入您的deviceName"  #需要填入物联网云平台申请到的deviceName信息
deviceSecret = "请输入您的deviceSecret" #需要填入物联网云平台申请到的deviceSecret信息

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_device.end()
utime.sleep(2)
```

其中产品key、设备名称、设备密钥需要在[物联网平台](https://iot.console.aliyun.com/product)上申请。

- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
```
## on - 回调注册

- 函数功能：注册device的回调函数，在相应事件触发时开始时触发。
- 注意事项：确保网络以及物联网平台连接成功
- 函数原型

> device.on(event_type, cb)

- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | event | int | 是 | 注册的事件类型 |
    | cb | fuction | 是 | 监听事件的回调函数 |

- 事件类型说明（event）
    | 事件 | 说明 |
    | --- | --- |
    | ON_CONNECT | 连接成功 |
    | ON_DISCONNECT | 断开连接 |
    | ON_SERVICE | service信息 |
    | ON_PROPS | 收到服务端的props消息 |
    | ON_CLOSE | 对象关闭 |
    | ON_ERROR | 异常 |
    | ON_SUBCRIBE | sub成功 |

- 函数示例
```python
同上个接口connect案例
```
- 输出

```shell
同上个接口connect案例
```
## getDeviceInfo - 获取设备信息
- 函数功能：获得保存在该设备上的设备信息。
- 注意事项：读取之前需要首先确认设备信息是否保存过。
- 函数原型
> getDeviceInfo()
- 参数说明：无
- 返回值说明：成功，设备信息（deviceInfo结构，字典类型）；失败，None
- deviceInfo结构体说明
    | 键值 | 类型 | 说明 |
    | --- | --- | --- |
    | deviceName | String | 物联网平台上注册的deviceName |
    | deviceSecret | String | 物联网平台上注册的deviceSecre |
    | productKey | String | 物联网平台上注册的productKey |
    | productSecret | String | 物联网平台上注册的productSecret |

- 函数示例

```python
from aliyunIoT import Device
aliyunIoT_deivce = Device()

deviceinfo = aliyunIoT_deivce.getDeviceInfo()

if deviceinfo != None:
	print('get DeviceInfo succeed')
    print(deviceinfo)
else:
    print('get DeviceInfo failed')
```

- 输出

```shell
get DeviceInfo succeed
{'productSecret': 'xxx', 'productKey': 'xxx', 'deviceName': 'mpy_002', 'deviceSecret': 'xxx'}
```
## getDeviceHandle - 获取句柄

- 函数功能：获得Device实例句柄
- 注意事项：确保已经成功创建过Device实例。
- 函数原型

> getDeviceHandle()

- 参数说明：无
- 返回值说明：成功，Device实例；失败，None
- 函数示例

```python
from aliyunIoT import Device
aliyunIoT_deivce = Device()

handle = aliyunIoT_deivce.getDeviceHandle()

if handle != None:
	print('get device handle succeed')
else:
    print('get device handle failed')
```
- 输出

```shell
get device handle succeed
```
## getNtpTime - NTP对时

- 函数功能：从物联网平台获得网络时间
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> getNtpTime(cb(data))
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | cb | func | 是 | 注册回调函数 |

- 返回值说明：成功，时间信息（字典类型，data） ；失败，非0整数
- data结构说明
    | 参数 | 类型 | 说明 |
    | --- | --- | --- |
    | year | String | 年 |
    | month | String | 月 |
    | day | String | 日 |
    | second | String | 秒 |
    | msecond | String | 毫秒 |
    | timestamp | int | 时间戳 |

- 函数示例

```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
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

aliyunIoT_device = Device()
aliyunIoT_device.connect(key_info)

ntptime_got = False

def on_getntptime(data):
  global ntptime_got
  print('timestamp:', data['timestamp'])
  ntptime_got = True

# main
aliyunIoT_device.getNtpTime(on_getntptime)

while(True):
    if ntptime_got:
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_device.end()
utime.sleep(2)
```
- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
timestamp: 1644408918
sleep for 2s
```
## postProps - 上报属性
- 函数功能：上报设备属性
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> postProps(data)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | payload |
- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | params | String | 是 | payload |

- 返回值说明：成功，0；失败，非0整数
- 函数示例

```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

value = {'test_prop' : 100}
data = {'params': json.dumps(value)}
ret = aliyunIoT_device.postProps(data)
if ret == 0 :
  print('属性上报成功')
else :
  print('属性上报失败')

utime.sleep(2)
aliyunIoT_device.end()
```
- 输出
```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
属性上报成功
```
## postEvent - 上报事件

- 函数功能：上报标准事件，格式见参数说明。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> postEvent(data)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | payload |

- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | id | String | 是 | 事件 |
    | params | String | 是 | payload |

- 返回值说明：成功，0；失败，非0整数
- 函数示例

```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

value =  {'test_event' : 100}
param_str = json.dumps(value)
data = {'id': 'EventTest' ,'params': param_str}
ret = aliyunIoT_device.postEvent(data)

if ret == 0 :
  print('事件上报成功')
else :
  print('事件上报失败')

utime.sleep(2)
aliyunIoT_device.end()
```
- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
事件上报成功
```
## postRaw - 上报自定义数据

- 函数功能：上报自定义Raw数据。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> postRaw(data)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | payload |
- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | id | String | 是 | 事件 |
    | params | String | 是 | payload |

- 返回值说明：成功，0；失败，非0整数
- 函数示例
```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

usertestdata = bytearray([0x00,0x00,0x00,0x00,0x01,0x00,0x32,0x01,0x00,0x00,0x00,0x00])
postdata={'param':usertestdata}
ret = aliyunIoT_device.postRaw(postdata)

if ret == 0 :
  print('RAW数据上报成功')
else :
  print('RAW数据上报失败')

utime.sleep(2)
aliyunIoT_device.end()
```

- 输出
```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
RAW数据上报成功
```
## uploadFile - 上传文件
- 函数功能：上传文件到物联网平台。
- 注意事项：
确保网络以及物联网平台连接成功。
文件大小需小于16MB，如超过请使用oss接口。
- 函数原型
> uploadFile(fileName, cb)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | fileName | str | 是 | 本地文件名字，包含路径名 |
    | cb | fun | 是 | 回调函数 |

- 返回值说明：成功，uploadID（字符串类型）；失败，None
- 函数示例
```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = '产品密钥'
deviceName = '设备名称'
deviceSecret = '设备密钥'

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)# 请首先添加连接WIFI连接代码

# 上传根目录的1.jpg到物联网平台，图片大小建议小于10K.
ret = aliyunIoT_device.uploadFile("1.jpg", None)
if ret != None :
  print('图片上传成功')
else :
  print('图片上传失败')

utime.sleep(10)
aliyunIoT_device.end()
```
- 输出

```shell
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
图片上传成功
```
## uploadContent - 上传数据流
- 函数功能：上传文件数据流，上传数据保存为指定文件文件名由参数指定。
- 注意事项：
确保网络以及物联网平台连接成功。
文件大小需小于16MB，超过请使用oss接口。
- 函数原型：
> uploadContent(fileName,  buf,  cb)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | fileName | str | 是 | 文件名字，用于在物联网云平台生成文件  |
    | buf | str | 是 | 待传输的数据 |
    | cb | fun | 是 | 回调函数 |
- 返回值说明：成功，uploadID（字符串类型）；失败，None
- 函数示例

```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = '产品密钥'
deviceName = '设备名称'
deviceSecret = '设备密钥'

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)# 请首先添加连接WIFI连接代码

# 上传根目录的1.jpg到物联网平台，图片大小建议小于10K.
ret = aliyunIoT_device.uploadFile("1.jpg", None)

# 上传根目录的1.jpg到物联网平台
import os
fileName = "1.jpg"
f = open(fileName, "r")
frame = f.read()

fileid = aliyunIoT_device.uploadContent(fileName, frame, None)
if fileid != None :
  print('图片上传成功')
else :
  print('图片上传失败')

utime.sleep(10)
aliyunIoT_device.end()
```
- 输出
```shell
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
图片上传成功
```
## subscribe - 订阅

- 函数功能：自定义Topic订阅
- 注意事项：确保网络以及物联网平台连接成功并且已经创建deveice对象。
- 函数原型
> subscribe(data)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |
- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

- 返回值说明：成功，0 ；失败，非0整数
- 函数示例
```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = '产品密钥'
deviceName = '设备名称'
deviceSecret = '设备密钥'

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

# 测试topic
topic_test_info = {
    'topic': '/' + productKey  + '/' + deviceName + '/user/aos_test',
    'qos': 1,
    'payload': 'device publish and subcribe test'
}

ret = aliyunIoT_device.subscribe(topic_test_info)
if ret == 0 :
  print('Topic订阅成功')
else :
  print('Topic订阅失败')

utime.sleep(2)
aliyunIoT_device.end()
```
测试topic需要预先在物联网平台上创建，本例代码中为aos_test，其中**产品key**、**设备名称**需要手动创建。

- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
Topic订阅成功
```
## publish - 发布

- 函数功能：
发布消息到指定topic
- 注意事项：
确保网络以及物联网平台连接成功并且已经创建对象。
确保已经subscribe成功。
- 函数原型
> publish(data)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |

- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

- 返回值说明：成功，0；失败，非0整数
- 函数示例
```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

# 测试topic
topic_test_info = {
    'topic': '/' + productKey  + '/' + deviceName + '/user/aos_test',
    'qos': 1,
    'payload': 'device publish and subcribe test'
}

g_subcribe = False

def cb_lk_subcribe(data):
	g_subcribe = True
	print('enter cb_lk_subcribe')

ret = aliyunIoT_device.subscribe(topic_test_info)
aliyunIoT_device.on(Device.ON_SUBCRIBE, cb_lk_subcribe)

ret = aliyunIoT_device.publish(topic_test_info)

while(True):
    if g_subcribe:
        print('Topic发布成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

utime.sleep(2)
aliyunIoT_device.end()
```
测试topic需要预先在物联网平台上创建，本样例代码中创建了aos_test，其中**产品key**、**设备名称**需要手动创建。

- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
enter cb_lk_subcribe
Topic发布成功
```

## unsubscribe - 取消订阅

- 函数功能：取消自定义订阅
- 注意事项：
确保网络以及物联网平台连接成功。
确保已经subscribe过。
- 函数原型：

> unsubscribe(data)

- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |
- data参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

- 返回值说明：成功，0；失败，非0整数
- 函数示例

```python
# 请首先添加连接WIFI连接代码

# 连接物联网云平台代码
from aliyunIoT import Device

productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"

# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True

key_info = {
    'region': 'cn-shanghai',
    'productKey': productKey,
    'deviceName': deviceName,
    'deviceSecret': deviceSecret,
    'keepaliveSec': 60
}
# 将三元组信息设置到iot组件中
aliyunIoT_device = Device()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_device.on(aliyunIoT_device.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_device.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

# 测试topic
topic_test_info = {
    'topic': '/' + productKey  + '/' + deviceName + '/user/aos_test',
    'qos': 1,
    'payload': 'device publish and subcribe test'
}
ret = aliyunIoT_device.unsubscribe(topic_test_info)
if ret == 0 :
  print('Topic取消订阅成功')
else :
  print('Topic取消订阅失败')

utime.sleep(2)
aliyunIoT_device.end()
```

- 输出

```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
Topic取消订阅成功
```
## end - 结束

- 函数功能：close对象 并销毁线程资源。
- 函数原型：
> end()
- 参数说明：无
- 返回值说明：成功，0； 失败，非0整数
- 函数示例

```python
ret = aliyunIoT_device.end()

if ret == 0:
	print('aliyunIoT end succeed')
    print(deviceinfo)
else:
    print('aliyunIoT end failed')
```

- 输出

```shell
aliyunIoT end succeed
```
