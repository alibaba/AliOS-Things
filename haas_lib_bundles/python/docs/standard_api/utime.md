# utime - 时间相关功能

* 模块功能:  
utime 模块用于获取当前时间和日期、测量时间间隔和延迟。

## localtime - 格式化时间戳

* 函数原型：
> utime.localtime([secs])

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|secs|int|否| 要转换的秒数 |

* 返回值：    
该函数用来将一个以秒表示的时间转换为包含年、月、日、时、分、秒、星期、一年中第几天的元组，如果没有给定参数sec，则获取当前时值。返回值形式如下：

`(year, month, mday, hour, minute, second, weekday, yearday)`

* `year` ：年份，int型
* `month` ：月份，1~12，int型
* `mday` ：日，当月几号，1~31，int型
* `hour` ：小时，0~23，int型
* `minute` ：分钟，0~59，int型
* `second` ：秒，0~59，int型
* `weekday` ：星期几，周一到周日是0~6，int型
* `yearday` ：一年中的第多少天，int型

* 示例：

```python
>>> utime.localtime()
(2021, 11, 22, 16, 52, 34, 0, 326)
>>> utime.localtime(138930193)
(2004, 5, 26, 23, 43, 13, 2, 147)
```

## mktime - 反向格式化时间戳

* 函数原型：
> utime.mktime(date)

* 函数功能：  
该函数作用与locatime()相反，它将一个存放在元组中的时间转换为以秒计的时间戳。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|secs|int|否| 要转换的秒数 |

* 返回值：    
返回date对应的秒数

* 示例：

```python
>>> date = (2021, 11, 2, 14, 54, 42, 1, 0)
>>> utime.mktime(date)
689180082
```

## sleep - 秒级别休眠

* 函数原型：
> utime.sleep(seconds)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|seconds|int|是| 休眠的目标秒数 |

注意：调用sleep()会导致程序处于阻塞状态。

## sleep_ms - 毫秒级别休眠

* 函数原型：
> utime.sleep_ms(ms)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|ms|int|是| 休眠的目标毫秒数 |

注意：调用sleep_ms()会导致程序处于阻塞状态。

## sleep_us - 微妙级别休眠

* 函数原型：
> utime.sleep_us(us)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|us|int|是| 休眠的目标微妙数 |

注意：调用sleep_us()会导致程序处于阻塞状态。


## ticks_ms - 获取系统毫秒计数器

* 函数原型：
> utime.ticks_ms()	

* 函数说明：  
返回系统的毫秒计数器，在某些值后会重新计数(未指定)。计数值本身无特定意义，只适合用在 `ticks_diff()`函数中。

## ticks_us - 获取系统微妙计数器

* 函数原型：
> utime.ticks_us()

和`ticks_ms()`类似，只是返回微秒计数器。

## ticks_cpu - 高精度计数器

* 函数原型：
> utime.ticks_cpu()

* 函数功能：  
和 ticks_ms/ticks_us 类似，具有更高精度 (使用 CPU 时钟)。

## ticks_diff - 计算时间差

* 函数原型：
> utime.ticks_diff(new_ticks, early_ticks)

* 函数功能：  
计算两次调用` ticks_ms()`， `ticks_us()`，或 `ticks_cpu()`之间的时间差。因为这些函数的计数值可能会回环，不能直接相减，需要使用 ticks_diff() 函数。early_ticks时间需要在new_ticks时间之前，否则结果无法确定。这个函数不能计算很长的时间的差值(因为 ticks_*()函数会回绕，回绕周期一般不长)。通常用法是在带超时的轮询事件中调用。

* 示例：

```python
cnt = 0
t1 = utime.ticks_us()
while (utime.ticks_diff(utime.ticks_us(), t1) < 100):
    cnt += 1
```

## time - 返回自纪元以来的秒数

* 函数原型：
> utime.time()	

* 返回值：  
返回自纪元以来的秒数。如果需要更高的精度，请使用 `ticks_ms()`和`ticks_us()`函数，如果需要日历时间，则直接调用`localtime()`。
