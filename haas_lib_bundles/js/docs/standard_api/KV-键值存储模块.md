# KV-键值存储模块

* 类功能：  
键值对存储是数据库最简单的组织形式，基本上所有的编程语言都带有应用在内存中的键值对存储。AliOS Things等常见的物联网操作系统都有对键值存储能力的支持。HaaS-JS的键值存储支持读取，写入，删除键值对的基础能力。

## getStorageSync - 读取KV的值
* 函数功能：  
读取KV中某个键值对的值。

* 函数原型：
> var val = kv.getStorageSync(key)

* 参数说明：  
String类型，键值对中key值。

* 返回值：
String类型，键值对中对应key的值。

## setStorageSync - 设置KV的值
* 函数功能：  
设置KV中某个键值对的值。

* 函数原型：
> kv.setStorageSync(key, value);

* 参数说明：  
String类型，参数描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| key | String | 是 | KV中的key值 |
| value | String | 是 | KV中的value值 |

* 返回值：
无

## removeStorageSync - 删除KV
* 函数功能：  
删除KV中的某个键值对。

* 函数原型：
> kv.removeStorageSync(key)

* 参数说明：  
String类型，键值对中key值。

* 返回值：
无。

## 使用实例
* kv模块使用示例<br>
-- [haaseeduk1 KV模块使用示例](https://gitee.com/alios-things/amp/blob/rel_3.3.0/example-js/haaseduk1/kv/app.js)<br>
-- [haas600/510/531 KV模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/common/kv)<br>
-- [ESP32 KV模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/kv)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|否||
|HaaS EDU K1|否||
|HaaS600-EC100Y|否||
|HaaS600-EC600S|否||
|HaaS510|否||
|HaaS531|否||
|ESP32|是||

<br>

