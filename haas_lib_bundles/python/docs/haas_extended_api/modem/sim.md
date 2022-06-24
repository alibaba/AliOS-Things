# modem - 设备模块

* 模块功能：
获取设备信息，实现设备间通信等功能


## sim - SIM卡功能

* 函数原型：
> sim()

* 参数说明：  
无

* 返回值：  
sim对象创建成功，返回Name(sim)

* 示例代码： 

```python
import modem

print(modem.sim())
```
* 输出：

```
Name(sim)
```

## getImsi - 获取IMSI(国际移动用户识别码)

* 函数功能：  
获取IMSI(国际移动用户识别码)

* 函数原型：
> sim.getImsi()

* 参数说明：  
 无

* 返回值：  
返回IMSI

* 示例： 

```python
from modem import sim

print("IMSI:",sim.getImsi())
```

* 输出：

```
IMSI: 460046042210087
```

## getIccid - 获取ICCID(集成电路卡识别码)

* 函数功能：  
获取ICCID(集成电路卡识别码)

* 函数原型：
> sim.getIccid()

* 参数说明：  
 无

* 返回值：  
返回ICCID

* 示例： 

```python
from modem import sim

print("ICCID:",sim.getIccid())
```

* 输出：

```
ICCID: 89860440112170090087
```

## getStatus - 获取SIM卡状态

* 函数功能：  
获取SIM卡状态

* 函数原型：
> sim.getStatus()

* 参数说明：  
 无

* 返回值：  

返回值=0，sim卡正常，返回值=-1，sim卡异常

* 示例： 

```python
from modem import sim

print("SIM卡状态：",sim.getStatus())
```

* 输出：

```
SIM卡状态：0
```
