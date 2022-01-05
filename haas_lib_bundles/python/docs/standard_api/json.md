# ujson - JSON编码和解码

* 模块功能:  
ujson 模块实现在Python数据对象和JSON数据格式之间进行转换的功能。

## dump - 将数据对象转化成JSON格式的字符串

* 函数原型：
> ujson.dump(obj, stream)

* 函数功能：  
将 `obj` 数据对象转化成JSON格式的字符串，将其写入到给定的 `stream` 中。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|obj|-|是| 要转换的数据对象 |
|stream|-|是| 流式对象 |

* 示例：
```python
>>> f = uio.StringIO()
>>> ujson.dump({"a":2}, f)
>>> f.getvalue()
'{"a": 2}'
```

## dumps - 将dict类型的数据转换成str

* 函数原型：
> ujson.dumps(dict)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|dict|字典|是| 要转换的字典对象 |

* 返回值：
`dict`转换后的json格式的字符串

## load - 解析给定的数据stream

* 函数原型：
> ujson.load(stream)

* 函数功能：
解析给定的数据 `stream`，将其解释为JSON字符串并反序列化成Python对象。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|stream|-|是| 流式对象 |

## loads - 解析JSON字符串并返回obj对象

* 函数原型：
> ujson.loads(str)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|str|string|是| JSON格式的字串 |

* 返回值：
JSON格式的对象

* 示例：

```python
>>> ujson.loads('{"a": 2}')
{'a': 2}
```
