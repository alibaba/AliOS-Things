# WDT - WDT(WatchDog Timer)驱动库

* 类功能：  
WDT(WatchDog Timer)设备驱动。

## WDT - 创建WDT对象

* 函数原型：
> WDT.WDT()

* 参数说明：  
无。

* 返回值：  
WDT对象创建成功，返回WDT对象；WDT对象创建失败，抛出`ENOMEN`异常。

* 示例代码

```python
from driver import WDT
WDT = WDT()
```

## open - 打开WDT设备

* 函数功能  
打开WDT设备节点，并根据节点的配置信息初始化WDT。

* 函数原型：
> WDT.open(node)

* 参数说明

| 属性 | 类型 | 必填 | 描述 |
| --- | --- | --- | --- |
| node | String | 是 | 待操作的WDT设备节点，定义在board.json文件中。 |

board.json中的WDT类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|timeout|int|3|否|配置WDT的超时时间（秒），默认3秒，超时没有喂狗会导致系统重启|

* 返回值：  
成功：0；失败：故障码。

* 示例代码

```python
from driver import WDT
WDT = WDT()
ret = WDT.open("wdt0")
```

## close - 关闭WDT设备

* 函数原型：
> WDT.close()

* 参数说明：  
无。

* 返回值：  
成功：0；失败：故障码。

* 示例代码

```python
from driver import WDT
WDT = WDT()
ret = WDT.open("WDT0")
ret = WDT.close()
```

## feed - 喂狗

* 函数原型：
> WDT.feed()

* 参数说明：  
无

* 返回值：  
   成功：0；失败：故障码。

* 示例代码

```python
from driver import WDT
import time
WDT = WDT()
ret = WDT.open("wdt0")
i = 0
while (i < 10):
    WDT.feed()
    time.sleep(1)
    i += 1
    print("feed the watchdog ", i)
WDT.close()
```
