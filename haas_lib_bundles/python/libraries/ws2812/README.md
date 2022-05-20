# WS2812 RGB点阵灯环

## 一、产品简介
&emsp;&emsp;
ws2812是一个集控制电路与发光电路于一体的智能外控LED光源。其外型与一个5050LED灯珠相同，每个元件即为一个像素点。像素点内部包含了智能数字接口数据锁存信号整形放大驱动电路，还包含有高精度的内部振荡器和12V高压可编程定电流控制部分，有效保证了像素点光的颜色高度一致。

<div align="center">
<img src=./../../docs/images/ws2812.png width=30%/>
</div>

引脚定义
* DI： 控制信号输入引脚
* VCC：电源5V
* GND：电源地
* DO： 信号输出引脚，串联下一个灯珠

## 二、技术参数
* 工作电压：5V<br>
* 发送速率：800Kbps<br>
* 亮度显示：支持256级亮度显示，rgb三色支持16777216种颜色的显示<br>
* 通信方式：单线通信<br>
* 工作特性：刷新速率30帧/秒时，级联数不小于1024<br>
* 工作温度：-20°C~85°C<br>

## 三、软件接口

WS2812 RGB点阵灯环 HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ws2812)
<br>

### WS2812 - 创建WS2812驱动对象

* 函数原型

> relayObj = WS2812(gpioObj, ledPixel)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|gpioObj|GPIO|是| WS2812控制引脚的GPIO对象。调用此函数前需确保对象已被open|
|ledPixel|int|否| 默认ledPixel为24个RGB点灯，开发者根据自己的实际需求创建|

* 返回值

若WS2812对象创建成功，返回WS2812对象；否则抛出Exception

* 示例代码
```python
import ws2812
from driver import GPIO

gpioObj=GPIO()
gpioObj.open("ws2812")  # 按照board.json中名为"ws2812"的设备节点的配置参数
print("GPIO inited!")

led=ws2812.WS2812(gpioObj)
```

* 输出
```log
GPIO inited!
```

### set(r,g,b,index) - 点亮对应index的RGB点阵灯环

* 函数功能：

点亮对应index的RGB点阵灯环

* 函数原型：

> WS2812.set(r,g,b,index)

* 参数说明：

参数|类型|说明|
|:-:|:-:|:-|
|r|int|红色灯珠的值，取值范围：0-255|
|g|int|绿色灯珠的值，取值范围：0-255|
|b|int|蓝色灯珠的值，取值范围：0-255|
|index|int|对应的灯珠索引号，范围：0～（ledPixel-1），例如：第一个灯珠的index为0|

* 返回值：

无

### dot(r,g,b,time) - 将RGB点阵灯环逐个点亮，每次只有一个灯珠被点亮

* 函数功能：

将RGB点阵灯环逐个点亮，每次只有一个灯珠被点亮

* 函数原型：

> WS2812.dot(r,g,b,time)

* 参数说明：

参数|类型|说明|
|:-:|:-:|:-|
|r|int|红色灯珠的值，取值范围：0-255|
|g|int|绿色灯珠的值，取值范围：0-255|
|b|int|蓝色灯珠的值，取值范围：0-255|
|time|int|逐个点灯的时间，单位（ms）|

* 返回值：

无

### wipe(r,g,b,time) - 将RGB点阵灯环一一点亮

* 函数功能：

将RGB点阵灯环一一点亮

* 函数原型：

> WS2812.wipe(r,g,b,time)

* 参数说明：

参数|类型|说明|
|:-:|:-:|:-|
|r|int|红色灯珠的值，取值范围：0-255|
|g|int|绿色灯珠的值，取值范围：0-255|
|b|int|蓝色灯珠的值，取值范围：0-255|
|time|int|逐次点灯的时间，单位（ms）|

* 返回值：

无

### rainbow(time) - RGB点阵灯环实现彩虹变换

* 函数功能：

RGB点阵灯环实现彩虹变换

* 函数原型：

> WS2812.rainbow(time)

* 参数说明：

参数|类型|说明|
|:-:|:-:|:-|
|time|int|彩虹渐变的时间，单位（ms）|

* 返回值：

无

### clear(index) - 熄灭对应index的RGB点阵灯环

* 函数功能：

熄灭对应index的RGB点阵灯环

* 函数原型：

> WS2812.clear(index)

* 参数说明：

参数|类型|是否必选|说明|
|:-:|:-:|:-:|:-|
|index|int|否|当不选参数的时候，所有的灯珠会熄灭。<br>index对应的灯珠索引号，范围：0～（ledPixel-1），例如：第一个灯珠的index为0|

* 返回值：

无

## 四、接口案例

board.json 配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
       "ws2812":{
        "type": "GPIO",
        "port": 0,
        "dir": "output",
        "pull": "pulldown"
      }
    }
}
```

```python
import ws2812
from driver import GPIO

gpioObj=GPIO()
gpioObj.open("ws2812")  # 按照board.json中名为"ws2812"的设备节点的配置参数
print("GPIO inited!")

led=ws2812.WS2812(gpioObj)
while True:
  led.dot(128,0,0,100)
```

## 五、工作原理
&emsp;&emsp;
数据协议采用单线归零码的通讯方式，像素点在上电复位以后，DIN端接受从控制器传输过来的数据，首先送过来的24bit数据被第一个像素点提取后，送到像素点内部的数据锁存器，剩余的数据经过内部整形处理电路整形放大后通过DO端口开始转发输出给下一个级联的像素点，每经过一个像素点的传输，信号减少24bit。像素点采用自动整形转发技术，使得该像素点的级联个数不受信号传送的限制，仅仅受限信号传输速度影响。
<div align="center">
<img src=./../../docs/images/ws2812_24bits.png width=70%/>
</div>


&emsp;&emsp;
ws2812时序波形图：
<div align="center">
<img src=./../../docs/images/ws2812_seq.png width=70%/>
</div>

## 参考文献及购买链接
[1] [WS2812原理介绍](https://blog.csdn.net/weixin_41659040/article/details/104058295)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=566313609891&spm=a1z09.2.0.0.32bc2e8dQGew8F&_u=ivktbc71044)
