# timer-定时器模块

* 类功能：  
在物联网MCU芯片上，通常集成了硬件timer，用于提供定时功能。相比软件定时器，硬件定时器定时精度更高。在初始化硬件timer时，用户需设置定时时间和中断处理函数，该函数用于执行定时到期后的用户逻辑，比如往GPIO发送一个数字脉冲。当硬件timer的定时时间到期后，它将触发中断，并执行用户设置的中断处理函数。硬件timer通常提供两种模式：

(1) 单次模式：定时时间到期后，触发一次定时到期事件。若需在再次触发，则需重新设置定时器。

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/116852/1628823944553-4effb305-96f2-49d1-b08c-10746edcddc5.png#clientId=ue540ad19-e68c-4&crop=0&crop=0&crop=1&crop=1&from=paste&height=144&id=u41880e71&margin=%5Bobject%20Object%5D&name=image.png&originHeight=178&originWidth=378&originalType=binary&ratio=1&rotation=0&showTitle=false&size=19375&status=done&style=none&taskId=ufa9198d4-d8db-4b82-8231-3ced48121b2&title=&width=306)

(2) 周期模式：定时器按固定的时间，周期性触发定时到期事件。

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/116852/1628823915025-58705d94-bdfd-4459-880e-25672990b284.png#clientId=ue540ad19-e68c-4&crop=0&crop=0&crop=1&crop=1&from=paste&height=108&id=u10730ecb&margin=%5Bobject%20Object%5D&name=image.png&originHeight=178&originWidth=662&originalType=binary&ratio=1&rotation=0&showTitle=false&size=39101&status=done&style=none&taskId=ub262cd9e-e5d7-40d7-84db-47b8675148f&title=&width=403)

HaaS-JS轻应用框架封装了硬件定时器的能力，为用户提供了方便的JS接口。

## open - 创建network对象
* 函数功能：  
打开硬件TIMER，创建 一个硬件timer实例。

* 函数原型：
> var timer0 = timer.open({id: 'TIMER0'});

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| id | String | 是 | 在app.json中定义的timer id名称 |

* 返回值：  
timer 实例，为空代表创建失败。

## setTimeout - 设置单次定时时间

* 函数功能：  
设置硬件timer单次定时时间。

* 函数原型：
> timer0.setTimeout(timeout_cb, 5000000);

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | 函数 | 是 | 到期执行的用户回调函数 |
| time | 整数 | 是 | 单次定时时间，单位微秒 |

* 返回值：  
整形，0表示成功，其他值表示失败。

## clearTimeout - 停止硬件单次定时

* 函数功能：  
停止硬件timer单次定时。

* 函数原型：
> timer0.clearTimeout();

* 参数说明：
无

* 返回值：  
整形，0表示成功，其他值表示失败。

## setInterval - 设置周期定时时间

* 函数功能：  
设置硬件timer周期定时时间。

* 函数原型：
> timer0.setInterval(timeout_cb, 5000000);

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| cb | 函数 | 是 | 到期执行的用户回调函数 |
| time | 整数 | 是 | 周期定时时间，单位微秒 |

* 返回值：  
整形，0表示成功，其他值表示失败。

## clearInterval - 停止硬件周期定时

* 函数功能：  
停止硬件timer周期定时。

* 函数原型：
> timer0.clearInterval();

* 参数说明：
无

* 返回值：  
整形，0表示成功，其他值表示失败。

## close -关闭硬件定时器

* 函数功能：  
关闭硬件定时器，并释放资源。

* 函数原型：
> timer0.close();

* 参数说明：
无

* 返回值：  
整形，0表示成功，其他值表示失败。

## 使用实例
* 设置单次定时<br>
-- [HaaS EDU K1 设置单次定时示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/timer/one-shot-timer)
<br>
-- [ESP32 设置单次定时示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/timer)
<br>
* 设置周期定时<br>
-- [HaaS EDU K1 设置周期定时示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/timer/repeat-timer)
<br>
## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|是||
|HaaS EDU K1|是||
|HaaS600-EC100Y|否||
|HaaS600-EC600S|否||
|HaaS510|否||
|HaaS531|否||
|ESP32|是||

<br>

