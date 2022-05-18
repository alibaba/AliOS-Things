# Log-日志模块

* 类功能：  
ulog 日志系统是HaaS轻应用为开发者提供的日志记录系统，为基于HaaS轻应用的开发者提供日志分级控制，日志本地存储（stdlog），日志信息上云(cloudlog)等服务。

## debug - 调试级别记录日志
* 函数功能：  
向日志端口输出调试级别（debug）记录日志。

* 函数原型：
>ulog.debug('test for debug')

* 参数说明：  
String类型，日志字符串。

* 返回值：
无。

## info - 信息级别记录日志
* 函数功能：  
向日志端口输出信息级别（info）记录日志。

* 函数原型：
>ulog.info('test for info')

* 参数说明：  
String类型，日志字符串。

* 返回值：
无。

## warn - 告警级别记录日志
* 函数功能：  
向日志端口输出警告级别（warn）记录日志。

* 函数原型：
>ulog.warn('test for warn')

* 参数说明：  
String类型，日志字符串。

* 返回值：
无。

## error - 错误级别记录日志
* 函数功能：  
向日志端口输出错误级别（error）记录日志。

* 函数原型：
>ulog.error('test for error')

* 参数说明：  
String类型，日志字符串。

* 返回值：
无。

## fatal - 致命级别记录日志
* 函数功能：  
向日志端口输出致命级别（fatal）记录日志。

* 函数原型：
>ulog.fatal('test for fatal')

* 参数说明：  
String类型，日志字符串。

* 返回值：
无。

## stdloglevel - 设置本地终端日志输出等级
* 函数功能：  
设置本地终端（例如串口输出，本地文件保存等）日志输出等级。

* 函数原型：
> ulog.stdloglevel('debug')

* 参数说明：  
String类型，设置本地日志等级：debug，info，warning，error，fatal，

* 返回值：
无

## setlogfilepath - 设置本地日志保存路径
* 函数功能：  
设置本地日志保存路径。

* 函数原型：
> ulog.setlogfilepath('/data/ulog.txt')

* 参数说明：  
设置本地日志文件路径。

* 返回值：
无

## setlogfilesize - 设置本地日志文件大小限制
* 函数功能：  
设置本地日志文件大小限制，单位字节。

* 函数原型：
> ulog.setlogfilesize(8192)

* 参数说明：  
设置本地日志文件大小限制，单位字节。

* 返回值：
无

## cloudloglevel - 设置上云日志输出等级
* 函数功能：  
设置上云日志（日志保存至阿里云）输出等级。

* 函数原型：
> ulog.cloudloglevel('debug')

* 参数说明：  
String类型，设置本地日志等级：debug，info，warning，error，fatal，

* 返回值：
无

## 使用实例
* Log模块使用示例<br>
-- [haaseeduk1 Log模块使用示例](https://gitee.com/alios-things/amp/blob/rel_3.3.0/example-js/haaseduk1/log/app.js)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|否||
|HaaS EDU K1|是||
|HaaS600-EC100Y|否||
|HaaS600-EC600S|否||
|HaaS510|否||
|HaaS531|否||
|ESP32|否||

<br>

