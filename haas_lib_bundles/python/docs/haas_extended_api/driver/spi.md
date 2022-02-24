# SPI - SPI Master驱动库

* 类功能：
SPI master设备驱动。

## SPI - 创建SPI对象

* 函数原型：
> SPI.SPI()

* 参数说明：：  
无。

* 返回值：  
SPI对象创建成功，返回SPI对象；SPI对象创建失败，抛出`ENOMEN`异常。

* 示例代码：

```python
from driver import SPI
spi = SPI()
```

## open - 打开SPI设备

* 函数功能：  
打开SPI设备节点，并根据节点的配置信息初始化SPI。

* 函数原型：
> SPI.open(node)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| node | String | 是 | 待操作的SPI设备节点，定义在board.json文件中。 |

board.json中的SPI类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|port|int|3|是|配置端口值，需要跟芯片datasheet上的端口对应。目前nodemcu32s只支持3号SPI。|
|mode|string|master、slave|否|配置SPI模式，默认master。目前nodemcu32s只支持master模式。|
|freq|int|3250000、6500000……|否|配置 SPI 总线频率，默认3250000。|

* 返回值：  
成功：0；失败：故障码。

* 示例代码：

```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "spi_bmp280": {
      "type": "SPI",
      "port": 3,
      "mode": "master",
      "freq": 2000000
    }
  },
  "debugLevel": "ERROR",
  "repl": "disable"
}
```
```python
from driver import SPI
spi = SPI()
ret = spi.open("spi_bmp280")
```

## close - 关闭SPI设备

* 函数原型：
> SPI.close()

* 参数说明：：  
无。

* 返回值：  
成功：0；失败：故障码。

* 示例代码：

```python
from driver import SPI
spi = SPI()
ret = spi.open("spi_bmp280")
ret = spi.close()
```

## read - 从SPI总线接收数据

* 函数原型：  
> SPI.read(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| buf | bytearray | 是 | 接收数据的存储数组，期待接收的字节数为`buf`的长度。 |

* 返回值：  
   - 0或正整数：真实接收到的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import SPI
spi = SPI()
ret = spi.open("spi_bmp280")
buf = bytearray([0xAA, 0xAA, 0xAA, 0xAA])
ret = spi.read(buf)
```

## write - 向SPI总线发送数据

* 函数原型：
> SPI.write(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| buf | bytearray | 是 | 发送数据的存储数组，期待发送的字节数为`buf`的长度。 |

* 返回值：  
   - 0或正整数：真实发送的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import SPI
spi = SPI()
ret = spi.open("spi_bmp280")
buf = bytearray([0xF4 & 0x7F, 0x02, 0x03, 0x04])
ret = spi.write(buf)
```

## readAfterWrite - 先发送后接收数据

* 函数原型：
> SPI.readAfterWrite(buf, data)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | ---
| buf | bytearray | 是 | 接收数据的存储数组，期待接收的字节数为`buf`的长度。 |
| data | int | 是 | 期待发送的数据，长度固定为1字节。 |

* 返回值：  
   - 0或正整数：真实接收到的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import SPI
spi = SPI()
ret = spi.open("spi_bmp280")
buf = bytearray(8)
ret = spi.readAfterWrite(buf, 0x0D)
```
