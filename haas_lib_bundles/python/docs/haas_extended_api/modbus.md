
# Modbus串行通信协议

* 模块功能：  
Modbus是一种串行通信协议，已经成为工业领域通信协议事实上的业界标准，并且现在是工业电子设备之间常用的连接方式。
Modbus允许多个 (大约240个) 设备连接在同一个网络上进行通信，举个例子，一个由测量温度和湿度的设备，并且将结果发送给计算机。在数据采集与监视控制系统（SCADA）中，Modbus通常用来连接监控计算机和远程终端控制系统（RTU）。

* 注意事项：  
Python 轻应用Modbus基于485串口实现的RTU模式能力，暂不支持ASCII/TCP模式。

## 常量说明
|常量|常亮数值|常量说明|
|:-|:-|:-|
|modbus.PARITY_NONE|0|串口不使用奇偶校验|
|modbus.PARITY_ODD|1|串口使用奇校验|
|modbus.PARITY_EVEN|2|串口使用偶校验|

|常量|常亮数值|常量说明|
|:-|:-|:-|
|MB_SUCCESS|0|成功|
|MB_MUTEX_ERROR|-1|互斥失败|
|MB_INVALID_SLAVE_ADDR|-2|无效的从地址|
|MB_INVALID_PARAM|-3|无效的参数|
|MB_RESPOND_EXCEPTION|-4|从机响应异常|
|MB_RESPOND_LENGTH_ERR|-5|从机响应长度错误|
|MB_RESPOND_FRAME_ERR|-6|从机响应帧太短或者CRC错误|
|MB_RESPOND_TIMEOUT|-7|从机响应超时|
|MB_CANNOT_GET_HANDLER|-8|分配Modbus句柄失败|
|MB_SLAVE_NO_RESPOND|-9|从机无响应|
|MB_FRAME_SEND_ERR|-10|帧发送错误|
|MB_SERIAL_INIT_FAILED|-11|串口初始化失败|
|MB_SERIAL_UNINIT_FAILED|-12|串口反初始化失败|
|MB_FUNCTION_CODE_NOT_SUPPORT|-13|串口使用偶校验|


## init - 初始化Modbus

* 函数功能：  
根据board.json中设备节点的配置初始化Modbus总线

* 函数原型：
> modbus.init(nodeName)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|mb1|string|是|board.json配置文件中Modbus设备节点的名称|

<br>

board.json中的UART类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|type|string|MODBUS|是|节点类型，表明当前节点为Modbus节点，固定设置为MODBUS|
|mode|int|0、1|是|Modbus物理通道类型，0表示串口，1表示TCP以太网。目前仅支持串口通道|
|port|int|0、1 等|否|Modbus采用串口通信时的串口号|
|baudrate|int|9600、115200等|否|Modbus采用串口通信时的串口波特率，默认为 9600|
|timeout|int|200, 1000等|否|Modbus采用串口通信时的超时参数，单位是毫秒(ms)，默认为200ms|
|parity|int|0、1、2|否|奇偶校验设置，默认 0|

<br>

* 返回值：  
打开Modbus设备成功返回0；打开UART设备失败返回失败错误码

* 示例： 

```python
# 请确保board.json中包含名为rfid的节点
'''
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "mb1": {
            "type": "MODBUS",
            "mode": 0,
            "port": 1,
            "baudrate": 9600,
            "priority": 0,
            "timeout": 200
        }
    }
}
'''

import modbus as mb

ret = mb.init("mb1")
print(ret)
```

* 输出：

```
0
```

## deinit - 关闭Modbus

* 函数功能：  
反初始化Modbus总线

* 注意事项：  
需确保要关闭的Modbus处于init状态

* 函数原型：
> modbus.deinit()

* 参数说明：  
无

<br>

* 返回值：  
反初始化Modbus设备成功返回0；反初始化Modbus设备失败返回失败错误码

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")

ret = md.deinit()
print(ret)
```

* 输出：

```
0
```

## writeHoldingRegister - 写单个保持寄存器

* 函数功能：  
向从机写单个保持寄存器

* 注意事项：  
需确保此UART处于open状态

* 函数原型：
> UART.writeHoldingRegister(slave_addr, register_addr, register_value, timeout)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|register_addr|int|是|写寄存器的地址|
|register_value|int|是|写寄存器的数据|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的4元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ resp_addr: 响应地址
	+ resp_value: 响应数据
	+ exception_code: 响应异常代码

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
ret = mb.writeHoldingRegister(0, 0, 0x00FF, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 0, 0x00FF, 0)
```

<br>

## writeMultipleHoldingRegisters - 写多个保持寄存器

* 函数功能：  
向从机多个保持寄存器中写入数据

* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：
> modbus.writeMultipleHoldingRegisters(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|register_addr|int|是|待写寄存器的起始地址|
|reg_quantity|int|是|待写寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|写寄存器的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|


* 返回值：  
返回一个tuple类型的4元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ resp_addr: 响应地址
	+ resp_quantity: 真实完成寄存器操作的数量
	+ exception_code: 响应异常代码

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray([0xFF, 0x00])
ret = mb.writeMultipleHoldingRegisters(0, 0, 1, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 0, 1, 0)
```

## writeCoil - 写单个线圈

* 函数功能：  
向从机某个线圈中写入数据


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.writeCoil(slave_addr, coil_addr, coil_value, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|coil_addr|int|是|写线圈的地址|
|coil_value|int|是|写线圈的数据|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的4元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ resp_addr: 响应地址
	+ resp_value: 响应数据
	+ exception_code: 响应异常代码

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray([0xFF, 0x00])
ret = mb.writeCoil(0, 0, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 0, 0x00FF, 0)
```

## writeMultipleCoils - 写多个线圈

* 函数功能：  
向从机多个线圈中写入数据


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.writeMultipleCoils(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|start_addr|int|是|写线圈的地址|
|reg_quantity|int|是|待写寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|写寄存器的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的4元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ resp_addr: 响应地址
	+ resp_quantity: 真实完成寄存器操作的数量
	+ exception_code: 响应异常代码

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray([0xFF])
ret = mb.writeMultipleCoils(0, 0, 8, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 0, 8, 0)
```

## readHoldingRegisters - 读多个保持寄存器值

* 函数功能：  
向从机读取多个保持寄存器的值


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.readHoldingRegisters(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|start_addr|int|是|待读寄存器的起始地址|
|reg_quantity|int|是|待读寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|读寄存器获得的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ respond_count: 读取到数据的字节数

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray(2)
ret = mb.writeMultipleCoils(0, 0, 1, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 2)
```

## readInputRegisters - 读取多个输入寄存器值

* 函数功能：  
向从机读取多个输入寄存器的值


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.readInputRegisters(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|start_addr|int|是|待读寄存器的起始地址|
|reg_quantity|int|是|待读寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|读寄存器获得的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ respond_count: 读取到数据的字节数

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray(10)
ret = mb.readInputRegisters(0, 0, 5, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 10)
```

## readDiscreteInputs - 读取多个离散输入值

* 函数功能：  
向从机读取多个输入寄存器的值


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.readInputRegisters(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|start_addr|int|是|待读寄存器的起始地址|
|reg_quantity|int|是|待读寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|读寄存器获得的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ respond_count: 读取到数据的字节数

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray(10)
ret = mb.readInputRegisters(0, 0, 5, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 10)
```

## readCoils - 读取多个线圈值

* 函数功能：  
向从机读取多个线圈的值


* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.readCoils(slave_addr, start_addr, reg_quantity, data, timeout)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|请求的从机地址，0代表广播|
|start_addr|int|是|待读寄存器的起始地址|
|reg_quantity|int|是|待读寄存器的数量，表示操作多少个寄存器|
|data|bytearray|是|读寄存器获得的数据，每个寄存器包含高低两个字节，高位在前，低位在后|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ respond_count: 读取到数据的字节数

* 示例： 

```python
# 请确保board.json中包含名为mb1的节点

import modbus as mb

mb.init("mb1")
data = bytearray(1)
ret = mb.readInputRegisters(0, 0, 8, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 1)
```

## recv - 从机读取主机数据

* 函数功能：  
该函数仅供从机使用，用来接收主机发送的请求命令。接收的超时参数由 modbus.init 初始化时指定的 timeout 决定，默认为200毫秒。

* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.recv()

* 参数说明：
* 
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|slave_addr|int|是|从机地址|
|data|bytearray|是|响应数据，格式为： 响应代码 + 响应数据|
|timeout|int|是|请求超时时间，单位是毫秒（ms）|

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ length: 发送的响应帧的长度，包含CRC校验长度

* 示例： 

```python
# 请确保board.json中包含名为mb_slave的节点

import modbus as mb

mb.init("mb_slave")
data = bytearray(128)
ret = mb.send(0, data, 200)
print(ret)
md.deinit()

```

* 输出：

```
(0, 128)
```

## send - 从机发行响应数据到主机

* 函数功能：  
该函数仅供从机使用，用于发送响应数据给主机。接收的超时参数由 modbus.init 初始化时指定的 timeout 决定，默认为200毫秒。

* 注意事项：  
需确保此Modbus处于init状态

* 函数原型：  
> modbus.recv()

* 参数说明：
	空

* 返回值：  
返回一个tuple类型的2元组，元组中的条目格式为：
	+ status: 请求状态，0表示成功，其他表示失败，具体含义参考 STATUS
  	+ bytes: 接收到的数据，包含CRC校验数据

* 示例： 

```python
# 请确保board.json中包含名为mb_slave的节点

import modbus as mb

mb.init("mb_slave")
ret = mb.recv()
print(ret)
md.deinit()

```

* 输出：

```
(0, [x, x, x, x, ...])
```

<br>


