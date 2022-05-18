# 步进电机驱动模组 - ULN2003A

## 一、产品简介
ULN2003A，是一种高压大电流的共发射极达林顿晶体管数组集成电路，内含7组达林顿对管，每组对管的电流容量是500mA，输出的电压最高50V。该集成电路还集成了7只共阴极形式连接的续流二极管，用于电感性负载的开关动作的电流续流，本驱动是通过ULN2003A控制5线4相步进电机。

<div align="center">
<img src=./../../docs/images/ext_uln2003A_实物.png  width=50%/>
</div>

### 引脚定义
* VCC: 5-12V
* IN1: host GPIO
* IN2: host GPIO
* IN3: host GPIO
* IN4: host GPIO
* GND: GND

## 二、技术参数
* 钳位二极管输出
* 高电压输出:50V
* 可兼容各类逻辑的输入

## 三、软件接口

ULN2003A步进电机驱动HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/uln2003)
<br>

### ULN2003(A, A_, B, B_) - 创建ULN2003驱动对象
* 函数原型:
> motorObj = ULN2003(a, a_, b, b_)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|a|GPIO|是|调用此函数前需确保a对象已经处于open状态|
|a_|GPIO|是|调用此函数前需确保a_对象已经处于open状态|
|b|GPIO|是|调用此函数前需确保b对象已经处于open状态|
|b_|GPIO|是|调用此函数前需确保b_对象已经处于open状态|
* 返回值:
创建对象成功，返回ULN2003对象；ULN2003A对象创建失败，抛出Exception

* 示例代码:

```python
from uln2003 import *                        # ULN2003驱动库

A = GPIO()
A.open("uln2003_a")
A_ = GPIO()
A_.open("uln2003_a_")
B = GPIO()
B.open("uln2003_b")
B_ = GPIO()
B_.open("uln2003_b_")
motorDev = ULN2003(A, A_, B, B_)
```

* 输出:
无

### motorCw(speed) - 步进电机正向旋转

* 函数功能:
驱动步进电机正向旋转

* 函数原型:
> ULN2003.motorCw(speed=4)

* 参数说明:
speed: 单位ms，步进电机旋转速度，speed越小，速度越快

* 返回值:
无

### motorCcw(speed) - 步进电机反向旋转

* 函数功能:
驱动步进电机反向旋转

* 函数原型:
> ULN2003.motorCcw(speed=4)

* 参数说明:
speed: 单位ms，步进电机旋转速度，speed越小，速度越快

* 返回值:
无

### motorStop() - 停止步进电机旋转

* 函数功能:
停止步进电机旋转

* 函数原型:
> ULN2003.motorStop()

* 参数说明:
无
* 返回值:
无

* 示例:

```python
from uln2003 import *                    # MQ3温湿度传感器驱动库
A = GPIO()
A.open("uln2003_a")
A_ = GPIO()
A_.open("uln2003_a_")
B = GPIO()
B.open("uln2003_b")
B_ = GPIO()
B_.open("uln2003_b_")
motorDev = ULN2003(A, A_, B, B_)
while 1:
    motorDev.motorCw()
```

* 输出:
无

## 四、接口案例
此使用实例在board.json中定义了名为uln2003的gpio数据类型，并控制步进电机正向旋转。

* 代码:
```json
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "uln2003_a": {
        "type": "GPIO",
        "port": 19,
        "dir": "output",
        "pull": "pullup"
      },

      "uln2003_a_": {
        "type": "GPIO",
        "port": 27,
        "dir": "output",
        "pull": "pullup"
      },

      "uln2003_b": {
        "type": "GPIO",
        "port": 0,
        "dir": "output",
        "pull": "pullup"
      },

      "uln2003_b_": {
        "type": "GPIO",
        "port": 2,
        "dir": "output",
        "pull": "pullup"
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}

```

```python
# Python代码
print("Testing uln2003  ...")

A = GPIO()
A.open("uln2003_a")
A_ = GPIO()
A_.open("uln2003_a_")
B = GPIO()
B.open("uln2003_b")
B_ = GPIO()
B_.open("uln2003_b_")
motorDev = ULN2003(A, A_, B, B_)
while 1:
    motorDev.motorCw()
A.close()
A_.close()
B.close()
B_.close()
del motorDev
print("Test uln2003 done!")
```
* 输出：
无
<br>

## 五、通信协议
主控芯片通过GPIO控制步进电机的正转反转。
* 正转序列

|序号|功能说明|命令字宏定义|
|:-:|:-|:-|
|1|GPIO A 输出高电平|CMD_PINA_OUT_HIGH|
|2|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|3|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|4|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
|5|delay xx ms|CMD_DELAY_XX_MS|
|6|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|7|GPIO A_ 输出高电平|CMD_PINA__OUT_HIGH|
|8|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|9|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
|10|delay xx ms|CMD_DELAY_XX_MS|
|11|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|12|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|13|GPIO B 输出高电平|CMD_PINB_OUT_HIGH|
|14|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
|15|delay xx ms|CMD_DELAY_XX_MS|
|16|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|17|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|18|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|19|GPIO B_ 输出高电平|CMD_PINB__OUT_HIGH|
|20|delay xx ms|CMD_DELAY_XX_MS|
<br>

* 反转序列

|序号|功能说明|命令字宏定义|
|:-:|:-|:-|
|1|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|2|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|3|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|4|GPIO B_ 输出高电平|CMD_PINB__OUT_HIGH|
|5|delay xx ms|CMD_DELAY_XX_MS|
|6|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|7|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|8|GPIO B 输出高电平|CMD_PINB_OUT_HIGH|
|9|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
|10|delay xx ms|CMD_DELAY_XX_MS|
|11|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|12|GPIO A_ 输出高电平|CMD_PINA__OUT_HIGH|
|13|GPIO B 输出低电平|CMD_PINB_OUT_HIGH|
|14|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
|15|delay xx ms|CMD_DELAY_XX_MS|
|16|GPIO A 输出高电平|CMD_PINA_OUT_HIGH|
|17|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|18|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|19|GPIO B_ 输出低电平|CMD_PINB__OUT_HIGH|
|20|delay xx ms|CMD_DELAY_XX_MS|
<br>

* 停止序列

|序号|功能说明|命令字宏定义|
|:-:|:-|:-|
|1|GPIO A 输出低电平|CMD_PINA_OUT_LOW|
|2|GPIO A_ 输出低电平|CMD_PINA__OUT_LOW|
|3|GPIO B 输出低电平|CMD_PINB_OUT_LOW|
|4|GPIO B_ 输出低电平|CMD_PINB__OUT_LOW|
</br>

* 控制电机流程
可以通过控制MCU的gpio 来输出正向旋转序列、反向旋转序列或停止序列来控制步进电机的正转、反转或者停止，并且可以通过控制正向序列或反向序列之间的延时间隔来控制步进电机的速度。

## 六、工作原理
步进电机是将电脉冲信号转变为角位移或线位移的开环控制元件, 通过GPIO控制脉冲变化从而触发磁场周期性变化，即可牵引转子周期性的变化，而ULN2003A能与TTL和CMOS电路直接相连，可以直接处理原先需要标准逻辑缓冲期来处理的数据。
如下是ULN2003内部的框图，1-7是ULN2003的输入段，经过达林顿管的电流放大，9-16进行输出。

<div align="center">
<img src=./../../docs/images/ext_uln2003_工作原理.png  width=50%/>
</div>

## 参考文献及购买链接
[1] [ULN2003A驱动器](https://www.ti.com.cn/product/cn/ULN2003A)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=41303683115)

