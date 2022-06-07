# 温湿度面板
&emsp;&emsp;
随着这几年智能家居行业的快速发展，家庭用的智能面板越来越普及。智能面板不仅可以用来显示时间、家用电器的状态、回家/离家模式选择还可以用来调节空调温度、加湿器状态等等信息，给人们的生活带来了很多便利。本文则是以最常用的智能温湿度面板为例，介绍如何使用快速打造一个UI面板。

## 1、简介
&emsp;&emsp;
本文将基于HaaS UI Lite开发框架使用Python语言进行家居温湿度面板的打造。

&emsp;&emsp;
本案例打造需要使用到如下硬件：
* [M5StackCore2开发板](../../../startup/M5StackCore2_startup.md)      一台
* [SHT3X温湿度传感器](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CD7F0F2F57FF800000002&dataId=800CD7F0F2F57FF8)   一个

&emsp;&emsp;
将SHT3X和M5StackCore2通过I2C接口连接起来，硬件连线图如下图所示。
<div align="center">
<img src=./../../../images/humiture_panel_硬件连线图_m5stack.png width=40%/>
</div>

## 2、设备端开发

### 2.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕，详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)中的说明。

### 2.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“温湿度面板”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

## 3、运行效果
&emsp;&emsp;
将humiture_panel工程推送到M5StackCore2开发板之后，脚本会自动运行。屏幕上面显示如下信息：

<div align="center">
<img src=./../../../images/humiture_panel_execute_result.png width=25%/>
</div>

&emsp;&emsp;
脚本运行过程中日志如下。其中：
* python execute from /data/pyamp/main.py：代表Python脚本开始运行<br>
* create humiture device：创建温湿度传感器对象<br>
* create humiture panel：创建温湿度面板<br>
* temperature: 21.32931：从温湿度传感器读取到的温度值
* humidity: 33.0602：从温湿度传感器读取到的相对湿度值

```log
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
create humiture device
create humiture panel
temperature: 21.32931
humidity: 33.0602
temperature: 21.35601
humidity: 33.11513
temperature: 21.38539
humidity: 33.12886
```

## 4、HaaS UI Lite 温湿度面板模板说明
&emsp;&emsp;
本案例中使用到了HaaS UI Lite封装的**温湿度面板**类型的**模板**。此模板的代码实现请参考humiturePanel.py的实现。基于此模板可以很方便的将温湿度信息显示在面板上。下面是对模板元素及面板库API的说明。

### 4.1、温湿度面板元素
&emsp;&emsp;
如下图所示，此温湿度面板一共有8个元素，编号为1-4的元素是温度显示的组成部分，编号为5-8的元素为相对湿度显示的组成部分。
<div align="center">
<img src=./../../../images/UI_Lite_Humiture_Panel_Overview.png width=40%/>
</div>

&emsp;&emsp;
下面是humiturePanel面板模板代码中显示各元素的位置，读者可以根据自己的需求对其进行替换或修改。
|元素ID|功能|相关代码|说明|
|-----|----|:---|----|
|1|温度图标|self.createTemperatureItem(self.container, RESOURCES_ROOT + "temperature.png", RESOURCES_ROOT + "centigrade_l.png", "Temperature")|静态图片，尺寸：64*64|
|2|温度值|self.temperatureLable.set_text(str(int(temperature)))|动态更新|
|3|温度单位图标|self.createTemperatureItem(self.container, RESOURCES_ROOT + "temperature.png",RESOURCES_ROOT + "centigrade_l.png", "Temperature")|静态图片，尺寸：64*64|
|4|温度英文字串|同“元素1”|字符串|
|5|相对湿度图标|self.createHumidityItem(self.container, RESOURCES_ROOT + "humidity.png", "Humidity")|静态图片，尺寸：|
|6|相对湿度值|self.humidityLable.set_text(str(int(humidity)) + " %")|动态更新|
|7|相对湿度单位图标|同“元素6”|%为符浩|
|8|相对湿度英文字串|同“元素5”|静态文字|

### 4.2、温湿度面板模板API说明

#### 4.2.1、HumiturePanel() - 创建温湿度面板对象
<br>

* 函数原型

> panelObj = HumiturePanel()

* 参数说明
> 无

* 返回值
> 创建成功返回温湿度面板类型的对象

* 示例代码
```python
from humiturePanel import HumiturePanel

humiturePage = HumiturePanel()
print('create humiture panel')
```

* 代码输出
```log
create humiture panel
```
</br>

#### 4.2.2、showTemperature(temperature) - 更新温度值到面板上

* 函数功能：

> 更新温度值temperature到面板上


* 函数原型

> HumiturePanel.showTemperature(temperature)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| temperature | int | 是 | 如果是float等类型的变量，则会被强制转换成int型 |

* 返回值

> 无

* 示例代码
```python
from humiturePanel import HumiturePanel

humiturePage = HumiturePanel()
print('create humiture panel')

humiturePage.showTemperature(26)
print('update temperature value done')

```
* 代码输出
```log
create humiture panel
update temperature value done
```

#### 4.2.3、showHumidity(humidity) - 更新相对湿度值到面板上

* 函数功能：

> 更新相对湿度值humidity到面板上


* 函数原型

> HumiturePanel.showHumidity(humidity)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| humidity | int | 是 | 如果是float等类型的变量，则会被强制转换成int型 |

* 返回值

> 无

* 示例代码
```python
from humiturePanel import HumiturePanel

humiturePage = HumiturePanel()
print('create humiture panel')

humiturePage.showHumidity(62)
print('update humidity value done')

```
* 代码输出
```log
create humiture panel
update humidity value done
```

#### 4.2.4、showHumiture(tempearture, humidity) - 同时更新温度和相对湿度值到面板上

* 函数功能：

> 更新温度tempearture和相对湿度值humidity到面板上


* 函数原型

> HumiturePanel.showHumiture(tempearture, humidity)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| temperature | int | 是 | 如果是float等类型的变量，则会被强制转换成int型 |
| humidity | int | 是 | 如果是float等类型的变量，则会被强制转换成int型 |

* 返回值

> 无

* 示例代码
```python
from humiturePanel import HumiturePanel

humiturePage = HumiturePanel()
print('create humiture panel')

humiturePage.showHumiture(26, 62)
print('update humiture value done')

```
* 代码输出
```log
create humiture panel
update humiture value done
```

> 本示例中使用的是SHT3X温湿度传感器获取温度和相对湿度信息并将其显示在屏幕上。您也可以参考[HaaS硬件积木](https://haas.iot.aliyun.com/solution/hardware)的页面来查来选择其它的温湿度传感器。

## 思考
&emsp;&emsp;
本案例中使用外置的温湿度传感器对环境温湿度进行测量并显示在面板上。HaaS官网中还有通过其它开发板(ESP32、HaaS EDU K1等)量测环境温度。假设有一台“ESP32温湿度计”开发板（和M5StackCore2不是同一台设备的情况下）布置在智慧农业的大棚中。请读者思考下如何通过物联网平台远程获取到“ESP32温湿度计”的温室度信息并显示在M5StackCore2的面板上。

<details>
<summary>提示</summary>

> 在物联网平台建立一个虚拟的温湿度面板设备，创建“温度”和“湿度”的物模型，通过物联网平台的“规则引擎”能力将“ESP32温湿度计”的温室度信息流转到“虚拟的温湿度面板设备”的物模型，物模型数据下发到M5StackCore2设备可以正确显示了。

</details>
