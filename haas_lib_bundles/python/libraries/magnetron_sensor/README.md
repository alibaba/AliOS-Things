# MagnetronSensor 磁控管开关

## 一、产品简介
&emsp;&emsp;
干簧管是干式舌簧管的简称，是一种有触点的无源电子开关元件，具有结构简单，体积小便于控制等优点，其外壳一般是一根密封的玻璃管，管中装有两个铁质的弹性簧片电板，还灌有一种叫金属铑的惰性气体，平时，玻璃管中的两个簧片是分开的。当有磁性物质靠近玻璃管时，在磁场的作用下，管内的两个簧片被磁化而互相吸引接触，簧片就会吸合在一起使结点所接的电路连通。外磁力消失后，两个簧片由于本身的弹性而分开，线路也就断开了。因此，作为一种利用磁场信号来控制的线路开关器件，干簧管可以作为传感器用，用于计数，限位等等（在安防系统中主要用于门磁、窗磁的制作），同时还被广泛使用于各种通信设备中。在实际运用中，通常用磁铁控制这两根金属片的接通与否，所以又被称为“磁控管”。

&emsp;&emsp;
磁控管开关在程控交换机、复印机、洗衣机、电冰箱、照相机、消毒碗柜、门磁、窗磁、电磁继电器、电子衡器、液位计、煤气表、水表中等等都得到了很好的应用。

<div align="center">
<img src=./../../docs/images/磁控管.jpeg width=60%/>
</div>


引脚定义

* VCC：电源正 3.3V~5V
* GND：电源地
* DO：数字信号输出

## 二、技术参数
* 干簧管需要和磁铁配合使用，在感应到有一定的磁力的时候，会呈导通状态，模块输出低电平，无磁力时，呈断开状态，输出高电平，干簧管与磁铁的感应距离在1.5cm之内，超出不灵敏或会无触发现象；
* 模块DO输出端可以单片机IO口直接相连，通过单片机可以检测干簧管的触发状态；
* 模块DO输出端可与继电器IN端相连，组成大功率干簧管开关，直接控制高电压。

## 三、软件接口

### MagnetronSensor - 创建磁控管开关对象

* 函数原型

> sensorObj = MagnetronSensor(gpioObj)

* 参数说明

该方法初始化一个 MagnetronSensor 对象，用于磁控管开关数据读取。

|  参数   | 类型  | 必选参数？ | 说明                                             |
| :-----: | :---: | :--------: | :----------------------------------------------- |
| gpioObj | GPIO  |     是     | DO引脚的GPIO对象。调用此函数前需确保对象已被open |

* 返回值

若MagnetronSensor对象创建成功，返回ibrationSensor对象；否则抛出Exception

* 示例代码
```python
from MagnetronSensor import MagnetronSensor
from driver import GPIO

doPin = GPIO()
doPin.open("MagnetronSensor")

sensor = MagnetronSensor(doPin)
```

### isMagnetic() - 获取磁控管开关状态
<br>

* 函数功能：

获取磁控管开关状态。

* 函数原型：

> MagnetronSensor.isMagnetic()

* 参数说明：

无

* 返回值：

若检测到磁性，则返回1；否则返回0.

* 示例代码
```python
from MagnetronSensor import MagnetronSensor
from driver import GPIO

doPin = GPIO()
doPin.open("MagnetronSensor")

sensor = MagnetronSensor(doPin)
print(sensor.isMagnetic())
```

## 四、接口案例

* board.json
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "MagnetronSensor": {
            "type": "GPIO",
            "port": 28,
            "dir": "input",
        },
    }
}
```

* Python代码
```python
from MagnetronSensor import MagnetronSensor
from driver import GPIO

doPin = GPIO()
doPin.open("MagnetronSensor")

sensor = MagnetronSensor(doPin)

while True:
    utime.sleep(1)
    print(sensor.isMagnetic())
```

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=41236271649)
