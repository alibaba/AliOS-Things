# ucollections - 集合和容器类型

* 模块功能:  
ucollections 模块用于创建一个新的容器类型，用于保存各种对象。

## namedtuple - 创建一个新namedtuple容器类型

* 函数原型：
创建一个具有特定名称和一组字段的新namedtuple容器类型，namedtuple是元组的子类，允许通过索引来访问它的字段。

* 函数原型：
> nt = ucollections.namedtuple(name, fields)

* 参数说明：

|参数|类型|必选参数？|说明|
| :----- | :----- |:-:| :----- |
| name   | str   | 是 | 新创建容器的类型名称         |
| fields | tuple | 是 | 新创建容器类型包含子类型的字段 |

* 示例：

```Pyhton
>>> nt = ucollections.namedtuple("mytuple", ("id", "name"))
>>> t1 = nt(1, "name1")
>>> t2 = nt(2, "name2")
>>> print(t1.name)
name1
```

## deque - 创建deque双向队列

* 函数原型：
> dq = ucollections.deque(iterable, maxlen, [flag])

* 参数说明：

|参数|类型|必选参数？|说明|
| :---- | :----- | :-:|:---- |
| iterable | tuple | 是 |必须是空元组  |
| maxlen | int | 是 |队列允许的最大长度 |
| flag | int | 否 | 默认为0：不检查队列是否溢出，达到最大长度时继续增加会丢弃之前的值；1：当队列达到最大设定长度会抛出IndexError: full的exception |

* 返回值  
deque对象



# append - deque队列插入数据

> ​deque.append(data)

* 参数

|参数|类型|必选参数？|说明|
| ---- | ------------ | :-: | -------------------- |
| data | 基本数据类型 | 是 | 需要添加到队列的数据 |

* 返回值  
无

## popleft - 从deque的左侧移除并返回移除的数据

> ​	deque.popleft()

如果deque为空，会引起索引错误

* 返回值  
返回移除的数据



* 示例：

```python
from ucollections import deque

dq = deque((),5)
dq.append(1)
dq.append(["abc"])
dq.append("aef")

dq.popleft()  # 1
dq.popleft()  # ["abc"]
dq.popleft()  # aef
```
