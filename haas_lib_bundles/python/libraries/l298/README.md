# L298 电机驱动模块

## 一、产品简介
&emsp;&emsp;
L298N是意法半导体（ST Semiconductor）集团旗下量产的一种双路全桥式电机驱动芯片，拥有工作电压高、输出电流大、驱动能力强、发热量低、抗干扰能力强等特点，通常用来驱动继电器、螺线管、电磁阀、直流电机以及步进电机。

&emsp;&emsp;
**L298N可以驱动一台两相步进电机或四相步进电机，也可以驱动两台直流电机。**

&emsp;&emsp;
L298N 采用Multiwatt 15封装，N是L298的封装标识符。如图是市面上较为常见的一款L298N驱动模块。其中黑色的直立芯片就是 L298N 。并且配备了散热器，其可以用于大功率场景。

<div align="center">
<img src=./../../docs/images/L298N.png />
</div>

&emsp;&emsp;
市面上也有一些采用其他封装的模块，采用更小体积的封装模式，以应对更小功率需求的场景。其引脚定义都大致相同。

引脚定义

* VCC：驱动电源正（在图示模块中可使用5V或12V）
* GND：驱动电源地
* Out1 Out2 Out3 Out4：功率输出
* IN1 IN2 IN3 IN4：逻辑控制
* ENA：IN1 & IN2 使能（TTL高电平使能）
* ENB：IN3 & IN4 使能（TTL高电平使能）

> 其中 ENA 和 ENB 在部分模组中可能未透出给开发者，此时它们默认连接至芯片内部的VDD，即默认使能。

## 二、技术参数
* 驱动部分端子供电范围 Vs：5V~ 35V
* 驱动部分峰值电流 lo：2A
* 逻辑部分端子供电范围 Vss：5V~7V(板内取电+5V)
* 逻辑部分工作电流范围：0 ~ 36mA
* 控制信号输入电压范围：

    低电平：-0.3VsVin≤1.5V

    高电平：2.3VsVinsVss
* 使能信号输入电压范围：

    低电平：-0.3svin≤1.5V(控制信号无效）

    高电平：2.3VsvinsVss(控制信号有效）
* 最/大功耗：20W(温度T=75°C时）
* 储存温度：-25°C~+130°C
* 驱动板尺寸：48mm * 43mm * 33mm

## 三、软件接口

L298N电机驱动HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/l298)
<br>

### L298DC - 创建直流电机驱动对象
<br>

* 函数原型

> L298DCObj = L298DC(Int1x:PWM, Int2x:PWM, en=None, freq=50, name="DCMotorX")

* 参数说明

该方法初始化一个 L298DC 对象，用于直流电机控制。其使用L298的单侧资源，即 ENA，IN1，IN2 以控制 OUT1， OUT2；或使用 ENB，IN3，IN4 以控制 OUT3， OUT4。

| 参数  |  类型  | 必选参数？ | 说明                                                                                                    |
| :---: | :----: | :--------: | :------------------------------------------------------------------------------------------------------ |
| Int1x |  PWM   |     是     | IN1或IN3引脚的PWM对象。调用此函数前需确保对象已被open                                                   |
| Int2x |  PWM   |     是     | IN2或IN4引脚的PWM对象。调用此函数前需确保对象已被open                                                   |
|  en   |  PWM   |     否     | ENA或ENB引脚的PWM对象。调用此函数前需确保对象已被open。若该入参不被指定，则默认对应的控制引脚已被使能。 |
| freq  |  int   |     否     | PWM驱动频率。默认为50Hz。开发者可以改变此值以寻找最佳的驱动效果。                                       |
| name  | String |     否     | 驱动对象名称。用于在多驱动对象场景下进行标注和区分。                                                    |

* 返回值

若L298DC对象创建成功，返回L298DC对象；否则抛出Exception

* 示例代码
```python
from l298 import L298DC
from driver import PWM

in1 = PWM()
in1.open("in1")
in2 = PWM()
in2.open("in2")
ena = PWM()
ena.open("ena")

wheel = L298DC(in1,in2,ena,100,"wheel_left")
```

### run(rate) - 驱动电机以指定速度转动
<br>

* 函数功能：

驱动电机以rate指定速度转动。其中，rate指定的是电机最大功率的百分比。例如当rate=50时，电机以50%最大功率转动。（因此，对于普通的直流电机而言，并不能严格控制转速）

* 函数原型：

> L298DC.run(rate:int)

* 参数说明：

| 参数  | 类型  | 必选参数？ | 说明                                                                                                                                       |
| :---: | :---: | :--------: | :----------------------------------------------------------------------------------------------------------------------------------------- |
| rate  |  int  |     是     | 范围从-100度到+100。rate指定的是电机最大功率的百分比。例如当rate=50时，电机以50%最大功率转动。当rate>0时，电机正转；当rate<0时，电机反转。 |

* 返回值：

无

### stop() - 电机停转
<br>

* 函数功能：

电机停转

* 函数原型：

> L298DC.stop()

* 参数说明：

无

* 返回值：

无

### enable() - 使能驱动
<br>

* 函数功能：

使能驱动。仅在指定en引脚时，该函数有效。

* 函数原型：

> L298DC.enable()

* 参数说明：

无

* 返回值：

无

### disable() - 去使能驱动
<br>

* 函数功能：

去使能驱动。仅在指定en引脚时，该函数有效。

* 函数原型：

> L298DC.disable()

* 参数说明：

无

* 返回值：

无


</br>

## 四、接口案例

此使用实例在board.json中定义了名为SERVO的PWM类型的对象。在Python脚本中执行正90度到负90度转动的动作并打印在日志中。
<br>

* 案例代码
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "in1": {
            "type": "PWM",
            "port": 16
        },
        "in2": {
            "type": "PWM",
            "port": 17
        },
        "ena": {
            "type": "PWM",
            "port": 5
        },
        "in3": {
            "type": "PWM",
            "port": 18
        },
        "in4": {
            "type": "PWM",
            "port": 19
        },
        "enb": {
            "type": "PWM",
            "port": 21
        }
    }
}
```

```python
from l298 import L298DC
from driver import PWM

# 初始化左轮驱动
in1 = PWM()
in1.open("in1")
in2 = PWM()
in2.open("in2")
ena = PWM()
ena.open("ena")
wheelLeft = L298DC(in1,in2,ena,100,"wheel_left")

# 初始化右轮驱动
in3 = PWM()
in3.open("in3")
in4 = PWM()
in4.open("in4")
enb = PWM()
enb.open("enb")
wheelRight = L298DC(in3,in4,enb,100,"wheel_right")

# 全速前进
wheelLeft.run(100)
wheelRight.run(100)
utime.sleep(10)

# 右转
wheelLeft.run(100)
wheelRight.run(50)
utime.sleep(10)

# 后退
wheelLeft.run(-50)
wheelRight.run(-50)
utime.sleep(10)

# 原地旋转
wheelLeft.run(-50)
wheelRight.run(50)
utime.sleep(10)

# 停车
wheelLeft.stop()
wheelRight.stop()
```

## 五、工作原理
### L298的控制信号
|  ENA  |  IN1  |  IN2  | 行为     |
| :---: | :---: | :---: | :------- |
|   0   |   x   |   x   | 电机关闭 |
|   1   |   0   |   1   | 电机正转 |
|   1   |   0   |   0   | 电机制动 |
|   1   |   1   |   0   | 电机反转 |
|   1   |   1   |   1   | 电机制动 |

## 参考文献及购买链接
[1] [商品链接](https://detail.tmall.com/item.htm?id=41248562401)
