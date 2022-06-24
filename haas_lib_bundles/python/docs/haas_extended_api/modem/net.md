# modem 设备类接口

* 模块功能：
获取设备信息，实现设备间通信等功能

## net - 网络功能

* 函数原型：
> net()

* 参数说明：  
无

* 示例： 

```python
from modem import net

print("信号强度：",net.getCsq())
```

* 输出：

```
信号强度：30
```

## setModem - 设置飞行模式

* 函数功能：  
设置飞行模式

* 函数原型：
> modem.setModem(value)

* 参数说明：  

|参数|类型|必选参数|说明|
|-----|----|----|:---:|
|value|int|是|value=0 全功能模式;value=1 打开飞行模式|

* 示例： 

```python
from modem import net

net.setModem(1)

```


## getModem - 获取工作模式

* 函数功能：  
获取工作模式

* 函数原型：
> net.getModem()

* 参数说明：  
 无

* 返回值：  
返回值=1，飞行模式;返回值=0，正常模式

* 示例： 

```python
from modem import net

print('工作状态：',net.getModem())

```

* 输出：

```
工作状态：0
```

## getCsq - 获取信号强度

* 函数功能：  
获取飞行模式状态

* 函数原型：
> net.getCsq()

* 参数说明：  
 无

* 返回值：  
成功返回整型的csq信号强度值

* 示例： 

```python
from modem import net

print("信号强度：",net.getCsq())
```

* 输出：

```
信号强度：30
```

## getMode - 获取当前网络模式

* 函数功能：  
获取当前网络模式

* 函数原型：
> net.getMode()

* 参数说明：  
 无

* 返回值： 

|数值|说明|
|-----|:---|
|0|auto|
|1|GSM only|
|3|LTE only|

* 示例： 

```python
from modem import net

print('网络模式：',net.getMode())
```

* 输出：

```
'网络模式：0
```

## getCellInfo - 获取附件小区信息

* 函数功能：  
获取附件小区信息

* 函数原型：
> net.getCellInfo()

* 参数说明：  
 无

* 返回值： 

成功返回包含三种网络系统（GSM、UMTS、LTE）的信息的list，如果对应网络系统信息为空，则返回空的List。(UMTS暂不支持，默认返回[]),返回值格式如下：

([(cid, mcc, mnc, lac, arfcn, bsic)], [], [(cid, mcc, mnc, pci,  earfcn,tac),...])

GSM网络系统返回值说明

|参数|	参数意义|
|-----|:---|
cid	|返回GSM网络下的cell id信息，0则为空，范围0 ~ 65535
mcc	|移动设备国家代码，范围 0 ~ 999     注意：EC100Y/EC600S/EC600N系列的模组，该值是用十六进制来表示，比如下面示例中的十进制数1120，用十六进制表示为0x460，表示移动设备国家代码460，其他型号模组，该值直接用十进制表示，比如移动设备国家代码460，就是用十进制的460来表示。
mnc	|移动设备网络代码，范围 0 ~ 99
lac|	位置区码，范围 1 ~ 65534
arfcn	|无线频道编号，范围 0 ~ 65535
bsic|	基站识别码，范围 0 ~ 255

LTE网络系统返回值说明
参数|参数意义
|-----|:---|
cid	|返回LTE网络下的 Cell identity 信息，Cell identity = RNC_ID * 65536 + Cell_ID，Cell identity范围 0x0000000 ~ 0xFFFFFFF（注意这里是28bits）；其中RNC_ID的范围是0 ~ 4095，Cell_ID的范围是0 ~ 65535
mcc	|移动设备国家代码，范围 0 ~ 999
mnc	|移动设备网络代码，范围 0 ~ 99
pci	|物理层小区标识号，0 ~ 503
tac	|跟踪区域码，0 ~ 65535
earfcn|	无线频道编号，范围 0 ~ 65535

* 示例： 

```python
from modem import net

print("获取邻近 CELL 的信息:",net.getCellInfo())
```

* 输出：

```
获取邻近 CELL 的信息:([], [], [(0, 68395803, 1120, 272, 46, 0, 16734, -65), (0, 68395804, 1120, 272, 45, 0, 16734, -68), (0, 68395825, 1120, 272, 69, 0, 16734, -73)])
```
