# 双轴按键摇杆 - rockerKey

## 一、产品简介
双轴按键摇杆传感器模块由采用PS2摇杆电位器制作，具有(X,Y)2轴模拟输出，(Z)1路按钮数字输出。由于X，Y输出的模拟量最大5V，超出了一些开发板的ADC量程，所以通过ADS1115传感器配合HaaS开发板来制作遥控器等互动作品。

ADS1115传感器外观及引脚功能说明如下：
<div align="center">
<img src=./../../docs/images/rockerkey.png  width=60%/>
</div>

双轴按键摇杆引脚定义：
* VCC：5V<br>
* GND：地<br>
* VRX：X轴模拟信号，连接ADS1115的A0<br>
* VRY：Y轴模拟信号，连接ADS1115的A1<br>
* SW：GPIO信号，连接MCU获取电平状态

ADS1115引脚定义：
* V：3.3V<br>
* G：地<br>
* SCL：I2C总线的时钟线<br>
* SDA：I2C总线的数据线<br>
* A0：ADC通道0，连接摇杆X轴引脚<br>
* A1：ADC通道1，连接摇杆Y轴引脚<br>

## 二、软件接口

双轴摇杆按键HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/rockerkey)
<br>

### ROCKERKEY(i2cObj,gpioObj,MaxVoltage,XAxis,YAxis) - 创建ROCKERKEY驱动对象
* 函数原型：
> xyzDev = ROCKERKEY(i2cObj,gpioObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|
|MaxVoltage|Number|否|ADS1115的最大ADC电压采样值，默认5500mv（单位：mv）|
|XAxis|Number|否|摇杆X轴连接ADS1115的通道，默认通道0，范围：0-3|
|YAxis|Number|否|摇杆Y轴连接ADS1115的通道，默认通道1，范围：0-3|

* 返回值：

执行成功，返回ROCKERKEY对象；创建失败，抛出Exception

* 示例代码：

```python
from driver import I2C,GPIO              # 驱动库
import rockerkey

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # ads1115的设备节点的配置参数
print("ADS1115 inited!")

gpioObj = GPIO()
gpioObj.open("rockerkey")                # 按照board.json中名为"rockerkey"的设备节点的配置参数
print("GPIO inited!")
xyzDev= rockerkey.ROCKERKEY(i2cObj,gpioObj)
print("rockerKey object create success!")
```

* 输出：
```log
ADS1115 inited!
GPIO inited!
rockerKey object create success!
```

### getXYZAxis - 读取双轴按键摇杆X、Y、Z的值

* 函数功能：

读取双轴按键摇杆X、Y、Z的值

* 函数原型：
> ROCKERKEY.getXYZAxis()

* 参数说明：

无

* 返回值：

返回X、Y、Z的状态值

|返回值|类型|必选参数？|说明|
|-----|----|:---:|----|
|返回值1|int|是|X轴的电压值，范围从0-5000mv|
|返回值2|int|是|Y轴的电压值，范围从0-5000mv|
|返回值3|int|是|Z轴的姿态，1-未按下，0-按下|

* 示例：

```python
from driver import I2C,GPIO              # 驱动库
import rockerkey

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # ads1115的设备节点的配置参数
print("ADS1115 inited!")

gpioObj = GPIO()
gpioObj.open("rockerkey")                # 按照board.json中名为"rockerkey"的设备节点的配置参数
print("GPIO inited!")
xyzDev= rockerkey.ROCKERKEY(i2cObj,gpioObj)
print("rockerKey object create success!")
X,Y,Z = xyzDev.getXYZAxis()
print("Rocker Key result is x:%d mV y:%d mV z:%d level"%(X,Y,Z))
```

* 输出：
```log
ADS1115 inited!
GPIO inited!
rockerKey object create success!
Rocker Key result is x:2478 mV y:2412 mV Z:1 level
```

## 三、接口案例
此使用实例在board.json中定义了名为rockerKey的复合类型对象。在Python脚本中周期性的量测摇杆X、Y、Z的状态值并打印到日志中。

* board.json配置：

```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
       "ads1115": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 72
      },
      "rockerkey":{
        "type": "GPIO",
        "port": 2,
        "dir": "input"
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

* 案例代码：

```python
from driver import I2C,GPIO              # 驱动库
import rockerkey
import utime

# 初始化I2C类型的设备
i2cObj = I2C()
i2cObj.open("ads1115")                # ads1115的设备节点的配置参数
print("ADS1115 inited!")

gpioObj = GPIO()
gpioObj.open("rockerkey")                # 按照board.json中名为"rockerkey"的设备节点的配置参数
print("GPIO inited!")
xyzDev= rockerkey.ROCKERKEY(i2cObj,gpioObj)
print("rockerKey object create success!")
while True:
    X,Y,Z = xyzDev.getXYZAxis()
    print("Rocker Key result is x:%d mV y:%d mV z:%d level"%(X,Y,Z))
    utime.sleep(1)
```

* 输出：
```log
ADS1115 inited!
GPIO inited!
rockerKey object create success!
Rocker Key result is x:2478 mV y:2412 mV Z:1 level
Rocker Key result is x:3410 mV y:12 mV Z:1 level
Rocker Key result is x:3715 mV y:1202 mV Z:1 level
Rocker Key result is x:4270 mV y:2310 mV Z:1 level
Rocker Key result is x:4468 mV y:2961 mV Z:1 level
Rocker Key result is x:4773 mV y:3901 mV Z:0 level
Rocker Key result is x:4971 mV y:4568 mV Z:1 level
```
<br>

## 四、工作原理
双轴按键摇杆为一个双向的10K电阻器，随着摇杆方向不同，抽头的阻值随着变化。本模块使用5V供电，原始状态下X,Y读出电压为2500mV左右，当随摇杆方向变化，读出电压值随着变化，变化范围从0-5000mV。

引脚说明: VRx，VRy(X、Y轴)为模拟输入信号，连接到模拟IO口，由于有些MCU不支持5V量程的电压测量，为了简化电路的设计，增加了ADS1115的ADC转换器，将摇杆上对应的X、Y模拟信号连接到ADS1115上。
SW(Z轴)是数字输入信号，连接到数字端口，并启用上拉电阻避免电平不稳。SW的值:1代表未按下，0代表按下。

## 参考文献及购买链接
[1] [ADS1115购买链接](https://item.taobao.com/item.htm?id=521133121509)<br>
[2] [双轴按键摇杆购买链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.7.77964390tNbpEF&id=40684149927&ns=1&abbucket=6&skuId=4509388425229)
