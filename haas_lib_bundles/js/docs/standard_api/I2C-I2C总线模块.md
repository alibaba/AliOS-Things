# I2C-I2C总线模块

* 类功能：  
I2C总线是由Philips公司开发的一种简单、双向二线制同步串行总线。在硬件上I2C总线只需要一根SCL时钟线和一根SDA数据线即可完成多主机多从设备的总线通信模式。主设备用于启动总线传送数据，并产生时钟驱动与从设备的数据通信，此时任何被寻址的器件均被认为是从器件。如果主机要发送数据给从器件，则主机首先寻址从器件，然后主动发送数据至从器件，最后由主机终止数据传送。一般而言，I2C总线的8位双向数据传输位速率在标准模式下可达100Kbit/s，快速模式下可达400Kbit/s，高速模式下可达3.4Mbit/s。

## open - 创建I2C实例
* 函数功能：  
打开I2C接口，创建 I2C实例。

* 函数原型：
> var sensor = i2c.open({id: 'sensor'});

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的I2C id名称 |

* 返回值：
Object类型，ADC实例，为空代表创建失败。

## write - 通过I2C发送数据
* 函数功能：  
通过I2C发送数据，该函数为阻塞函数，I2C发送完成后才会返回。

* 函数原型：
> I2C.write([0xF3]);

* 参数说明：  
String | ArrayBuffer类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| data | String ArrayBuffer | 是 | 需要发送的数据 |

* 返回值：
无

## writeMem - 通过I2C写连续内存块
* 函数功能：  
通过I2C向寄存器地址memaddr发送数据，该函数为阻塞函数，I2C发送完成后才会返回。

* 函数原型：
> var data = [‘0x01’, ‘0x02’, ‘0x03’, ‘0x04’, ‘0x05’]
>  I2C.writeMem(0x10, data);

* 参数说明：  
两个参数分别是Number类型和
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| memaddr | Number | 是 | 寄存器偏移地址memaddr |
| data | String ArrayBuffer | 是 | 需要发送的数据 |

* 返回值：
无。

## read - 通过I2C读取制定bytes数据
* 函数功能：  
读取指定bytes的I2C数据，该函数为阻塞函数，读取到指定bytes的数据后才会返回。

* 函数原型：
> I2C.read(2);

* 参数说明：  
Number类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | Number | 是 | 需要读取的数据字节数 |

* 返回值：
ArrayBuffer类型，读取到的I2C数据

## readMem - 通过I2C读连续内存块
* 函数功能：  
通过I2C向寄存器地址memaddr发送数据，该函数为阻塞函数，I2C读取完成后才会返回。

* 函数原型：
>  I2C.readMem(0x10, 5);

* 参数说明：  
两个参数分别是Number类型和
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| memaddr | Number | 是 | 寄存器偏移地址memaddr |
| bytes | Number | 是 | 需要读取的字节数 |

* 返回值：
ArrayBuffer类型，读取到的I2C数据。

## close - 释放I2C实例
* 函数功能：  
关闭I2C接口，释放 I2C实例。

* 函数原型：
>  I2C.close();

* 参数说明：  
无

* 返回值：
无

## 使用实例
* I2C模块使用示例<br>
-- [haaseeduk1 I2C模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1)<br>
-- [haas600 I2C模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600-EC600S/basic/i2c)<br>
-- [ESP32 I2C模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/i2c)<br>

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

