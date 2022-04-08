# 超声波测距传感器

## 一、产品简介
&emsp;&emsp;
HC-SR04超声波传感器使用声纳来确定物体的距离，其工作原理类似与蝙蝠。测量范围从2厘米到450厘米，HC-SR04超声波传感器上配有超声波发射器和接收器两个模块。
<br>
&emsp;&emsp;
超声波测距传感器是数字接口类型，其基本工作过程如下:
* 发射器（触发引脚-Trig），发送信号 一段高频声音<br>
* 当信号遇到一个物体时，它会被反射<br>
* 然后发射器（回声引脚-Echo）接收该信号<br>

<div align="center">
<img src=./../../docs/images/ext_超声波测距_定义尺寸.png  width=35%/>
</div>

引脚定义
* VCC：DC-5V<br>
* Trig：触发(输入)<br>
* Echo：回声(输出)<br>
* GND：地

<br>

## 二、技术参数
* 电源：+ 5V DC<br>
* 静态电流：<2mA<br>
* 工作电流：15mA<br>
* 有效角度：<15°<br>
* 测距：2厘米 - 450厘米<br>
* 分辨率：0.3厘米<br>
* 测量角度：30度<br>
* 触发输入脉冲宽度：10uS<br>
* 尺寸：45mm x 20mm x 15mm<br>

## 三、软件接口
<br>

### HCSR04(trigObj,echoObj) - 创建超声波传感器对象
<br>

* 函数原型

> hcsr04Obj = HCSR04(trigObj,echoObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|trigObj|GPIO|是|GPIO配置输出，触发引脚，调用此函数前需确保trigObj对象已经处于open状态|
|echoObj|GPIO|是|GPIO配置输入，测量引脚，调用此函数前需确保echoObj对象已经处于open状态|

* 返回值

> HCSR04对象成功，返回HCSR04对象；HCSR04对象创建失败，抛出Exception

* 示例代码
```python
import hcsr04
from driver import GPIO

print("Testing HCSR04 ...")
echoDev = GPIO()
echoDev.open("echo")

trigDev = GPIO()
trigDev.open("trig")

hcsr04Dev = hcsr04.HCSR04(trigDev,echoDev)
```

* 输出
```python
Testing HCSR04 ...
```
</br>

### measureDistance() - 获取超声波测量的距离
<br>

* 函数功能：

> 获取超声波测量的距离

* 函数原型：

> HCSR04.measureDistance()

* 参数说明：
> 无

* 返回值：
> 返回值为cm单位的距离值

* 示例：

```python
import hcsr04
from driver import GPIO
import utime

print("Testing HCSR04 ...")
echoDev = GPIO()
echoDev.open("echo")

trigDev = GPIO()
trigDev.open("trig")

hcsr04Dev = hcsr04.HCSR04(trigDev,echoDev)
while True:             # 无限循环
    distance = disDev.measureDistance()
    print(str(distance)+' CM')
    utime.sleep(1)      # 打印完之后休眠1秒
```

* 输出
```python
Testing HCSR04 ...
34 CM
492 CM
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为IR的GPIO类型的对象。在Python脚本中执行一次获取IR人体红外传感器状态值并打印在日志中。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "trig": {
      "type": "GPIO",
      "port": 25,
      "dir": "output",
      "pull": "pullup"
    },
    "echo": {
      "type": "GPIO",
      "port": 26,
      "dir": "input",
      "pull": "pullup"
    }
  }
}
```

```python
import hcsr04
from driver import GPIO
import utime

print("Testing HCSR04 ...")
echoDev = GPIO()
echoDev.open("echo")

trigDev = GPIO()
trigDev.open("trig")

hcsr04Dev = hcsr04.HCSR04(trigDev,echoDev)
while True:             # 无限循环
    distance = disDev.measureDistance()
    print(str(distance)+' CM')
    utime.sleep(1)      # 打印完之后休眠1秒
```

* 输出
```python
Testing HCSR04 ...
34 CM
492 CM
```

## 五、工作原理

### 超声波工作示意图
<div align="center">
<img src=./../../docs/images/ext_超声波测距原理_定义尺寸.png  width=40%/>
</div>
<br>

### 基本原理
* 采用IO口Trig触发测距，对Trig口输出至少10us的高电平
* 模块自动发送8个40Khz的方波，自动检测是否有信号返回
* 有信号返回，通过IO口Echo输出一个高电平，高电平持续的时间就是超声波从发送到返回的时间
* 测试距离 = （高电平持续的时间/声速340m/s）/2

<br>

### 时序原理
<div align="center">
<img src=./../../docs/images/ext_超声波测距时序_定义尺寸.png width=40%/>
</div>

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=13674895744)
