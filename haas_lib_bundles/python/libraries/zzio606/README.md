# 郎汉德6路继电器 - ZZIO606

## 一、产品简介

&emsp;&emsp;
郎汉德6路继电器支持Modbus-RTU协议，是工农业场景选用最多的传感器之一。详细参数如下：
<div align="center">
<img src=../../docs/images/485环境监测/zzio606.jpg width=50%/>
</div>
<br>

## 二、技术参数
<div align="center">
<img src=../../docs/images/485环境监测/ZZIO详细参数.jpg width=80%/>
</div>
<br>

## 三、软件接口

### ZZIO606(...) - 创建 ZZIO606 驱动对象
* 函数原型：
> zzioObj = ZZIO606(mbObj, mbAddr)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|mbObj|modbus|是|调用此函数前需确保 modbus 对象已初始化|
|mbAddr|int|是|默认值是1|

* 返回值：
成功返回ZZIO606对象，失败抛出 Exception

* 示例代码：

```python
import modbus as mb
import zzio606

mb.init('modbus_485_4800')
zzioObj = zzio606.ZZIO606(mb, 1)
```

### openChannel() - 关闭指定通道

* 函数原型：
> zzioObj.openChannel(channelID)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|channelID|int|是|通道id|


* 返回值：
打开指定通道，失败抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)


### closeChannel() - 关闭指定通道

* 函数原型：
> zzioObj.closeChannel(channelID)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|channelID|int|是|通道id|

* 返回值：
关闭指定通道，失败抛出异常，参考[modbus错误码表](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1KQYjx1#/Python/docs/zh-CN/haas_extended_api/modbus)


* 参考代码
```python
import modbus as mb
import zzio606

mb.init('modbus_485_4800')
zzioObj = zzio606.ZZIO606(mb, 1)

zzio606.openChannel(1)
zzio606.closeChannel(1)

```


## 四、接口案例
控制第一个通道开闭

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
import modbus as mb
import zzio606

mb.init('modbus_485_4800')
zzioObj = zzio606.ZZIO606(mb, 1)

zzio606.openChannel(1)
zzio606.closeChannel(1)

```

## 参考文献及购买链接
[1] [郎汉德官网](https://www.lonhand.com/index/io/22.html?hmsr=tb)

[3] [商品链接](https://item.taobao.com/item.htm?spm=a230r.1.14.12.68c03096UXtt9a&id=566347546505&ns=1&abbucket=18#detail)
