# FS-文件系统模块

* 类功能：  
System模块提供了JavaScript的接口，用于查询当前系统版本，时间，内存使用等能力。

## versions - 获取固件版本号
* 函数功能：  
读取硬件模组的固件版本号。

* 函数原型：
> var version = system.versions().module

* 参数说明：  
无

* 返回值：
Object类型，固件版本号信息。

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| module | String | 是 | 硬件模组的固件版本号 |

## version - 获取JS轻应用框架版本号
* 函数功能：  
读取JS轻应用框架版本号。

* 函数原型：
> var version = system.version()

* 参数说明：  
无

* 返回值：
String类型，获取JS轻应用框架版本号。

## appversion - 获取用户app版本号
* 函数功能：  
读取用户app版本号。

* 函数原型：
> var version = system.appversion()

* 参数说明：  
无

* 返回值：
String类型，获取用户app版本号。

## getSystemInfo - 获取系统软件版本信息
* 函数功能：  
读取系统软件常见的版本信息。

* 函数原型：
> var version = system.getSystemInfo().userjsVersion

* 参数说明：  
无

* 返回值：
Object类型，系统软件版本号信息。

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| userjsVersion | String | 是 | HaaS JS轻应用框架版本号，同version()方法的结果 |
| appVersion | String | 是 | 用户app版本号，同appversion()方法的结果 |
| kernelVersion | String | 是 | OS版本号 |
| systemVersion | String | 是 | 系统软件版本号 |
| buildTime | String | 是 | 固件编译时间 |
| hardwareVersion | String | 是 | 硬件版本号 |

## platform - 获取OS平台版本
* 函数功能：  
获取OS平台版本。

* 函数原型：
> var platform = system.platform()

* 参数说明：  
无

* 返回值：
String类型，获取JS轻应用框架版本号。

## uptime - 获取系统时间
* 函数功能：  
获取当前系统运行时间。单位为毫秒。

* 函数原型：
> var time = system.uptime()

* 参数说明：  
无

* 返回值：
Number类型，当前系统运行时间。单位为毫秒。

## memory - 获取heap内存信息
* 函数功能：  
读取当前系统heap内存信息，单位为字节。

* 函数原型：
> var mem = system.memory().heapTotal

* 参数说明：  
无

* 返回值：
Object类型，其属性包含,

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| heapTotal | Number | 是 | 读取系统heap总大小 |
| heapUsed | Number | 是 | 读取系统heap已使用大小 |
| heapFree | Number | 是 | 读取系统heap剩余大小 |

## gc - 获取当前内存垃圾数据
* 函数功能：  
获取当前内存垃圾数据。单位为字节。

* 函数原型：
> var gc = system.gc()

* 参数说明：  
无

* 返回值：
Number类型，当前内存垃圾数据。单位为字节。

## reboot - 软件复位系统
* 函数功能：  
软件复位系统。

* 函数原型：
> system.reboot()

* 参数说明：  
无

* 返回值：
无

## 使用实例
* system模块使用示例<br>
-- [haaseduk1 system模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/system)<br>
-- [haas600/510/531 system模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/common/system)<br>
-- [ESP32 system模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/system)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|是||
|HaaS EDU K1|是||
|HaaS600-EC100Y|是||
|HaaS600-EC600S|是||
|HaaS510|是||
|HaaS531|是||
|ESP32|是||

<br>

