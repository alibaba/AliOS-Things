# 家居温湿度面板
&emsp;&emsp;
随着这几年智能家居行业的快速发展，家庭用的智能面板越来越普及。智能面板不仅可以用来显示时间、家用电器的状态、回家/离家模式选择还可以用来调节空调温度、加湿器状态等等信息，给人们的生活带来了很多便利。本文则是以最常用的智能温湿度面板为例，介绍如何使用快速打造一个UI面板。

## 简介
&emsp;&emsp;
本文将基于HaaS UI Lite开发框架使用Python语言进行家居温湿度面板的打造。

## 准备
&emsp;&emsp;
本案例打造需要使用到如下硬件：
* [M5StackCore2开发板](M5StackCore2快速开始)      一台
* [SHT3X温湿度传感器](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800CD7F0F2F57FF800000002&dataId=800CD7F0F2F57FF8)   一个

### 硬件连线图
&emsp;&emsp;
将SHT3X和M5StackCore2通过I2C接口连接起来，硬件连线图如下图所示。

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕，详情请参考[M5StackCore2](M5StackCore2开发板快速开始链接)中的说明。

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“humiture_panel”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/humiture_panel_创建工程_m5stack.png width=40%/>
</div>

&emsp;&emsp;
将[本案例](./code/)脚本的代码全部复制后，覆盖“humiture_panel”工程根目录下，main.py文件如下图所示：
<div align="center">
<img src=./../../../images/4_1_3_humiture_panel工程_esp32.jpg width=80%/>
</div>

## 运行效果
&emsp;&emsp;
Python脚本推送到M5StackCore2开发板之后 ，会自动运行，运行过程中日志如下。其中：
* xxxx：代表系统启动成功<br>
* yyyy：代表从温湿度传感器读取到的温湿度信息<br>
* zzzz：代表将温湿度信息刷新到屏幕上<br>

```log
xxxx
yyyy
zzzz
eeee
```

## HaaS UI Lite 温湿度面板模板说明
&emsp;&emsp;
本案例中使用到了HaaS UI Lite封装的温湿度面板类型的模板。基于此模板可以很方便的将温湿度信息显示在面板上。下面是此模板的API的说明：

> 超众：请参考下面的函数说明把模板的用法及实例代码描述清楚，下面AP3216C的两个API说明是例子。


### AP3216C(i2cObj) - 创建AP3216C驱动对象
<br>

* 函数原型

> AP3216CObj = AP3216C(i2cObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值

> AP3216C对象成功，返回AP3216C对象；AP3216C对象创建失败，抛出Exception

* 示例代码
```
import ap3216c
from driver import I2C

print("Testing ap3216c ...")

i2cDev = I2C()
i2cDev.open("ap3216c")

ap3216cDev = ap3216c.AP3216C(i2cDev)
```

* 输出效果
```python
Testing ap3216c ...
```
</br>

### start() - 启动ap3216c光照强度传感器

* 函数功能：

> 启动ap3216c光照强度传感器


* 函数原型

> AP3216C.start()

* 参数说明

> 无

* 返回值

> 无

* 示例代码

```python
import ap3216c
from driver import I2C

print("Testing ap3216c ...")

i2cDev = I2C()
i2cDev.open("ap3216c")

ap3216cDev = ap3216c.AP3216C(i2cDev)

ap3216cDev.start()
```

* 输出效果
```python
Testing ap3216c ...
```

## 思考
&emsp;&emsp;
本案例中使用外置的温湿度传感器对环境温湿度进行测量并显示在面板中。HaaS官网中还有通过其它开发板(ESP32、HaaS EDU K1等)量测环境温度。假设有一台“ESP32温湿度计”开发板布置在智慧农业的大棚中，请读者思考下如何通过物联网平台远程获取到“ESP32温湿度计”的温室度信息并显示在M5StackCore2的面板上。
> 提示：在物联网平台建立一个虚拟的温湿度面板设备，创建“温度”和“湿度”的物模型，通过物联网平台的“规则引擎”能力将“ESP32温湿度计”的温室度信息流转到“虚拟的温湿度面板设备”的物模型，物模型数据下发到M5StackCore2设备可以正确显示了。
