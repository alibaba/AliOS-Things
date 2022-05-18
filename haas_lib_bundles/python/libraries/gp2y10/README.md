# 粉尘传感器 - GP2Y10

## 一、产品简介
&emsp;&emsp;
GP2Y10粉尘传感器用于检测非常细的空气漂浮颗粒物，主要用于空气净化系统中，传感器外观引脚如下图所示。

<div align="center">
<img src=./../../docs/images/ext_粉尘传感器_定义尺寸.png  width=50%/>
</div>

引脚定义
* GND：地<br>
* VCC：5V<br>
* LED：输入信号<br>
* OUT：模拟信号输出
<br>

## 二、技术参数
工作电压：5V～7V<br>
工作电流：20mA<br>
最小粒子检出值：0.8微米<br>
灵敏度：0.5V/（0.1mg/m3）
工作温度：-10°C～65°C<br>
存储温度：-20°C～80°C<br>
模块重量：15g<br>
板子尺寸：46mm x 30mm x 17.6mm <br>

## 三、软件接口

GP2Y10粉尘传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/gp2y10)
<br>

### GP2Y10(adcObj,gpioObj) - 创建粉尘传感器驱动对象
<br>

* 函数原型

> gp2y10Obj = GP2Y10(adcObj,gpioObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|adcObj|ADC|是|传感器OUT配置引脚<br>调用此函数前需确保adcObj对象已经处于open状态|
|gpioObj|GPIO|是|传感器LED配置引脚<br>调用此函数前需确保gpioObj对象已经处于open状态|

* 返回值

> GP2Y10对象成功，返回GP2Y10对象；GP2Y10对象创建失败，抛出Exception

* 示例代码
```python
import gp2y10
from driver import GPIO,ADC

gpioDev = GPIO()
gpioDev.open("gp2y10led")
adcDev = ADC()
adcDev.open("gp2y10out")
gp2y10Obj=gp2y10.GP2Y10(adcDev,gpioDev)
print("gp2y10Obj inited!")
```

* 输出
```log
gp2y10Obj inited!
```
</br>

### getVoltage() - 获取当前adc值
<br>

* 函数功能：

>  获取当前adc值

* 函数原型：

> GP2Y10.getVoltage()

* 参数说明：

> 无

* 返回值：

> 返回当前adc值

* 示例：

```python
import gp2y10
from driver import GPIO,ADC
import utime

gpioDev = GPIO()
gpioDev.open("gp2y10led")
adcDev = ADC()
adcDev.open("gp2y10out")
gp2y10Obj=gp2y10.GP2Y10(adcDev,gpioDev)
print("gp2y10Obj inited!")
while True:
    readvalue = gp2y10Obj.getVoltage()
    print("value is ", readvalue)
    utime.sleep(1)
```

* 输出
```log
gp2y10Obj inited!
value is 8
value is 11
value is 13
value is 9
value is 13
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为GP2Y10的ADC和GPIO两种类型的对象。在Python脚本中获取粉尘传感器的adc值并打印在日志中。

* 案例代码
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "gp2y10out": {
        "type": "ADC",
        "port": 0,
        "atten": 1,
        "width": 3,
        "sampling": 12000000
      },
      "gp2y10led": {
        "type": "GPIO",
        "port": 26,
        "dir": "output",
        "pull": "pullup"
      }
    }
}
```

```python
import gp2y10
from driver import GPIO,ADC
import utime

gpioDev = GPIO()
gpioDev.open("gp2y10led")
adcDev = ADC()
adcDev.open("gp2y10out")
gp2y10Obj=gp2y10.GP2Y10(adcDev,gpioDev)
print("gp2y10Obj inited!")
while True:
    readvalue = gp2y10Obj.getVoltage()
    print("value is ", readvalue)
    utime.sleep(1)
```

* 输出
```log
gp2y10Obj inited!
value is 8
value is 11
value is 13
value is 9
value is 13
```

## 五、工作原理

其原理是粉尘传感器中心有个孔，可以让空气自由流过，定向发射LED光，通过检测经过空气中灰尘折射过后的光线来判断灰尘的含量。传感器装置中有一个红外发光二极管和光电晶体管，对角布置成允许其检测到在空气中的灰尘发射光。传感器内部电路图如下所示：

<div align="center">
<img src=./../../docs/images/ext_粉尘传感器_内部电路.png  width=50%/>
</div>

## 参考文献及购买链接
[1] [粉尘传感器](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.798c71b1CMgpmX&id=569075916681&ns=1&abbucket=17) <br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=569075916681)
