# MQ-136 硫化氢传感器

## 一、产品简介
&emsp;&emsp;
MQ-136 是一种硫化氢检测传感器，感应范围为 1 - 200ppm。传感元件是 SnO2，它在清洁空气中的电导率较低。当存在 H₂S 气体时，传感器的电导率随着气体浓度的升高而升高，表现为ADC读取到的电压值升高。

    ⚠️ 传感器通电后，需要使用其内部的电热丝预热20秒左右以使测量的数据稳定，因此传感器发热属于正常现象。

<div align="center">
<img src=./../../docs/images/MQ136.jpeg  width=50%/>
</div>

引脚定义

* VCC:接电源正极（5V）
* GND:接电源负极
* AO:模拟信号输出

## 二、技术参数
* 具有输出调节电位器：顺时针调节大，逆时针调节小
* 传感器类型：模拟/数字
* 快速响应和高灵敏度
* 简单电路板
* 5V供电

## 三、软件接口

MQ-136硫化氢传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/mq136)
<br>

### MQ136(adcObj) - 创建MQ136燃气气体传感器驱动对象

* 函数原型

> mqObj = MQ136(adcObj)

* 参数说明

|  参数  | 类型  | 必选参数？ | 说明                                         |
| :----: | :---: | :--------: | :------------------------------------------- |
| adcObj |  ADC  |     是     | 调用此函数前需确保adcObj对象已经处于open状态 |

* 返回值

> MQ136对象成功，返回MQ136对象；MQ136对象创建失败，抛出Exception

* 示例代码
```python
from mq136 import MQ136
from driver import ADC

print("Testing mq136 ...")

adcDev = ADC()
adcDev.open("mq136")

mq136Dev = MQ136(adcDev)
```

* 输出
```python
Testing mq136 ...
```
</br>

### getVoltage() - 获取硫化氢浓度电压值
<br>

* 函数功能：

> 获取硫化氢浓度电压值

* 函数原型：

> MQ136.getVoltage()

* 参数说明：

> 无

* 返回值：

> 成功返回硫化氢浓度电压值（数据类型：整型；单位：mv）；失败抛出Exception

* 示例：

```python
from mq136 import MQ136
from driver import ADC

print("Testing mq136 ...")

adcDev = ADC()
adcDev.open("mq136")

mq136Dev = MQ136(adcDev)

value = mq136Dev.getVoltage()
print("The mq136 voltage: ",value)
```

* 输出
```python
Testing mq136 ...
The mq136 voltage: 1396
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为MQ136的ADC类型的对象。在Python脚本中获取燃气气体值并打印在日志中。

* 案例代码
```python
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "mq136":{
            "type": "ADC",
            "port": 5,
            "sampling": 12000000,
            "atten": 3,
            "width": 3
      }
    }
}
```

```python
import mq136
from driver import ADC

print("Testing mq136 ...")

adcDev = ADC()
adcDev.open("mq136")

mq136Dev = mq136.MQ136(adcDev)

value = mq136Dev.getVoltage()
print("The mq136 voltage: ",value)
```

* 输出
```python
Testing mq136 ...
The mq136 voltage: 1396
```

## 五、工作原理
MQ-136 是一种硫化氢检测传感器，感应范围为 1 - 200ppm。传感元件是 SnO2，它在清洁空气中的电导率较低。当存在 H₂S 气体时，传感器的电导率随着气体浓度的升高而升高，表现为ADC读取到的电压值升高。

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=665515935747)
