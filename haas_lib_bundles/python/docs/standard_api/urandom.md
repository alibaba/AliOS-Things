# urandom - 生成随机数

* 模块功能:  
urandom 模块提供了生成随机数的功能。

## choice - 随机生成对象中的元素

* 函数原型：
> urandom.choice(obj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|obj|string|是| 随机元素限定范围字串 |

* 返回值：  
随机生成的元素

* 示例：

``` Python
>>> urandom.choice("HaaS Python - HaaS JavaScript - HaaS UI - AliOS Things")
'H'
```

## 随机产生一个在k bits范围内的十进制数

* 函数原型：
> urandom.getrandbits(k)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|k|int|是| 最大bit数|

* 返回值：  
生成的随机数

* 示例：

``` Python
>>> urandom.getrandbits(3) # 返回值范围：000 - 111
6
>>> urandom.getrandbits(9) # 返回值范围：000000000 - 111111111
220
```

## 随机生成一个 start 到 end 之间的整数

* 函数原型：
> urandom.randint(start, end)

随机生成一个 start 到 end 之间的整数。

* 示例：

``` Python
>>> import urandom
>>> urandom.randint(1, 4)
4
>>> urandom.randint(1, 4)
2
```

## 随机生成一个 0 到 1 之间的浮点数

* 函数原型：
> urandom.random()

* 返回值：  
生成的随机浮点数

* 示例：

``` Python
>>> urandom.random()
0.6558424
```

## randrange - 随机生成 s 到 e 间并且递增为 step 的正整数

* 函数原型：
> urandom.randrange(s, e, step)

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|s|int|是| 随机数的最小值 |
|e|int|是| 随机数的最大值 |
|step|int|是| 随机数必须为step的整数倍 |

* 返回值：  
生成的随机浮点数

* 示例：

```
>>> urandom.randrange(0, 11, 3)
3
>>> urandom.randrange(0, 11, 3)
9
>>> urandom.randrange(0, 11, 13)
0
```

## seed - 指定随机数种子

* 函数原型：
> urandom.seed(sed)

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|sed|int|是| 该种子通常由其它随机数生成 |

* 示例：

```
>>> urandom.seed(urandom.randint(1, 10))  #指定随机数种子
>>> urandom.random()
0.2382998
```

## uniform 随机生成 s 到 e 范围内的浮点数

* 函数原型：
> urandom.uniform(s, e)

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|s|int|是| 随机数的最小值 |
|e|int|是| 随机数的最大值 |

* 示例：

```
>>> urandom.uniform(3, 8)
6.349555
>>> urandom.uniform(5, 10)
6.33503
```