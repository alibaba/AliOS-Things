# 超声测距传感器 - URM37

## 一、产品简介
URM37是一款功能强大的超声波传感器模块，内置温度补偿，确保在温度变化的应用场景中实现准确测距。它具有丰富的接口，集成了开关量、串口（TTL和RS232电平可选）、脉宽输出、模拟量输出等功能。模块还具备舵机角度驱动测距的功能，可以通过外接一个舵机组成一个空间超声波扫描仪。

<div align="center">
<img src=./../../docs/images/ext_urm37_pins.png width=50%/>
</div>

引脚定义
* VCC：电源输入（3.3V~5.5V）
* GND：电源地
* NRST：模块复位，低电平复位（不用时可以悬空）
* ECHO：测量到的距离数据以PWM脉宽方式输出0~25000us，每50us代表1厘米
* SERVO：舵机控制脚
* COMP/TRIG：COMP：比较模式开关量输出，测量距离小于设置比较距离时输出低电平 / TRIG：PWM模式触发脉冲输入
* DAC_OUT：模拟电压输出，电压和测量距离成正比
* RXD：异步通讯模块接收数据管脚，RS232电平或者TTL电平
* TXD：异步通讯模块发送数据管脚，RS232电平或者TTL电平

## 二、技术参数
* 工作电源：+3.3V~+5.5V
* 工作电流：20mA
* 工作温度范围 ：-10℃~＋70℃
* 有效测距量程：2cm~800cm（极限量程1000cm）
* 分辨率：1cm
* 精度：1%
* 测量周期：100ms（最大）
* 模块尺寸：22mm × 51mm
* 模块重量：约25g

## 三、软件接口

URM37超声测距传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/urm37)
<br>

### URM37(uartObj) - 创建URM37驱动对象
* 函数原型：
> urm37Obj = URM37(uartObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|uartObj|UART|是|调用此函数前需确保uartObj对象已经处于open状态|

* 返回值：
URM37对象成功，返回URM37对象；URM37对象创建失败，抛出Exception。

* 示例代码：

```python
import urm37

uartObj = UART()
if uartObj.open('serial2') != 0:
    print('UART init error!')
    return
urm37Obj = urm37.URM37(uartObj)
print("URM37 inited!")
```

* 输出：
```log
URM37 inited!
```

### getRange - 获取测距值

* 函数功能：
获取测距值。

* 函数原型：
> URM37.getRange()

* 参数说明：
无

* 返回值：
返回整数值。失败时返回65535；成功时返回距离测量值，单位cm。

* 示例：

```python
import urm37
from driver import UART

uartObj = UART()
if uartObj.open('serial2') != 0:
    print('UART init error!')
    return
urm37Obj = urm37.URM37(uartObj)
print("URM37 inited!")
dist = urm37Obj.getRange()
if dist != 65535:
    print("Distance: %d cm" % dist)
```

* 输出：
```log
URM37 inited!
Distance: 37 cm
```

### getTemperature - 获取温度

* 函数功能：
获取温度。

* 函数原型：
> URM37.getTemperature()

* 参数说明：
无

* 返回值：
返回浮点值。失败时返回-4095.0；成功时返回温度测量值，单位摄氏度。

* 示例：

```python
import urm37
from driver import UART

uartObj = UART()
if uartObj.open('serial2') != 0:
    print('UART init error!')
    return
urm37Obj = urm37.URM37(uartObj)
print("URM37 inited!")
temp = urm37Obj.getTemperature()
if temp != -4096.0:
    print("Temperature: %.1f" % temp)
```

* 输出：
```log
URM37 inited!
Temperature: 23.7
```

## 四、接口案例
此使用实例在board.json中定义了名为serial2的UART节点。在Python脚本中周期性地获取距离测量值、温度并打印在日志中。

* 代码：
```python
# board.json配置：
{
    "name": "m5stackcore2",
    "version": "1.0.0",
    "io": {
      "serial2": {
        "type": "UART",
        "port": 2,
        "dataWidth": 8,
        "baudRate": 9600,
        "stopBits": 1,
        "flowControl": "disable",
        "parity": "none"
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
# Python代码
import utime
import urm37
from driver import UART

uartObj = UART()
if uartObj.open('serial2') != 0:
    print('UART init error!')
    return
urm37Obj = urm37.URM37(uartObj)
print("URM37 inited!")
while True:
    dist = urm37Obj.getRange()
    if dist != 65535:
        print("Distance: %d cm" % dist)
    temp = urm37Obj.getTemperature()
    if temp != -4096.0:
        print("Temperature: %.1f" % temp)
    utime.sleep(1)
```

* 输出：
```log
URM37 inited!
Distance: 37 cm
Temperature: 23.7
Distance: 51 cm
Temperature: 23.5
```

## 五、工作原理

测距的模式有三种：一PWM触发测量模式、二自动测量模式、三串口被动测量。还具有以下特性：模拟量输出（和测量距离正比）、温度读取、和串口电平方式设置（TTL或者RS232电平）、内部EEPROM掉电不丢失数据保存、串口读取EEPROM数据等。URM37出厂时我们进行了严格的测试，用户购买到之后可根据自己的需求进行相关设置，首先是串口电平方式的设置（TTL或者RS232电平），设置好之后我们就可以通过串口对模块进行访问，然后设置测距模式（对内部EEPROM地址0x02写入数据），之后就可以通过MCU或者PC对超声波模块驱动。

## 六、资料及购买链接
[1] [URM37资料](http://wiki.dfrobot.com.cn/index.php?title=(SKU:SEN0001)URM37V5.0%E8%B6%85%E5%A3%B0%E6%B3%A2%E6%B5%8B%E8%B7%9D%E4%BC%A0%E6%84%9F%E5%99%A8)
<br>
[2] [URM37购买链接](https://item.taobao.com/item.htm?id=554819705693)
