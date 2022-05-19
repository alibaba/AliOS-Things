# 红外接收传感器

## 一、产品简介
&emsp;&emsp;
一般家庭环境里面会有很多的遥控器，比如空调、电视机、机顶盒等等，这些都是通过红外遥控器发出的信号控制，红外遥控器发出的就是一连串的二进制脉冲码。红外接收传感器可以将这一连串的二进制脉冲码接收过来传递给MCU解析控制对应的设备。
<br>

<div align="center">
<img src=./../../docs/images/ext_红外接收_定义尺寸.png  width=30%/>
</div>

引脚定义
* -：地<br>
* S：输出信号<br>
* +：DC-3.3V<br>

## 二、技术参数
* 工作电压：2.7-5.5V<br>
* 低功耗：宽角度及长距离接收<br>
* 输入频率：38Khz<br>
* 存储温度：-40°C-125°C<br>
* 抗干扰：抗干扰能力强<br>
* 输出电平：TTL<br>

## 三、软件接口

红外接收传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/irremote)
<br>

### IRREMOTE(gpioObj) - 创建红外接收对象
<br>

* 函数原型

> irrecvObj = IRREMOTE(gpioObj)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|

* 返回值

> IRREMOTE对象成功，返回IRREMOTE对象；IRREMOTE对象创建失败，抛出Exception

* 示例代码
```python
import irremote
from driver import GPIO

print("Testing IRremote ...")
irrecvDev = GPIO()
irrecvDev.open("irrecv")
irrecvObj = irremote.IRREMOTE(irrecvDev)

```

* 输出
```python
Testing IRremote ...
```
</br>

### getRemoteChar() - 获取遥控器对应的按键状态值
<br>

* 函数功能：

> 获取遥控器对应的按键状态值

* 函数原型：

> IRREMOTE.getRemoteChar()

* 参数说明：
> 无

* 返回值：
> changed，data；
</br>
其中changed为True，说明有数据值更新，False，说明没有数据值更新
</br>
data如下所示：

data返回值|类型|说明|
|:-:|:-:|:-|
|0|string|获取遥控器按键0值|
|1|string|获取遥控器按键1值|
|2|string|获取遥控器按键2值|
|3|string|获取遥控器按键3值|
|4|string|获取遥控器按键4值|
|5|string|获取遥控器按键5值|
|6|string|获取遥控器按键6值|
|7|string|获取遥控器按键7值|
|8|string|获取遥控器按键8值|
|9|string|获取遥控器按键9值|
|PREV|string|获取遥控器按键上一步|
|NEXT|string|获取遥控器按键下一步|
|VOL+|string|获取遥控器按键音量增|
|VOL-|string|获取遥控器按键音量减|
|START/STOP|string|获取遥控器按键启动或暂停|
|100+|string|获取遥控器按键 step100增量值|
|200+|string|获取遥控器按键 step200增量值|

* 示例：

```python
import irremote
from driver import GPIO
import utime

print("Testing IRremote ...")
irrecvDev = GPIO()
irrecvDev.open("irrecv")
irrecvObj = irremote.IRREMOTE(irrecvDev)
while True:             # 无限循环
    changed, data = irrecvObj.getRemoteChar()
    if changed == True and data != None:
        print('char %s'%data)
    utime.sleep(0.2)      # 打印完之后休眠1秒
```

* 输出
```python
Testing IRremote ...
char VOL-
char VOL+
```

</br>

## 四、接口案例

此使用实例在board.json中定义了名为IRREMOTE的GPIO类型的对象。在Python脚本中获取红外遥控器按键状态值并打印在日志中。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
    "irrecv": {
      "type": "GPIO",
      "port": 34,
      "dir": "irq",
      "pull": "pullup",
      "intMode": "both"
    }
  }
}
```

```python
import irremote
from driver import GPIO
import utime

print("Testing IRremote ...")
irrecvDev = GPIO()
irrecvDev.open("irrecv")
irrecvObj = irremote.IRREMOTE(irrecvDev)
while True:             # 无限循环
    changed, data = irrecvObj.getRemoteChar()
    if changed == True and data != None:
        print('char %s'%data)
    utime.sleep(0.2)      # 打印完之后休眠1秒
```

* 输出
```python
Testing IRremote ...
char VOL-
char VOL+
```

## 五、工作原理
### 1、红外收发概述
<div align="center">
<img src=./../../docs/images/ext_红外接收_硬件示意图.png  width=30%/>
</div>

<br>
&emsp;&emsp;
红外遥控器发出的信号是一连串的二进制脉冲码。<br>
&emsp;&emsp;
为了使其在无线传输过程中免受其他红外信号的干扰,通常都是先将其调制在特定的载波频率上, 然后再经红外发射二极管发射出去,而红外线接收装置则要滤除其他杂波,只接收该特定频率的信号并将其还原成二进制脉冲码,也就是解调，接收头的信号输出脚输入到MCU进行解码控制的过程。

<br>

### 光电参数（T=35°C,Vcc=5V,f=38Khz）
<div align="center">
<img src=./../../docs/images/ext_红外接收_光电参数.png  width=50%/>
</div>

### 测试波形
<div align="center">
<img src=./../../docs/images/ext_红外接收_测试波形.png  width=50%/>
</div>

## 参考文献及购买链接
[1] [红外接收传感器](https://item.taobao.com/item.htm?spm=a230r.1.14.170.16d05050X8h59m&id=524001649988&ns=1&abbucket=17#detail)
