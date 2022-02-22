# bluetooth - 蓝牙

* 模块功能:  
该模块为板上的蓝牙控制器提供接口。目前支持低功耗蓝牙 (BLE)的Central（中央）, Peripheral（外设）, Broadcaster（广播者）, 和Observer （观察者）角色，以及 GATT 的服务器（Server）和客户端（Client）。

* 注意事项:
当前该模块只支持低功耗蓝牙（BLE）

# bluetooth.BLE
* 类功能：
   该类为低功耗蓝牙（BLE），是bluetooth模块中主要的类。

## BLE - 创建BLE对象
* 函数功能:
创建BLE对象。

* 函数原型:
> ble = bluetooth.BLE()

* 参数说明:
无

* 返回值:
BLE对象成功，返回单例的BLE对象；BLE对象创建失败，返回None

* 示例代码:
```Python
import bluetooth
ble = bluetooth.BLE()
```

## active - 更改BLE射频的活动状态

* 函数功能:
更改 BLE 射频的活动状态，并返回当前状态。

* 注意事项:
在使用此类上的任何其他方法之前，必须使射频处于活动状态。

* 函数原型：
> BLE.active(active)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|active|布尔型|是|True：激活BLE射频; False: 关闭BLE射频|

* 返回值:
无

* 示例：
```Python
import bluetooth
ble = bluetooth.BLE()
ble.active(True)
```

## config - 获取或配置BLE的配置值

* 函数功能:
获取或设置 BLE 接口的配置值。要获得一个值，参数名称应为一个字符串，并且一次只查询一个参数。要设置值，请使用关键字语法，并且一次可以设置一个或多个参数。

* 注意事项:
需确保BLE射频处于active状态。如果不使用此函数配置将使用空值或默认值。

* 函数原型：
> BLE.config('param' )
> BLE.config(*, param=value, ...)

* 参数说明:
当前支持的值是：

'mac'：当前使用的地址，取决于当前的地址模式。这将返回一个元组。(addr_type, addr)

'addr_mode': 设置地址模式。值可以是：

 > 0x00 - PUBLIC - 使用控制器的公共地址。
 > 0x01 - RANDOM - 使用生成的静态地址。
 > 0x02 - RPA - 使用可解析的私有地址。
 > 0x03 - NRPA - 使用不可解析的私有地址。
 > 默认情况下使用 PUBLIC 地址，否则将使用 RANDOM 地址。

'gap_name': 获取/设置服务0x1800使用的GAP设备名称，特征0x2a00。这可以随时设置并多次更改。

'rxbuf'：获取/设置用于存储传入事件的内部缓冲区的大小（以字节为单位）。该缓冲区对整个 BLE 驱动程序是全局的。此项配置值改大可以更好地处理突发传入数据（例如扫描结果）和接收更大特征值的能力。

'mtu'：获取/设置将在 ATT MTU 交换期间使用的 MTU。生成的 MTU 将是该 MTU 和远程设备的 MTU 中的最小值。ATT MTU 交换不会自动发生（除非远程设备通过gattc_exchange_mtu启动它）。

'bond'：设置配对时是否启用绑定。启用后，配对请求将设置“绑定”标志，并且密钥将由两个设备存储。

'mitm': 设置配对是否需要 MITM 保护。

'io'：设置此设备的 I/O 功能。

可用选项有：

>  _IO_CAPABILITY_DISPLAY_ONLY = const(0)
>  _IO_CAPABILITY_DISPLAY_YESNO = const(1)
>  _IO_CAPABILITY_KEYBOARD_ONLY = const(2)
>  _IO_CAPABILITY_NO_INPUT_OUTPUT = const(3)
>  _IO_CAPABILITY_KEYBOARD_DISPLAY = const(4)

'le_secure'：设置是否需要“LE Secure”配对。默认为 false（即允许“传统配对”）。

* 返回值:
  无

* 示例：
```Python
import bluetooth
ble = bluetooth.BLE()
ble.active(True)
ble.config(rxbuf=1024)
```

## irq - 事件处理

* 函数功能:
注册一个BLE协议栈的事件回调。回调函数带两个参数: event - 事件码 和 data - 事件值的元组类型。

