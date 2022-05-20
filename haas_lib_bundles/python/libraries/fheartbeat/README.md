# 手指侦测心跳传感器
## 一、产品简介
手指侦测心跳传感器是通过LED和光电晶体管监测手指血压脉冲，来判断人的心脏跳动。其结构简单成本低廉，只能是做一些实验和学习相关的知识（没有医疗实用价值）

<div align="center">
<img src=./../../docs/images/fheartbeat.png  width=25%/>
</div>

### 引脚定义
* -: GND
* +: 3.3V
* S: 模拟信号输出

## 二、软件接口
手指侦测心跳传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/fheartbeat)
<br>

### FHEARTBEAT(adcObj) - 创建FHEARTBEAT驱动对象
* 函数原型:
> fheartbeatObj = FHEARTBEAT(adcObj)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|adcObj|ADC|是|调用此函数前需确保adcObj对象已经处于open状态|
<br>

* 返回值:

创建对象成功，返回FHEARTBEAT对象；FHEARTBEAT对象创建失败，抛出Exception

* 示例代码:

```python
from fheartbeat import *

print("Testing fheartbeat  ...")
adc = ADC()
adc.open("fheartbeat")
fheartbeatDev = FHEARTBEAT(adc)
```

* 输出:
```log
Testing fheartbeat  ...
```

### getVoltage - 测量手指侦测心跳电压值

* 函数功能:

量测手指侦测心跳电压值并返回测量结果

* 函数原型:
> FHEARTBEAT.getVoltage()

* 参数说明:
无

* 返回值:

成功返回手指侦测心跳电压值测量结果（数据类型：浮点型；单位：mV）；失败抛出Exception

* 示例:

```python
from fheartbeat import *

print("Testing fheartbeat  ...")
adc = ADC()
adc.open("fheartbeat")
fheartbeatDev = FHEARTBEAT(adc)
voltage = fheartbeatDev.getVoltage()
print("voltage: %d"%voltage)
```

* 输出:
```log
Testing fheartbeat  ...
voltage: 800.0
```

## 三、接口案例
此使用实例在board.json中定义了名为FHEARTBEAT的ADC类型的对象。在Python脚本中周期性的获取手指侦测心跳电压值测量结果并打印在日志中。

* 代码:
```json
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "fheartbeat": {
        "type": "ADC",
        "port": 4,
        "width": 2,
        "atten": 3,
        "sampling": 12000000
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
# Python代码
import utime                         # 延时API所在组件
from fheartbeat import *

i = 0
trip = {}
print("Testing fheartbeat  ...")
adc = ADC()
adc.open("fheartbeat")
fheartbeatDev = FHEARTBEAT(adc)
while 1:
    voltage = fheartbeatDev.getVoltage()
    trip[i] = voltage
    i  = i + 1
    if i == 50:
        i = 0
        print("trip:",trip)
    utime.sleep_ms(10)
```

* 输出：
```log
...
Testing fheartbeat  ...
trip: {0: 3719, 1: 3720, 2: 3719, 3: 3720, 4: 3717, 5: 3734, 6: 3726, 7: 3732, 8: 3730, 9: 3732, 10: 3724, 11: 3724, 12: 3732, 13: 3732, 14: 3734, 15: 3730, 16: 3726, 17: 3719, 18: 3734, 19: 3728, 20: 3724, 21: 3728, 22: 3732, 23: 3720, 24: 3732, 25: 3724, 26: 3720, 27: 3720, 28: 3724, 29: 3719, 30: 3717, 31: 3726, 32: 3720, 33: 3722, 34: 3719, 35: 3734, 36: 3722, 37: 3717, 38: 3722, 39: 3720, 40: 3719, 41: 3720, 42: 3711, 43: 3717, 44: 3734, 45: 3724, 46: 3722, 47: 3720, 48: 3724, 49: 3717}
trip: {0: 3732, 1: 3720, 2: 3720, 3: 3713, 4: 3719, 5: 3719, 6: 3717, 7: 3720, 8: 3717, 9: 3717, 10: 3719, 11: 3717, 12: 3724, 13: 3724, 14: 3720, 15: 3720, 16: 3720, 17: 3724, 18: 3713, 19: 3719, 20: 3719, 21: 3715, 22: 3713, 23: 3713, 24: 3720, 25: 3717, 26: 3713, 27: 3717, 28: 3720, 29: 3709, 30: 3722, 31: 3715, 32: 3719, 33: 3719, 34: 3715, 35: 3709, 36: 3724, 37: 3719, 38: 3717, 39: 3717, 40: 3719, 41: 3711, 42: 3728, 43: 3720, 44: 3719, 45: 3724, 46: 3717, 47: 3717, 48: 3717, 49: 3732}
...

```

<br>

## 四、通信协议
主控芯片通过ADC读取手指侦测心跳传感器的电压值数据。

## 五、工作原理
当手指放在发射器与接收器之间，红外发射LED发射的光将穿过手指，到被接收器接收。而血压会随着脉搏变化，导致红外接收器接收到的光通量也会随之变化，因此可以通过红外光接收的情况来统计监测心跳。其大概的电原理示意图如下：
<div align="center">
<img src=./../../docs/images/fheatbeat-princ.png  width=70%/>
</div>

## 参考文献及购买链接
[1] [商品链接](https://item.taobao.com/item.htm?spm=a1z0d.6639537.1997196601.3.7a0b7484Tdo3id&id=571175470073)
