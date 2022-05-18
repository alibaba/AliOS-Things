# SPI-SPI总线模块

* 类功能：  
SPI是串行外设接口（Serial Peripheral Interface）的缩写，是一种高速的，全双工，同步的通信总线，并且在芯片的管脚上只占用四根线，节约了芯片的管脚，同时为PCB的布局上节省空间，提供方便，正是出于这种简单易用的特性，越来越多的芯片集成了这种通信协议，例如HaaS EDU K1硬件上通过SPI扩展了OLED屏幕。

## open - 创建SPI实例
* 函数功能：  
打开SPI接口，创建 SPI实例。

* 函数原型：
> var oled = spi.open({id: 'oled_spi'});

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的SPI id名称 |

* 返回值：
Object类型，SPI实例，为空代表创建失败。

## write - 通过SPI发送数据
* 函数功能：  
通过SPI发送数据，该函数为阻塞函数，SPI发送完成后才会返回。

* 函数原型：
> var data = [0x01, 0x02, 0x03, 0x04, 0x05]
> spi.write(data);

* 参数说明：  
String | ArrayBuffer类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| data | String ArrayBuffer | 是 | 需要发送的数据 |

* 返回值：
无

## read - 通过SPI读取制定bytes数据
* 函数功能：  
读取指定bytes的SPI数据，该函数为阻塞函数，读取到指定bytes的数据后才会返回。

* 函数原型：
> spi.read(2);

* 参数说明：  
Number类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | Number | 是 | 需要读取的数据字节数 |

* 返回值：
ArrayBuffer类型，读取到的SPI数据

## close - 释放SPI实例
* 函数功能：  
关闭SPI接口，释放SPI实例。

* 函数原型：
>  spi.close();

* 参数说明：  
无

* 返回值：
无

## 使用实例
* SPI模块使用示例<br>
-- [haaseeduk1 SPI模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/spi)<br>
-- [haas600 SPI模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600-EC600S/basic/spi)<br>
-- [ESP32 SPI模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/spi)<br>

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

