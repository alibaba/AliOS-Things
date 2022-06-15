# PAJ7620u2 手势识别传感器

## 一、产品简介
手势识别传感器PAJ7620u2是一款集成3D手势识别和运动跟踪为一体的交互式传感器，传感器可以在有效范围内识别手指的顺时针/逆时针转动方向和手指的运动方向等。它可以识别13种手势，包括向上移动，向下移动，向左移动，向右移动，向前移动，向后移动，顺时针方向，从左到右，从左到右等等手势姿态。

传感器可以应用于各类互动的场景：可以挥一挥手定义你想完成的任何动作，比如挥一挥手可以关灯。
<br>

<div align="center">
<img src=./../../docs/images/paj7620u2.png  width=30%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：地<br>
* SCL：I2C时钟<br>
* SDA：I2C数据<br>
* INT：中断引脚<br>

## 二、技术参数
* 工作电压：3.3V-5V<br>
* 通讯方式：I2C接口<br>
* 手势识别：支持9种手势姿态
* 工作温度：-40°C~85°C<br>
* 支持环境光：<100K Lux
* 工作电流：3mA-10mA<br>
* 电路板尺寸：20mm * 15mm<br>

## 三、软件接口

PAJ7620 手势识别传感器 HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/paj7620)
<br>

### PAJ7620(i2cObj) - 创建PAJ7620传感器对象

* 函数原型

> paj7620Obj = PAJ7620(i2cObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值

>PAJ7620对象成功，返回PAJ7620对象；PAJ7620对象创建失败，抛出Exception

* 示例代码-无回调函数
```python
from driver import I2C
import paj7620

i2cobj = I2C()
i2cobj.open("paj7620")
paj7620_obj = paj7620.PAJ7620(i2cobj)
print('start init paj7620')
```

* 输出
```python
start init paj7620
```
</br>

### setGestureHighrate(rate:bool) - 设置手势识别的速率
<br>

* 函数功能：

> 设置RTC时钟信息

* 函数原型：

> PAJ7620.setDatetime(data)

* 参数说明：
> rate：True-高速；False-慢速

* 返回值：
> 无

### getGesture() - 获取检测到的姿势状态值
<br>

* 函数功能：

> 获取检测到的姿势状态值

* 函数原型：

>  gesture = PAJ7620.getGesture()

* 参数说明：
> 无

* 返回值：
> 成功返回gesture的手势姿态值

gesture|说明|
|:-:|:-|
|0|未检测姿态|
|1|向右|
|2|向左|
|4|向上|
|8|向下|
|16|向前|
|32|向后|
|64|顺时针|
|128|逆时针|
|256|摇摆|

### gestureDescription(gesture) - 获取手势姿势的描述信息
<br>

* 函数功能：

> 获取手势姿势的描述信息

* 函数原型：

>  description = PAJ7620.gestureDescription(gesture)

* 参数说明：
> 手势姿态状态值

* 返回值：
> 成功返回gesture的手势姿态描述

gesture|description|说明|
|:-:|:-:|:-|
|0|None|未检测姿态|
|1|Right|向右|
|2|Left|向左|
|4|Up|向上|
|8|Down|向下|
|16|Forward|向前|
|32|Backward|向后|
|64|Clockwise|顺时针|
|128|Anti-Clockwise|逆时针|
|256|Wave|摇摆|

* 示例：

```python
from driver import I2C
import paj7620

i2cobj = I2C()
i2cobj.open("paj7620")
paj7620_obj = paj7620.PAJ7620(i2cobj)
print('start init paj7620')
paj7620_obj.setGestureHighrate(True)
gesture = paj7620_obj.getGesture()
print("gesture code        = %d"%(gesture))
print("gesture description  = "+ description)
```

* 输出
```python
start init paj7620
gesture code        = 2
gesture description  = Left
```
</br>

## 四、接口案例

此使用实例在board.json中定义了名为PAJ7620的I2C类型的对象。在Python脚本中获取手势姿态并且打印出来。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "paj7620": {
      "type": "I2C",
      "port": 0,
      "addrWidth": 7,
      "freq": 400000,
      "mode": "master",
      "devAddr": 115
    }
  },
}
```

```python
import utime   # 延时函数在utime库中
from driver import I2C
import paj7620

paj7620_obj = None

def paj7620_init():
    global paj7620_obj
    i2cobj = I2C()
    i2cobj.open("paj7620")
    paj7620_obj = paj7620.PAJ7620(i2cobj)
    print('start init paj7620')
    paj7620_obj.setGestureHighrate(True)

if __name__ == '__main__':
    paj7620_init()
    while True:
        gesture = paj7620_obj.getGesture()
        if gesture != paj7620_obj.GESTURE_NONE:
            description  = paj7620_obj.gestureDescription(gesture)
            print("--------------Gesture Recognition System---------------------------")
            print("gesture code        = %d"%(gesture))
            print("gesture description  = "+ description)
```

* 输出
```log
start init paj7620
--------------Gesture Recognition System---------------------------
gesture code        = 1
gesture description  = Right
--------------Gesture Recognition System---------------------------
gesture code        = 4
gesture description  = Up
--------------Gesture Recognition System---------------------------
gesture code        = 8
gesture description  = Down
--------------Gesture Recognition System---------------------------
gesture code        = 1
gesture description  = Right
--------------Gesture Recognition System---------------------------
gesture code        = 2
gesture description  = Left
--------------Gesture Recognition System---------------------------
gesture code        = 2
gesture description  = Left
```

## 参考文献及购买链接
[1] [购买链接](https://item.taobao.com/item.htm?spm=a230r.1.14.11.46ad45c3K8EgVe&id=580429587340&ns=1&abbucket=11#detail)
