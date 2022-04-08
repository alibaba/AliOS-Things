# 3轴加速度传感器 - ADXL345

## 一、产品简介
ADXL345是一款完整的3轴加速度测量系统，可选择的测量范围有士2g，士4g，士8g或士16g。它既能测量运动或冲击导致的动态加速度，也能测量静止加速度，例如重力加速度，这样的特性让它可作为倾斜传感器使用。该传感器还具备单击 /双击探测，自由落体探测，并允许用户设置一个加速度阀值，当加速度值超过设定阀值后可以产生一个信号输出。所有这些功能都可以映射到2个中断上。

主控板可以通过I2C或SPI和该模块进行通信，本文中的驱动仅提供通过I2C接口和它进行通信，该传感器模组管脚定义及I2C模式下的典型电路请参考下图。

<div align="center">
<img src=./../../docs/images/ADXL345_外观.png  width=30%/>
</div>

|名称|说明|
|:-:|:-|
|GND|地线|
|VCC|电源正极|
|CS|SPI模式下片选信号|
|INT1|中断1引脚|
|INT2|中断2引脚|
|SDO|SPI模式下数据输出引脚|
|SCL|I2C模式下时钟线|
|SDA|I2C模式下数据线|

## 二、ADXL345芯片技术参数
* 工作电压：2.0v~3.6v<br>
* 功耗：40~145uA，待机模式仅0.1uA<br>
* 分辨率：13位<br>
* 三轴加速度计可编程范围：士2g，士4g，士8g或士16g可变量程
* 工作温度：-40°C～+85°C<br>
* 通信接口：400KHz I2C或2MHz SPI<br>
* 加速度计噪声：100μg/√Hz<br>
* 片上16位ADC<br>
* 用户可编程中断<br>
* 封装尺寸：LGA 3mm*5mm*1mm<br>

## 三、软件接口

### ADXL345(i2cObj) - 创建ADXL345驱动对象
* 函数原型：
> gyroDev = ADXL345(i2cObj)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：  
执行成功，返回ADXL345对象；创建失败，抛出Exception

* 示例代码： 

```python
from driver import I2C               # 驱动库
import adxl34x                       # ADXL345加速度计传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("adxl345")                # 按照board.json中名为"adxl345"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADXL345 inited!")

gyroDev = adxl34x.ADXL345(i2cObj)    # 初始化ADXL345传感器对象

i2cObj.close()                       # 关闭I2C设备
del gyroDev                          # 删除ADXL345设备对象
```

* 输出：
```log
ADXL345 inited!
```

### getAcceleration - 创建ADXL345驱动对象
* 函数原型：
> ADXL345.getAcceleration()

* 参数说明：  
无

* 返回值：  
x,y,x三个方向的加速度值，格式：(xa, ya, za)，单位：m/s²。

|参数|类型|说明|
|-----|----|----|
|xa|int|X轴方向的加速度|
|ya|int|X轴方向的加速度|
|za|int|X轴方向的加速度|

* 示例代码： 

```python
from driver import I2C               # 驱动库
import adxl34x                       # ADXL345加速度计传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("adxl345")                # 按照board.json中名为"adxl345"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("ADXL345 inited!")

gyroDev = adxl34x.ADXL345(i2cObj)    # 初始化ADXL345传感器对象

xa, ya, za = gyroDev.getAcceleration()
print("acceleration:", xa, ya, za)

i2cObj.close()                       # 关闭I2C设备
del gyroDev                          # 删除ADXL345设备对象
```

* 输出：  
<xx> <yy> <zz>分别代表x,y,z三个方向的实际加速度测量值
```log
ADXL345 inited!
acceleration: <xx> <yy> <zz>
```

<br>

## 五、通信协议

主控芯片和ADXL345传感器之间通信所用命令字如下表所示：
|序号|寄存器|功能说明|命令字宏定义|
|:-:|:-:|:-|:-|
|1|0x0|读取ADC采样结果|ADS1x15_POINTER_CONVERSION|
|2|0x1|设定ADC采样参数<br> - 目标通道<br> - 增益<br> - 测量模式<br> - 单次/连续测量模式<br> - 测量速率等|ADS1x15_POINTER_CONFIG|
|3|0x2|设定阈值的低8位|ADS1x15_POINTER_LOW_THRESHOLD|
|4|0x3|设定阈值的高8位|ADS1x15_POINTER_HIGH_THRESHOLD|

实际控制此传感器的流程可参考如下伪代码。 
</br>

* 设定ADC量测的参数并读取量测结果
```
    通过I2C向ADS1x15_POINTER_CONFIG寄存器写入ADC量测的参数（目标通道、增益、测量模式、测量速率）
    等待ADC采样结束（时间为一个采样周期（1/采样周期）+一个很小的偏差，如0.1ms）
    通过I2C发送读取ADS11x5的ADS1x15_POINTER_CONVERSION寄存器值的指令
    将测量结果转换成实际电压
```
> 测量的数字结果转化为实际电压算法请参考驱动中的readVoltage函数的实现

## 六、工作原理
ADXL34x传感器属于微型机电系统传感器，主要由硅晶片上的微机械结构组成。该结构由多晶硅弹簧悬挂，当在X，Y和/或Z轴上受到加速度时，它可以在任何方向上平滑偏转。挠曲会导致固定板和连接到悬挂结构的板之间的电容发生变化。每个轴上的电容变化都会转换为与该轴上的加速度成比例的输出电压。该传感器便是通过对此电压进行采样从而再计算的得出每个方向上的加速度。

ADXL34x传感器的系统框图如下图所示：
<div align="center">
<img src=./../../docs/images/adxl345_传感器框图.png  width=60%/>
</div>

## 参考文献及购买链接
[1] [ADXL345 ADC转换器](https://www.ti.com/product/ADXL345)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=17949627089)