# GPIO-通用输入输出模块

* 类功能：  
GPIO (General Purpose Input & Output)是MCU常用的IO控制模块，可配置为输入读，输入中断，输出（高电平/3.3V，低电平/0V）等模式。常用于继电器，LED，开关输入等场景。

## open - 创建GPIO实例
* 函数功能：  
打开GPIO接口，创建 GPIO 实例。

* 函数原型：
> var LED1 = gpio.open({id: 'L1'});

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的GPIO id名称 |

* 返回值：
Object类型，GPIO实例，为空代表创建失败。

## writeValue - 控制GPIO输出
* 函数功能：  
设置GPIO电平值，GPIO引脚为输出模式时可用。

* 函数原型：
> GPIO.writeValue(1);

* 参数说明：  
Number类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| level | Number | 是 | 电平值，1（高电平） 或者 0（低电平） |

* 返回值：
无

## toggle - 反转GPIO输出
* 函数功能：  
反转GPIO的电平，当前GPIO电平为低电平时设置为高电平，当前GPIO电平为高电平时设置为低电平。

* 函数原型：
> GPIO.toggle();

* 参数说明：  
无

* 返回值：
无

## onIRQ - 开启GPIO中断触发
* 函数功能：  
开启引脚中断触发，引脚为输入模式时可用。

* 函数原型：
> GPIO.onIRQ({
>  cb: function() {
>      console.log('gpio: KEY2 interrupt happens');
>   }
> });

* 参数说明：  
Function类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | Function | 是 | 引脚中断触发回调函数 |

* 返回值：
无

## readValue - 读取GPIO电平
* 函数功能：  
读取GPIO电平值，输入模式和输出模式时均可用。

* 函数原型：
> var level = GPIO.readValue();

* 参数说明：  
无

* 返回值：
Number类型，电平值，1（高电平） 或者 0（低电平）。

## close - 释放GPIO实例
* 函数功能：  
关闭GPIO接口，释放 GPIO 实例。

* 函数原型：
>  GPIO.close();

* 参数说明：  
无

* 返回值：
无

## 使用实例
* GPIO 模块使用示例<br>
-- [haaseeduk1 控制GPIO使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/gpio)<br>
-- [haas100 控制GPIO使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haas100/gpio)<br>
-- [haas600 控制GPIO使用示例](https://gitee.com/alios-things/amp-examples/tree/master/board/HaaS600-EC600S/basic/gpio)
-- [ESP32 控制GPIO使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/gpio)<br>

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

