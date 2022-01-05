# ubinascii - 二进制与ASCII转换

* 模块功能:  
ubinascii模块实现了二进制数据与各种ASCII编码之间的转换。

## a2b_base64 - 解码base64编码的数据

* 函数原型：
> ubinascii.a2b_base64(data)

* 注意事项：
在解码过程中会自动忽略输入中的无效字符

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|string|是|要转换的ascii字串|


* 返回值：
返回对应的bytes对象

```python
>>> ubinascii.a2b_base64('1234')
b'\xd7m\xf8'
```

## b2a_base64 - 以base64格式编码二进制数据

* 函数原型：
> ubinascii.b2a_base64(data)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|bytes|是|要转换的二进制格式的数据|

* 返回值：
转换后的bytes对象，后面跟换行符。

* 示例：

```python
>>> ubinascii.b2a_base64(b'123456')
b'MTIzNDU2\n'
```

## hexlify - 将二进制数据转换为十六进制字符串表示

* 函数原型：
> ubinascii.hexlify(data, [sep])

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|bytes|是|要转换的二进制格式的数据|
|sep|str|否|分隔符，单字符字串|

* 示例：

```
>>> import ubinascii
# 没有sep参数
>>> ubinascii.hexlify('\x11\x22\x33')
b'112233'
>>> ubinascii.hexlify('abcd')
b'61626364'
# 指定分隔符sep，它将用于分隔两个十六进制数
>>> ubinascii.hexlify('\x11\x22\x33', ':')
b'11:22:33'
>>> ubinascii.hexlify('\x11\x22\x33', ' ')
b'11 22 33'
```

## unhexlify - 将十六进制形式的字符串转换成二进制形式的字符串表示

* 函数原型：
> ubinascii.unhexlify(data)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|bytes|是|要转换的十六进制格式的数据|

* 示例：

```
>>> ubinascii.unhexlify('313233')
b'123'
```
