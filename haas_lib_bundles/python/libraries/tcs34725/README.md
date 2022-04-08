# 颜色传感器 - tcs34725

## 一、产品简介
TCS34725是一款低成本，高性价比的RGB全彩颜色识别传感器，传感器通过光学感应来识别物体的表面颜色。支持红、绿、蓝(RGB)三基色和明光感应，可以输出RGB三色的数值以还原周围颜色。

本文介绍的颜色传感器外如下图所示：

<div align="center">
<img src=./../../docs/images/TCS34725_外观图.png  width=20%/>
</div>

其管脚定义如下表所示：
|管脚名称|管脚功能|说明|
|:-|:-|:-|
|LED|LED开关|高电平或悬空LED灯亮、低电平LED灯灭|
|INT|中断输出|低电平有效|
|VCC|电源正极|3.3V-5V|
|GND|电源地|-|
|SCL|I2C接口时钟线|-|
|SDA|I2C接口数据线|-|

## 二、颜色传感器技术参数
* 工作电压：3.3-5V<br>
* 检测距离：3mm-10mm<br>
* 工作温度：-30°C～+70°C<br>
* 通信接口：I2C，最高频率：400k<br>
* 工作电流：65uA<br>

## 三、软件接口

### TCS34725(i2cObj) - 创建颜色传感器驱动对象
* 函数原型：
> colorDev = TCS34725(i2cObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
执行成功，返回颜色传感器对象；创建失败，抛出Exception

* 示例代码：

```python
import utime
from driver import I2C
i2cObj = I2C()
i2cObj.open('colorSensor')

colorSensor = TCS34725(i2cObj)

print('colorSensor init done')
```

* 输出：
```log
colorSensor init done
```

### getRGB - 获取环境颜色的RGB值
* 函数原型：
> TCS34725.getRGB()

* 参数说明：
无

* 返回值：
(R,G,B)分别代表红色（R）、绿色(G)和蓝色(B)三个通道上面的亮度
> 此API获取到的返回结果经过了3个算法处理，如果想获取颜色传感器的直接测量结果，请参考getRawData的说明。

* 示例代码：

```python
import utime
from driver import I2C
i2cObj = I2C()
i2cObj.open('colorSensor')

colorSensor = TCS34725(i2cObj)
print('colorSensor init done')

r, g, b = colorSensor.getRGB()
print('r:0x%x, g:0x%x, b:0x%x' % (r, g, b))
```

* 输出：
<R> <G> <B>分别是对实际颜色的测量结果
```log
colorSensor init done
r:<R>, g:<G>, b:<B>
```

### getRawData - 获取颜色传感器原始测量结果
* 函数原型：
> TCS34725.getRawData()

* 参数说明：
无

* 返回值：
(r, g, b, c)分别代表红色（r）、绿色(g)、蓝色(b)和归一参考值(c)

* 示例代码：

```python
import utime
from driver import I2C
i2cObj = I2C()
i2cObj.open('colorSensor')

colorSensor = TCS34725(i2cObj)
print('colorSensor init done')

r, g, b, c = colorSensor.getRawData()
print('r:0x%x, g:0x%x, b:0x%x, c:0x%x' % (r, g, b, c))
```

* 输出：
<R> <G> <B> <C>分别是颜色传感器实际测量结果
```log
colorSensor init done
r:<R>, g:<G>, b:<B>, c:<C>
```

### calculateLux - 计算亮度值
* 函数原型：
> TCS34725.calculateLux(r, g, b)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|r|int|是|颜色传感器测量结果原始值中的红色分量, 通过getRawData获取|
|g|int|是|颜色传感器测量结果原始值中的绿色分量, 通过getRawData获取|
|b|int|是|颜色传感器测量结果原始值中的绿色分量, 通过getRawData获取|

* 返回值：
亮度值，单位：流明

* 示例代码：

```python
import utime
from driver import I2C
i2cObj = I2C()
i2cObj.open('colorSensor')

colorSensor = TCS34725(i2cObj)
print('colorSensor init done')

r, g, b, c = colorSensor.getRawData()

lux = colorSensor.calculateLux(r, g, b)
print('luminosity:', lux)
```

* 输出：
{L}代表计算后的亮度值
```log
colorSensor init done
luminosity:{L}
```

### calculateCT - 计算色温
* 函数原型：
> TCS34725.calculateCT(r, g, b)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|r|int|是|颜色传感器测量结果原始值中的红色分量, 通过getRawData获取|
|g|int|是|颜色传感器测量结果原始值中的绿色分量, 通过getRawData获取|
|b|int|是|颜色传感器测量结果原始值中的绿色分量, 通过getRawData获取|

* 返回值：
色温值

* 示例代码：

```python
import utime
from driver import I2C
i2cObj = I2C()
i2cObj.open('colorSensor')

colorSensor = TCS34725(i2cObj)
print('colorSensor init done')

r, g, b, c = colorSensor.getRawData()

colorTemperature = colorSensor.calculateCT(r, g, b)
print('color temperature:', colorTemperature)
```

* 输出：
{CT}代表转换后的色温值
```log
colorSensor init done
color temperature:{CT}
```

## 四、接口案例  
此使用实例在board.json中定义了名为colorSensor的I2C类型的对象。在Python脚本中周期性的量测输入电压并打印到日志中

* 代码：

board.json配置如下：
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "colorSensor": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 41
        }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
