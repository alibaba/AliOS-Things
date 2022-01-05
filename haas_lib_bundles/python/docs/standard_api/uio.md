# uio - 输入输出流

* 模块功能:  
uio 模块包含其他类型的stream（类文件）对象和辅助函数。

## open - 打开文件
> fd = uio.open(name, mode=’r’, **kwarg)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|name|stirng|是| 文件名 |
|mode|int|是| 打开模式 |
|**kwarg|-|否| 可变长参数列表 |

* 打开模式说明如下：
  - r  只读模式打开文件
  - w  写入模式打开文件，每次写入会覆盖上次写入数据
  - a  只写追加模式打开文件，可连续写入文件数据而不是覆盖数据

* 返回值  
返回所打开文件的文件描述符，打开文件之后就可以使用如下函数来对文件进行操作：
  * read()  - 读操作
  * write() - 写操作
  * seek()  - 定位到特定位置
  * flush() - 将对文件的修改刷新到到flash中
  * getvalue() - 获取存储数据的底层缓冲区的当前内容

## close - 关闭打开的文件

* 函数原型：
> fd.close()

输入输出流有如下的类型：
## uio.FileIO(...) - 用二进制模式打开的文件  
用open(name, "rb")打开的文件对象属于FileIO类对象

## uio.TextIOWrapper(...) - 用文本模式打开的文件
用open(name, "rt")打开的文件对象属于TextIOWrapper类对象

## uio.StringIO([string]) - 字符串类型的输入输出流
字符串IO用于文本模式的I/O（类似于用“t”修饰符打开的普通文件）。
* 示例：
```python
>>> import uio
>>>
>>> f = uio.StringIO()
>>> f.write("Hello")
5
>>> f.write("World")
5
>>> print(f.getvalue())
HelloWorld
```

## uio.BytesIO - 字节类型的输入输出流
字节IO 用于二进制模式的I/O（类似于用“b”修饰符打开的普通文件）。

