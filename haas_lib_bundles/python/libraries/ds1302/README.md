# DS1302 RTC时钟模块

## 一、产品简介
DS1302 是DALLAS 公司推出的涓流充电时钟芯片，内含有一个实时时钟/日历和31字节静态RAM，实时时钟/日历电路提供秒、分、时、日、周、月、年的信息，每月的天数和闰年的天数可自动调整。
<br>

<div align="center">
<img src=./../../docs/images/ds1302.png  width=30%/>
</div>

DS1302 与单片机之间能简单地采用同步串行的方式进行通信，只需要三个数据线：<br>
（1）RST 复位<br>
（2）I/O 数据线<br>
（3）SCLK串行时钟
<br>

引脚定义
* VCC：3.3V<br>
* GND：地<br>
* CLK：时钟输出<br>
* DAT：数据输入输出<br>
* RST：复位引脚<br>

## 二、技术参数
* 工作电压：2V-5.5V<br>
* 接口简单：GPIO三线接口<br>
* 工作温度：-40°C~85°C<br>
* 低功耗：工作电压2V，电流300nA<br>
* 电源供应：支持主电源和备份电源<br>
* 电路板尺寸：4.4cm*2.3cm<br>

## 三、软件接口

DS1302 RTC时钟传感器 HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ds1302)
<br>

### DS1302(o_clk, o_rst, b_di, b_do) - 创建RTC时钟对象

* 函数原型

> ds1302Obj = DS1302(o_clk, o_rst, b_di, b_do)

* 参数说明

参数|类型|必选参数？|说明|
|:-:|:-:|:-:|:-|
|o_clk|GPIO|是|调用此函数前需确保o_clk对象已经处于open状态|
|o_rst|GPIO|是|调用此函数前需确保o_rst对象已经处于open状态|
|b_di|String|是|b_di为board.json中定义DAT输入的对象名称|
|b_do|String|是|b_do为board.json中定义DAT输出的对象名称|

* 返回值

>DS1302对象成功，返回DS1302对象；DS1302对象创建失败，抛出Exception

* 示例代码-无回调函数
```python
from driver import GPIO
import ds1302

gpioclk = GPIO()
gpioclk.open("ds1302_clk")
gpiorst = GPIO()
gpiorst.open("ds1302_rst")
ds1302_obj = ds1302.DS1302(gpioclk,gpiorst,"ds1302_idata","ds1302_odata")
print('start init ds1302')
```

* 输出
```python
start init ds1302
```
</br>

### setDatetime() - 设置RTC时钟信息
<br>

* 函数功能：

> 设置RTC时钟信息

* 函数原型：

> DS1302.setDatetime(data)

* 参数说明：
> data日期格式分别为：年-月-日-时-分-秒-星期

* 返回值：
> 无

### getDatetime() - 获取RTC时钟信息
<br>

* 函数功能：

> 获取RTC时钟信息

* 函数原型：

>  data = DS1302.getDatetime()

* 参数说明：
> 无

* 返回值：
> 成功返回data日期格式分别为：年-月-日-时-分-秒-星期

* 示例：

```python
from driver import GPIO
import ds1302

gpioclk = GPIO()
gpioclk.open("ds1302_clk")
gpiorst = GPIO()
gpiorst.open("ds1302_rst")
ds1302_obj = ds1302.DS1302(gpioclk,gpiorst,"ds1302_idata","ds1302_odata")
print('start init ds1302')
ds1302_obj.getDatetime([2022, 6, 13 , 0, 0, 30, 1])
print(ds1302_obj.getDatetime())
```

* 输出
```python
start init ds1302
[2022, 6, 13, 0, 0, 31, 1]
```
</br>

## 四、接口案例

此使用实例在board.json中定义了名为DS1302的GPIO类型的对象。在Python脚本中设置时钟并且打印时钟信息的状态。

* 案例代码
```python
{
  "name": "board-name",
  "version": "1.0.0",
  "io": {
   "ds1302_clk": {
      "type": "GPIO",
      "port": 12,
      "dir": "output",
      "pull": "pulldown"
    },
    "ds1302_idata": {
      "type": "GPIO",
      "port": 14,
      "dir": "input",
      "pull": "pullup"
    },
    "ds1302_odata": {
      "type": "GPIO",
      "port": 14,
      "dir": "output",
      "pull": "pulldown"
    },
    "ds1302_rst": {
      "type": "GPIO",
      "port": 13,
      "dir": "output",
      "pull": "pulldown"
    }
  },
}
```

```python
import utime   # 延时函数在utime库中
from driver import GPIO
import ds1302

ds1302_obj = None

def ds1302_init():
    global ds1302_obj
    gpioclk = GPIO()
    gpioclk.open("ds1302_clk")
    gpiorst = GPIO()
    gpiorst.open("ds1302_rst")
    ds1302_obj = ds1302.DS1302(gpioclk,gpiorst,"ds1302_idata","ds1302_odata")
    print('start init ds1302')

if __name__ == '__main__':
    ds1302_init()
    ds1302_obj.setDatetime([2022, 6, 13 , 0, 0, 30, 1])
    while True:
        print(ds1302_obj.getDatetime())
        utime.sleep(1)
```

* 输出
```log
start init ds1302
[2022, 6, 13, 0, 0, 31, 1]
[2022, 6, 13, 0, 0, 32, 1]
[2022, 6, 13, 0, 0, 33, 1]
[2022, 6, 13, 0, 0, 34, 1]
[2022, 6, 13, 0, 0, 35, 1]
[2022, 6, 13, 0, 0, 36, 1]
```

## 参考文献及购买链接
[1] [购买链接](https://item.taobao.com/item.htm?spm=a230r.1.14.48.2e642464ROOvfe&id=531486841000&ns=1&abbucket=2#detail)
