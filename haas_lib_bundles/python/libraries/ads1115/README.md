# ADC转换器 - ADS1115

## 一、产品简介
ADS1115是德州仪器出品的一个超小，低功率，16位精度并带有内部参考电压的AD转换器（Analog to Digital Converter）。主要运用于高精度仪表，汽车电子，蓄电池电压采集等高精度采集场合。

ADS1115传感器外观及引脚功能说明如下：
<div align="center">
<img src=./../../docs/images/ADS1115_外观图.png  width=20%/>
</div>

|名称|说明|
|:-:|:-|
|V|电源正极|
|G|地线|
|SCL|I2C总线的时钟线|
|SDA|I2C总线的数据线|
|ADDR|I2C接口地址选择线|
|ALERT|数字比较器输出或转换完成信号|
|A0|通道0|
|A1|通道1|
|A2|通道2|
|A2|通道3|

## 二、技术参数
* 供电电压：2.0V ~ 5.5V
* 采用I2C通信协议，最高传输速率可到3.4MHz，I2C地址是0X48
* ADC测量速率：8Bps~860Bps，可调节
* 支持4个单通道输入或2路差分输入
* 可采集电压范围：+/-6.144V
* 支持单次测量和连续测量模式
* 芯片封装：QFN-10封装或MSP-10，2mm × 1.5mm × 0.4mm
* 传感器尺寸：26mm x 9.3mm

## 三、软件接口

ADS1115 ADC转换器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ads1115)
<br>

### ADS1115(i2cObj) - 创建ADS1115驱动对象
* 函数原型：
> adcObj = ADS1115(i2cObj)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：  
执行成功，返回ADS1115对象；创建失败，抛出Exception

* 示例代码： 

```python
from driver import I2C               # 驱动库
import ads1x15                       # ADS1x15系列ADC传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # 按照board.json中名为"ads1115"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADS1115 inited!")

adcDev = ads1x15.ADS1115(i2cObj)  # 初始化ADS1115传感器对象

i2cObj.close()                    # 关闭I2C设备
del adcDev                        # 删除ADS1115设备对象
```

* 输出：
```log
ADS1115 inited!
```

### readVoltage - 读取ADC通道上的输入电压

* 函数功能：  
量测ADC通道的输入电压

* 函数原型：
> ADS1115.readVoltage(channel)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|channel|int|是|通道ID, 范围[0, 1, 2, 3]，分别对应A0, A1, A2, A3通道|


* 返回值：  
返回目标通道的输入电压，单位：mV

* 示例： 

```python
from driver import I2C               # 驱动库
import ads1x15                       # ADS1x15 ADC传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # 按照board.json中名为"ads1115"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADS1115 inited!")

adcDev = ads1x15.ADS1115(i2cObj)  # 初始化ADS1115传感器对象

temp = adcDev.readVoltage(0)      # 量测A0通道的输入电压值
print("A0 input voltage is ", temp, " mV")

i2cObj.close()                    # 关闭I2C设备
del adcDev                        # 删除ADS1115设备对象
```

* 输出：
```log
ADS1115 inited!
A0 input voltage is 1100 mV
```

### setMaxVoltage - 设置量程

* 函数功能：  
设置ADS1115传感器的最大量程

* 函数原型：
> ADS1115.setMaxVoltage(maxVoltage)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|maxVoltage|int|是|设置待量测电压最大电压，单位：mV<br>需小于ADS1115最大可测量电压（6144）|

> 如果待量测电压的范围为[-vMin, +vMax]，则maxVoltage取max(vMin, vMax)， 即vMin和vMax中的最大值。<br>
> 此API并非必须调用，但合理设置此量程可以获得更高的测量精度，ADS1115提供了+/-6144, +/-4096， +/-2048， +/-1024，+/-512，+/-256几个级别，如果maxVoltage非6144/4096/2048/1024/512/256里边的值，则此函数中会向上选取最接近的量程。

