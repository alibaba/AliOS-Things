# 指纹识别模块 - ATK-AS608

## 一、产品简介
ATK-AS608指纹识别模块是ALIENTEK推出的一款高性能的光学指纹识别模块。它采用的是指纹识别芯片公司杭州晟元芯片技术有限公司生产的AS608指纹识别芯片。该芯片内置DSP运算单元，集成了指纹识别算法，能高效快速采集图像并对指纹特征进行识别。 该模块配备了串口和USB接口，用户无需研究复杂的图像处理及指纹识别算法，只需通过简单的串口、USB按照通讯协议便可控制模块。本模块可应用于各种考勤机、保险箱柜 、指纹门禁系统、指纹锁等场合。

本文中主控板和AS608模块之间使用UART进行通信，实际使用过程中最少只需要使用VCC/TX/RX/GND四根引脚和主控板进行通信。模块接口外观及其定义如下：

<div align="center">
<img src=./../../docs/images/AS608_接口图.png width=30%/>
</div>

|名称|说明|
|:-:|:-|
|VCC|模块电源正极|
|TX|串行数据输出，TTL逻辑电平|
|RX|串行数据输入，TTL逻辑电平|
|GND|地线|
|TOUCH|触摸板感应信号，默认高电平有效|
|UA|触摸板电源输入，3.3V|
|D+|USB D+数据线|
|D-|USB D-数据线|

## 二、技术参数
* 供电电压：3.0V~5.5V，典型值：3.3V
* 工作电流：30~60mA，典型值：40mA
* ADC测量速率：8Bps~860Bps，可设置
* UART接口配置（默认波特率：57600，数据位：8，停止位1，校验位：none，TTL电平）
* USB接口支持全速2.0标准
* 图像处理时间：<0.4秒
* 上电延时：<0.1秒，模块上电后需要约 0.1S 初始化工作
* 指纹搜索时间：<0.3秒
* 拒真率：<1%
* 认假率：<0.001%
* 指纹存容量：300枚
* 工作环境：温度-20~60摄氏度，湿度<90%（无凝露）

## 三、软件接口

