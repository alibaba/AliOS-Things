# motion  - 行为识别

- 模块功能：  
本模块可以识别人体动作行为，包括[跌倒检测](../examples/fall_detection/esp32/README.md)、[单击手势检测](../examples/tap_detection/esp32/README.md)、[双击手势检测](../examples/tap_detection/esp32/README.md)。

-  注意事项：
在使用前，请确保设备具有获取加速度和角速度的传感器，比如：[mpu6050](../../libraries/mpu6050/README.md)、[mpu6886](../../libraries/mpu6886/mpu6886.py)等，以检测人体动作行为。

## 常量说明

| 常量 | 数值类型 | 常量说明 |
| --- | --- | --- |
| ACCELERATION_LOW_THREADHOLD | float | 加速度下阈值，越大越灵敏 |
| ACCELERATION_UP_THREADHOLD | float | 加速度上阈值，越小越灵敏 |
| ANGULAR_VELOCITY_LOW_THREADHOLD | float | 角速度下阈值，越大越灵敏 |
| ANGULAR_VELOCITY_UP_THREADHOLD | float | 角速度上阈值，越小越灵敏 |


## 模块导入

本示例基于M5StackCore2开发板，M5StackCore2内置mpu6886传感器。[motion](../../libraries/motion/)和[mpu6886](../../libraries/mpu6886)模块的python驱动需分别下载，然后将下载的motion和mpu6886文件夹放到main.py的同级目录下。

```python
from motion import motion
```

## motion - 创建motion对象

-  函数原型： 
> motionObj = motion.Motion(action, get_data, detected_callback)

-  返回值：  
motion对象

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| action | string | 是 | 行为名称 |
| get_data | function | 是 | 获取设备传感器数据的函数，需返回六轴加速度、角速度 |
| detected_callback | function | 是 | 识别到动作行为时的回调函数 |

- action参数说明：

| 可取值 | 说明 |
| --- | --- |
| "fall" | 跌倒检测 |
| "single_tap" | 单击手势检测 |
| "double_tap" | 双击手势检测 |

- get_data函数说明：

| 返回值 | 说明 |
| --- | --- |
| acc | array，包含acc_x、acc_y和acc_z |
| gyro | array，包含gyro_x、gyro_y和gyro_z |

get_data函数示例：

```python
def get_data():
    acc[0] = 0.0000001
    acc[1] = 0.0000001
    acc[2] = 0.0000001
    gyro[0] = 0.0000002
    gyro[1] = 0.0000002
    gyro[2] = 0.0000002

    return tuple(acc), tuple(gyro)    # 返回读取到的加速度数组、角速度数组
```

- detected_callback函数说明：

在检测到对应的动作后会回调，用户可在这里执行自己的逻辑，如向物联网平台发送报警信息等。detected_callback函数示例：

```
def fall_detected():
    print('fall_detected')
```

-  创建motion对象示例：

本示例基于M5StackCore2开发板，M5StackCore2内置mpu6886传感器。[motion](../../libraries/motion/)和[mpu6886](../../libraries/mpu6886)模块的python驱动需分别下载，然后将下载的motion和mpu6886文件夹放到main.py的同级目录下。

```python
from motion import motion

def get_data():
    acc[0] = 0.0000001
    acc[1] = 0.0000001
    acc[2] = 0.0000001
    gyro[0] = 0.0000002
    gyro[1] = 0.0000002
    gyro[2] = 0.0000002

    return tuple(acc), tuple(gyro)    # 返回读取到的加速度数组、角速度数组

def fall_detected():
    print('fall_detected')

# 获取跌倒检测的motion实例
motionObj = motion.Motion("fall", get_data, fall_detected)
```

## enable - 使能检测

-  函数功能：  
使能人体行为动作检测

-  函数原型： 
> enable(sensitivity)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| sensitivity | Dictionary | 否 | 灵敏度参数，可以设置以下四个参数：ACCELERATION_LOW_THREADHOLD、ACCELERATION_UP_THREADHOLD、ANGULAR_VELOCITY_LOW_THREADHOLD、ANGULAR_VELOCITY_UP_THREADHOLD |

跌倒检测的sensitivity参数默认值：

| 常量 | 数值类型 | 常量说明 |
| --- | --- | --- |
| ACCELERATION_LOW_THREADHOLD | float | 4，越大越灵敏 |
| ACCELERATION_UP_THREADHOLD | float | 12，越小越灵敏 |
| ANGULAR_VELOCITY_LOW_THREADHOLD | float | 1，越大越灵敏 |
| ANGULAR_VELOCITY_UP_THREADHOLD | float | 40，越小越灵敏 |

单击手势检测的sensitivity参数默认值：

| 常量 | 数值类型 | 常量说明 |
| --- | --- | --- |
| ACCELERATION_LOW_THREADHOLD | float | 不需要设置 |
| ACCELERATION_UP_THREADHOLD | float | 30，越大越灵敏 |
| ANGULAR_VELOCITY_LOW_THREADHOLD | float | 不需要设置 |
| ANGULAR_VELOCITY_UP_THREADHOLD | float | 不需要设置 |

双击手势检测的sensitivity参数默认值：

