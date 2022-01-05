# driver - 系统外设库

* 模块功能：  
driver系统外设库是阿里云IoT HaaS团队推出的微处理器外设驱动库。它和micropython原生提供的machine库的区别在于，它采用了硬件配置信息和代码分离的方式，将所有的硬件配置信息都放入board.json中进行配置，这样如果您的python项目需要进行产品升级，在所选外围传感器不变的情况下只需要对board.json中的硬件端口配置信息进行修改，无需修改python脚本就可以完成产品升级，真正做到了控制硬件的python应用程序也可以无缝移植到新产品。

* board.json - 硬件资源配置文件  
board.json中主要是存储了GPIO, UART, I2C, SPI, ADC, DAC, PWM, Timer, WatchdogTimer等类型的外设配置信息。

* board.json语法说明  
一个完整的board.json包含如下表格所示的配置项：

|配置项|类型|是否是必需项|描述|
|:-|:-|:-|:-|
|name|String|否|板子名称|
|version|String|否|Python轻应用版本号|
|io|Object|是|硬件接口配置信息|
|debugLevel|String|否|设置日志登记，默认为ERROR级别|

下面是以NodeMCU-32S为例的一个board.json配置。

```python
{
    "name": "NodeMCU-32S",
    "version": "1.0.0",
    "io": {
      "led": {
        "type": "GPIO",
        "port": 2,
        "dir": "input",
        "pull": "pullup"
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
  }
```
> 说明："io"里边的设备节点名称请按照实际功能进行定义，比如，NodeMCU-32S上面GPIO2外接了一颗LED，则名称建议起成"led"或"light"， `不要` 使用"gpio2"这样的名称定义。因为Python程序在移植到其它开发板的时候，LED灯的可能会接到GPIO5上，如果将其命名为类似"gpio2"这样的名称，配置项里边的portid需要修改为5，不修改"gpio2"的名称会很奇怪，而修改"gpio2"的名称就需要修改python脚本，因此会破坏Python脚本的可移植性。

debugLevel是系统日志的输出级别，分为如下几个等级，如果没有配置默认为ERROR级别。

|等级|说明|
|:-|:-|
|DEBUG|显示debug及以上级别的日志|
|INFO|显示info及以上级别的日志|
|WARN|显示warning及以上级别的日志|
|ERROR|显示error及以上级别的日志|
|FATAL|显示fatal及以上级别的日志|

* board.json外设配置项说明  
不同类型的外设配置项是不同的，具体请参考下面各个驱动类API说明中关于board.json配置项的说明。
  * [I2C](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/i2c)
  * [GPIO](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/gpio)
  * [UART](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/uart)
  * [SPI](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/spi)
  * [PWM](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/pwm)
  * [ADC](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/adc)
  * [WDT](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/driver/wdt)


<br>

* 不同目录下的board.json的优先级说明  
系统目录中可能存在多个board.json文件，在实际加载的时候，Python引擎会根据下表的优先级加载优先级最高的board.json中的内容到系统中。

|优先级|配置文件路径|说明|
|:-|:-|:-|
|1|/sdcard/board.json|最高优先级|
|2|/data/pyamp/board.json|第2优先级|
|3| /data/python-apps/driver/board.json|最低优先级|


<br>