* 返回值：  
成功返回0；失败抛出Exception

* 示例： 

```python
from driver import I2C               # 驱动库
import ads1x15                       # ADS1x15 ADC传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # 按照board.json中名为"ads1115"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADS1115 inited!")

adcDev = ads1x15.ADS1115(i2cObj)  # 初始化ADS1115传感器对象

adcDev.setMaxVoltage(3300)           # 设置电压量测范围为+/-3300mV
print("setMaxVoltage done")

i2cObj.close()                    # 关闭I2C设备
del adcDev                        # 删除ADS1115设备对象
```

* 输出：
```log
ADS1115 inited!
setMaxVoltage done
```

## 四、接口案例  
此使用实例在board.json中定义了名为ads1115的I2C类型的对象。在Python脚本中周期性的量测输入电压并打印到日志中。

* board.json配置：

```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "ads1115": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 72
        }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

* 案例代码：

```python
from driver import I2C               # 驱动库
import ads1x15                       # ADS1x15 ADC传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # 按照board.json中名为"ads1115"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADS1115 inited!")

adcDev = ads1x15.ADS1115(i2cObj)  # 初始化ADS1115传感器对象

while True:
    temp = adcDev.readVoltage(0)      # 量测A0通道的输入电压值
    print("A0 input voltage is ", temp, " mV")

i2cObj.close()                    # 关闭I2C设备
del adcDev                        # 删除ADS1115设备对象
```

* 输出：
```python
ADS1115 inited!
...
A0 input voltage is 1100 mV
...
A0 input voltage is 1121 mV
...
```

<br>

## 五、通信协议

主控芯片和ADS1115传感器之间通信所用命令字如下表所示：
|序号|寄存器|功能说明|命令字宏定义|
|:-:|:-:|:-|:-|
|1|0x0|读取ADC采样结果|ADS1x15_POINTER_CONVERSION|
|2|0x1|设定ADC采样设定<br>（目标通道、增益、测量模式、单次/连续测量模式、测量速率等）|ADS1x15_POINTER_CONFIG|
|3|0x2|设定阈值的低8位|ADS1x15_POINTER_LOW_THRESHOLD|
|4|0x3|设定阈值的高8位|ADS1x15_POINTER_HIGH_THRESHOLD|

实际控制此传感器的流程可参考如下伪代码。 
</br>

* 设定ADC量测的参数并读取量测结果
```python
    通过I2C向ADS1x15_POINTER_CONFIG寄存器写入ADC量测的参数（目标通道、增益、测量模式、测量速率）
    等待ADC采样结束（时间为一个采样周期（1/采样周期）+一个很小的偏差，如0.1ms）
    通过I2C发送读取ads1x15的ADS1x15_POINTER_CONVERSION寄存器值的指令
    将测量结果转换成实际电压
```
> 测量的数字结果转化为实际电压算法请参考驱动中的readVoltage函数的实现

## 六、工作原理
从下图可以看到th06-si7006内部集成了湿度传感器和温度传感器，通过ADC采样输入到校正储存器。通过数字接口IIC读取数据。
<div align="center">
<img src=./../../docs/images/ADS1115_框图.png width=50%/>
</div>

其中AI0, AI1, AI2, AI3为测量引脚，ALERT/RDY为中断引脚，ADDR为可变地址控制引脚，SCL/SDA为I2C总线引脚。


典型电路如下，由于SCL和SDA为开漏输出，驱动能力不足，需要接上拉电阻。VDD和VSS之间接一个小电容滤除高频杂波，GND引脚接地即可。

<div align="center">
<img src=./../../docs/images/ADS1115_典型电路.png  width=25%/>
</div>


## 参考文献及购买链接
[1] [ADS1115 ADC转换器](https://www.ti.com/product/ADS1115)<br>
[2] [购买链接](https://item.taobao.com/item.htm?id=521133121509)
