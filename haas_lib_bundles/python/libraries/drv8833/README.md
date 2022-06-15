# DRV8833 电机驱动模块

## 一、产品简介
&emsp;&emsp;
DRV8833电机驱动模块是一种单H桥电机驱动芯片，可驱动2个直流电机和4线步进电机。内置过流保护，短路保护，欠压闭锁和过热保护，带低功耗睡眠模式。

<div align="center">
<img src=./../../docs/images/drv8833.png width=40%/>
</div>

引脚定义

* VM：芯片和电机的驱动电源正供电脚
* GND：驱动电源地
* AIN1 AIN2 BIN1 BIN2：逻辑控制
* AO1 AO2 BO1 BO2：输出端，接电机
* STBY：接地或悬空芯片不工作，无输出，接5V工作
* NC：悬空

## 二、技术参数
* 驱动部分端子供电范围：2.7V~ 10.8V
* 驱动部分峰值电流：1.5A
* 驱动板尺寸：18mm * 21mm

## 三、软件接口

DRV8833电机驱动HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/drv8833)
<br>

### DRV8833 - 创建电机驱动对象
<br>

* 函数原型

> DRV8833Obj = DRV8833(xIN1:PWM, xIN2:PWM, freq=50)

* 参数说明

该方法初始化一个DRV8833对象，用于直流电机控制。其使用DRV8833，使用AIN1，AIN2以控制AO1，AO2；使用BIN1，BIN2以控制BO1，BO2。

| 参数  |  类型  | 必选参数？ | 说明                                                                                                    |
| :---: | :----: | :--------: | :------------------------------------------------------------------------------------------------------ |
| xIN1 |  GPIO/PWM   |     是     | AIN1或BIN1引脚的GPIO/PWM对象。调用此函数前需确保对象已被open                                                   |
| xIN2 |  GPIO/PWM   |     是     | AIN2或BIN2引脚的GPIO/PWM对象。调用此函数前需确保对象已被open                                                   |
| freq  |  int   |     否     | PWM驱动频率。xIN1、xIN2为GPIO类型控制是，不要设置|

* 返回值

若DRV8833对象创建成功，返回DRV8833对象；否则抛出Exception

* 示例代码
```python
import drv8833  #温度驱动库
from driver import GPIO

ain1 = GPIO()
ain1.open("ain1")
ain2 = GPIO()
ain2.open("ain2")

wheel = drv8833.DRV8833(ain1,ain2)
```

### run(status,rate) - 驱动电机转动
<br>

* 函数功能：

驱动电机，控制器正传和反转，以及PWM模式下，rate指定速度转动。其中，rate指定的是电机最大功率的百分比。例如当rate=50时，电机以50%最大功率转动。（因此，对于普通的直流电机而言，并不能严格控制转速）

* 函数原型：

> DRV8833.run(status,rate:int)

* 参数说明：

| 参数  | 类型  | 必选参数？ | 说明                                                                                                                                       |
| :---: | :---: | :--------: | :----------------------------------------------------------------------------------------------------------------------------------------- |
| status  |  int  |     否    | GPIO控制模式使用。当status=1时，电机正转；当status=2时，电机反转。|
| rate  |  int  |    否     | PWM控制模式使用。范围从-100度到+100。rate指定的是电机最大功率的百分比。例如当rate=50时，电机以50%最大功率转动。当rate>0时，电机正转；当rate<0时，电机反转。 |

* 返回值：

无

### stop() - 电机停转
<br>

* 函数功能：

电机停转

* 函数原型：

> DRV8833.stop()

* 参数说明：

无

* 返回值：

无

</br>

## 四、接口案例

此使用实例在board.json中定义了GPIO类型的对象。在Python脚本中执行正转和反转转动的动作并打印在日志中。
<br>

* 案例代码
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "ain1": {
            "type": "GPIO",
            "port": 16,
            "dir": "output",
            "pull": "pulldown"
        },
        "ain2": {
            "type": "GPIO",
            "port": 17,
            "dir": "output",
            "pull": "pulldown"
        },
        "bin1": {
            "type": "GPIO",
            "port": 18,
            "dir": "output",
            "pull": "pulldown"
        },
        "bin2": {
            "type": "GPIO",
            "port": 19,
            "dir": "output",
            "pull": "pulldown"
        }
    }
}
```

```python
import drv8833  #温度驱动库
from driver import GPIO

ain1 = GPIO()
ain1.open("ain1")
ain2 = GPIO()
ain2.open("ain2")

wheel_A = drv8833.DRV8833(ain1,ain2)

bin1 = GPIO()
bin1.open("bin1")
bin2 = GPIO()
bin2.open("bin2")

wheel_B = drv8833.DRV8833(bin1,bin2)

# a电机正传
wheel_A.run(1)
utime.sleep(10)

# a电机反传
wheel_A.run(2)
utime.sleep(10)

# b电机正传
wheel_B.run(1)
utime.sleep(10)

# b电机反传
wheel_B.run(2)
utime.sleep(10)

# AB电机停止
wheel_A.stop()
wheel_B.stop()
```

## 五、工作原理
### DRV8833的控制信号
|   xIN1  |  xIN2  |  xO1  |  xO2  |行为     |
|  :---: | :---: | :---: | :---: |:------- |
|   0   |   1   |  L   |   H   |  电机反转 |
|   1   |   0   | H    |   L   |  电机正转 |
|   1   |   1   | L    |   L   |  电机制动 |
|   0   |   0   | Z    |   Z   |  惯性运作 |

## 参考文献及购买链接
[1] [商品链接](https://item.taobao.com/item.htm?spm=a230r.1.14.16.19c14835qRKdcS&id=660790671685&ns=1&abbucket=10#detail)
