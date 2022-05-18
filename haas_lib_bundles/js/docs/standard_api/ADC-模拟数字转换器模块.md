# ADC-模拟数字转换器模块

* 类功能：  
模拟数字转换器即A/D转换器，或简称ADC，通常是指一个将模拟信号转变为数字信号的电子元件。通常的模数转换器是将一个输入电压信号转换为一个输出的数字信号。由于数字信号本身不具有实际意义，仅仅表示一个相对大小。故任何一个模数转换器都需要一个参考模拟量作为转换的标准，比较常见的参考标准为最大的可转换信号大小。而输出的数字量则表示输入信号相对于参考信号的大小。

## open - 创建ADC实例
* 函数功能：  
打开ADC接口，创建 ADC实例。

* 函数原型：
> var vbattery = adc.open({id: 'ADC0'});

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的ADC id名称 |

* 返回值：
Object类型，ADC实例，为空代表创建失败。

## readValue - 读取ADC数据
* 函数功能：  
读取ADC数据。

* 函数原型：
> var value = ADC.readValue()

* 参数说明：  
无

* 返回值：
Number类型，读取到的ADC数据。

## close - 关闭ADC实例
* 函数功能：  
关闭ADC实例，释放资源。

* 函数原型：
> ADC.close();

* 参数说明：  
无

* 返回值：
无。

## 使用实例
* adc模块使用示例<br>
-- [haaseeduk1 ADC模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/adc)<br>
-- [ESP32 ADC模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/adc)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|否||
|HaaS200|否||
|HaaS EDU K1|是||
|HaaS600-EC100Y|否||
|HaaS600-EC600S|否||
|HaaS510|否||
|HaaS531|否||
|ESP32|是||

<br>