* 注意事项:
在元组中的addr， adv_data，char_data，notify_data，和uuid 是只读的，指向bluetooth的内部ringbuffer，并且仅在IRQ处理函数的调用期间有效。如果程序需要保存这些值，以在 IRQ 处理程序返回后访问（例如，通过将其保存在类实例或全局变量中），那么它需要通过使用bytes() 或bluetooth.UUID()等方式获取数据的副本，像这样：
>connected_addr = bytes(addr)  # equivalently: adv_data, char_data, or notify_data
>matched_uuid = bluetooth.UUID(uuid)

例如，扫描结果的 IRQ 处理程序可能会检查adv_data 以决定它是否是正确的设备，然后才复制地址数据以在程序的其他地方使用。并且需要从 IRQ 处理程序中打印数据 print(bytes(addr))。

* 函数原型：
>  BLE.irq(handler)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|handler|函数型|是|函数带两个输入参数event和data|

* 返回值:
无

* 示例：

```Python

def bt_irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        # A central has connected to this peripheral.
        conn_handle, addr_type, addr = data
    elif event == _IRQ_CENTRAL_DISCONNECT:
        # A central has disconnected from this peripheral.
        conn_handle, addr_type, addr = data
    elif event == _IRQ_SCAN_RESULT:
        # A single scan result.
        addr_type, addr, adv_type, rssi, adv_data = data
    elif event == _IRQ_SCAN_DONE:
        # Scan duration finished or manually stopped.
        pass
    elif event == _IRQ_PERIPHERAL_CONNECT:
        # A successful gap_connect().
        conn_handle, addr_type, addr = data
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        # Connected peripheral has disconnected.
        conn_handle, addr_type, addr = data

BLE.irq(bt_irq)
```
事件代码是：
```
from micropython import const
_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)
_IRQ_GATTS_INDICATE_DONE = const(20)
_IRQ_MTU_EXCHANGED = const(21)
_IRQ_L2CAP_ACCEPT = const(22)
_IRQ_L2CAP_CONNECT = const(23)
_IRQ_L2CAP_DISCONNECT = const(24)
_IRQ_L2CAP_RECV = const(25)
_IRQ_L2CAP_SEND_READY = const(26)
_IRQ_CONNECTION_UPDATE = const(27)
_IRQ_ENCRYPTION_UPDATE = const(28)
_IRQ_GET_SECRET = const(29)
_IRQ_SET_SECRET = const(30)
```

## gap_advertise - 开启或关闭BLE广播

* 函数功能:
设置一个特定的广播间隔（单位是微秒）开启BLE广播，此间隔最低为625us。
关闭BLE广播时，将广播间隔参数（interval_us）设为None。

* 注意事项：
  广播者角色时使用

* 函数原型：
> BLE.gap_advertise(interval_us, adv_data=None, *, resp_data=None, connectable=True)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|interval_us|整型|是|广播间隔，为None时关闭广播|
|adv_data |string|否|adv_data可指向实现缓冲协议的任何类型（例如bytes，bytearray，str），为None时则使用上一次调用时候的值|
|resp_data |string|否|resp_data可指向实现缓冲协议的任何类型（例如bytes，bytearray，str），为None时则使用上一次调用时候的值|
|connectable|布尔型|否|表示是否可连接|

* 返回值:
无

* 示例：

```Python
import bluetooth
ble = bluetooth.BLE()
ble.active(True)
# payload 需要具体定义
ble.gap_advertise(500000, adv_data=payload)

```

## gap_scan - BLE扫描

* 函数功能:
以一个特定的间隔（interval_us）和 窗口（window_us）执行BLE扫描操作; 扫描器将每interval_us微秒运行window_us 微秒，总共持续duration_ms毫秒。默认间隔和窗口分别为 1.28 秒和 11.25 毫秒（后台扫描）

* 注意事项:
对于每个扫描结果，将触发_IRQ_SCAN_RESULT事件，并带有事件数据(addr_type, addr, adv_type, rssi, adv_data)。
当扫描停止时（由于持续时间完成或明确停止），将触发_IRQ_SCAN_DONE事件。
观察者角色时使用。

addr_type 值表示公共或随机地址：
>0x00 - 公共
>0x01 - RANDOM（静态、RPA 或 NRPA，类型在地址本身中编码）

