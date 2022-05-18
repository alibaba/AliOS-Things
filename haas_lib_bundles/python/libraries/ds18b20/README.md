# 温度传感器 - DS18B20

## 一、产品简介
DS18B20是一款极具性价比的数字温度传感器，包括测量水温和空气的两种温度传感器，无需再增加外围电路即可直接使用，开发者可以使用同一套代码获取两款传感器的温度。<br>

<div align="center">
<img src=./../../docs/images/ext_ds18b20水温温度度_定义尺寸.png  width=30%/>
</div>

引脚定义
* 红线:3.0V-5.5V<br>
* 黑线:接地<br>
* 黄线:数据线
<br>
<br>
<div align="center">
<img src=./../../docs/images/ext_ds18b20空气温度度_定义尺寸.png  width=30%/>
</div>

引脚定义
* VCC：3.3V<br>
* DQ：数据线<br>
* GND：接地
<br>

## 二、技术参数
### 1、水温探测传感器
* 供电电压：3.0V-5.5V<br>
* 接口：支持1-Wire<br>
* 精度范围：正负0.5°C(在工作温度-10°C～+85°C下)<br>
* 工作温度：-55°C～+125°C<br>
* 分辨率：9-12位可调分辨率<br>
* 尺寸：不锈钢外壳(6mm*50mm),引线长度50CM
<br>

### 2、空气温度传感器
* 供电电压：3.0V-5.5V<br>
* 数据接口：支持1-Wire<br>
* 高度精度：正负0.5°C(温度在-10°C～85°C下);正负2°C(温度在-10°C～85°C下)<br>
* 工作温度：-55°C～+125°C<br>
* 分辨率：9-12位可调分辨率<br>
* 尺寸：21mm*10mm
<br>

## 三、软件接口

DS18B20温度传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ds18b20)
<br>

### DS18B20(gpioObj,resolution) - 创建DS18B20驱动对象
* 函数原型：
> ds18b20Obj = DS18B20(gpioObj , resolution)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|gpioObj|GPIO|是|调用此函数前需确保gpioObj对象已经处于open状态|
|resolution|Number|否|默认分辨率12<br>参数范围(9-12)|

* 返回值：
DS18B20对象成功，返回DS18B20对象；DS18B20对象创建失败，抛出Exception

* 示例代码：

```python
import ds18b20  #温度驱动库
from driver import GPIO

gpioDev = GPIO()
gpioDev.open("ds18b20")
ds18b20Obj=ds18b20.DS18B20(gpioDev,12)
print("ds18b20 inited!")
```

* 输出：
```log
ds18b20 inited!
```

### getTemperature - 测量温度

* 函数功能：
量测空气或水的温度并返回测量结果

* 函数原型：
> DS18B20.getTemperature()

* 参数说明：
无

* 返回值：
成功返回温度测量结果（数据类型：浮点型；单位：摄氏度）；失败抛出Exception

* 示例：

```python
import ds18b20  #温度驱动库
from driver import GPIO

gpioDev = GPIO()
gpioDev.open("ds18b20")
ds18b20Obj=ds18b20.DS18B20(gpioDev,12)
print("ds18b20 inited!")

while True:
    readtemp = ds18b20Obj.getTemperature()
    print("temperature is ", readtemp, "°C")
```

* 输出：
```log
ds18b20 inited!
temperature is 25.3°C
temperature is 26.1°C
temperature is 25.7°C
```

## 四、接口案例
此使用实例在board.json中定义了名为ds18b20的GPIO类型的对象。在Python脚本中周期性的获取温度测量结果并打印在日志中。

* 代码：
```python
# board.json配置：
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "ds18b20": {
        "type": "GPIO",
        "port": 2
      }
    }
}

```

```python
# Python代码
import ds18b20  #温度驱动库
from driver import GPIO

gpioDev = GPIO()
gpioDev.open("ds18b20")
ds18b20Obj=ds18b20.DS18B20(gpioDev,12)
print("ds18b20 inited!")

while True:
    readtemp = ds18b20Obj.getTemperature()
    print("temperature is ", readtemp, "°C")
```

* 输出：
```python
ds18b20 inited!
temperature is 25.3°C
temperature is 26.1°C
temperature is 25.7°C
```

<br>

## 五、工作原理
DS18B20内部结构如下图所示，主要由4部分组成：温度传感器、64位ROM、非挥发的温度报警触发器TH和TI、配置寄存器。DS18B20只有一个数据输入输出口，属于单总线专用芯片之一。DS18B20工作时被测温度值直接以“单总线”的数字方式传输，大大提高了系统的抗干扰能力。其内部采用在线温度测量技术，测量范围为55~125°C，在-10~85℃时，精度为±0.5°C。每个DS18B20在出厂时都已具有唯一的64位序列号，因此一条总线上可以同时挂接多个DS18B20，而不会出现混乱现象。另外用户还可自设定非易失性温度报警上下限值TH和TL（掉电后依然保存）。DS18B20在完成温度变换后，所测温度值将自动与存储在TH和TL内的触发值相比较，如果测温结果高于TH或低于TL， DS18B20内部的告警标志就会被置位，表示温值超出了测量范围，同时还有报警搜索命令识别出温度超限的DS18B20。
<div align="center">
<img src=./../../docs/images/ext_ds18b20内部方框图_图1.png  width=40%/>
</div>

当DS18B20接收到温度转换命令后，开始启动转换。转换完成后的温度值就以16位带符号扩展的二进制补码形式存储在高速暂存存储器的第1、2字节。单片机可通过单线接口读到该数据，读取时高位在后、低位在前，数据格式以0.0625°C/LSB形式表示。温度值格式如下图所示。符号位S=0时，直接将二进制位转换为十进制；当S=1时，先将补码变换为原码，再计算十进制值。
<div align="center">
<img src=./../../docs/images/ext_ds18b20温度传感器寄存器表_图2.png  width=40%/>
</div>

如下表是对应的一部分温度值。
<div align="center">
<img src=./../../docs/images/ext_ds18b20温度传感器温度和数据关系_图3.png  width=40%/>
</div>

想要了解更多的请参考参考文献！

<br>

## 参考文献及购买链接
[1] [DS18B20温度传感器原理](https://blog.csdn.net/yannanxiu/article/details/43916515)<br>
[2] [DS18B20温度传感器中文数据手册参考](https://max.book118.com/html/2019/0814/5114212130002114.shtm)<br>
[3] [DS18B20水温传感器购买链接](https://detail.tmall.com/item.htm?id=609907614619)<br>
[4] [DS18B20空气传感器购买链接](https://detail.tmall.com/item.htm?id=41272469644)
