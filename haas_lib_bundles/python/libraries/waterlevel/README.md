# 水位监测传感器

## 一、产品简介
&emsp;&emsp;
水位监测传感器主要用于测量水位深度的传感器，该传感器测量范围在0cm-4cm，通过使用一系列五个连接到系统接地的裸露走线来工作。在每两个接地迹线之间交错插入一个感测迹线，五个接地迹线和总共五个感测迹线。

<div align="center">
<img src=./../../docs/images/ext_水位监测_定义尺寸.png  width=30%/>
</div>

引脚定义
* GND：地<br>
* VCC：5V<br>
* NC：不接<br>
* AO：模拟输出
<br>

## 二、技术参数
工作电压：DC5V<br>
工作电流：小于35mA<br>
数据类型：模拟信号<br>
检测面积：40mm x 18mm最深只能测4cm<br>
制作工艺：FR4双面喷锡<br>
工作温度：10°C-30°C<br>
工作湿度：10%-90%无凝结<br>
模块重量：3.5g<br>
板子尺寸：60mm x 22mm <br>

## 三、软件接口

### WATER4LEVEL(adcObj) - 创建水位监测传感器驱动对象
<br>

* 函数原型

> waterlevelObj = WATER4LEVEL(adcObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|adcObj|ADC|是|调用此函数前需确保adcObj对象已经处于open状态|

* 返回值

> WATER4LEVEL对象成功，返回WATER4LEVEL对象；WATER4LEVEL对象创建失败，抛出Exception

* 示例代码
```python
import water4level
from driver import ADC

print("Testing water level ...")
adcObj = ADC()
adcObj.open("waterlevel")
print("waterlevel inited!")
waterlevelObj = water4level.WATER4LEVEL(adcObj)
```

* 输出
```python
Testing water level ...
waterlevel inited!
```
</br>

### measureLevel() - 测量水位值，单位cm
<br>

* 函数功能：

> 测量水位值，单位cm

* 函数原型：

> WATER4LEVEL.measureLevel()

* 参数说明：

> 无

* 返回值：

> 返回测到的水位值（数据类型：整型；单位：cm）

* 示例：

```python
import water4level
from driver import ADC
import utime

print("Testing water level ...")
adcObj = ADC()
adcObj.open("waterlevel")
print("waterlevel inited!")
waterlevelObj = water4level.WATER4LEVEL(adcObj)
while True:             # 无限循环
    level = waterlevelObj.measureLevel()
    print("%d"%level+" cm")
    utime.sleep(1)
```

* 输出
```python
Testing water level ...
waterlevel inited!
0 cm
1 cm
1 cm
1 cm
2 cm
2 cm
3 cm
3 cm
3 cm
3 cm
4 cm
4 cm
4 cm
4 cm
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为WATER4LEVEL的ADC类型的对象。在Python脚本中获取测量到的水位值并打印在日志中。

* 案例代码
```python
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
       "waterlevel": {
        "type": "ADC",
        "port": 0,
        "atten": 2,
        "width": 3,
        "sampling": 12000000
      },
    }
}
```

```python
import water4level
from driver import ADC
import utime

print("Testing water level ...")
adcObj = ADC()
adcObj.open("waterlevel")
print("waterlevel inited!")
waterlevelObj = water4level.WATER4LEVEL(adcObj)
while True:             # 无限循环
    level = waterlevelObj.measureLevel()
    print("%d"%level+" cm")
    utime.sleep(1)
```

* 输出
```python
Testing water level ...
waterlevel inited!
0 cm
1 cm
1 cm
1 cm
2 cm
2 cm
3 cm
3 cm
3 cm
3 cm
4 cm
4 cm
4 cm
4 cm
```

## 五、工作原理
&emsp;&emsp;
Water Sensor液位（水位）传感器是一款简单易用、小巧轻便、性价比较高的水位/水滴识别检测传感器，其是通过具有一系列的暴露的平行导线线迹测量其水滴/水量大小从而判断水位。轻松完成水量到模拟信号的转换，输出的模拟值可以直接被程序中函数所应用，达到水位报警的功效，低功耗，灵敏度是其又一大特点。

<br>

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=557306585811)
