# 水质检测传感器 - TDS

## 一、产品简介
&emsp;&emsp;
TDS是Total Dissolved Solids的缩写，中文名总溶解固体，也叫溶解性固体总量，他标识1升水中溶有多少毫克的可溶解性固体。一般来说，TDS值越高，表示水中含有的溶解物越多，水就越不干净；反之，TDS值越低代表水越干净。因此，TDS值的大小，可作为反映水的洁净程度的依据之一。 常用的TDS检测设备为TDS笔，虽然价格低廉，简单易用，但不能把数据传给控制系统，做长时间的在线监测，并做水质状况分析。将TDS探头浸入水中测量水的TDS值。 该产品可应用于生活用水、水培等领域的水质检测。
TDS传感器外观及引脚如下图所示。

<div align="center">
<img src=./../../docs/images/ext_TDS_外观图.png  width=70%/>
</div>

<div align="center">
<img src=./../../docs/images/ext_TDS_引脚图.png  width=70%/>
</div>


引脚定义
* 1：电源负极<br>
* 2：电源正极<br>
* 3：输出模拟信号<br>
* 4：电源指示灯<br>
* 5：TDS表笔接口
<br>

## 二、技术参数
工作电压：3.3V～5.5V<br>
输出电流：0~2.3V<br>
工作电流：3mA~6mA<br>
TDS测量范围：0~1000ppm<br>
TDS测量精度：+-10%F.S.(25摄氏度）
工作温度：-10°C～65°C<br>
存储温度：-20°C～80°C<br>
模块重量：15g<br>
板子尺寸：42mm x 32mm<br>
> 注意：TDS表笔不能用于55摄氏度以上的水中
> TDS探头放置位置不能太靠近容器边缘，否则会影响测量结果
> TDS探头仅头部与导线为防水，可浸入水中，和信号转接板的连线接口处不防水

## 三、软件接口

TDS水质传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/tds)
<br>

### TDS(adcObj,T) - 创建TDS传感器驱动对象
<br>

* 函数原型

> tdsObj = TDS(adcObj,T=25)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|adcObj|ADC|是|用于测量传感器A引脚输出电压<br>调用此函数前需确保adcObj对象已经处于open状态|
|T|Number|否|水温，单位：摄氏度，默认值为25|

* 返回值

> TDS传感器对象成功，返回tdsObj对象；TDS传感器对象创建失败，抛出Exception

* 示例代码
```python
import tds
from driver import ADC

adcDev = ADC()
adcDev.open("tds")

tdsObj=tds.TDS(adcDev, T=26)
print("tdsObj inited!")
```

* 输出
```log
tdsObj inited!
```
</br>

### getTDS() - 获取水质检测结果
<br>

* 函数功能：

>  获取当前水质检测结果

* 函数原型：

> TDS.getTDS()

* 参数说明：

> 无

* 返回值：

> 测量水质并返回水质监测结果，单位：ppm

* 示例：

```python
import tds
from driver import ADC

adcDev = ADC()
adcDev.open("tds")

tdsObj=tds.TDS(adcDev, T=26)
print("tdsObj inited!")

tds = tdsObj.getTDS()
print("TDS measure result:", tds)
```

* 输出
```log
tdsObj inited!
TDS measure result: 153
```

### setTemperature() - 设置水温
<br>

* 函数功能：

>  设置当前水温<br>

* 函数原型：

> TDS.setTemperature(T)

* 参数说明：

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|T|Number|是|水温，单位：摄氏度<br>相同水质下不同温度水的导电率是不同的，所以需要用水温对测量结果进行校准|


* 返回值：

> 无

* 示例：

```python
import tds
from driver import ADC

adcDev = ADC()
adcDev.open("tds")

tdsObj=tds.TDS(adcDev, T=26)
print("tdsObj inited!")

tdsObj.setTemperature(28)
print("set temperature done")

tds = tdsObj.getTDS()
print("TDS measure result:", tds)
```

* 输出
```log
tdsObj inited!
set temperature done
TDS measure result: 151
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为tds的ADC类型的对象。在Python脚本中测量水质检测结果并打印在日志中。

* 案例代码
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "tds": {
        "type": "ADC",
        "port": 0,
        "sampling": 12000000
      }
    }
}
```

```python
import utime
import tds
from driver import ADC

adcDev = ADC()
adcDev.open("tds")

tdsObj=tds.TDS(adcDev, T=26)
print("tdsObj inited!")

tdsObj.setTemperature(28)
print("set temperature done")

while True:
    tds = tdsObj.getTDS()
    print("TDS measure result:", tds)
    utime.sleep(1)
```

* 输出
```log
tdsObj inited!
set temperature done
TDS measure result: 150
TDS measure result: 153
...
```

## 五、工作原理

TDS传感器检测的是“溶解性固体总量”，这些可溶解的物质有很多，包括钙、钠、镁离子和碳酸根离子、碳酸氢根离子、氯离子、硫离子等等。

TDS的检测原理是利用正负电极形成高低电场，让水中的离子向一定的电场方向移动，移动的离子流过的电流形成一定的数值，数值越大，水中的杂质和导电性离子就越多。不过水质条件会影响TDS检测的准确性，如水温太高就不行，同样的水质，温度越高TDS值也会越高。

进行TDS测量需要如下3个步骤：
1. 通过ADC接口读取TDS传感器引脚3的模拟电压测量结果，测量结果经过电压单位转换，用Vt标识，单位：V
2. 用温度值(T，单位：摄氏度)对电压测量结果进行校准,结果用Vc标识，计算公式：Vc=Vt*(1+0.02*(T - 25))
3. 将校准过后的电压转换成水质的ppm值，用T标识，计算公式：T=66.71 * Vc * Vc * Vc - 127.93 * Vc * Vc + 428.7 * Vc，单位：ppm

## 参考文献及购买链接

[1] [购买链接](https://detail.tmall.com/item.htm?id=656324337911&spm=a1z09.2.0.0.61742e8dcTENyL&_u=rb3iuit1671)