import utime
from driver import I2C

i2cObj = I2C()                    # 创建I2C设备对象
i2cObj.open('colorSensor')        # 打开名为colorSensor的I2C设备节点

colorSensor = TCS34725(i2cObj)    # 创建颜色传感器对象
print('colorSensor init done')

while True:
    r, g, b = colorSensor.getRGB()                # 读取RGB测量结果
    print('r:0x%x, g:0x%x, b:0x%x' % (r, g, b))

    r, g, b, c = colorSensor.getRawData()         # 读取颜色传感器测量的原始数据

    lux = colorSensor.calculateLux(r, g, b)       # 将原始数据转化成亮度值
    print('luminosity:', lux)

    CT = colorSensor.calculateCT(r, g, b)         # 将原始数据转化成色温值
    print('color temperature:', CT)

i2cObj.close()                    # 关闭I2C设备
del colorSensor                   # 删除颜色传感器对象
```

* 输出：

其中：
* {CT}代表转换后的色温值
* {L}代表计算后的亮度值

```python
colorSensor init done

luminosity:{L}
color temperature:{CT}
```
<br>

## 五、工作原理

TCS34725传感器属于光电传感器，光线进入该传感器后会由光电二极管阵列将入射光转换为R、G、B的光电流，再经过放大转换成模拟电压，模拟电压经过模数转换元件转换成代表电压的数字信号，再对该数字信号进行处理来得到R、G、B测量值。该传感器功能外观图如下图所示：
<div align="center">
<img src=./../../docs/images/TCS34725_功能框图.png  width=60%/>
</div>

TCS34725传感器的工作过程也是典型的颜色传感器，主要经过**光到光电流** -> **电流放大并转化成模拟电压** -> **模拟电压信号转换成数字电压信号**几个步骤。

光到光电流的转换动作是由搭配色彩滤波器的光电二极管阵列完成；电流放大并转换成电压则有一个阻抗放大器来完成；模拟电压信号转换为数字信号则是由模拟数字转换器来完成。

颜色信息转化为数字信息之后，便会存储在芯片内部的寄存器中，主控芯片可以通过传感器对外的接口（TCS34725是通过I2C接口）对颜色信息进行读取。

## 参考文献及购买链接
[1] [TCS34725数据手册](https://ams.com/documents/20143/36005/TCS3472_DS000390_3-00.pdf)<br>
[2] [颜色传感器工作原理](http://www.elecfans.com/yuanqijian/sensor/20180309645056.html)<br>
[3] [购买链接](https://detail.tmall.com/item.htm?id=41323693774)


