# mpu6050

## 一、产品简介
&emsp;&emsp;
MPU6050是一款6轴运动传感器，它集成了3 轴MEMS 陀螺仪，3 轴MEMS加速度计，以及一个可扩展的数字运动处理器DMP（Digital Motion Processor），可用I2C接口连接一个第三方的数字传感器，比如磁力计。扩展之后就可以通过其I2C 或SPI 接口输出一个9 轴的信号。

&emsp;&emsp;
MPU6050主要应用在智能手机、平板电脑、可穿戴设备、机顶盒、3D遥控器和游戏控制器等产品中。

<div align="center">
<img src=./../../docs/images/MPU6050.jpeg  width=50%/>
</div>

引脚定义
* VCC：电源正（5V）
* GND：电源地
* SCL：I2C时钟线,用于与主设备通讯
* SDA：I2C数据线,用于与主设备通讯
* XDA：I2C数据线,用于与拓展处理器通讯
* XCL：I2C时钟线,用于与拓展处理器通讯
* AD0：I2C地址控制引脚
* INT：中断上报引脚

## 二、技术参数
* 工作电压：2.375V~3.46V
* 睡眠电流：<=10uA@25°C
* 工作电流(Low-Noise Mode)：

  (1) 6轴(陀螺仪+加速度)：<=3mA

  (2) 3轴(加速度)：<=370uA

  (3) 3轴(陀螺仪)：<=2.8mA

* 工作温度：-40°C～+85°C
* 通信接口：400KHz I2C或10MHz SPI
* 三轴陀螺仪可编程范围：±250dps、±500dps、±1000dps和±2000dps
* 陀螺仪灵敏度误差：±1%
* 陀螺仪噪声：4mdps/√Hz
* 三轴加速度计可编程范围：±2g、±4g、±8g和±16g
* 加速度计噪声：100μg/√Hz
* 片上16位ADC和可编程滤波器
* 用户可编程中断
* 封装尺寸：LGA 3mm * 3mm * 0.75mm




* 引脚定义


## 三、软件接口
<br>

### MPU6050(i2cObj) - 创建MPU6050驱动对象
* 函数原型：
> MPU6050Obj = MPU6050(i2cObj)

* 参数说明：

| 参数   | 类型 | 必选参数？ | 说明                              |
| ------ | ---- | :--------: | --------------------------------- |
| i2cObj | I2C  |     是     | 调用此函数前需确保I2C对象已被open |

* 返回值：
MPU6050对象成功，返回MPU6050对象；MPU6050对象创建失败，抛出Exception

* 示例代码：

```python
from driver import I2C
from mpu6050 import MPU6050

i2cObj = I2C()
i2cObj.open("MPU6050")

MPU6050Dev = MPU6050(i2cObj)
```

### getTemperature - 测量温度

* 函数功能：
量测周围温度并返回测量结果

* 函数原型：
> MPU6050.getTemperature()

* 参数说明：
无

* 返回值：
成功返回温度测量结果（数据类型：浮点型；单位：摄氏度）；失败抛出Exception

* 示例：

```python
temp = MPU6050Dev.getTemperature()
print("temperature is ", temp, "°C")
```

* 输出：
```log
temperature is 28.9°C
```

### getAcceleration - 获取加速度值

* 函数功能：
获取传感器的加速度值

* 函数原型：
> MPU6050.getAcceleration()

* 参数说明：
无

* 返回值：
成功返回加速度计的原始值数值，其数据类型为tuple (gx,gy,gz) ；失败抛出Exception

* 示例：

```python
acc = MPU6050Dev.getAcceleration()
print(acc)
```

* 输出：
```log
(7382, 14618, 3306)
```

### getGyroscope - 获取陀螺仪值

* 函数功能：
获取传感器的陀螺仪值

* 函数原型：
> MPU6050.getGyroscope()

* 参数说明：
无

* 返回值：
成功返回陀螺仪的原始值数值，其数据类型为tuple (gx,gy,gz) ；失败抛出Exception

* 示例：

```python
gyro = MPU6050Dev.getGyro()
print(gyro)
```

* 输出：
```log
(14, 9, -49)
```

## 四、接口案例

* board.json

⚠️对于MPU6050而言，其 I2C 地址并不是完全确定的，开发者可以通过MPU6050的AD0引脚来决定其 I2C 通讯地址。 其DataSheet中相关的描述如下：

>AD0是从I2C接口的地址控制引脚，该引脚控制I2C地址的最低位。如果接GND，则MPU60SO的IIC地址是：0x68，如果接VDD，则是0x69。

因此，开发者在配置和使用此段案例时，需注意你使用的模块具体的硬件连接。

```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "MPU6050": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 104
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
  }

```

```python
import utime
from driver import I2C
import MPU6050

print("Testing mpu6050 ...")

i2cDev = I2C()
i2cDev.open("mpu6050")

mpu6050Dev = MPU6050(i2cDev)

temp = mpu6050Dev.getTemperature()
print("The temperature is:", temp)

acc = mpu6050Dev.getAcceleration()
print("The Acceleration data is:", acc)

gyro = mpu6050Dev.getGyroscope()

print("The Gyro data is: ", gyro)

i2cDev.close()

del mpu6050Dev
print("Test mpu6050 done!")
```

* 输出：
```log
Testing mpu6050 ...
The temperature is: 25.13
The Acceleration data is: (7382, 14618, 3306)
The Gyro data is:  (14, 9, -49)
The mpu6050 data is:  (14, 9, -49)
Test mpu6050 done!
```

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=41337720621)
