# VibrationSensor 震动传感器模块

## 一、产品简介
&emsp;&emsp;
震动传感器用于各种震动触发作用，如防盗报警，智能小车，地震报警等。

<div align="center">
<img src=./../../docs/images/震动传感器.jpeg width=30%/>
</div>


引脚定义

* VCC：电源正 3.3V~5V
* GND：电源地
* DO：数字信号输出
* AO：不使用

## 二、技术参数
* SW-18010P震动传感器
* 比较器输出，信号干净，波形好，驱动能力强，超过15mA
* 工作电压3.3V-5V
* 输出形式：数字开关量输出（0和1）
* 设有固定螺栓孔，方便安装
* 小板PCB尺寸：5.3cmx1.4cm
* 使用宽电压LM393比较器

## 三、软件接口

震动传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/vibration_sensor)
<br>

### VibrationSensor - 创建震动传感器对象

* 函数原型

> sensorObj = VibrationSensor(gpioObj)

* 参数说明

该方法初始化一个 VibrationSensor 对象，用于震动传感器数据读取。

|  参数   | 类型  | 必选参数？ | 说明                                             |
| :-----: | :---: | :--------: | :----------------------------------------------- |
| gpioObj | GPIO  |     是     | DO引脚的GPIO对象。调用此函数前需确保对象已被open |

* 返回值

若VibrationSensor对象创建成功，返回VibrationSensor对象；否则抛出Exception

* 示例代码
```python
from VibrationSensor import VibrationSensor
from driver import GPIO

doPin = GPIO()
doPin.open("VibrationSensor")

sensor = VibrationSensor(doPin)
```

### isVibrating() - 获取震动传感器状态
<br>

* 函数功能：

获取震动传感器状态

* 函数原型：

> VibrationSensor.isVibrating()

* 参数说明：

无

* 返回值：

若传感器震动，返回1；否则返回0。

* 示例代码
```python
from VibrationSensor import VibrationSensor
from driver import GPIO

doPin = GPIO()
doPin.open("VibrationSensor")

sensor = VibrationSensor(doPin)
print(sensor.isVibrating())
```

## 四、接口案例

board.json配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "VibrationSensor": {
            "type": "GPIO",
            "port": 28,
            "dir": "input",
        },
    }
}
```

Python代码
```python
from VibrationSensor import VibrationSensor
from driver import GPIO

doPin = GPIO()
doPin.open("VibrationSensor")

sensor = VibrationSensor(doPin)

while True:
    utime.sleep(1)
    print(sensor.isVibrating())
```

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=41236271649)