| 常量 | 数值类型 | 常量说明 |
| --- | --- | --- |
| ACCELERATION_LOW_THREADHOLD | float | 不需要设置 |
| ACCELERATION_UP_THREADHOLD | float | 30，越大越灵敏 |
| ANGULAR_VELOCITY_LOW_THREADHOLD | float | 不需要设置 |
| ANGULAR_VELOCITY_UP_THREADHOLD | float | 不需要设置 |

-  示例1： 

本示例基于M5StackCore2开发板，M5StackCore2内置mpu6886传感器。[motion](../../libraries/motion/)和[mpu6886](../../libraries/mpu6886)模块的python驱动需分别下载，然后将下载的motion和mpu6886文件夹放到main.py的同级目录下。

```python
# main.py
from motion import motion
from driver import I2C
from mpu6886 import mpu6886

i2cObj = None
mpu6886Dev = None

def get_data():
    acc = mpu6886Dev.acceleration
    gyro = mpu6886Dev.gyro
    # print(acc)
    # print(gyro)
    return acc, gyro                    # 返回读取到的加速度、角速度值

def fall_detected():
    print('attention')

if __name__ == '__main__':
     # 硬件初始化
    i2cObj = I2C()
    i2cObj.open("mpu6886")                 # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 获取跌倒检测的motion实例
    motionObj = motion.Motion("fall", get_data, fall_detected)

    # 使能action检测
    motionObj.enable()
```

```json
board.json文件配置如下：
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "mpu6886": {
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

-  示例2：

本示例基于M5StackCore2开发板，M5StackCore2内置mpu6886传感器。[motion](../../libraries/motion/)和[mpu6886](../../libraries/mpu6886)模块的python驱动需分别下载，然后将下载的motion和mpu6886文件夹放到main.py的同级目录下。

```python
# main.py
from motion import motion
from driver import I2C
from mpu6886 import mpu6886

i2cObj = None
mpu6886Dev = None

def get_data():
    acc = mpu6886Dev.acceleration
    gyro = mpu6886Dev.gyro
    # print(acc)
    # print(gyro)
    return acc, gyro                    # 返回读取到的加速度、角速度值

def fall_detected():
    print('attention')

if __name__ == '__main__':
     # 硬件初始化
    i2cObj = I2C()
    i2cObj.open("mpu6886")                 # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 获取跌倒检测的motion实例
    motionObj = motion.Motion("fall", get_data, fall_detected)

    # 使能action检测，并以Dictionary格式传入灵敏度参数
    sensitivity = { "ACCELERATION_LOW_THREADHOLD" : 4, "ACCELERATION_UP_THREADHOLD" : 12, "ANGULAR_VELOCITY_LOW_THREADHOLD" : 1, "ANGULAR_VELOCITY_UP_THREADHOLD" : 10 }
    motionObj.enable(sensitivity)
```

```json
board.json文件配置如下：
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "mpu6886": {
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

## disable - 关闭检测

-  函数功能：  
关闭人体行为动作检测

-  函数原型： 
> disable()

-  示例：

本示例基于M5StackCore2开发板，M5StackCore2内置mpu6886传感器。[motion](../../libraries/motion/)和[mpu6886](../../libraries/mpu6886)模块的python驱动需分别下载，然后将下载的motion和mpu6886文件夹放到main.py的同级目录下。

```python
# main.py
import utime   # 延时函数在utime
from motion import motion
from driver import I2C
from mpu6886 import mpu6886

i2cObj = None
mpu6886Dev = None

def get_data():
    acc = mpu6886Dev.acceleration
    gyro = mpu6886Dev.gyro
    # print(acc)
    # print(gyro)
    return acc, gyro                    # 返回读取到的加速度、角速度值

def fall_detected():
    print('attention')

if __name__ == '__main__':
     # 硬件初始化
    i2cObj = I2C()
    i2cObj.open("mpu6886")                 # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 获取跌倒检测的motion实例
    motionObj = motion.Motion("fall", get_data, fall_detected)

    # 使能action检测，并以Dictionary格式传入灵敏度参数
    sensitivity = { "ACCELERATION_LOW_THREADHOLD" : 4, "ACCELERATION_UP_THREADHOLD" : 12, "ANGULAR_VELOCITY_LOW_THREADHOLD" : 1, "ANGULAR_VELOCITY_UP_THREADHOLD" : 10 }
    motionObj.enable(sensitivity)

    # sleep 100秒，期间可以进行跌倒检测，灵敏度为sensitivity
    utime.sleep(100)

    # 关闭action检测，可再次使能，支持传入新的灵敏度
    motionObj.disable()

    # 再次使能action检测，并以Dictionary格式传入新的灵敏度参数
    sensitivity1 = { "ACCELERATION_LOW_THREADHOLD" : 5, "ACCELERATION_UP_THREADHOLD" : 13, "ANGULAR_VELOCITY_LOW_THREADHOLD" : 2, "ANGULAR_VELOCITY_UP_THREADHOLD" : 20 }
    motionObj.enable(sensitivity1)

    # sleep 100秒，期间可以进行跌倒检测，灵敏度为sensitivity1
    utime.sleep(100)

    # 关闭action检测，可再次使能，支持传入新的灵敏度
    motionObj.disable()

    # 关闭I2C设备对象
    i2cObj.close()
    del mpu6886Dev
```

```json
board.json文件配置如下：
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "mpu6886": {
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