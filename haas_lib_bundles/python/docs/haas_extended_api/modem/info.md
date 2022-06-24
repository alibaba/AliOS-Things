# modem 设备类接口

* 模块功能：
获取设备信息，实现设备间通信等功能

## info 获取模组信息的模块

* 函数原型：
> info()

* 参数说明：  
无

- 函数示例

```python
from modem import info   

print('设备IMEI:',info.getDevImei())

```

## getDevImei - 获取设备的IMEI(国际移动设备识别码)

* 函数功能：  
获取设备的IMEI

* 函数原型：
> info.getDevImei()

* 参数说明：  
 无

* 返回值：  
返回设备的IMEI号

* 示例： 

```python
from modem import info  

print('设备IMEI:',info.getDevImei())
```

* 输出：

```
设备IMEI:861755054458788
```

## getDevModel - 获取设备model

* 函数功能：  
获取设备model

* 函数原型：
> info.getDevModel()

* 参数说明：  
 无

* 返回值：  
返回设备model

* 示例： 

```python
from modem import info  

print("设备模组:",info.getDevModel())
```

* 输出：

```
设备模组: M601
```

## getDevProductId - 获取设备id

* 函数功能：  
获取设备id

* 函数原型：
> info.getDevProductId()

* 参数说明：  
 无

* 返回值：  
返回设备id

* 示例： 

```python
from modem import info  

print("设备id:",info.getDevProductId())
```

* 输出：

```
设备id: HaaS506
```

## getDevFwVersion - 获取设备固件版本号

* 函数功能：  
获取设备固件版本号

* 函数原型：
> info.getDevFwVersion()

* 参数说明：  
 无

* 返回值：  
返回固件版本号

* 示例： 

```python
from modem import info

print("设备固件版本",info.getDevFwVersion())
```

* 输出：

```
设备固件版本 amp-v2.02 build on 2022-04-28, 12:26:52
```
