# checksum-校验算法模块

* 类功能：  
数据摘要算法是密码学算法中非常重要的一个分支，它通过对所有数据提取指纹信息以实现数据签名、数据完整性校验等功能，由于其不可逆性，有时候会被用做敏感信息的加密。数据摘要算法也被称为哈希（Hash）算法或散列算法。本文主要是通过crc16, crc32, md5 进行算法校验.

## crc16 - crc16校验算法
* 函数功能：  
crc16校验原始数据。

* 函数原型：
> var crc16 = checksum.crc16(input);

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| src | String | 是 | 原始数据字符串 |

* 返回值：
String类型，crc16校验结果。

## crc32 - crc32校验算法
* 函数功能：  
crc32校验原始数据。

* 函数原型：
> var crc32 = checksum.crc32(input);

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| src | String | 是 | 原始数据字符串 |

* 返回值：
String类型，crc32校验结果。

## md5 - md5校验算法
* 函数功能：  
md5校验原始数据。

* 函数原型：
> var md5 = checksum.md5(input);

* 参数说明：  
Object类型，描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| src | String | 是 | 原始数据字符串 |

* 返回值：
String类型，md5校验结果。

## 使用实例
* checksum模块使用示例<br>
-- [haaseeduk1 checksum模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/checksum)<br>
-- [ESP32 checksum模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/checksum)<br>

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

