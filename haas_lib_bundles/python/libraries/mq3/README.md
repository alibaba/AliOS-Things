# 乙醇传感器 - MQ3

## 一、产品简介
MQ3是汉威电子出品的乙醇传感器，它有快速相应、高敏感度、稳定、寿命长以及外围电路简单等多个优点，常用于酒精监测、呼吸分析仪等场景。

<div align="center">
<img src=./../../docs/images/ext_mq3_实物.png  width=50%/>
</div>

### 引脚定义
* VCC: 3.3V
* A0:  host ADC
* D0:  host GPIO
* GND: GND

## 二、技术参数
* 预热时间:不少于48小时
* 监测乙醇浓度范围:0.05mg/L - 10mg/L
* 使用温度范围: -10 - 50摄氏度

## 三、软件接口

### MQ3(adcObj) - 创建MQ3驱动对象
* 函数原型:
> mq3Obj = MQ3(adcObj)

* 参数说明:

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|adcObj|ADC|是|调用此函数前需确保adcObj对象已经处于open状态|
<br>

* 返回值:
创建对象成功，返回MQ3对象；MQ3对象创建失败，抛出Exception

* 示例代码:

```python
from mq3 import *                        # MQ3温湿度传感器驱动库

# 初始化ADC类型的设备
adc = ADC()
adc.open("mq3")
mq3Dev = MQ3(adc)
```

* 输出:
无

### getVoltage - 测量乙醇浓度电压值

* 函数功能:
量测乙醇并返回测量结果

* 函数原型:
> MQ3.getVoltage()

* 参数说明:
无

* 返回值:
成功返回温度测量结果（数据类型：浮点型；单位：mV）；失败抛出Exception

* 示例:

```python
from mq3 import *                    # MQ3温湿度传感器驱动库
adc = ADC()
adc.open("mq3")
mq3Dev = MQ3(adc)
while 1:
    voltage = mq3Dev.getVoltage()
    print("voltage:%d" % voltage)
    sleep_ms(500)
```

* 输出:
```log
voltage: 800.0
```

## 四、接口案例
此使用实例在board.json中定义了名为mq3的ADC类型的对象。在Python脚本中周期性的获取温湿度测量结果并打印在日志中。

* 代码:
```json
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "mq3": {
            "type": "ADC",
            "port": 7,
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
from mq3 import *
print("Testing mq3  ...")
adc = ADC()
adc.open("mq3")
mq3Dev = MQ3(adc)
while 1:
    voltage = mq3Dev.getVoltage()
    print("voltage:%d" % voltage)
    sleep_ms(500)
adc.close()
del mq3Dev
print("Test mq3 done!")
```

* 输出：
```log
...
Testing mq3  ...
voltage: 500
...

```

<br>

## 五、通信协议
主控芯片通过ADC读取MQ3传感器的乙醇数据。

## 六、工作原理
mq-3 需要预热， 从下面对结构图中可以看到它周围有两层叫做防爆网的钢罩，这样可以确保当我们测量乙醇浓度的时候传感器不会发生爆炸。
<div align="center">
<img src=./../../docs/images/ext_mq3_structure.png  width=50%/>
</div>

从下图可以看到mq-3内部集成的乙醇传感器对乙醇最敏感，并且随着乙醇浓度的升高，乙醇传感器的电阻会变小。
<div align="center">
<img src=./../../docs/images/ext_mq3_工作原理.png  width=50%/>
</div>

## 参考文献及购买链接
[1] [MQ3乙醇传感器](https://components101.com/sites/default/files/component_datasheet/MQ-3%20Sensor%20Datasheet%20In%20Detail.pdf)<br>
[2] [商品链接]()


