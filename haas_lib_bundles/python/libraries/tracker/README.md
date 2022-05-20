# 黑白循迹传感器

## 一、产品简介
&emsp;&emsp;
黑白循迹传感器具有一对红外线发射与接收管，发射管不断发射出红外线，当发射出的红外线没有被反射回来或给反射回来强度不大时，红外接收管一直处于关断状态，此时模块的输出端为低电平；当发射出的红外线被检测物阻挡发射回来强度足够大的时候，红外接收管达到饱和状态，模块输出高电平。黑白循迹传感器可以广泛应用于机器人避障、避障小车、流水线计数及黑白线循迹等众多场合。
<br>

<div align="center">
<img src=./../../docs/images/tracker.png  width=35%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：地<br>
* OUT：输出信号
<br>

## 二、技术参数
* 工作电压：3.3V-5V<br>
* 有效检测距离：1mm-25mm<br>
* 输出电平：TTL<br>
* 工作温度：-20°C~85°C<br>
* 电路板尺寸：35mm*10mm<br>

## 三、软件接口

黑白循迹传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/tracker)
<br>

### TRACKER(gpioObj) - 创建黑白循迹传感器对象
<br>

* 函数原型

> trackerDev = TRACKER(gpioObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|

* 返回值

> TRACKER对象成功，返回TRACKER对象；TRACKER对象创建失败，抛出Exception

* 示例代码
```python
from driver import GPIO     # 驱动库
import tracker

gpioObj = GPIO()
gpioObj.open("tracker")    # 按照board.json中名为"tracker"的设备节点的配置参数
print("GPIO inited!")

trackerDev= tracker.TRACKER(gpioObj)
```

* 输出
```log
GPIO inited!
```
</br>

### objectDetection() - 获取黑白循迹传感器的状态
<br>

* 函数功能：

> 获取黑白循迹传感器的状态

* 函数原型：

> TRACKER.getTrackerStatus()

* 参数说明：
> 无

* 返回值：
> 成功返回黑白循迹传感器的状态，0 - 白线 1 - 黑线；失败抛出Exception

* 示例：

```python
from driver import GPIO     # 驱动库
import tracker

gpioObj = GPIO()
gpioObj.open("tracker")    # 按照board.json中名为"tracker"的设备节点的配置参数
print("GPIO inited!")

trackerDev= tracker.TRACKER(gpioObj)
status = trackerDev.getTrackerStatus()
print("Tracker status is :%d level"%(status))
```

* 输出
```log
GPIO inited!
Tracker status is :0 level
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为TRACKER的GPIO类型的对象。在Python脚本中获取黑白循迹传感器的状态值并打印在日志中。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
   "tracker":{
        "type": "GPIO",
        "port": 0,
        "dir": "input",
        "pull": "opendrain"
    }
  }
}
```

```python
from driver import GPIO     # 驱动库
import tracker
import utime

gpioObj = GPIO()
gpioObj.open("tracker")    # 按照board.json中名为"tracker"的设备节点的配置参数
print("GPIO inited!")

trackerDev= tracker.TRACKER(gpioObj)
while True:
    status = trackerDev.getTrackerStatus()
    print("Tracker status is :%d level"%(status))
    utime.sleep(1)
```

* 输出
```python
GPIO inited!
Tracker status is :0 level
Tracker status is :0 level
Tracker status is :1 level
Tracker status is :1 level
Tracker status is :1 level
Tracker status is :1 level
Tracker status is :0 level
Tracker status is :0 level
Tracker status is :0 level
```

<br>

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.781f457bZ2c4W3&id=40700980557&ns=1&abbucket=6)
