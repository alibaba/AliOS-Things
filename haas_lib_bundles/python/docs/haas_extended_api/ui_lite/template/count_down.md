
# countDown - 倒计时计数器面板模板

&emsp;&emsp;
倒计时计数器面板模板可以很方便的将倒计时信息显示在面板上。该模板的默认运行效果如下：
<div align="center">
<img src=./../../../images/count_down_show.png width=20%/>
</div>

&emsp;&emsp;
倒计时面板模板上有4个指定的倒计时时间，单位是分钟，默认的数值分别是1，3，5，10；可以在创建count down页面的时候修改默认值，最大倒计时时间是10分钟。

&emsp;&emsp;
下面是对该模板API及UI面板元素的说明。

## 倒计时计数器模板API说明

### CountDown() - 创建计数器面板对象
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

### createPage(value1 = 1, value2 = 3, value3 = 5, value4 = 10) - 创建计时器面板

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
from countDown import CountDown

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

## 倒计时计数器模板元素
&emsp;&emsp;
如下图所示，此倒计时计数器一共有8个元素，编号为1-4的元素是温度显示的组成部分，编号为5-8的元素为相对湿度显示的组成部分。
<div align="center">
<img src=./../../../images/count_down_elements.png width=20%/>
</div>

&emsp;&emsp;
下面是音乐播放器模板代码中显示各元素的位置，读者可以根据自己的需求对其进行替换或修改。

|元素ID|功能|相关代码|说明|
|-----|----|:---|----|
|1|当前选中定时器的外观|arc_event_handler中arc相关代码|arc_event_handler中arc相关代码会在定时器按钮被点中后执行|
|2|定时器未被选中时外观|createPage函数中totalTime相关代码|timeCount[x]的x代表timer index，四个定时器从左到右index依次为0,1,2,3|
|3|当前选中定时器倒计时剩余时间|setLabelValue|该函数中负责刷新UI时间|
|4|开始/暂停按钮|createPage函数中startButton相关代码|按钮被按下后的事件处理函数请参考start_button_event_handler|
|5|复位按钮|createPage函数中resetButton相关代码|按钮被按下后的事件处理函数请参考reset_button_event_handler|

&emsp;&emsp;
如果读者想要基于此模板进行UI布局的调整，可能需要用到UI lite的更多组件，UI Lite组件的详细用法请参考[UI Lite控件合集文章](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)

&emsp;&emsp;
另外我们提供了温湿度面板、音乐播放器等模版，只需要几行代码就能使用这些功能。基于这些模板的案例详情请参考如下趣味案例。
* [温湿度面板](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AEA703FFFF1A300000001&dataId=800AEA703FFFF1A3)
* [倒计时计时器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800A1DAF896F599700000001&dataId=800A1DAF896F5997)
* [音乐播放器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AE9E2A8B7DCDB00000001&dataId=800AE9E2A8B7DCDB)
