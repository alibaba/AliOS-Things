# 电度表 - DDS5188

## 一、产品简介
DDS5188是单相两线轨道式电能表。该表具有以下功能特点：采用大屏幕液晶显示，带有RS485（通讯协议MODBUS-RTU）远程抄表功能，能精确地计量有功电能。

<div align="center">
<img src=./../../docs/images/ext_dds5188_pins.jpg width=50%/>
</div>

引脚定义
* 1：火线进
* 3：火线出
* 4：零线进
* 6：零线出
* 23：RS485-A
* 24：RS485-B

## 二、技术参数
* 精度等级：1级
* 工作温度：-10度～50度
* 通讯方式：RS485，波特率9600，偶校验
* 通讯协议：MODBUS-RTU
* 电流量程：5(30)A

## 三、软件接口

DDS5188电度表HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/dds5188)
<br>

### DDS5188(node, slaveAddr) - 创建DDS5188驱动对象
* 函数原型：
> dds5188Obj = DDS5188(node, slaveAddr)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|node|String|是|board.json中的modbus节点名称。|
|slaveAddr|Int|是|电度表在MODBUS协议中的slave adress，标注在电度表侧面。例如，NO.002表示slave address为2。|

* 返回值：
DDS5188对象成功，返回DDS5188对象；DDS5188对象创建失败，抛出Exception。

* 示例代码：

```python
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")
```

* 输出：
```log
DDS5188 inited!
```

### getVoltage - 获取电压值

* 函数功能：
获取电压值并且返回状态。

* 函数原型：
> DDS5188.getVoltage()

* 参数说明：
无

* 返回值：
返回一个元组：第一个元素是故障码，0值表示成功，非0值表示失败；第二个元素是电压值，数据类型为浮点型，单位V。

* 示例：

```python
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")
tup = dds5188Obj.getVoltage()
if tup[0] == 0:
    print("Voltage: %.3f V" % tup[1])
```

* 输出：
```log
DDS5188 inited!
Voltage: 221.324 V
```

### getCurrent - 获取电流值

* 函数功能：
获取电流值并且返回状态。

* 函数原型：
> DDS5188.getCurrent()

* 参数说明：
无

* 返回值：
返回一个元组：第一个元素是故障码，0值表示成功，非0值表示失败；第二个元素是电流值，数据类型为浮点型，单位A。

* 示例：

```python
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")
tup = dds5188Obj.getCurrent()
if tup[0] == 0:
    print("Current: %.3f A" % tup[1])
```

* 输出：
```log
DDS5188 inited!
Current: 0.833 A
```

### getActivePower - 获取有功功率值

* 函数功能：
获取有功功率值并且返回状态。

* 函数原型：
> DDS5188.getActivePower()

* 参数说明：
无

* 返回值：
返回一个元组：第一个元素是故障码，0值表示成功，非0值表示失败；第二个元素是有功功率值，数据类型为浮点型，单位W。

* 示例：

```python
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")
tup = dds5188Obj.getActivePower()
if tup[0] == 0:
    print("Active Power: %.3f W" % tup[1])
```

* 输出：
```log
DDS5188 inited!
Active Power: 5.674 W
```

### getKWh - 获取总有功电能值

* 函数功能：
获取总有功电能值并且返回状态。

* 函数原型：
> DDS5188.getKWh()

* 参数说明：
无

* 返回值：
返回一个元组：第一个元素是故障码，0值表示成功，非0值表示失败；第二个元素是总有功电能值，数据类型为浮点型，单位KWh。

* 示例：

```python
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")
tup = dds5188Obj.getKWh()
if tup[0] == 0:
    print("Energy: %.3f KWh" % tup[1])
```

* 输出：
```log
DDS5188 inited!
Energy: 1.237 KWh
```

## 四、接口案例
此使用实例在board.json中定义了名为mb1的modbus节点。在Python脚本中周期性地获取电压、电流、有功功率、总有功电能并打印在日志中。

* 代码：
```python
# board.json配置：
{
    "name": "nodemcu32s",
    "version": "1.0.0",
    "io": {
        "mb1": {
            "type": "MODBUS",
            "mode": 0,
            "port": 2,
            "baudrate": 9600,
            "priority": 0,
            "timeout": 200,
            "parity": 2
        }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
# Python代码
import utime
import dds5188

dds5188Obj = dds5188.DDS5188('mb1', 2)
print("DDS5188 inited!")

while True:
    tup = dds5188Obj.getVoltage()
    if tup[0] == 0:
        print("Voltage: %.3f V" % tup[1])
    tup = dds5188Obj.getCurrent()
    if tup[0] == 0:
        print("Current: %.3f A" % tup[1])
    tup = dds5188Obj.getActivePower()
    if tup[0] == 0:
        print("Active Power: %.3f W" % tup[1])
    tup = dds5188Obj.getKWh()
    if tup[0] == 0:
        print("Energy: %.3f KWh" % tup[1])
    utime.sleep(1)
```

* 输出：
```log
DDS5188 inited!
Voltage: 234.061 V
Current: 0.031 A
Active Power: 6.518 W
Energy: 1.379 KWh
Voltage: 234.065 V
Current: 0.030 A
Active Power: 6.531 W
Energy: 1.380 KWh
```

## 五、工作原理

DDS5188寄存器描述如下表所示：

|序号|寄存器地址|功能说明|
|:-:|:-:|:-:|
|1|0x0000|电压|
|2|0x0008|电流|
|3|0x0012|有功功率|
|4|0x1000|总有功电能|

## 六、购买链接
[1] [DDS5188电度表](https://detail.tmall.com/item.htm?id=557012643181)
