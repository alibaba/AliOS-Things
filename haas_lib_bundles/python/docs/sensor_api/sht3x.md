# sht3x - SHT3X-DIS温湿度传感器驱动

* 模块功能：  
驱动SHT3X-DIS传感器，测量环境的温湿度信息。详情请参考[SHT3X-DIS硬件说明](xxxx)。 

* 注意事项：  
在选择此温湿度传感器时，请注意其技术参数（通信接口、量程、精度、供电电压等）是否能满足实际场景的需求

## SHT3X(i2cObj) - 创建SHT3X驱动对象
* 函数原型：
> sht3xObj = SHT3X(i2cObj)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：  
SHT3X对象成功，返回SHT3X对象；SHT3X对象创建失败，抛出Exception

* 示例代码： 

```python
from driver import I2C               # 驱动库
import sht3x                         # SHT3X-DIS温湿度传感器驱动库

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("sht3x")                 # 按照board.json中名为"sht3x"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("sht3x inited!")

humitureDev = sht3x.SHT3X(i2cObj)   # 初始化SHT3X-DIS传感器
```

* 输出：
```
sht3x inited!
```

## getTemperature - 测量温度

* 函数功能：  
量测周围温度并返回测量结果

* 函数原型：
> SHT3X.getTemperature()

* 参数说明：  
无

* 返回值：  
成功返回温度测量结果（数据类型：浮点型；单位：摄氏度）；失败抛出Exception

* 示例： 

```python
import I2C
import sht3x

# 初始化I2C类型的设备
i2cObj = I2C()

# 按照board.jsono中名为"sht3x"的I2C类型的设备节点配置（总线频率、从设备地址信息等）打开i2cObj对象
i2cObj.open("sht3x")

humitureDev = sht3x.SHT3X(i2cObj)

temp = humitureDev.getTemperature()
print("temperature is ", temp, "°C")

```

* 输出：
```
temperature is 25.3°C
```

## getHumidity - 测量相对湿度

* 函数功能：  
量测周围相对湿度并返回测量结果

* 函数原型：
> SHT3X.getHumidity()

* 参数说明：  
无

* 返回值：  
成功返回相对湿度测量结果（数据类型：整型；单位：%RH）；失败抛出Exception

* 示例： 

```python
import I2C
import sht3x

# 初始化I2C类型的设备
i2cObj = I2C()
# 按照board.jsono中名为"sht3x"的I2C类型的设备节点配置（总线频率、从设备地址信息等）打开i2cObj对象
i2cObj.open("sht3x")

humitureDev = sht3x.SHT3X(i2cObj)

humi = humitureDev.getHumidity()
print("humidity is ", humi, "%H")

```

* 输出：
```
humidity is 60%H
```

## getTempHumidity - 量测温度相对湿度

* 函数功能：  
同时量测周围温度和相对湿度并返回测量结果

* 函数原型：
> SHT3X.getTempHumidity()

* 参数说明：  
无

* 返回值：  
成功返回温度和相对湿度测量结果（数据类型：数组，[温度值, 相对湿度值]；温度单位：摄氏度；相对湿度单位：%RH）；失败抛出Exception

* 示例： 

```python
import I2C
import sht3x
# 初始化I2C类型的设备
i2cObj = I2C()
# 按照board.jsono中名为"sht3x"的I2C类型的设备节点配置（总线频率、从设备地址信息等）打开i2cObj对象
i2cObj.open("sht3x")

humitureDev = sht3x.SHT3X(i2cObj)

data = humitureDev.getTempHumidity()
print("temperature is ", data[0], "°C")
print("humidity is ", data[1], "%H")
```

* 输出：
```
temperature is 25.3°C
humidity is 60%H
```

## 使用实例  
此使用实例在board.json中定义了名为sht3x的I2C类型的对象。在Python脚本中周期性的获取温湿度测量结果并打印在日志中。

* 代码：
```python
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "sht3x": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 68
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
import sht3x                         # SHT3X-DIS温湿度传感器驱动库

i2cObj = I2C()

i2cObj.open("sht3x")                 # 按照board.json中名为"sht3x"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
print("sht3x inited!")

humitureDev = sht3x.SHT3X(i2cObj)    # 初始化SHT3X-DIS传感器

# 通过温湿度传感器读取温湿度信息
def get_temp_humi():
    temperature = humitureDev.getTemperature()         # 获取温度测量结果
    humidity = humitureDev.getHumidity()               # 获取相对湿度测量结果
    '''
    # 如果需要同时获取温湿度信息，可以呼叫getTempHumidity，实例代码如下:
    humniture = humitureDev.getTempHumidity()          # 获取温湿度传感器测量到的温湿度值
    temperature = humniture[0]                         # get_temp_humidity返回的字典中的第一个值为温度值
    humidity = humniture[1]                            # get_temp_humidity返回的字典中的第二个值为相对湿度值
    '''
    print("The temperature is: %.1f" % temperature)
    print("The humidity is: %d" % humidity)

    return temperature, humidity                        # 返回读取到的温度值和相对湿度值

if __name__ == '__main__':

    while True:
        data = get_temp_humi()                          # 读取温度信息和相对湿度信息
        utime.sleep(1)                                  # 每隔1秒钟进行一次温湿度信息测量和打印

    i2cObj.close()                                      # 关闭I2C设备对象
```

* 输出：
```python
...
The temperature is 25.3°C
The humidity is 60%H
...
The temperature is 25.3°C
The humidity is 60%H
...

```

<br>



