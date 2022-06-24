# humiturePanel - 温湿度面板模板

&emsp;&emsp;
基于温湿度面板模板可以很方便的将温湿度信息显示在面板上。该模板的代码实现请参考humiturePanel.py的实现。该模板的默认运行效果如下：
<div align="center">
<img src=./../../../images/humiture_panel_execute_result.png width=20%/>
</div>

&emsp;&emsp;
下面是改模板API及UI面板元素的说明。

## 温湿度面板模板API说明
### HumiturePanel() - 创建温湿度面板对象
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

### showTemperature(temperature) - 更新温度值到面板上

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

### showHumidity(humidity) - 更新相对湿度值到面板上

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

### showHumiture(tempearture, humidity) - 同时更新温度和相对湿度值到面板上

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

## 温湿度面板模板元素
&emsp;&emsp;
如下图所示，此温湿度面板一共有8个元素，编号为1-4的元素是温度显示的组成部分，编号为5-8的元素为相对湿度显示的组成部分。
<div align="center">
<img src=./../../../images/UI_Lite_Humiture_Panel_Overview.png width=20%/>
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

&emsp;&emsp;
如果读者想要基于此模板进行UI布局的调整，可能需要用到UI lite的更多组件，UI Lite组件的详细用法请参考[UI Lite控件合集文章](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)

&emsp;&emsp;
另外我们提供了计时器、音乐播放器等模版，只需要几行代码就能使用这些功能。基于这些模板的案例详情请参考如下创意案例。
* [温湿度面板](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AEA703FFFF1A300000001&dataId=800AEA703FFFF1A3)
* [倒计时计时器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800A1DAF896F599700000001&dataId=800A1DAF896F5997)
* [音乐播放器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AE9E2A8B7DCDB00000001&dataId=800AE9E2A8B7DCDB)
