# timer - TIMER外设驱动库

* 类功能：  
TIMER硬件定时器功能。


## TIMER - 创建Timer对象
* 函数原型：
> timerObj = TIMER(ID)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|ID|Number|是|硬件定时器ID设置，ESP32内置4个硬件定时器，ID设置范围0~3|

<br>

* 返回值：  
TIMER对象成功，返回TIMER对象；TIMER对象创建失败，返回None

* 示例代码： 

```python
from driver import TIMER
timerObj = TIMER(0)
```


## open - 打开硬件定时器

* 函数功能：  
打开TIMER硬件定时器功能，并启动定时器

* 函数原型：
> TIMER.open(period, mode, function callback)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|period|Number|是|硬件定时器时间设置，单位ms，即硬件定时器启动后该设定时候到时呼叫callback函数|
|mode|String|是|定时模式，配置该定时器只工作一次还是循环工作|
|function callback|Function|是|定时器到时间后触发的执行函数|

<br>

TIMER中定时属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|ONE_SHOT|Number|0|否|配置定时器只工作一次|
|PERIODIC|Number|0|否|配置定时器循环工作|

<br>

* 返回值：  
打开TIMER设备成功返回0；打开TIMER设备失败返回失败错误码

* 示例： 

```python
from driver import TIMER

timerObj = TIMER(0)
ret = timerObj.open(mode=timerObj.ONE_SHOT, period=5000, callback=lambda t:print('beep'))
```

* 输出：

```
#5s 后控制台输出
beep
```

## close - 关闭硬件定时器

* 函数功能：  
关闭硬件定时器

* 注意事项：  
需确保要关闭的TIMER处于open状态

* 函数原型：
> TIMER.close()

* 参数说明：  
无

<br>

* 返回值：  
关闭TIMER设备成功返回0；关闭TIMER设备失败返回失败错误码

* 示例： 

```python
from driver import TIMER

timerObj = TIMER(0)
timerObj.open(mode=timerObj.ONE_SHOT, period=5000, callback=lambda t:print('beep'))
timerObj.close()
```

* 输出：

```
0
```

## start - 硬件定时器开始计时

* 函数功能：  
打开硬件定时器，并开始计时

* 注意事项：  
需确保此TIMER处于open状态

* 函数原型：
> TIMER.start()

* 参数说明：
无

<br>

* 返回值：  
打开TIMER计时成功返回0；否则返回错误码；

* 示例： 

```python
from driver import TIMER
import time

timerObj = TIMER(0)
timerObj.open(mode=timerObj.ONE_SHOT, period=5000, callback=lambda t:print('beep'))
timerObj.stop()
time.sleep(2)
timerObj.start()
time.sleep(2)
timerObj.stop()
timerObj.close()
```

* 输出：

```
beep
```

<br>

## stop - 硬件定时器停止计时

* 函数功能：  
关闭硬件定时器，并停止计时

* 注意事项：  
需确保此TIMER处于open状态

* 函数原型：
> TIMER.stop()

* 参数说明：
无

<br>

* 返回值：  
关闭TIMER计时成功返回0；否则返回错误码；

* 示例： 

```python
from driver import TIMER
import time

timerObj = TIMER(0)
timerObj.open(mode=timerObj.ONE_SHOT, period=5000, callback=lambda t:print('beep'))
timerObj.stop()
```

<br>

## period - 设置定时器定时时间

* 函数功能：  
设置定时器定时时间，并重新开始计时

* 注意事项：  
需确保此TIMER处于open状态

* 函数原型：
> TIMER.period(period)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|period|Number|是|硬件定时器时间设置，单位ms，即硬件定时器启动后该设定时候到时呼叫callback函数|

<br>

* 返回值：  
设置定时器成功返回0；否则返回错误码；

* 示例： 

```python
from driver import TIMER
import time

timerObj = TIMER(0)
timerObj.open(mode=timerObj.PERIODIC, period=5000, callback=lambda t:print('beep'))
timerObj.stop()
timerObj.period(3000)
```

* 输出：

```
# 设置后 beep 由 5S 时间间隔输出变为 3S 时间间隔输出
beep
```

<br>

## reload - 重启定时器

* 函数功能：  
重启TIMER定时器，并根据之前的设置重新开始计时

* 注意事项：  
需确保此TIMER处于open状态

* 函数原型：
> TIMER.reload()

* 参数说明：
无

<br>

* 返回值：  
设置定时器成功返回0；否则返回错误码；

* 示例： 

```python
from driver import TIMER
import time

timerObj = TIMER(0)
timerObj.open(mode=timerObj.PERIODIC, period=5000, callback=lambda t:print('beep'))
timerObj.stop()
timerObj.reload()
```

* 输出：

```
beep
```

## 使用实例
* 代码：

```python
import time
from driver import TIMER

def oneshot_callback(args):
    print('****  oneshot_callback  ****')

tim1 = TIMER(0)
tim1.open(mode=tim1.ONE_SHOT, period=5000, callback=oneshot_callback)

i = 1
while (i<5):
    print('blink')
    time.sleep(3)
    i=i+1

tim1.close()

```

* 输出：

```python
blink
blink
****  oneshot_callback  ****
blink
blink
```


<br>

