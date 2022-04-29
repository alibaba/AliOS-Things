# 空气质量检测模块 - CCS811

## 一、产品简介
通过CCS811传感器模块可以测量环境中TVOC(总挥发性有机物质)浓度和eCO2(二氧化碳)浓度，作为衡量空气质量（IAQ）的指标。

<div align="center">
<img src=./../../docs/images/ccs811-front.png  width=30%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：接地<br>
* SCL：I2C时钟<br>
* SDA：I2C数字<br>
* WAK：接地<br>
* INT：NC（阈值中断引脚）<br>
* RST：NC（复位引脚）<br>
* ADD：NC<br>

## 二、技术参数
* 供电电压：3.3V
* 接口：I2C接口
* 分辨率：14位
* 工作电流：20mA
* 睡眠模式的电流：3uA
* 模块尺寸：15mm * 21mm

## 三、软件接口
CCS811传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ccs811)
### CCS811(i2cObj) - 创建CCS811驱动对象
* 函数原型：
> ccs811Obj = CCS811(i2cObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
CCS811对象创建成功，返回CCS811对象；CCS811对象创建失败，抛出Exception

* 示例代码：

```python
from driver import I2C
import ccs811

i2cObj = I2C()
i2cObj.open("ccs811")
devObj = ccs811.CCS811(i2cObj)
print("ccs811 inited!")
```

* 输出：
```log
ccs811 inited!
```

### geteCO2 - 获取空气中二氧化碳浓度

* 函数功能：
获取空气中二氧化碳浓度

* 函数原型：
> CCS811.geteCO2()

* 参数说明：
无

* 返回值：

|返回值|类型|说明|
|-----|----|----|
|status|整型|0-无效数值；<br>1-有效数值|
|eco2|整型|单位：ppm，>=400|

* 示例：

```python
from driver import I2C
import ccs811
import utime

i2cObj = I2C()
i2cObj.open("ccs811")
devObj = ccs811.CCS811(i2cObj)
print("ccs811 inited!")
while True:
    status,eco2 = devObj.geteCO2()
    if status == 1:
        print('eCO2: %d ppm' %(eco2))
    utime.sleep(1)
```

* 输出：
```log
ccs811 inited!
eCO2: 400 ppm
eCO2: 868 ppm
eCO2: 400 ppm
eCO2: 400 ppm
```

### getTVOC - 获取空气中挥发有机物的浓度

* 函数功能：
获取空气中挥发有机物的浓度

* 函数原型：
> CCS811.getTVOC()

* 参数说明：
无

* 返回值：

|返回值|类型|说明|
|-----|----|----|
|status|整型|0-无效数值；<br>1-有效数值|
|tvoc|整型|单位：ppb|

* 示例：

```python
from driver import I2C
import ccs811
import utime

i2cObj = I2C()
i2cObj.open("ccs811")
devObj = ccs811.CCS811(i2cObj)
print("ccs811 inited!")
while True:
    status,tvoc = devObj.getTVOC()
    if status == 1:
        print('TVOC: %d ppb' %(tvoc))
    utime.sleep(1)
```

* 输出：
```log
ccs811 inited!
TVOC: 0 ppb
TVOC: 0 ppb
TVOC: 0 ppb
TVOC: 3 ppb
TVOC: 734 ppb
TVOC: 68 ppb
TVOC: 129 ppb
TVOC: 62 ppb
TVOC: 25 ppb
```

### geteCO2TVOC - 获取空气中二氧化碳以及挥发有机物的浓度

* 函数功能：
获取空气中二氧化碳以及挥发有机物的浓度

* 函数原型：
> CCS811.geteCO2TVOC()

* 参数说明：
无

* 返回值：

|返回值|类型|说明|
|-----|----|----|
|status|整型|0-无效数值；<br>1-有效数值|
|eco2|整型|单位：ppm,>=400|
|tvoc|整型|单位：ppb|

* 示例：

```python
from driver import I2C
import ccs811
import utime

i2cObj = I2C()
i2cObj.open("ccs811")
devObj = ccs811.CCS811(i2cObj)
print("ccs811 inited!")
while True:
    status,eco2,tvoc = devObj.geteCO2TVOC()
    if status == 1:
        print('eCO2: %d ppm, TVOC: %d ppb' % (eco2, tvoc))
    utime.sleep(1)
```

* 输出：
```log
ccs811 inited!
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 1495 ppm, TVOC: 168 ppb
eCO2: 481 ppm, TVOC: 12 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
```

## 四、接口案例
此使用实例在board.json中定义了名为CCS811的I2C类型的对象。在Python脚本中周期性的获取二氧化碳、有机物浓度并打印在日志中。

* 代码：
board.json配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "ccs811": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 90
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
# Python代码
from driver import I2C
import ccs811
import utime

i2cObj = I2C()
i2cObj.open("ccs811")
devObj = ccs811.CCS811(i2cObj)
print("ccs811 inited!")
while True:
    status,eco2,tvoc = devObj.geteCO2TVOC()
    if status == 1:
        print('eCO2: %d ppm, TVOC: %d ppb' % (eco2, tvoc))
    utime.sleep(1)
```

* 输出：
```log
ccs811 inited!
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 1495 ppm, TVOC: 168 ppb
eCO2: 481 ppm, TVOC: 12 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
eCO2: 400 ppm, TVOC: 0 ppb
```

<br>

## 五、工作原理

INA219传感器寄存器描述如下表所示：
|序号|寄存器地址|功能说明|默认值|
|:-:|:-:|:-:|:-|
|1|0x00|状态寄存器|--|
|2|0x01|测量模式寄存器|--|
|3|0x02|测量结果寄存器|--|
|4|0x20|硬件ID寄存器|0x81|
|5|0x21|硬件版本寄存器|0x1X|
|6|0xF4|APP启动寄存器|--|
|7|0xFF|软件重启寄存器|--|
</br>

从下图可以看到CCS811内置I2C通信单元，与MCU进行数据交互。其中nINT和nRESET是可选的，根据实际项目需要而定，nWAKE低电平状态为激活CCS811。<br>
<div align="center">
<img src=./../../docs/images/ccs811-sch.png  width=70%/>
</div>

引脚分布如下：

|序号|引脚名|功能说明|
|:-:|:-:|:-:|
|1|I2C_ADDR|低电平 - I2C地址总线0x5A </br> 高电平 - I2C地址总线0x5B|
|2|nRESET|复位引脚，低电平有效|
|3|nINT|阈值中断引脚|
|4|PWM|--|
|5|Sense|--|
|6|VDD|电源正|
|7|nWAKE|唤醒引脚，低电平有效|
|8|AUX|--|
|9|I2C_SDA|I2C数据线|
|10|I2C_SCK|I2C时钟线|
<br>

 ## 参考文献及购买链接
[1] [CCS811传感器DATASHEET](https://www.docin.com/p-1777585777.html)<br>
[2] [CCS811传感器模块购买](https://detail.tmall.com/item.htm?spm=a1z10.3-b.w4011-21581912015.17.123b5535z3NXou&id=598782110904&rn=9480f4ac043fa94be0e5c8a2a5a264f0&abbucket=7)