### AS608(uartObj) - 创建ADS1115驱动对象
* 函数原型：
> fig = AS608(uartObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|uartObj|UART|是|调用此函数前需确保uartObj对象已经处于open状态|

* 返回值：

执行成功，返回指纹识别模块对象；创建失败，抛出Exception

* 示例代码：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：
```log
AS608 inited!
```

### getEmptyPosition - 获取空白位置ID

* 函数功能：
查询AS608内部空闲指纹存储ID，即未存储有效指纹模板的存储空间

* 函数原型：
> ADS1115.getEmptyPosition()

* 参数说明：

无


* 返回值：
成功返回非负值，代表可用指纹存储ID，失败返回as608.FAIL

* 示例：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

id = fig.getEmptyPosition()
print("available id:", id)

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：
```log
AS608 inited!
available id: 1
```

### fingerEnroll - 指纹注册

* 函数功能：
开始指纹注册操作，指纹识别的是通过将采集到的指纹和指纹库中的指纹进行特征匹配，匹配成功则指纹识别通过；否则指纹识别失败。需要“指纹注册”过程才能完成向指纹库录入指纹的目的。

* 函数原型：
> AS608.fingerEnroll(id)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|id|int|是|本次指纹注册成功后放在指纹识别库中的位置ID，范围为0~299|

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

id = fig.getEmptyPosition()
print("available id:", id)

if id >= 0:
    fig.fingerEnroll(id)

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：

```log
AS608 inited!
available id:1
wait for finger print on the pannel
finger detected
take off your finger, please
put on your finger again, please
creating finger model
store finger model
store finger model success
```

### fingerSearch - 指纹识别
* 函数功能：
进行指纹识别操作

* 函数原型：
> AS608.fingerSearch()

* 参数说明：

无

* 返回值：
(result, id, confidence)，返回值说明如下：

|参数|类型|说明|
|-----|----|----|
|result|int|as608.SUCCESS代表在指纹库中搜索到指纹；as608.FAIL代表未搜索到指纹|
|id|int|该指纹在指纹库中的位置|
|confidence|int|可信度，范围：0~65535，数值越大，可信度越高|

* 示例：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

result, id, confidence = fig.fingerSearch()

if result == as608.SUCCESS:
    print('finger search successs,', id, confidence)
else:
    print('your finger was not enrolled')

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：

```log
AS608 inited!
finger search successs, 0, 190
```

### deleteModel - 从指纹库中删除特定指纹

* 函数功能：
从指纹库中删除特定指纹

* 函数原型：
> AS608.deleteModel(id)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|id|int|是|要删除的指纹在指纹识别库中的位置ID，范围为0~299|

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

id = 0
fig.deleteModel(id)
print("model delete done")

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：

```log
AS608 inited!
model delete done
```

### emptyDatabase - 清空指纹库

* 函数功能：
清空指纹库
> !慎用！

* 函数原型：
> AS608.emptyDatabase()

* 参数说明：

无

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

fig.emptyDatabase()
print("empty database done")

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：

```log
AS608 inited!
empty database done
```

下面的API是AS608模块提供的原子功能的API，如果开发者对这个模块的功能比较熟悉，可以采用，但如果开发者对这个模块不熟悉，不建议直接使用下面的API。

### getImage - 采集指纹图片

* 函数功能：
清空指纹库
> !慎用！

* 函数原型：
> AS608.getImage()

* 参数说明：

无

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：
请参考fingerEnroll函数实现里边的用法

### image2Character - 指纹图片生成特征值

* 函数功能：
清空指纹库
> !慎用！

* 函数原型：
> AS608.image2Character(bufferId)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|bufferId|int|否|转换后的特征值存储缓冲区ID, 1代表将特征值存储在Buffer1中；2代表将特征值存储在Buffer2中|

* 返回值：
成功返回0；失败返回as608.FAIL

* 示例：
请参考fingerEnroll函数实现里边的用法


### createModel - 合并特征并生成模板

* 函数功能：
合并特征并生成模板

* 函数原型：
> AS608.createModel()

* 参数说明：

无

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：
请参考fingerEnroll函数实现里边的用法

### storeModel - 存储模板

* 函数功能：
将指纹对应的特征模板存放到指纹数据库中

* 函数原型：
> AS608.storeModel(id)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|id|int|否|指纹存储在指纹库中的位置，范围[0,299]|

* 返回值：
成功返回as608.SUCCESS；失败返回as608.FAIL

* 示例：
请参考fingerEnroll函数实现里边的用法

### search - 在指纹库中搜索指纹

* 函数功能：
在指纹库中搜索特征模板

* 函数原型：
> AS608.search()

* 参数说明：
无

* 返回值：
(result, id, confidence)，变量说明如下：

|参数|类型|说明|
|-----|----|----|
|result|int|as608.SUCCESS代表在指纹库中搜索到指纹；as608.FAIL代表未搜索到指纹|
|id|int|该指纹在指纹库中的位置|
|confidence|int|可信度，范围：0~65535，数值越大，可信度越高|

* 示例：
请参考fingerSearch函数实现里边的用法


## 四、接口案例
此使用实例在board.json中定义了名为as608的UART类型的对象。在Python脚本中首先录制指纹，然后周期性的进行指纹识别

* board.json配置：

```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "as608": {
            "type": "UART",
            "port": 2,
            "dataWidth": 8,
            "baudRate": 57600,
            "stopBits": 2,
            "flowControl": "disable",
            "parity": "none"
        }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

* 案例代码：

```python
from driver import UART    # 驱动库
import as608               # AS608指纹识别模块库

uartDev = UART()
uartDev.open('as608')      # 按照board.json中名为"as608"的设备节点的配置参数（主设备UART波特率、起始位、数据位及停止位长度等）初始化UART类型设备对象
fig = as608.AS608(uartDev) # AS608指纹识别设备初始化
print("AS608 inited!")

id = fig.getEmptyPosition()
print("empty record id:", id)

ret = fig.fingerEnroll(id)
if ret:
    print("finger enroll failed!")
else:
    print("finger enroll success!")

while True:
    ret, id, score = fig.fingerSearch()
    print(ret, id, score)

uartDev.close()             # 关闭UART设备
del fig                     # 删除AS608指纹识别设备对象
```

* 输出：
```python
AS608 inited!
empty record id:1
wait for finger print on the pannel
finger detected
take off your finger, please
put on your finger again, please
creating finger model
store finger model
store finger model success
finger enroll success!

```

<br>

## 五、通信协议

主控芯片和AS608之间采用UART进行通信。

发往AS608模块的指令都遵循如下的格式：
|名称|包头|芯片地址|包标识|包长度|指令标识|参数1|...|参数n|校验和|
|:-|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|字节数（单位：bytes）|2|4|1|2|1|N|N|N|2|
|内容|0xEF01|xxxx|01|N=||||||

AS608模块回复的响应都遵循如下的格式：

|名称|包头|芯片地址|包标识|包长度|确认码|返回参数|校验和|
|:-|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|字节数（单位：bytes）|2|4|1|2|1|N|2|
|内容|0xEF01|xxxx|01|N=||||||

AS608模组共提供31条指令，实际使用中最常用的指令有如下几条：

|指令码|功能说明|
|:-:|:-|
|01H|从传感器上读入图像存于图像缓冲区|
|02H|根据原始图像生成指纹特征存于CharBuffer1或CharBuffer2|
|03H|精确比对CharBuffer1与CharBuffer2中的特征文件|
|04H|以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库|
|05H|将CharBuffer1与CharBuffer2中的特征文件合并生成模板存于CharBuffer1与CharBuffer2|
|06H|将特征缓冲区中的文件储存到flash指纹库中|
|0CH|删除flash指纹库中的一个特征文件|
|0DH|清空flash指纹库|
|0EH|设置系统参数|
|0FH|读系统基本参数|
|1BH|高速搜索FLASH|
|1DH|读有效模板个数|

AS608模块回复的响应包中的确认码如下：

|确认码|功能说明|
|:-:|:-|
|00H|表示指令执行完毕或OK|
|01H|表示数据包接收错误|
|02H|表示传感器上没有手指|
|03H|表示录入指纹图像失败|
|04H|表示指纹图像太干、太淡而生不成特征|
|05H|表示指纹图像太湿、太糊而生不成特征|
|06H|表示指纹图像太乱而生不成特征|
|07H|表示指纹图像正常，但特征点太少（或面积太小）而生不成特征|
|08H|表示指纹不匹配|
|09H|表示没搜索到指纹|
|0aH|表示特征合并失败|
|0bH|表示访问指纹库时地址序号超出指纹库范围|
|0cH|表示从指纹库读模板出错或无效|
|0dH|表示上传特征失败|
|0eH|表示模块不能接受后续数据包|
|0fH|表示上传图像失败|
|10H|表示删除模板失败|
|11H|表示清空指纹库失败|
|13H|表示口令不正确|
|15H|表示缓冲区内没有有效原始图而生不成图像|
|18H|表示读写 FLASH 出错|
|19H|未定义错误|
|1AH|无效寄存器号|
|1BH|寄存器设定内容错误号|
|1CH|记事本页码指定错误|
|1DH|端口操作失败|
|1EH|自动注册失败|
|1FH|指纹库满|

实际控制此指纹识别模块进行指纹录入和指纹识别的流程请参考下图。
<div align="center">
<img src=./../../docs/images/AS608_功能流程图.png width=50%/>
</div>

</br>

## 参考文献及购买链接
[1] [AS60x指纹识别SoC通讯手册](https://www.docin.com/p-518404906.html)<br>
[2] [购买链接](https://item.taobao.com/item.htm?id=561808592573)