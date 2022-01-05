# UART - UART外设驱动库

* 类功能：  
UART通用异步收发传输器驱动。

## UART - 创建UART对象

* 函数原型：
> UART.UART()

* 参数说明：  
无。

* 返回值：  
UART对象创建成功，返回UART对象；UART对象创建失败，抛出`ENOMEN`异常。

* 示例代码：

```python
from driver import UART
uart = UART()
```

## open - 打开UART设备

* 函数功能：  
打开UART设备节点，并根据节点的配置信息初始化UART。

* 函数原型：
> UART.open(node)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| node | string | 是 | 待操作的UART设备节点，定义在board.json文件中。 |

board.json中的UART类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|port|int|2|是|配置端口值，这里跟芯片 datasheet上 的端口对应。目前nodemcu32s只支持2号UART。|
|dataWidth|int|5、6、7、8|否|串口数据宽度值，默认为 8(bits)|
|baudRate|int|9600、115200 等|否|串口波特率，默认为 115200|
|stopBits|int|1、2|否|串口停止位，默认为 1|
|flowControl|string|disable、cts、rts、rtscts|否|流控设置，默认 disable|
|parity|string|none、odd、even|否|奇偶校验设置，默认 none|

* 返回值：  
成功：0；失败：故障码。

* 示例代码：

```python
from driver import UART
uart = UART()
ret = uart.open("serial2")
```

## close - 关闭UART设备

* 函数原型：
> UART.close()

* 参数说明：  
无。

* 返回值：  
成功：0；失败：故障码。

* 示例代码：

```python
from driver import UART
uart = UART()
ret = uart.open("serial2")
ret = uart.close()
```

## read - 接收数据

* 函数功能：  
从UART接收指定字节数的数据。

* 函数原型：
> UART.read(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| buf | bytearray | 是 | 读取数据的存储数组，期待读取的字节数为`buf`的长度。 |

* 返回值：  
成功：0或者正整数，真实读到的数据字节数；失败：负整数。

* 示例代码：

```python
from driver import UART
uart = UART()
ret = uart.open("serial2")
buf = bytearray(256)
ret = uart.read(buf)
```

## write - 发送数据

* 函数功能：  
从UART发送指定字节数的数据。

* 函数原型：
> UART.write(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| buf | bytearray | 是 | 发送数据的存储数组，期待发送的字节数为`buf`的长度。 |

* 返回值：  
成功：0或者正整数，真实发送的数据字节数；失败：负整数。

* 示例代码：

```python
from driver import UART
uart = UART()
ret = uart.open("serial2")
buf = bytearray([0x01, 0x02, 0x03, 0x04])
ret = uart.write(buf)
```
