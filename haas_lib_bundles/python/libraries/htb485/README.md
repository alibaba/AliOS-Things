# 建大仁科光照温湿度变送器 - htb485

## 一、产品简介

&emsp;&emsp;
建大仁科光照温湿度变送器支持Modbus-RTU协议，是工农业场景选用最多的传感器之一。详细参数如下：
<div align="center">
<img src=../../docs/images/485环境监测/HTB485.jpg width=40%/>
</div>
<br>

&emsp;&emsp;
接线方法
|线色|说明|
|---:|----|
|棕色|电源正（10～30V DC）|
|黑色|电源负|
|黄色|485-A|
|蓝色|485-B|


## 二、技术参数
<div align="center">
<img src=../../docs/images/485环境监测/HTB485_详细参数.jpg width=80%/>
</div>
<br>

## 三、软件接口

### HTB485(...) - 创建 htb485 驱动对象
* 函数原型：
> htb485Obj = HTB485(mbObj, mbAddr)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|mbObj|modbus|是|调用此函数前需确保 modbus 对象已初始化|
|mbAddr|int|是|默认值 1|

* 返回值：
成功返回HTB485对象，失败抛出 Exception

* 示例代码：

```python
import modbus as mb
import yuanda_htb485

mb.init('modbus_485_4800')
htb485Obj = yuanda_htb485.HTB485(mb, 1)
```

### getHumidity() - 获取湿度值

* 函数功能：
获取环境湿度（百分比)


* 返回值：
成功返回当前环境湿度值，抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)


### getTemperature() - 获取温度值

* 函数功能：
获取环境温度(摄氏度)


* 返回值：
成功返回当前环境温度值，抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)


### getBrightness() - 获取光照强度值

* 函数功能：
获取当前光照强度（流明）


* 返回值：
成功返回当前环境光照强度，抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)


### getHTB() - 获取湿度，温度，光照强度

* 函数功能：
获取湿度，温度，光照强度


* 返回值：
成功返回当前环境湿度，温度，光照强度，抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)

* 参考代码
```python
import modbus as mb
import yuanda_htb485

mb.init('modbus_485_4800')
htb485Obj = yuanda_htb485.HTB485(mb, 1)

htb = htb485Obj.getHTB()
print('Humidity:', htb[0])
print('Temperature:', htb[1])
print('Brightness:', htb[3])

* 输出

Humidity: 30.1
Temperature: 28
Brightness: 300
```


## 四、接口案例
采集当前环境的温度，湿度，光照强度

* 代码：
```python
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "modbus_485_4800": {
        "type": "MODBUS",
        "mode": 0,
        "port": 2,
        "baudrate": 4800,
        "priority": 0,
        "timeout": 200
      },
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}

```
```python
# Python代码
import modbus as mb
import yuanda_htb485

mb.init('modbus_485_4800')
htb485Obj = yuanda_htb485.HTB485(mb, 1)

htb = htb485Obj.getHTB()
print('Humidity:', htb[0])
print('Temperature:', htb[1])
print('Brightness:', htb[3])

* 输出：
```python
...
Humidity: 100
Temperature: 28
Brightness: 300
...

```

## 参考文献及购买链接
[1] [光照温湿度变送器使用说明书](http://save.jnrsmcu.com/%E4%BA%A7%E5%93%81%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E%E4%B9%A6/%E6%99%AE%E9%80%9A%E5%9E%8B%E4%BA%A7%E5%93%81/%E5%85%89%E7%85%A7CO2/%E5%85%89%E7%85%A7%E5%BA%A6%E6%B8%A9%E6%B9%BF%E5%BA%A6%E4%B8%89%E5%90%88%E4%B8%80%E5%8F%98%E9%80%81%E5%99%A8%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E%E4%B9%A6.pdf)

[2] [商品链接](https://item.taobao.com/item.htm?spm=a230r.1.14.66.74a04798JXmWL7&id=541168327286&ns=1&abbucket=18#detail)
