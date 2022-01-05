# adc - ADC外设驱动库

* 类功能：  
ADC模数转换器  

## ADC - 创建ADC对象
* 函数原型：
> adcObj = ADC()

* 参数说明：  
无

* 返回值：  
ADC对象成功，返回ADC对象；ADC对象创建失败，抛出ENOMEN异常

* 示例代码： 

```python
from driver import ADC
adcObj = ADC()
```


## open - 打开ADC

* 函数功能：  
根据board.json中设备节点的配置打开ADC设备

* 函数原型：
> ADC.open(nodeName)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|nodeName|string|是|board.json配置文件中ADC设备节点的名称|

<br>

board.json中的ADC类型设备属性配置项说明如下：

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|port|Number|1|是|配置端口值，这里跟芯片 datasheet上的端口对应, ESP32目前支持端口0~7，ADC2部分使用WIFI无法正常使用|
|sampling|Number|12000000|否|采样率和芯片设置对应，ESP32暂时未用到该参数|
|atten|Number|0|否|衰减配置，跟芯片datasheet对应|
|width|Number|0|否|精度设置，跟芯片datasheet对应|


> ESP32精度标准，目前最高精度为12bit，即为2^12 = 4096，ESP32可以设置4个档位如下：
```python
	  typedef enum {
	  ADC_WIDTH_BIT_9  = 0, /*!< ADC capture width is 9Bit. Only ESP32 is supported. */
	  ADC_WIDTH_BIT_10 = 1, /*!< ADC capture width is 10Bit. Only ESP32 is supported. */
	  ADC_WIDTH_BIT_11 = 2, /*!< ADC capture width is 11Bit. Only ESP32 is supported. */
	  ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit. Only ESP32 is supported. */
 	 } adc_bits_width_t;
```
> ESP32衰减配置，用于确定ADC的读数范围，ESP32衰减等级如下：
```python
	ADC_ATTEN_DB_0 = 0：     电压范围 100mV~950mV ， 满量程电压 1100 mV
	ADC_ATTEN_DB_2_5 = 1：  电压范围 100mV~950mV ，满量程电压 1500 mV
	ADC_ATTEN_DB_6 = 2：     电压范围 100mV~950mV ，满量程电压 2200 mV
	ADC_ATTEN_DB_11 = 3：   电压范围 100mV~950mV ，满量程电压 3900 mV
```
> ESP32 ADC读数和电压转换： Vout = Dout * Vmax /Dmax  Dout为ESP32引脚量测到值，Vmax为 atten 设置的满量程电压，Dmax为 width 设置的精度值， Vout即为转换的电压值。如 width 设置为 3，atten 设置为 0 时，加入量测到的 RAW 值为 120， 则相应的电压转换为： Vout(mV) = 120 * 1100 / (2^12 - 1)


<br>

* 返回值：  
打开ADC设备成功返回0；打开ADC设备失败返回失败错误码

* 示例： 

```python
# 假设ADC通道6的用途是用来量测电池电压，在board.json中将此节点命名为“voltmeter”
'''
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "voltmeter": {
            "type": "ADC",
            "port": 6,
            "sampling": 12000000,
            "atten": 0,
            "width": 3
        }
    }
}
'''

from driver import ADC

adcObj = ADC()
ret = adcObj.open("voltmeter")
print(ret)
```

* 输出：

```
#open正常返回0， 异常返回异常码
0
```

## close - 关闭ADC

* 函数功能：  
关闭数模转换设备

* 注意事项：  
需确保要关闭的ADC处于open状态

* 函数原型：
> ADC.close()

* 参数说明：  
无

<br>

* 返回值：  
关闭ADC设备成功返回0；关闭ADC设备失败返回失败错误码

* 示例： 

```python
# 请确保board.json中包含名为adc2的节点

from driver import ADC

adcObj = ADC()
ret = adcObj.open("voltmeter")

ret = adcObj.close()
print(ret)
```

* 输出：
```
0
```

## readVoltage - 读取采集到电压数据

* 函数功能：  
读取ADC电压数据

* 注意事项：  
需确保此ADC处于open状态

* 函数原型：
> ADC.readVoltage()

* 参数说明：
无

* 返回值：  
读取成功则返回读取到的电压值；负数代表读取数据失败

* 示例： 

```python
# 请确保board.json中包含名为adc6的节点

from driver import ADC

adcObj = ADC()
ret = adcObj.open("voltmeter")
value = adcObj.readVoltage()

print(value)

adcObj.close()
```

* 输出：
```
打印实际从串口读到的内容或打印错误信息
```

<br>

## readRaw - 读取采集到原始值

* 函数功能：  
读取ADC原始数据

* 注意事项：  
需确保此ADC处于open状态

* 函数原型：
> ADC.readRaw()

* 参数说明：
无

* 返回值：  
读取成功则返回读取到的原始值；负数代表读取数据失败

* 示例： 

```python
# 请确保board.json中包含名为adc6的节点

from driver import ADC

adcObj = ADC()
ret = adcObj.open("voltmeter")
value = adcObj.readRaw()

print(value)

adcObj.close()
```

* 输出：
```
打印实际从串口读到的内容或打印错误信息
```

<br>

## getHwParam - 读取芯片设置的参数

* 函数功能：  
读取设置的ADC相关参数，用于手动进行电压计算

* 注意事项：  
需确保此ADC处于open状态

* 函数原型：
> ADC.getHwParam()

* 参数说明：
无

* 返回值：  
读取成功则返回字典，字典中带有设置的参数

* 示例： 

```python
# 请确保board.json中包含名为adc6的节点

from driver import ADC

adcObj = ADC()
ret = adcObj.open("voltmeter")
value = adcObj.getHwParam()

print(value)

adcObj.close()
```

* 输出：
```
{'ADC_WIDTH_BIT': 3, 'ADC_ATTEN_DB': 0}
```

## 使用实例

* 代码：

```python
from driver import ADC
import time

voltmeter = ADC()
ret = voltmeter.open('voltmeter')

ret = voltmeter.readVoltage()
print("adc value is : %d" % ret)

voltmeter.close()
```

* 输出：

```python
adc value is : 1100
```

<br>
