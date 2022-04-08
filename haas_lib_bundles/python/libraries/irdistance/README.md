# 红外避障传感器

## 一、产品简介
&emsp;&emsp;
红外避障传感器具有一对红外线发射与接收管，发射管发射出一定频率的红外线，当检测方向遇到障碍物（反射面）时，红外线反射回来被接收管接收。它常用于安装在小车上，判断前方是否有障碍物。可通过电位器设置阀值。正前方有障碍时绿灯亮起，OUT引脚为低电平，反之为高电平。红外避障传感器可以广泛应用于机器人避障、避障小车、流水线计数及黑白线循迹等众多场合。
<br>

<div align="center">
<img src=./../../docs/images/ext_红外避障_定义尺寸.png  width=40%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：地<br>
* OUT：输出信号
<br>

## 二、技术参数
* 工作电压：3.3V-5V<br>
* 有效检测距离：2cm-30cm<br>
* 工作电流：10mA<br>
* 输出电平：TTL<br>
* 检测角度：35°<br>
* 工作温度：-20°C~85°C<br>
* 检测距离：可调节电位器<br>
* 电路板尺寸：3.2cm*1.4cm<br>

## 三、软件接口
<br>

### IRDISTANCE(gpioObj) - 创建红外避障传感器对象
<br>

* 函数原型

> irObj = IRDISTANCE(gpioObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|

* 返回值

> IRDISTANCE对象成功，返回IRDISTANCE对象；IRDISTANCE对象创建失败，抛出Exception

* 示例代码
```python
import irdistance
from driver import GPIO
print("Testing IR detector ...")
gpioDev = GPIO()
gpioDev.open("ir")
irDev = irdistance.IRDISTANCE(gpioDev)
```

* 输出
```python
Testing IR detector ...
```
</br>

### objectDetection() - 检查红外避障传感器的状态
<br>

* 函数功能：

> 检查红外避障传感器的状态

* 函数原型：

> IRDISTANCE.objectDetection()

* 参数说明：
> 无

* 返回值：
> 成功返回IRDISTANCE热体红外状态，0 - 有障碍物 1 - 无障碍物；失败抛出Exception

* 示例：

```python
import irdistance
from driver import GPIO
import utime

print("Testing IR detector ...")
gpioDev = GPIO()
gpioDev.open("ir")
irDev = irdistance.IRDISTANCE(gpioDev)
while True:             # 无限循环
    status = irDev.objectDetection()
    if status == 0:
        print("object detected")
    elif status == 1:
        print('no object detected')
    utime.sleep(1)      # 打印完之后休眠1秒
```

* 输出
```python
Testing IR detector ...
object detected
object detected
object detected
no object detected
no object detected
no object detected
object detected
object detected
no object detected
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为IRDISTANCE的GPIO类型的对象。在Python脚本中获取红外避障传感器的状态值并打印在日志中。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
   "ir": {
      "type": "GPIO",
      "port": 2,
      "dir": "input",
      "pull": "pullup"
    }
  }
}
```

```python
import irdistance
from driver import GPIO
import utime

print("Testing IR detector ...")
gpioDev = GPIO()
gpioDev.open("ir")
irDev = irdistance.IRDISTANCE(gpioDev)
while True:             # 无限循环
    status = irDev.objectDetection()
    if status == 0:
        print("object detected")
    elif status == 1:
        print('no object detected')
    utime.sleep(1)      # 打印完之后休眠1秒
```

* 输出
```python
Testing IR detector ...
object detected
object detected
object detected
no object detected
no object detected
no object detected
object detected
object detected
no object detected
```

## 五、工作原理
### 1、原理介绍
<div align="center">
<img src=./../../docs/images/ext_红外避障_硬件原理图.png  width=70%/>
</div>

<br>
&emsp;&emsp;
红外线接收管是将红外线光信号变成电信号的半导体器件，它的核心部件是一个特殊材料的PN结，和普通二极管相比，在结构上采取了大的改变，红外线接收管为了更多更大面积的接受入射光线，PN结面积尽量做的比较大，电极面积尽量减小，而且PN结的结深很浅，一般小于1微米。红外线接收二极管是在反向电压作用之下工作的。没有光照时，反向电流很小（一般小于0.1微安），称为暗电流。当有红外线光照时，携带能量的红外线光子进入PN结后，把能量传给共价键上的束缚电子，使部分电子挣脱共价键，从而产生电子—空穴对（简称：光生载流子）。它们在反向电压作用下参加漂移运动，使反向电流明显变大，光的强度越大，反向电流也越大。这种特性称为“光电导”。红外线接收二极管在一般照度的光线照射下，所产生的电流叫光电流。如果在外电路上接上负载，负载上就获得了电信号，而且这个电信号随着光的变化而相应变化。

<br>

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=40844279989)