adv_type 值对应于蓝牙规范：
>0x00 - ADV_IND - 可连接和可扫描的无向广告
>0x01 - ADV_DIRECT_IND - 可连接的定向广告
>0x02 - ADV_SCAN_IND - 可扫描的无向广告
>0x03 - ADV_NONCONN_IND - 不可连接的无向广告
>0x04 - SCAN_RSP - 扫描响应

* 函数原型:
> BLE.gap_scan( duration_ms , interval_us=1280000 , window_us=11250 , active=False  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|duration_ms|整型|是|扫描持续时间，单位为毫秒，当设为0时，无限期扫描；当设为None时停止扫描|
|interval_us|整型|否|扫描间隔时间，单位为微秒|
|window_us|整型|否|扫描窗口时间，单位为微秒|
|active|布尔型|否|设为True时，表示要在结果中接受扫描响应（scan_rsp）,默认为False|

* 返回值:
无

* 示例：

```Python
import bluetooth
ble = bluetooth.BLE()
ble.active(True)

ble.gap_scan(2000, 30000, 20000)
```

## gap_connect - 连接BLE外围设备

* 函数功能:
连接BLE外围设备

* 注意事项:
   中央设备（Central）以观察者角色通过gap_scan方法获得外围设备信息，或者已知外围设备地址, 之后可通过设备信息触发BLE连接请求。
   成功后，将触发_IRQ_PERIPHERAL_CONNECT事件，该事件data中带有连接句柄。

* 函数原型：
> BLE.gap_connect( addr_type , addr , scan_duration_ms=2000 , min_conn_interval_us=None , max_conn_interval_us=None )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|addr_type|枚举型|是|0x00 - 公共地址；0x01 - 随机地址|
|addr|string|是|要连接的设备地址|
|scan_duration_ms|整型|否|设备将等待最多scan_duration_ms以从设备接收广播负载|
|min_conn_interval_us|整型|否|连接间隔的最小值，单位为微秒|
|max_conn_interval_us|整型|否|连接间隔的最大值，单位为微秒|


* 返回值:
无

* 示例：
```Python
import bluetooth
ble = bluetooth.BLE()
ble.active(True)
# addr = xx:xx:xx:xx:xx:xx
ble.connect(0x00, addr)

```

## gap_disconnect - 断开BLE的连接

* 函数功能:
断开指定连接句柄的BLE连接。可以是从中央设备发起，也可是由外围设备发起。

* 注意事项：
   成功后，将触发_IRQ_PERIPHERAL_DISCONNECT 或者 _IRQ_CENTRAL_DISCONNECT事件

* 函数原型：
> BLE.gap_disconnect(conn_handle)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|

* 返回值:
False: 该连接句柄未连接；
True: 表示调用成功；

* 示例：
```Python
import bluetooth
ble = bluetooth.BLE()
...
ble.gap_disconnect(handle)
```


## gatts_register_services - 注册GATT服务

一个 GATT 服务器有一组注册服务。每个服务可能包含一个或多个特性，每个特性都有一个值。特征还可以包含描述符，描述符本身也具有值。
这些值存储在本地，并由服务注册期间生成的“值句柄”访问。它们也可以从远程客户端设备读取或写入。此外，服务器可以通过连接句柄主动向连接的客户端“通知”特征。
在大多数情况下，外围设备充当GATT 服务器。

* 函数功能:
使用指定的服务配置服务器

* 注意事项：
   在注册服务之前必须停BLE广播。

   每个服务都是一个两个元素的元组，其中一个是UUID， 另一个是特征值的列表。
   每个特征值都是一个两个或三个元素的元组，包含一个UUID, 一个标示值，另一个可选的描述符列表；
   每个描述符都一个两个元素的元组，包含一个UUID, 一个标示值。
   这些标示值是由下面定义的标示按位或的组合，他们设置了特征或者描述符的行为及安全和隐私要求。

```Python
 from micropython import const
_FLAG_BROADCAST = const(0x0001)
_FLAG_READ = const(0x0002)
_FLAG_WRITE_NO_RESPONSE = const(0x0004)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)
_FLAG_INDICATE = const(0x0020)
_FLAG_AUTHENTICATED_SIGNED_WRITE = const(0x0040)

_FLAG_AUX_WRITE = const(0x0100)
_FLAG_READ_ENCRYPTED = const(0x0200)
_FLAG_READ_AUTHENTICATED = const(0x0400)
_FLAG_READ_AUTHORIZED = const(0x0800)
_FLAG_WRITE_ENCRYPTED = const(0x1000)
_FLAG_WRITE_AUTHENTICATED = const(0x2000)
_FLAG_WRITE_AUTHORIZED = const(0x4000)
```

* 函数原型：
> BLE.gatts_register_services( services_definition  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|services_definition|列表|是|服务列表|

* 返回值:
一个元组列表, 每个元组代表一个服务，元组中的每个元素对应一个值句柄，特征和描述符句柄按照定义的顺序平展为同一个元组。
以下示例注册了两个服务（HR 和 UART），这三个值句柄（hr, tx, rx）可用于gatts_read, gatts_write, gatts_notify和gatts_indicate。

* 示例：
```Python
HR_UUID = bluetooth.UUID(0x180D)
HR_CHAR = (bluetooth.UUID(0x2A37), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
HR_SERVICE = (HR_UUID, (HR_CHAR,),)
UART_UUID = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
UART_TX = (bluetooth.UUID('6E400003-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
UART_RX = (bluetooth.UUID('6E400002-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_WRITE,)
UART_SERVICE = (UART_UUID, (UART_TX, UART_RX,),)
SERVICES = (HR_SERVICE, UART_SERVICE,)
( (hr,), (tx, rx,), ) = bt.gatts_register_services(SERVICES)
```

## gatts_read - 读取本地值
* 函数功能:
读取本地GATT服务中的特征值

* 注意事项：
   这个值可能是通过本地gatts_write写入的，也可能是通过远程客户端写入的。

* 函数原型：
> BLE.gatts_read(value_handle)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|value_handle|整型|是|值句柄|

* 返回值:
无

* 示例：
无

## gatts_write - 写入本地值
* 函数功能:
写入本地GATT服务中的特征值

* 注意事项：
   写入后，客户端可以读取该值。

* 函数原型：
> BLE.gatts_write( value_handle , data , send_update=False  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|value_handle|整型|是|值句柄|
|data|string|是|写入数据|
|send_update|布尔型|否|为True时，任何订阅此值的此客户端都将收到该特征值写入的通知|

* 返回值:
无

* 示例：
无

## gatts_notify - 发送通知请求
* 函数功能:
向连接的客户端发送通知请求

* 注意事项：
   无论客户端是否订阅了此特征值，都会发送通知。

* 函数原型：
> BLE.gatts_notify( conn_handle , value_handle , data=None )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|value_handle|整型|是|值句柄|
|data|string|否|如果data不为None,则将该值发送给客户端，本地值不会被修改。如果data为None,则将发送当前本地值给客户端|

* 返回值:
无

* 示例：
无

## gatts_indicate - 发送指示请求
* 函数功能:
向连接的客户端发送指示请求

* 注意事项：
   无论客户端是否订阅了此特征值，都会发送通知。
   在确认（或失败，如超时）时，将触发_IRQ_GATTS_INDICATE_DONE 事件

* 函数原型：
> BLE.gatts_indicate( conn_handle , value_handle  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|value_handle|整型|是|值句柄|

* 返回值:
无

* 示例：
无

## gatts_set_buffer - 设置缓冲区
* 函数功能:
设置值的内部缓冲区大小（与Byte为单位），默认值是20.

* 注意事项：
   无

* 函数原型：
> BLE.gatts_set_buffer ( value_handle , len , append = False)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|value_handle|整型|是|值句柄|
|len|整型|是|缓冲区大小|
|append|布尔值|否|为True时，将使所有远程写入追加到当前值，而不是覆盖当前值，执行gatts_read读取值后该值清零|

* 返回值:
无

* 示例：
无

## gattc_discover_services - 查询服务

GATT 客户端（Client）可发现和读取/写入远程GATT服务上的特征。
一般情况下，Central 角色充当GATT 客户端。

* 函数功能:
  查询已连接GATT服务器的服务

* 注意事项：
  每发现一个服务，都将触发 _IRQ_GATTC_SERVICE_RESULT 事件，服务操作完毕后，将触发_IRQ_GATTC_SERVICE_DONE 事件。

* 函数原型：
> BLE.gattc_discover_services( conn_handle , uuid=None  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|uuid|string|否|指定服务的uuid, 当uuid不为None时，仅查询该服务|


* 返回值:
无

* 示例：
无


## gattc_discover_characteristics - 查询特征
* 函数功能:
查询连接的服务器以获取指定范围内的特征

* 注意事项：
每发现一个特征，都将触发 _IRQ_GATTC_CHARACTERISTIC_RESULT 事件，发现特征操作完毕后，将触发 _IRQ_GATTC_CHARACTERISTIC_DONE 事件。

* 函数原型：
> BLE.gattc_discover_characteristics( conn_handle , start_handle , end_handle , uuid=None  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|start_handle|整型|是|起始的特征句柄|
|end_handle|整型|是|结束的特征句柄|
|uuid|string|否|指定特征的uuid, 当uuid不为None时，仅查询该特征|


* 返回值:
无

* 示例：
无

## gattc_discover_descriptors - 查询描述符
* 函数功能:
向连接的服务器查询指定范围内的描述符

* 注意事项：
每发现一个描述符，都将触发 _IRQ_GATTC_DESCRIPTOR_RESULT 事件，发现描述符操作结束后，将触发 _IRQ_GATTC_DESCRIPTOR_DONE 事件。

* 函数原型：
> BLE.gattc_discover_descriptors( conn_handle , start_handle , end_handle )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|start_handle|整型|是|起始的特征句柄|
|end_handle|整型|是|结束的特征句柄|

* 返回值:
无

* 示例：
无

## gattc_read - GATT远程读操作
* 函数功能:
向连接的服务器发出远程读取

* 注意事项：
当值可用时，将触发 _IRQ_GATTC_READ_RESULT 事件，里面带有读取的结果。此外，读取完毕将触发_IRQ_GATTC_READ_DONE 事件。

* 函数原型：
> BLE.gattc_read( conn_handle , value_handle  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|value_handle|整型|是|要读取的特征的句柄|

* 返回值:
无

* 示例：
无

## gattc_write - GATT远程写操作
* 函数功能:
向连接的服务器发出远程写

* 注意事项：
如果收到来自远程服务器的响应，将触发 _IRQ_GATTC_WRITE_DONE 事件。

* 函数原型：
> BLE.gattc_write( conn_handle , value_handle , data , mode=0  )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|
|value_handle|整型|是|要写的特征的句柄|
|mode|整型|是|mode=0（默认值），是无响应写入，对方不返回确认，也不会触发任何事件； mode=1是 write-with-response，远程服务器发送它收到数据的响应/确认|

* 返回值:
无

* 示例：
无

## gattc_exchange_mtu - 交换MTU
* 函数功能:
触发MTU交换流程，使用之前config的MTU值。

* 注意事项：
当MTU交换完成后，将触发 _IRQ_MTU_EXCHANGED 事件。
MTU交换通常由Central 角色设备发起。有些底层协议栈不支持MTU交换功能。

* 函数原型：
> BLE.gattc_exchange_mtu( conn_handle )

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|conn_handle|整型|是|连接句柄|

* 返回值:
无

* 示例：
无

# bluetooth.UUID
* 类功能：
  创建具有指定值的UUID实例。

* 注意事项：
   UUID可以是一个16位整数，例如 0x2908; 也可以是128位的UUID字符，例如’6E400001-B5A3-F393-E0A9-E50E24DCCA9E‘

* 示例：
```Python
HR_UUID = bluetooth.UUID(0x180D)
HR_CHAR = (bluetooth.UUID(0x2A37), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
HR_SERVICE = (HR_UUID, (HR_CHAR,),)
UART_UUID = bluetooth.UUID('6E400001-B5A3-F393-E0A9-E50E24DCCA9E')
UART_TX = (bluetooth.UUID('6E400003-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY,)
UART_RX = (bluetooth.UUID('6E400002-B5A3-F393-E0A9-E50E24DCCA9E'), bluetooth.FLAG_WRITE,)
UART_SERVICE = (UART_UUID, (UART_TX, UART_RX,),)
SERVICES = (HR_SERVICE, UART_SERVICE,)
( (hr,), (tx, rx,), ) = bt.gatts_register_services(SERVICES)
```
