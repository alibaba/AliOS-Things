# 温湿度传感器 - BMP280

## 一、产品简介
BMP280是博世出品的气压传感器，它集成了压电压力传感单元、信号处理电路及模数转换器,气压值可以通过SPI或I2C读出,在室内导航、无人机、气象站等场景有广泛应用。

<div align="center">
<img src=./../../docs/images/ext_bmp280_实物.png  width=50%/>
</div>

### 引脚定义
* VCC: 3.3V
* GND: GND
* SCL: IIC时钟
* SDA: IIC数据
* CSB: 3.3V
* SDO: GND
## 二、技术参数
* 供电电压:1.9~3.6V
* 可以采用I2C或SPI通信协议（HaaS Python实现了IIC接口驱动），I2C最高传输速率可到3.4MHz，SPI最高传输速率可达10MHz，当CSB管脚接VCC时选择为IIC接口，当CSB管脚接GND时选择为SPI接口，I2C地址可配置0X76或者0x77,取决于SDO，SDO接GND时I2C地址维0x76，SDO接VCC时I2C地址为0x77
* 气压测量范围： 300hPa至1100hPa，最大误差为±12hPa
* 封装:8-pin LGA metal-lid，2.0mm * 2.5mm, height: 0.95mm
* 工作温度范围: -40摄氏度 - +85摄氏度

## 三、软件接口

### BMP280(i2cObj) - 创建BMP280驱动对象
* 函数原型：
> bmpObj = BMP280(i2cObj)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值:
BMP280对象成功，返回BMP280对象；BMP280对象创建失败，抛出Exception

* 示例代码:

```python
from driver import I2C               # 驱动库
import bmp280                        # BMP280温湿度传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("bmp280")                # 按照board.json中名为"bmp280"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("bmp280 inited!")

baroDev = bmp280.BMP280(i2cObj)  # 初始化BMP280传感器
```

* 输出:
```log
bmp280 inited!
```

### getPressure - 测量气压

* 函数功能:
量测气压值并返回测量结果

* 函数原型:
> BMP280.getPressure()

* 参数说明:
无

* 返回值:
成功返回温度测量结果（数据类型：浮点型；单位：Pa）；失败抛出Exception

* 示例:

```python
from driver import I2C               # 驱动库
import bmp280                        # BMP280温湿度传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("bmp280")                # 按照board.json中名为"bmp280"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象

baroDev = bmp280.BMP280(i2cObj)  # 初始化BMP280传感器

pressure = baroDev.getPressure()
print("pressure is ", pressure, "Pa")

```

* 输出:
```log
pressure is 103000Pa
```

## 四、接口案例
此使用实例在board.json中定义了名为bmp280的I2C类型的对象。在Python脚本中周期性的获取温湿度测量结果并打印在日志中。

* 代码:
```json
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "bmp280": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 118
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}

```
```python
# Python代码
import utime                         # 延时API所在组件
from driver import I2C               # 驱动库
import bmp280                        # BMP280温湿度传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("bmp280")                # 按照board.json中名为"bmp280"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象

baroDev = bmp280.BMP280(i2cObj)  # 初始化BMP280传感器

# 通过温湿度传感器读取温湿度信息
def get_pressure():
    pressure = baroDev.getPressure()         # 获取大气压测量结果
    print("The pressure is: %.1f" % pressure)

    return pressure                        # 返回读取到的大气压值

if __name__ == '__main__':

    while True:
        data = get_pressure()                          # 读取温度信息和相对湿度信息
        utime.sleep(1)                                  # 每隔1秒钟进行一次温湿度信息测量和打印

    i2cObj.close()                                      # 关闭I2C设备对象
```

* 输出:
```log
...
The pressure is 103000Pa
...

```

<br>

## 五、通信协议
主控芯片和 BMP280 传感器之间通信所用命令字如下表所示：
|序号|命令字|功能说明|命令字宏定义|
|:-:|:-:|:-|:-|
|1|0x88|读T1校准参数|CMD_READ_DIG_T1|
|2|0x82|读T2校准参数|CMD_READ_DIG_T2|
|3|0x8c|读T3校准参数|CMD_READ_DIG_T3|
|4|0x8e|读P1校准参数|CMD_READ_DIG_P1|
|5|0x90|读P2校准参数|CMD_READ_DIG_P2|
|6|0x92|读P3校准参数|CMD_READ_DIG_P3|
|7|0x94|读P4校准参数|CMD_READ_DIG_P4|
|8|0x96|读P5校准参数|CMD_READ_DIG_P5|
|9|0x98|读P6校准参数|CMD_READ_DIG_P6|
|10|0x9a|读P7校准参数|CMD_READ_DIG_P7|
|11|0x9c|读P8校准参数|CMD_READ_DIG_P8|
|12|0xFA|读取温度|CMD_READ_TEMPERATURE|
|13|0xF7|读取大气压|CMD_READ_PRESSURE|

实际控制此传感器的流程可参考如下描述。
</br>

* 一次获取数据
```python
通过 IIC 发送CMD_READ_DIG_T1-CMD_READ_DIG_T3获取温度校准参数并且发送CMD_READ_DIG_P1-CMD_READ_DIG_P8获取气压校准参数；
通过 IIC 发送CMD_READ_TEMPERATURE获取温度RAW数据，通过温度校准参数计算
得到实际摄氏度温度；
通过 IIC 发送CMD_READ_PRESSURE获取气压RAW数据， 通过气压校准参数和温度校准
参数数计算得到实际气压Pa；

```


## 六、工作原理
从下图可以看到bmp280内部集成了压力传感单元，通过ADC采样输入到校正储存器。通过数字接口IIC或SPI读取数据。
<div align="center">
<img src=./../../docs/images/ext_bmp280_工作原理.png  width=50%/>
</div>

引脚分布如下，3和4号脚是IIC总线接口的SCL和SDA管脚。

<div align="center">
<img src=./../../docs/images/ext_bmp280_引脚.png  width=50%/>
</div>

典型电路如下，由于SCL和SDA为开漏输出，驱动能力不足，需要接上拉电阻。要通过IIC总线访问bmp280，CSB需要接到VDDIO。

<div align="center">
<img src=./../../docs/images/ext_bmp280_apply.png  width=50%/>
</div>


## 参考文献及购买链接
[1] [BMP280气压传感器](https://www.sekorm.com/doc/3095173.html)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=543204451587)

