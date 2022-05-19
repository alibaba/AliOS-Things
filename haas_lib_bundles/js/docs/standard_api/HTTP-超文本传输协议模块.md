# http - 超文本传输协议模块

* 类功能：  
超文本传输协议（Hyper Text Transfer Protocol，HTTP）是一个简单的请求/响应协议，它通常运行在TCP协议族之上。它指定了客户端可能发送给服务器什么样的消息以及得到什么样的响应。请求和响应消息的头以ASCII形式给出。HaaS JS轻应用目前支持HTTP和HTTPS协议。

## request - 发起一个http请求
* 函数功能：  
发起一个http请求，支持HTTPS POST和GET请求。

* 函数原型：
> http.request(Object options)

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| url | String | 是 | 目标服务器URL，包括协议类型、地址、端口、URI等，比如**http://appengine.oss-cn-hangzhou.aliyuncs.com/httpTest.txt** |
| headers | Object | 否 | 设置请求的 HTTP 头对象，默认 **{'content-type': 'application/json'}**，该对象里面的 key 和 value 必须是 String 类型 |
| method | String | 否 | 默认 GET，暂时只支持GET/POST |
| timeout | Number | 否 | 超时时间，单位是ms，默认是30000 |
| param | String | 否 | 当请求方法为POST时，需要增加post的参数，格式：tab_index=0&count=3 |
| success | Function | 否 | 调用成功的回调函数 |
| fail | Function | 否 | 调用失败的回调函数 |

**success 回调函数**
入参为 Object 类型，属性如下：

| 参数 | 类型 | 描述 |
| --- | --- | --- |
| data | String | 响应数据，格式取决于请求时的 dataType 参数 |
| status | Number | 响应码 |
| headers | Object | 响应头 |

* 返回值：整型，0表示成功，其他值表示失败

## download - 发起http下载请求

* 函数功能：  
通过http下载某个url的文件到指定文件系统路径。

* 函数原型：
> http.download(Object options);

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| url | String | 是 | 目标服务器URL，包括协议类型、地址、端口、URI等，比如**http://appengine.oss-cn-hangzhou.aliyuncs.com/httpTest.txt** |
| filepath | String | 是 | 设置下载文件的保存地址 |
| timeout | Number | 否 | 超时时间，单位是ms，默认是30000 |
| success | Function | 否 | 调用成功的回调函数 |
| fail | Function | 否 | 调用失败的回调函数 |

* 返回值：  
整型，0表示成功，其他值表示失败。

## 使用实例
* http post使用示例<br>
-- [haaseduk1 http post测试示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/http/http_post)<br>

* http get使用示例<br>
-- [haaseduk1 http get测试示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/http/http_get)<br>

* http download使用示例<br>
-- [haaseduk1 http download测试示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/http/http_download)<br>

注：HaaS100/ESP32等其他硬件的HTTP示例可完全复用以上haaseduk1示例<br>

* 其他<br>
-- [haase600/510/531 http使用示例](https://gitee.com/alios-things/amp-examples/tree/master/common/http)<br>

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

