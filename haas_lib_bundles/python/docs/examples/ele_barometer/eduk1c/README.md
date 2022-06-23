# 电子气压计
## 1、简介
### 1.1、背景
&emsp;&emsp;
电子气压计在生活中是一种比较常见的气压测量仪器，具有体积小、精度高、反应灵敏等特点。常被用于天气预测、山体的高度测量、无人机高度计等用途。

### 1.2、准备
&emsp;&emsp;
硬件列表
1. HaaS EDU K1C开发板一个
2. uart扩展屏一个

&emsp;&emsp;
本文将基于HaaS UI Lite开发框架使用Python语言进行电子气压计的打造，实际运行效果图如下：
<div align="center">
<img src=./../../../images/ele_barometer/电子气压计.jpg width=80%/>
</div>

## 2、设备端开发
### 2.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保HaaS EDU K1开发环境已经搭建完毕。详情请参考[HaaS EDU K1开发环境](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.665960b1PR2nSt#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup)的说明。

### 2.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“电子气压计”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

## 3、运行效果
&emsp;&emsp;
将创建的工程推送到开发板之后，脚本会自动运行。程序运行之后，屏幕会显示日期、时间、气压值等数据，如下图所示：
<div align="center">
<img src=./../../../images/ele_barometer/电子气压计.gif width=60%/>
</div>

## 4、HaaS UI Lite 电子气压计模板说明
&emsp;&emsp;
本案例中使用到了HaaS UI Lite封装的**电子气压计**类型的**模板**。此模板的代码实现请参考airpressure.py的实现。基于此模板可以很方便的搭建自己的气压计工程。下面是对模板元素及面板库API的说明。

### 4.1、电子气压计元素说明
&emsp;&emsp;
如下图所示，此面板一共有5个元素如下图所示。
<div align="center">
<img src=./../../../images/ele_barometer/电子气压计UI元素.png width=60%/>
</div>

&emsp;&emsp;
下面是电子气压计模板代码中显示各元素的位置，读者可以根据自己的需求对其进行替换或修改。
|元素ID|功能|相关代码|说明|
|-----|----|:---|----|
|1|日期、时间|setDate(self, time)|设置日期、时间，time = utime.localtime()|
|2|图标|iconImg.set_src(iconPath)|iconPath为图标路径|
|3|气压值|setValue(self, pressure)|设置气压值|
|4|单位|unityLabel.set_text(" PA")|设置单位为PA|
|5|说明|tipLabel.set_text("Air pressure")|无|

### 4.2、电子气压计API说明
目前电子气压计提供了气压计传感器初始化接口与屏幕显示接口。

#### 4.2.1、qmp6988_init() - 初始化气压计
<br>

* 函数原型

> qmp6988_init()

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| - | - | 否 | 初始化qmp6988气压计传感器|

* 返回值
> 无

* 示例代码
```python
# -*- encoding: utf-8 -*-
import page_airpressure

if __name__ == '__main__':
    # 初始化气压计
    page_airpressure.qmp6988_init()
```

* 代码输出
```log
无
```
</br>

#### 4.2.2、load_page() - 将电子气压计面板内容送显示器显示
<br>

* 函数原型

> load_page()

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| - | - | 否 | 将电子气压计内容送显示器显示|

* 返回值
> 无

* 示例代码
```python
# -*- encoding: utf-8 -*-
import page_airpressure

if __name__ == '__main__':
    # 初始化气压计
    page_airpressure.qmp6988_init()
    import display_driver

    # 显示气压值
    page_airpressure.load_page()

```

* 代码输出
```log
无
```
</br>

&emsp;&emsp;
本案例中使用到了多种控件，关于HaaS UI Lite的控件说明请参考[链接](https://haas.iot.aliyun.com/haasapi/index.html?#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)。
