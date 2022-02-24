# aliyunIoT 网关类接口

- 模块功能

Gateway是aliyunIoT模块的一个实例类，提供端侧网关能力，支持为不具备联网能力的设备接入到物联网云端平台。使用时，由网关建立子设备与物联网平台之间的连接，包括把子设备拓扑上报到云端、协助子设备登录到云端、把子设备的数据上传到云端以及把云端的数据或控制命令转发到子设备。

请使用下面的代码导入aliyunIoT模块：
```python
from aliyunIoT import Gateway
```
请使用下面的代码连接Wi-Fi: essid和passwd请填写为实际的路由器名称及密码。
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
```shell
connecting to network...
connect succeed!  ('192.168.170.30', '255.255.255.0', '192.168.170.26', '192.168.170.26')
```
> 1、为了节约篇幅，接下去的各接口示例代码都在前一个案例基础上追加即可。
>
> 2、在示例以及输出结果中对敏感数据做了替换处理。

## gateway - 初始化

- 函数功能：
对象初始化，初始化物联网平台Gateway类，获取Gateway实例。
- 注意事项：
请替换在[物联网平台](https://iot.console.aliyun.com/product)申请到的产品和设备信息。
确保网络以及物联网平台连接成功。
- 函数原型：
> Gateway()

- 参数说明：
无
- 返回值说明:
成功：Gateway实例，失败：None。
- 函数示例
```python
from aliyunIoT import Gateway     # iot组件是连接阿里云物联网平台的组件

aliyunIoT_gateway = Gateway()
if aliyunIoT_gateway is None:
    print("create gateway error")
else:
    print("create gateway succeed!")
```

- 输出
```shell
create gateway succeed!
```
## connect - 连接物联网平台

- 函数功能：通过已经申请好的三元组信息连接物联网平台。
- 注意事项：确保网络连接成功并且已经创建gateway对象。
- 函数原型：
> connect(data)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 在[物联网平台](https://iot.console.aliyun.com/product)创建的三元组信息 |

- data结构说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 否 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

- 返回值：
成功返回0， 其他表示失败。
- 函数示例：
```python
# 请首先添加连接Wi-Fi连接代码

from aliyunIoT import Gateway

def cb_lk_register(data):
  global g_register_flag
  print("gateway regiter succeed !")
  g_register_flag = True

productKey = "产品key"
deviceName = "设备名称"
productSecret = "产品密钥"

dev_register_info = {
    'productKey': productKey,
    'deviceName': deviceName,
    'productSecret': productSecret
}

# 将三元组信息设置到iot组件中
aliyunIoT_gateway = Gateway()

# 设定连接到物联网平台的回调函数，如果连接物联网平台成功，则调用on_connect函数
aliyunIoT_gateway.on(aliyunIoT_gateway.ON_CONNECT, on_connect)

# 启动连接阿里云物联网平台过程
aliyunIoT_gateway.connect(key_info)

# 等待设备成功连接到物联网平台
while(True):
    if iot_connected:
        print('物联网平台连接成功')
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_gateway.end()
utime.sleep(2)
```

- 输出结果:
```shell
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
```
## on - 监听回调函数

- 函数功能：注册Gateway的回调函数，在相应事件触发时回调。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> gateway.on(event,  cb)

- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | event | int | 是 | 注册的事件类型 |
    | cb | fuction | 是 | 监听事件的回调函数 |
- 事件类型：
    | 事件 | 说明 |
    | --- | --- |
    | ON_CONNECT | 连接成功 |
    | ON_DISCONNECT | 断开连接 |
    | ON_SERVICE | service信息 |
    | ON_TOPOGET | 获取子设备信息 |
    | ON_TOPOADD | 添加子设备成功 |
    | ON_TOPOREMOVE | 删除子设备成功  |
    | ON_LOGIN | 登陆成功 |
    | ON_LOGOUT | 退出成功 |
    | ON_SUBREGISTER | 子设备绑定 |

- 函数示例：
```shell
参考上个接口-connect案例
```
## getGatewayInfo - 获取设备信息

- 函数功能：获得保存在该设备上的设备信息。
- 注意事项：读取之前需要首先确认设备信息是否保存过。
- 函数原型：
>getGatewayInfo()
- 参数说明：无
- 返回值说明：成功，设备信息（gatewayInfo，字典类型）；失败，None
- gatewayInfo结构说明
    | 参数 | 类型 | 说明 |
    | --- | --- | --- |
    | deviceName | String | 物联网平台上注册的deviceName​ |
    | deviceSecret | String | 物联网平台上注册的deviceSecre​ |
    | productKey | String | 物联网平台上注册的productKey​ |
    | productSecret | String| 物联网平台上注册的productSecret​ |
- 函数示例
```python
from aliyunIoT import Gateway

aliyunIoT_gateway = Gateway()

gatewayinfo = aliyunIoT_gateway.getGatewayInfo()

if gatewayinfo != None:
	print('get gatewayinfo succeed')
    print(deviceinfo)
else:
    print('get gatewayinfo failed')
```

- 输出
```shell
get gatewayinfo succeed
{'productSecret': 'xxx', 'productKey': 'xxx', 'deviceName': 'xxx', 'deviceSecret': 'xxx'}
```

## getGatewayHandle - 获取网关句柄
- 函数功能：获得gateway实例句柄, 用于其他模块使用。
- 注意事项：确保已经创建getway对象。
- 函数原型：
> getGatewayHandle()
- 参数说明：无
- 返回值说明：成功，gateway实例；失败，None。
- 函数示例：
```python
from aliyunIoT import Gateway
aliyunIoT_gateway = Gateway()

handle = aliyunIoT_gateway.getGatewayHandle()

if handle != None:
	print('get gateway handle succeed')
else:
    print('get device handle failed')
```
- 输出结果:
```shell
get gateway handle successfully
```
## getNtpTime - 获取网络时间
- 函数功能：从物联网平台获得网络时间。
- 注意事项：确保网络以及物联网平台连接成功并且已经创建gateway对象。
- 函数原型：
> getNtpTime(cb(data))
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | cb | func | 是 | 回调函数 |
- 返回值说明：成功：Gateway实例，失败：None
- 参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | 时间字典文件 |
- data参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | year | String | 是 | 年 |
    | month | String | 是 | 月 |
    | day | String | 是 | 日 |
    | second | String | 是 | 秒 |
    | msecond | String | 是 | 毫秒 |
    | timestamp | int | 是 | 时间戳 |
- 函数示例：
```python
# 请首先添加连接Wi-Fi连接代码

# 连接物联网云平台代码
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
aliyunIoT_gateway.connect(key_info)

ntptime_got = False

def on_getntptime(data):
  global ntptime_got
  print('timestamp:', data['timestamp'])
  ntptime_got = True

# main
aliyunIoT_gateway.getNtpTime(on_getntptime)

# 断开物联网云平台连接
while(True):
    if ntptime_got:
        break
    else:
        print('sleep for 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_gateway.end()
utime.sleep(2)
```
- 输出结果:
```shell
success to establish tcp, fd=54
sleep for 1 s
timestamp: 1645005337
sleep for 2s
```
## addTopo - 添加子设备

- 函数功能：对网关设备添加子设备。
- 注意事项：确保网络以及物联网平台连接成功并且已经创建gateway对象。网关设备跟子设备需要提前注册。
- 函数原型
> addTopo(subdev_info)
- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | dictionary | 是 | 子设备信息 |
- subdev_info 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | deviceSecret | String | 是 | 物联网平台上注册的deviceSecre |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 否 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |
- 返回值说明：成功：0，失败：非0整数。
- 函数示例：
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
# 物联网平台连接标志位
iot_connected = False

def on_connect(data):
    global iot_connected
    iot_connected = True
    print('物联网连接成功')

aliyunIoT_gateway = Gateway()

aliyunIoT_gateway.on(Gateway.ON_CONNECT, on_connect)
aliyunIoT_gateway.connect(key_info)

# 等待物联网云平台连接
while(True):
    if iot_connected:
        break
    else:
        print('sleep for iot_connected 1 s')
        utime.sleep(1)

# add topo
g_addtopo_flag = False

def on_topo_add(data):
    global g_addtopo_flag
    print('enter on_topo_add')
    g_addtopo_flag = True

aliyunIoT_gateway.on(Gateway.ON_TOPOADD, on_topo_add)
aliyunIoT_gateway.addTopo(subdev_info)

while(True):
    if g_addtopo_flag:
        break
    else:
        print('sleep for on_topo_add 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_gateway.end()
utime.sleep(2)
```

- 输出
```shell
sleep for on_topo_add 1 s
enter on_topo_add
sleep for 2s
```
## getTopo - 获取子设备
- 函数功能：获得该网关已经添加过子设备列表，结果在msg中查看。
- 注意事项：确保网络以及物联网平台连接成功。操作之前确保已经添加了子设备到该网关。
- 函数原型
> getTopo()
- 参数说明：无
- 返回值说明：成功，0；失败，非0整数
- 函数示例
在上个案例的基础上添加如下getTopo示例：
```python
g_topogot_flag = False

def on_topo_get(data):
  global g_topogot_flag
  g_topogot_flag = True
  print(data)

aliyunIoT_gateway.on(Gateway.ON_TOPOGET, on_topo_get)
aliyunIoT_gateway.getTopo()
while(True):
    if g_topogot_flag:
        break
    else:
        print('sleep for on_topo_get 1 s')
        utime.sleep(1)

print('sleep for 2s')
aliyunIoT_gateway.end()
```
- 输出
```shell
sleep for on_topo_get 1 s
{'code': 0, 'message': '[{"deviceSecret":"xxx","productKey":"xxx","deviceName":"xxx"},{"deviceSecret":"xxx","productKey":"xxx","deviceName":"xxx"}]'}
sleep for 2s
```
## login - 子设备登陆
- 函数功能：子设备登陆。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> login(subdev_info)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |

- subdev_info 参数说明：subdev_info参数同上
- 返回值说明：成功：0, 失败：非0整数
- 函数示例：

在上个案例的基础上添加如下代码：
```python
g_login_flag = False

def on_login(data):
    global g_login_flag
    g_login_flag = True

aliyunIoT_gateway.on(Gateway.ON_LOGIN, on_login)
aliyunIoT_gateway.login(subdev_info)
while(True):
    if g_login_flag:
        print('login out succeed')
        break
    else:
        print('sleep for on_login 1 s')
        utime.sleep(1)
```

- 输出
```shell
sleep for on_login 1 s
login out succeed
```
## registerSubDevice - 子设备动态注册

- 函数功能：子设备动态注册，用于一型一密量产的设备。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> registerSubDevice(subdev_register_info)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_register_info | dict | 是 | 子设备三元组集（不包含deviceSecret） |
- subdev_register_info参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | deviceName | String | 是 | 物联网平台上注册的deviceName |
    | productKey | String | 是 | 物联网平台上注册的productKey |
    | productSecret | String | 是 | 物联网平台上注册的productSecret |
    | region | String | 否 | 阿里云region，默认值：cn-shanghai |
    | keepaliveSec | int | 否 | 物联网平台上注册的keepaliveSec |

- 返回值说明:
成功：0，失败：非0整数
- 函数示例：

在上个案例的基础上添加如下代码：
```python
subdev_register_info = [
    {
        'productKey': '子设备3产品key',
        'deviceName': '子设备3名字',
        'productSecret': '子设备3产品Secret',
    },
    {
        'productKey': '子设备4产品key',
        'deviceName': '子设备4名字',
        'productSecret': '子设备4产品Secret',
    },
]

# sub register
g_subregister_flag = False

# 子设备动态注册回调函数
def on_subregister(data):
    global g_subregister_flag
    g_subregister_flag = True

aliyunIoT_gateway.on(Gateway.ON_SUBREGISTER, on_subregister)
ret = aliyunIoT_gateway.registerSubDevice(subdev_register_info)

while(True):
    if g_subregister_flag:
        print('subdevice register succeed')
        break
    else:
        print('sleep for on_subregister 1 s')
        utime.sleep(1)
```

- 输出
```shell
sleep for on_subregister 1 s
subdevice register succeed
```
## subscribe - 订阅
- 函数功能：
用于自定义Topic订阅。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> subscribe(data)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | data | dictionary | 是 | tpoic信息 |
- data参数说明:
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | topic | String | 是 | 创建时指定 |
    | qos | int | 是 | 0，1 |
    | payload | String | 否 | 描述信息 |

- 返回值说明：成功，0；失败，非0整数
- 函数示例：

在上个案例的基础上添加如下代码：
```python
# 测试topic
topic_test_info = {
  'topic': '/产品key/设备名称/user/aos_test',
  'qos': 1,
  'payload': 'device publish and subcribe test'
}

# 订阅topic
ret = aliyunIoT_gateway.subscribe(topic_test_info)
if ret == 0:
    print('topic subscribe succeed')
else:
    print('topic subscribe failed')
```

测试topic需要预先在物联网平台上创建，本案例中创建了aos_test，其中**产品key**、**设备名称**需要手动创建。

- 输出
```shell
[   7.040]<I>AIOT_GATEWAY subcribe topic: /xxx/xxx/user/aos_test  succeed
topic subscribe succeed
```
## publish - 发布

- 函数功能：发布消息到指定topic。
- 注意事项：确保网络以及物联网平台连接成功并且已经创建对象，确保已经订阅该topic。
- 函数原型：
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

- 返回值说明：成功：0, 失败：非0整数
- 函数示例：
在上个案例的基础上添加如下代码： **​**
```python
g_subcribe_count = False
def on_subcribe(data):
    global g_subcribe_count
    g_subcribe_count = True

aliyunIoT_gateway.on(Gateway.ON_SUBCRIBE, on_subcribe)
aliyunIoT_gateway.publish(topic_test_info)
while(True):
    if g_subcribe_count:
        print('topic publsih succeed')
        break
    else:
        print('sleep for publsih_subcribe 1 s')
        utime.sleep(1)
```
- 输出
```shell
sleep for publsih_subcribe 1 s
topic publsih succeed
```
## unsubscribe - 取消订阅
- 函数功能：取消订阅自定义topic。
- 注意事项：确保网络以及物联网平台连接成功，并且该topic已经订阅。
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
- 函数示例：
在上个案例的基础上添加如下代码：
```python
# unsubcribe
ret = aliyunIoT_gateway.unsubscribe(topic_test_info)
if ret == 0:
    print('topic unsubscribe succeed')
else:
    print('topic unsubscribe failed')
```

测试topic需要预先在物联网平台上创建，案例中创建了aos_test，其中“产品key”、“设备名称”需要手动创建。

- 输出
```shell
topic unsubscribe succeed
```
## logout - 子设备退出

- 函数功能：子设备退出。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> logout(subdev_info)
- 参数说明：
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |
- subdev_info 参数说明：subdev_info参数同上
- 返回值说明：成功，0；失败，非0整数
- 函数示例：

在上个案例的基础上添加如下代码：
```python
g_logout_flag = False

def on_logout(data):
    global g_logout_flag
    g_logout_flag = True

aliyunIoT_gateway.on(Gateway.ON_LOGOUT, on_logout)
aliyunIoT_gateway.logout(subdev_info)
while(True):
    if g_logout_flag:
        print('login out succeed')
        break
    else:
        print('sleep for on_logout 1 s')
        utime.sleep(1)
```

- 输出
```shell
sleep for on_logout 1 s
login out succeed
```


## removeTopo - 删除子设备

- 函数功能：从网关中删除子设备。
- 注意事项：确保网络以及物联网平台连接成功。
- 函数原型：
> removeTopo(subdev_info)

- 参数说明
    | 参数 | 类型 | 必选参数 | 说明 |
    | --- | --- | --- | --- |
    | subdev_info | 数组 | 是 | 子设备列表 |
- subdev_info 参数说明：subdev_info参数同上
- 返回值说明：成功，0；失败，非0整数
- 函数示例：
在上个案例的基础上添加如下代码：
```python
g_removetopo_flag = False

def on_topo_remove(data):
    global g_removetopo_flag
    g_removetopo_flag = True

aliyunIoT_gateway.on(Gateway.ON_TOPOREMOVE, on_topo_remove)
aliyunIoT_gateway.removeTopo(subdev_info)
while(True):
    if g_removetopo_flag:
        print('topo remove succeed')
        break
    else:
        print('sleep for on_topo_remove 1 s')
        utime.sleep(1)
```

- 输出
```shell
sleep for on_topo_remove 1 s
topo remove succeed
```
## end - 结束

- 函数功能：close gateway对象，销毁资源。
- 注意事项:：无
- 函数原型：
> end()
- 参数说明：无
- 返回值说明：成功，0；失败，非0整数。
- 函数示例：
```python
ret = aliyunIoT_gateway.end()
if ret == 0:
    print('getway end succeed')
else:
    print('getway end failed')
```

- 输出
```shell
getway end succeed
```
