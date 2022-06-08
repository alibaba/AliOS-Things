# 倒计时计数器
&emsp;&emsp;
通过UI界面和用户交互已经成为各种设备的首选，界面上不但可以展示各种信息，并且用户根据屏幕上提示进行点击就能进行各种操作。倒计时计数器是我们在日常生活中常用的功能，在锻炼身体、烹饪、比赛等场景经常可以看到。本例便是基于HaaS UI Lite开发一个倒计时器案例。

## 1、简介
&emsp;&emsp;
本文将基于HaaS UI Lite开发框架使用Python语言进行倒计时计数器的打造。

### 1.1、准备
&emsp;&emsp;
本案例打造需要使用到如下硬件：
* [M5StackCore2开发板](../../../startup/M5StackCore2_startup.md)      一台

## 2、设备端开发

### 2.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕，详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)中的说明。

### 2.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“倒计时计数器”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
main.py文件如下图所示：
<div align="center">
<img src=./../../../images/count_down_main界面.png width=100%/>
</div>

### 2.3、运行效果
&emsp;&emsp;
将工程推送到M5StackCore2开发板之后，脚本会自动运行。屏幕上面显示如下信息：

<div align="center">
<img src=./../../../images/count_down_show.png width=25%/>
</div>

## 3、HaaS UI Lite 倒计时计数器模板说明
&emsp;&emsp;
本案例中使用到了HaaS UI Lite封装的**倒计时计数器**类型的**模板**。基于此模板可以很方便的将倒计时信息显示在面板上。

&emsp;&emsp;
倒计时面板上有4个指定的倒计时时间，单位是分钟，默认的数值分别是1，3，5，10；可以在创建count down页面的时候修改默认值，最大倒计时时间是10分钟。

&emsp;&emsp;
下面是**倒计时计数器模板**API的说明。

#### CountDown() - 创建计数器面板对象
<br>

* 函数原型

> stopWatch = CountDown()

* 参数说明
> 

* 返回值
> 创建成功返回计数器类型的对象

* 示例代码
```python
from countDown import CountDown

countDownPage = CountDown()
print('create countDown object')

```

* 代码输出
```log
create countDown object
```
</br>

#### createPage(value1 = 1, value2 = 3, value3 = 5, value4 = 10) - 创建计时器面板

* 函数功能：

> 创建计时器面板，上面有4个可选的倒计时数字，单位是分钟，默认值分别是1，3，5，10；创建的倒计时分钟数不能大于10分钟.


* 函数原型

> CountDown.createPage(value1, value2, value3, value4)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| value1 | int | 否 | 界面最左侧定时器时间长度，单位：分钟 |
| value2 | int | 否 | 界面左侧第二个定时器时间长度，单位：分钟 |
| value3 | int | 否 | 界面最右侧定时器时间长度，单位：分钟 |
| value4 | int | 否 | 界面右侧第二个定时器时间长度，单位：分钟 |

* 返回值

> 无

* 示例代码
```python
from count_down import CountDown

countDownPage = CountDown()
print('create countDown object')

# max value is 10
countDownPage.createPage()
print("create page")

```
* 代码输出
```log
create countDown object
create page
```
