# 马达测速传感器

## 一、产品简介
&emsp;&emsp;
马达测速传感器与电机，码盘组成测速模块，有遮挡，输出高电平；无遮挡，输出低电平，有了高低电平，然后通过脉冲个数来确定电机的转数。马达测速传感器广泛用于电机转速检测，脉冲计数，位置限位。
<br>

<div align="center">
<img src=./../../docs/images/motorspeed.png  width=30%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：地<br>
* OUT：输出信号,有遮挡，输出高电平；无遮挡，输出低电平
<br>

## 二、技术参数
* 工作电压：3.3V-5V<br>
* 驱动能力：驱动能力强，超过 15mA<br>
* 输出形式：数字开关量输出（0和1）<br>
* 工作温度：-20°C~85°C<br>
* 比较器：宽电压LM393<br>
* 电路板尺寸：2.3cm*2cm<br>

## 三、软件接口

马达测速传感器 HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/motorspeed)
<br>

### MOTORSPEED(gpioObj) - 创建马达测速传感器对象
<br>

* 函数原型

> motorspeedObj = MOTORSPEED(gpioObj,func)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|
|func|回调函数|否|默认无回调函数，如果需要使用中断回调处理，则需要传入该参数|

* 返回值

> MOTORSPEED对象成功，返回MOTORSPEED对象；MOTORSPEED对象创建失败，抛出Exception

* 示例代码-无回调函数
```python
import motorspeed
from driver import GPIO

print("Testing motorspeed detector ...")
gpioDev = GPIO()
gpioDev.open("motorspeed")
motorspeedDev = motorspeed.MOTORSPEED(gpioDev)
```

* 输出
```python
Testing motorspeed detector ...
```
</br>

* 示例代码-有回调函数
```python
import motorspeed
from driver import GPIO

speed_cnts = 0

def speed_handler():
    global speed_cnts
    speed_cnts += 1
    print('cnts: %d'%speed_cnts)

print("Testing motorspeed detector ...")
gpioDev = GPIO()
gpioDev.open("motorspeed")
motorspeedDev = motorspeed.MOTORSPEED(gpioDev,speed_handler)
```

* 输出
```python
Testing motorspeed detector ...
cnts: 0
cnts: 1
cnts: 2
cnts: 3
cnts: 4

```
</br>

### objectDetection() - 检查马达测速传感器的状态
<br>

* 函数功能：

> 检查马达测速传感器的状态

* 函数原型：

> MOTORSPEED.objectDetection()

* 参数说明：
> 无

* 返回值：
> 成功返回MOTORSPEED检测状态，0 - 无障碍物 1 - 有障碍物；失败抛出Exception

* 示例：

```python
import motorspeed
from driver import GPIO

print("Testing motorspeed detector ...")
gpioDev = GPIO()
gpioDev.open("motorspeed")
motorspeedDev = motorspeed.MOTORSPEED(gpioDev)
print(motorspeedDev.objectDetection())
```

* 输出
```python
Testing motorspeed detector ...
0
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为MOTORSPEED的GPIO类型的对象。在Python脚本中获取传感器的输出状态并打印在日志中。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
   "motorspeed": {
        "type": "GPIO",
        "port": 2,
        "dir": "irq",
        "pull": "pullup",
        "intMode": "falling"
    }
  },
}
```

```python
import motorspeed
from driver import GPIO
import utime

speed_cnts = 0
def speed_handler(data):
    global speed_cnts
    speed_cnts += 1
    print('cnts: %d'%speed_cnts)
print("Testing motorspeed detector ...")
gpioDev = GPIO()
gpioDev.open("motorspeed")
motorspeedDev = motorspeed.MOTORSPEED(gpioDev,speed_handler)
while 1:
    print(motorspeedDev.objectDetection())
    utime.sleep(1)
```

* 输出
```log
Testing motorspeed detector ...
0
0
0
1
cnts: 1
0
cnts: 2
cnts: 3
cnts: 4
cnts: 5
0
```

## 参考文献及购买链接
[1] [购买链接](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-24362052664.33.455c1eedmwZki6&id=554539887611)
