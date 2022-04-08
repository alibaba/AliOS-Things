# 电压电流计 - INA219

## 一、产品简介
INA219电压电流计可用于测量负载的电压、电流、功率等参数，可以用于服务器、电脑笔记本、电源、电池管理等电子电路设备提供电流电压值监测提供解决方案。

<div align="center">
<img src=./../../docs/images/ext_电压电流计_定义尺寸.png  width=30%/>
</div>

引脚定义
* VCC：3.3V<br>
* GND：接地<br>
* SCL：I2C时钟
* SDA：I2C数字
* Vin-：负载输入<br>
* Vin+：负载输出
<br>

## 二、技术参数
* 供电电压：3.3V
* 接口：I2C接口，工作频率高达3.4MHz
* 高度精度：精度范围为0.5%
* 工作温度：-40度～125度
* 支持16个可编程地址
* 总线电压：0-26V
* 支持最大电流：3.2A

## 三、软件接口
### INA219(i2cObj) - 创建INA219驱动对象
* 函数原型：
> ina219Obj = INA219(i2cObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
INA219对象成功，返回INA219对象；INA219X对象创建失败，抛出Exception

* 示例代码：

```python
import utime
import ina219
from driver import I2C    # 驱动库

i2cObj = I2C()
i2cObj.open("ina219")
print("ina219 inited!")
ina219Dev = ina219.INA219(i2cObj)
```

* 输出：
```log
ina219 inited!
```

### getVoltage - 测量电压值

* 函数功能：
测量负载的电压值并且返回状态

* 函数原型：
> INA219.getVoltage()

* 参数说明：
无

* 返回值：
成功返回电压测量结果（数据类型：浮点型；单位：V）；失败抛出Exception

* 示例：

```python
import utime
import ina219
from driver import I2C    # 驱动库

i2cObj = I2C()
i2cObj.open("ina219")
print("ina219 inited!")
ina219Dev = ina219.INA219(i2cObj)
while True:
    print("Bus Voltage: %.3f V" % ina219Dev.getVoltage())
    utime.sleep(1)
```

* 输出：
```log
No OTA upgrade.
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ina219 inited!
Bus Voltage: 4.872 V
Bus Voltage: 4.868 V
Bus Voltage: 4.868 V
Bus Voltage: 4.868 V
```

### getCurrent - 测量电流值

* 函数功能：
测量负载的电流值并且返回测量结果

* 函数原型：
> INA219.getCurrent()

* 参数说明：
无

* 返回值：
成功返回负载电流测量结果（数据类型：浮点；单位：mA）；失败抛出Exception

* 示例：

```python
import utime
import ina219
from driver import I2C    # 驱动库

i2cObj = I2C()
i2cObj.open("ina219")
print("ina219 inited!")
ina219Dev = ina219.INA219(i2cObj)
while True:
    print("Current: %.3f mA" % ina219Dev.getCurrent())
    utime.sleep(1)
```

* 输出：
```log
No OTA upgrade.
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ina219 inited!
Current: 3.305 mA
Current: 3.402 mA
Current: 3.305 mA
Current: 3.195 mA
```

### getPower - 获取功率值

* 函数功能：
测量负载的功率并且返回测量结果

* 函数原型：
> INA219.getPower()

* 参数说明：
无

* 返回值：
成功返回负载的功率测量结果（数据类型：浮点；单位：mW）；失败抛出Exception

* 示例：

```python
import utime
import ina219
from driver import I2C    # 驱动库

i2cObj = I2C()
i2cObj.open("ina219")
print("ina219 inited!")
ina219Dev = ina219.INA219(i2cObj)
while True:
    print("Power: %.3f mW" % ina219Dev.getPower())
    utime.sleep(1)
```

* 输出：
```log
No OTA upgrade.
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ina219 inited!
Power: 15.610 mW
Power: 16.098 mW
Power: 16.098 mW
Power: 15.610 mW
```

## 四、接口案例
此使用实例在board.json中定义了名为ina219的I2C类型的对象。在Python脚本中周期性的获取负载的测量电压、电流、功率结果并打印在日志中。

* 代码：
```python
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "ina219": {
      "type": "I2C",
      "port": 0,
      "addrWidth": 7,
      "freq": 400000,
      "mode": "master",
      "devAddr": 64
     }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}

```
```python
# Python代码
import utime
import ina219
from driver import I2C    # 驱动库

i2cObj = I2C()
i2cObj.open("ina219")
print("ina219 inited!")
ina219Dev = ina219.INA219(i2cObj)
while True:
    print("Bus Voltage: %.3f V" % ina219Dev.getVoltage())
    print("Current: %.3f mA" % ina219Dev.getCurrent())
    print("Power: %.3f mW" % ina219Dev.getPower())
    utime.sleep(1)
```

* 输出：
```log
No OTA upgrade.
mount fs
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ina219 inited!
Bus Voltage: 4.872 V
Current: 3.305 mA
Power: 15.610 mW
Bus Voltage: 4.868 V
Current: 3.402 mA
Power: 16.098 mW
Bus Voltage: 4.868 V
Current: 3.305 mA
Power: 16.098 mW
Bus Voltage: 4.868 V
Current: 3.195 mA
Power: 15.610 mW
Bus Voltage: 4.864 V
Current: 3.195 mA
Power: 16.098 mW
```

<br>

## 五、工作原理

INA219传感器寄存器描述如下表所示：
|序号|寄存器地址|功能说明|默认值|
|:-:|:-:|:-:|:-|
|1|0x00|配置寄存器|0x399F|
|2|0x01|分流电压寄存器|--|
|3|0x02|总线电压寄存器|--|
|4|0x03|功率寄存器|0x00|
|5|0x04|当前寄存器|0x00|
|6|0x05|校准寄存器|0x00|

</br>

从下图可以看到INA219内置I2C通信单元，与MCU进行数据交互。通过ADC采样输入到数据处理和线性化单元，同时带有校正储存器，处理环境对器件测量的影响。带警报引脚，可通过修改寄存器的值设定阈值，当测量的温湿度超过阈值时它会被置位。<br>
<div align="center">
<img src=./../../docs/images/ext_ina219_工作原理.png  width=50%/>
</div>

引脚分布如下：

|序号|引脚名|功能说明|
|:-:|:-:|:-:|
|1|Data|I2C数据线|
|2|CLK|I2C时钟线|
|3|GND|地线引脚|
|4|VCC|电源输入引脚|
|5|Vin-|接负载低电压端引线|
|6|Vin+|接负载高电压端引线|
|7|A0|可编程地址线|
|8|A1|可编程地址线|
<br>

 ## 参考文献及购买链接
[1] [INA219电压电流计传感器介绍](http://www.elecfans.com/d/1067986.html)<br>
[2] [INA219电压电流计模块介绍](https://detail.tmall.com/item.htm?id=599140970431)
