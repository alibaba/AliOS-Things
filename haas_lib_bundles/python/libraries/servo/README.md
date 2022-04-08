# 舵机(Servo)控制器

## 一、产品简介
&emsp;&emsp;
舵机（英文叫Servo）是一种通过发送信号，指定输出旋转角度。舵机一般与普通直流电机的区别主要在于直流电机是一圈圈转动的，舵机只能在一定角度内转动，不能一圈圈转(舵机一般而言都有最大旋转角度,比如180度），可以反馈转动的角度信息。所以舵机主要用于控制某物体转动一定角度用的（比如机器人的关节、开关门），本驱动传感器型号是MG995.

<div align="center">
<img src=./../../docs/images/ext_舵机_外观图.png  width=30%/>
</div>

引脚定义
* 棕色：地<br>
* 黄色：输入信号<br>
* 红色：3.3V
<br>

## 二、技术参数
* 使用电压：3-7.2V<br>
* 反应转速：63-62r/m<br>
* 工作电流：100mA<br>
* 工作扭矩：13Kg/cm<br>
* 工作温度：30°C-55°C<br>
* 结构：金属锯齿、空心杯电机、双滚珠轴承<br>
* 线长：25cm<br>

## 三、软件接口
<br>

### SERVO(pwmObj) - 创建舵机驱动对象
<br>

* 函数原型

> servoObj = SERVO(pwmObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|pwmObj|PWM|是|调用此函数前需确保pwmObj对象已经处于open状态|

* 返回值

> SERVO对象成功，返回SERVO对象；SERVO对象创建失败，抛出Exception

* 示例代码
```python
import servo
from driver import PWM

print("Testing SERVO ...")
pwmObj = PWM()
pwmObj.open("servo")
servoObj = servo.SERVO(pwmObj)
```

* 输出
```python
Testing SERVO ...
```
</br>

### setOptionSero(data) - 设置舵机的转动
<br>

* 函数功能：

> 设置舵机的转动

* 函数原型：

> SERVO.setOptionSero(data)

* 参数说明：

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
| data|number|是|范围从-90度到+90度，0为设置初始原始位置|

* 返回值：

> 无

* 示例：

```python
import servo
from driver import PWM
import utime

print("Testing SERVO ...")
pwmObj = PWM()
pwmObj.open("servo")
servoObj = servo.SERVO(pwmObj)

servoObj.setOptionSero(0)
print("0")
utime.sleep(10)
servoObj.setOptionSero(90)
print("90")
```

* 输出
```python
Testing SERVO ...
0
90
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为SERVO的PWM类型的对象。在Python脚本中执行正90度到负90度转动的动作并打印在日志中。
<br>

* 案例代码
```python
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "servo": {
            "type": "PWM",
            "port": 27
        }
    }
}
```

```python
import servo
from driver import PWM
import utime

print("Testing SERVO ...")
pwmObj = PWM()
pwmObj.open("servo")
print("buzzer inited!")
servoObj = servo.SERVO(pwmObj)
servoObj.setOptionSero(0)
print("0")
utime.sleep(10)

while True:             # 无限循环
    servoObj.setOptionSero(90)
    print("90")
    utime.sleep(1)
    servoObj.setOptionSero(0)
    print("0")
    utime.sleep(1)
    servoObj.setOptionSero(-90)
    print("-90")
    utime.sleep(1)
    servoObj.setOptionSero(0)
    print("0")
    utime.sleep(1)
```

* 输出
```python
Testing SERVO ...
0
90
0
-90
0
90
```

## 五、工作原理

舵机的转动角度跟输入脉冲表
角度|脉冲周期|脉冲高电平时间|对应占空比|
|:-:|:-:|:-:|:-|
|0|20ms|0.5ms|2.5%|
|45|20ms|1ms|5.0%|
|90|20ms|1.5ms|7.5%|
|135|20ms|2ms|10.0%|
|180|20ms|2.5ms|12.5%|


从上表可以看出角度是由来自控制线的持续脉冲所产生。这种控制方法叫做脉冲调制。脉冲的长短决定舵机转动多大角度。例如：1.5毫秒脉冲会到转动到中间位置（对于180°舵机来说，就是90°位置）。当控制系统发出指令，让舵机移动到某一位置，并让他保持这个角度，这时外力的影响不会让他角度产生变化，但是这个是由上限的，上限就是他的最大扭力。除非控制系统不停的发出脉冲稳定舵机的角度，舵机的角度不会一直不变。
<br>

当舵机接收到一个小于1.5ms的脉冲，输出轴会以中间位置为标准，逆时针旋转一定角度。接收到的脉冲大于1.5ms情况相反。不同品牌，甚至同一品牌的不同舵机，都会有不同的最大值和最小值。一般而言，最小脉冲为1ms，最大脉冲为2ms。
<br>

总结：舵机角度的转动就是通过高、低脉冲的变化实现的。
<br>

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=587390473616)

