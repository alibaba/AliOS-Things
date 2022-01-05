# HTTP - 超文本传输协议
* 模块功能：  
实现HTTP客户端的相关功能函数，包括请求，下载两个功能。

* 注意事项：  
需要确保网络连接成功，请使用下面的示例代码连接网络：
```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('<AP_name>', '<password>')
utime.sleep_ms(500)
while not sta_if.isconnected():
    utime.sleep_ms(500)
```
直到`sta_if.isconnected()`返回 **True** 则表明网络连接成功


## request - 发起一个请求

* 函数功能：  
发起一个HTTP请求

* 注意事项：  
需确保此网络已经连接

* 函数原型：
> http.request(data, func)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|dictionary|是|设置请求的参数|
|func|funcCallback(data)|是|请求成功后的回调函数|

data参数说明：
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|url|string|是|目标服务器URL，包括协议类型、地址、端口、URI等|
|method|string|是|请求方式，目前支持下面几种类型： - GET - PUT - POST - HEAD - DELETE|
|headers|string|否|设置请求的 HTTP 头对象，默认为 {‘content-type’: ‘application/json’}，该对象里面的 key 和 value 必须是string格式|
|timeout|int|否|超时时间，单位是ms，默认是30000|
|params|string|否|请求方式为POST时候的请求参数，其他请求方式可以忽略该参数|

funcCallback函数参数说明：
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|dictionary|是|request请求的回调参数，包含请求header和响应body两个信息|

<br>

* 返回值：  
异步接口，请求发送成功返回0；失败返回其他负数值

* 示例： 

```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('haas-open', '12345678')
utime.sleep_ms(500)
while not sta_if.isconnected():
    utime.sleep_ms(500)

import http

data_post = {
    'url': 'http://httpbin.org/get',
    'method': 'GET',
    'headers': {
        'Content-Type':'application/json'
    },
    'timeout': 30000,
    'params': ''
}

def cb(data):
    print(data)

http.request(data_post, cb)
```

* 输出：
```
0
 >>> {'header': '{\n  "args": {}, \n  "headers": {\n    "Content-Length": "0", \n    "Content-Type": "application/json", \n    "Host": "httpbin.org", \n    "User-Agent": "ESP32 HTTP Client/1.0", \n    "X-Amzn-Trace-Id": "Root=1-6195bdce-05db82167a1ccb841b1e0ab2"\n  }, \n  "origin": "42.120.75.154", \n  "url": "http://httpbin.org/get"\n}\n', 'body': 'Date: Thu, 18 Nov 2021 02:43:26 GMT\r\nContent-Type: application/json\r\nContent-Length: 311\r\nConnection: keep-alive\r\nServer: gunicorn/19.9.0\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Credentials: true\r\n'}
```

## download - 下载文件

* 函数功能：  
下载文件到本地

* 注意事项：  
需确保此网络已经连接

* 函数原型：
> http.download(data, func)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|dictionary|是|设置下载的参数|
|func|funcCallback(data)|是|下载结束后的回调函数|

data参数说明：
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|url|string|是|下载地址的URL|
|filepath|string|是|需要保存的文件路径|

funcCallback函数参数说明：
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|string|是|下载请求的回调参数，成功返回“success”， 失败返回“fail”|

<br>

* 返回值：  
异步接口，下载发送成功返回0；失败返回其他负数值

* 示例： 

```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('haas-open', '12345678')
utime.sleep_ms(500)
while not sta_if.isconnected():
    utime.sleep_ms(500)

import http
data_dl = {'url': 'http://httpbin.org/image/png', 'filepath': '/httpbin.dat'}

def cb(data):
    print(data)

http.download(data_dl, cb)
```

* 输出：
```
0
 >>> success
```

## 使用实例
* 代码：
```python

请编写一个此模块完整的使用实例

```

* 输出：
```python

此使用案例的输出，如果没有输出，请删除“输出”标题

```

<br>

