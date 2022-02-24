# I2C - I2C Master驱动库

* 类功能  
I2C master设备驱动。

## I2C - 创建I2C对象

* 函数原型：
> I2C.I2C()

* 参数说明：  
无。

* 返回值：  
I2C对象创建成功，返回I2C对象；I2C对象创建失败，抛出`ENOMEN`异常。

* 示例代码：

```python
from driver import I2C

i2c = I2C()
```
## open - 打开I2C设备

* 函数功能  
打开I2C设备节点，并根据节点的配置信息初始化I2C。

* 函数原型：
> I2C.open(node)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| node | String | 是 | 待操作的I2C设备节点，定义在`board.json`文件中。 |

`board.json`中的I2C类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|port|int|0|是|配置端口值，需要跟芯片datasheet上的端口对应。目前nodemcu32s只支持0号I2C。|
|addrWidth|int|7、10|否|配置I2C总线地址宽度，默认7。目前nodemcu32s只支持7位地址。|
|mode|string|master、slave|否|配置I2C模式，默认master。目前nodemcu32s只支持master模式。|
|freq|int|100000、300000……|否|配置 I2C总线频率，默认300000。|
|devAddr|int|224|否|配置I2C从设备地址，默认224。必须为十进制。|

* 返回值：  
   - 成功：0
   - 失败：故障码。

* 示例代码：
```python
# 假设I2C通道0外接了一个陀螺仪传感器-MPU6886，在board.json中将此节点命名为"mpu6886"，则board.json中的内容如下：
{
  "name": "esp32",
  "version": "1.0.0",
  "io": {
    "mpu6886": {
      "type": "I2C",
      "port": 0,
      "addrWidth": 7,
      "freq": 400000,
      "mode": "master",
      "devAddr": 68
    }
  },
  "debugLevel": "ERROR",
  "repl": "disable"
}
```

```python
from driver import I2C

i2c = I2C()
ret = i2c.open("mpu6886")
```
## close - 关闭I2C设备

* 函数原型：
> I2C.close()

* 参数说明：  
无。

* 返回值：  
   - 成功：0
   - 失败：故障码。

* 示例代码：

```python
from driver import I2C

i2c = I2C()
ret = i2c.open("mpu6886")
ret = i2c.close()
```
## read - 从I2C总线接收数据

* 函数原型：
> I2C.read(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| buf | bytearray | 是 | 接收数据的存储数组，期待接收的字节数为`buf`的长度。 |

* 返回值：  
   - 0或正整数：真实接收到的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import I2C
i2c = I2C()
ret = i2c.open("mpu6886")
buf = bytearray(8)
ret = i2c.read(buf)
```
## write - 向I2C总线发送数据

- 函数原型：
> I2C.write(buf)

* 参数说明：

| 属性 | 类型 | 必填 | 描述
| --- | --- | --- | --- |
| buf | bytearray | 是 | 发送数据的存储数组，期待发送的字节数为`buf`的长度。 |

* 返回值：  
   - 0或正整数：真实发送的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import I2C

i2c = I2C()
ret = i2c.open("mpu6886")
buf = bytearray([0x0D])
ret = i2c.write(buf)
```
## memRead - 从I2C设备指定地址读取一段数据

* 函数原型：
> I2C.memRead(buf, memaddr, addrsize)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| memaddr | int | 是 | 起始地址。 |
| buf | bytearray | 是 | 接收数据的存储数组，期待接收的字节数为`buf`的长度。 |
| addrsize | int | 是 | memaddr的长度，有效值为8或16，单位：bit |


* 返回值：  
   - 0或正整数：真实接收到的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import I2C

i2c = I2C()
ret = i2c.open("mpu6886")
buf = bytearray(8)
ret = i2c.memRead(buf, b'\x10',  8)
```


## memWrite - 向I2C设备指定地址写一段数据

* 函数原型：
> I2C.memWrite(buf, memaddr, addrsize)

* 参数说明：

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| memaddr | int | 是 | 起始地址。 |
| buf | bytearray | 是 | 待写数据的存储数组，需要写入的数据长度为buf的长度。 |
| addrsize | int | 是 | memaddr的长度，有效值为8或16，单位：bit  |


* 返回值：  
   - 0或正整数：真实接收到的数据字节数。
   - 负整数：故障码。

* 示例代码：

```python
from driver import I2C

i2c = I2C()
ret = i2c.open("mpu6886")
buf = bytearray([0x01],[0x02],[0x03])
ret = i2c.memWrite(buf, b'\0x10', 8)
```
