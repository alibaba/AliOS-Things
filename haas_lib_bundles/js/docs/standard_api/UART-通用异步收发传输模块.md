# uart-通用异步收发传输模块

* 类功能：  
通用异步收发传输器（Universal Asynchronous Receiver/Transmitter)，通常称作UART。它将要传输的资料在串行通信与并行通信之间加以转换。作为把并行输入信号转成串行输出信号的芯片，UART通常被集成于其他通讯接口的连结上。
具体实物表现为独立的模块化芯片，或作为集成于微处理器中的周边设备。

## open - 创建uart实例
* 函数功能：  
打开UART接口，创建 UART实例。

* 函数原型：
> var serial = uart.open({id: 'serial'});

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的UART id名称 |

* 返回值：
Object类型，UART串口实例，为空代表创建失败。

## on - 串口事件的回调接口
* 函数功能：  
串口事件的回调函数。

* 函数原型：
> serial.on('data', function(data, len) {
    console.log('uart receive data len is : ' + len + '  data is:  ' + ab2str(data));
})

* 参数说明：  
两个输入参数分别为String类型和Function类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| event | String | 是 | 监控的事件类型，目前仅支持data 事件 |
| cb | Function | 是 | 事件对应的回调函数，带着数据参数 |

* 返回值：
无

## write - 通过UART发送数据
* 函数功能：  
通过UART发送数据，该函数为阻塞函数，发送完成后才会返回。

* 函数原型：
> var msgbuf = 'this is amp uart test';
> uart.write(msgbuf);

* 参数说明：  
String|ArrayBuffer类型

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| data | String | ArrayBuffer | 是 | 需要发送的数据 |

* 返回值：
无。

## read - 通过UART接收数据
* 函数功能：  
主动读取指定bytes的串口数据。

* 函数原型：
> uart.read(100);

* 参数说明：  
Number类型

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | Number | 是 | 需要读取的数据长度 |

* 返回值：
读取到的串口数据，ArrayBuffer类型，如果没有数据，则返回空数组。

## close - 关闭UART实例
* 函数功能：  
关闭UART接口，释放 UART实例。

* 函数原型：
> uart.close();

* 参数说明：  
无

* 返回值：
无

## 使用实例
* uart模块使用示例<br>
-- [haaseeduk1 uart模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/uart)<br>
-- [haas600 uart模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600-EC600S/basic/uart)<br>
-- [ESP32 uart模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/uart/gps_ht2828z3g5l)<br>

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

