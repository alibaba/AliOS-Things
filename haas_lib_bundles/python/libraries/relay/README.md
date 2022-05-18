# relay 继电器模块

## 一、产品简介
&emsp;&emsp;
继电器（英文名称：relay）是一种电控制器件，是当输入量的变化达到规定要求时，在电气输出电路中使被控量发生预定的阶跃变化的一种电器。它具有控制系统（又称输入回路）和被控制系统（又称输出回路）之间的互动关系。通常应用于自动化的控制电路中，它实际上是用小电流去控制大电流运作的一种“自动开关”。故在电路中起着自动调节、安全保护、转换电路等作用。

<div align="center">
<img src=./../../docs/images/继电器.jpeg width=50%/>
</div>

引脚定义
- 控制端
  * VCC：电源正
  * GND：电源地
  * IN： 控制触发引脚
- 受控端
  * NO(Nomal Open)：常开触点，继电器未触发时断开
  * NC(Nomal Close)：常闭触点，继电器未触发时闭合
  * COM(Common)：公共端，当触点闭合时，即与公共端连接

> ⚠️ 在连接受控端时，请务必注意用电安全

## 二、技术参数
不同的继电器模块有着不同的技术参数，其一般会打印在继电器模块顶部，以上图为例，该模块的技术参数为：

* 常开接口最大负载：交流250V/10A
* 工作电压：直流5V

在使用中，开发者可以根据场景的电压需求来选用不同技术参数的传感器。

## 三、软件接口

继电器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/relay)
<br>

### Relay - 创建Relay驱动对象

* 函数原型

> relayObj = Relay(gpioObj:GPIO, trigger:int)

* 参数说明

|  参数   | 类型  | 必选参数？ | 说明                                                    |
| :-----: | :---: | :--------: | :------------------------------------------------------ |
| gpioObj | GPIO  |     是     | 继电器控制引脚的GPIO对象。调用此函数前需确保对象已被open |
| trigger |  int  |     是     | 继电器触发方式。低电平触发为0；高电平触发为1            |

* 返回值

若Relay对象创建成功，返回Relay对象；否则抛出Exception

* 示例代码
```python
from relay import Relay
from driver import GPIO

relayIO = GPIO()
relayIO.open("relayIO")

relayObj = Relay(relayIO, 1)    # 高电平触发
```

### untriger() - 取消触发继电器

* 函数功能：

取消触发继电器。此时，继电器处于常置状态。

常开端保持**开启**，常闭端保持**闭合**。

**常闭端与公共端形成回路**

**常开端与公共端断开**

* 函数原型：

> Relay.untriger()

* 参数说明：

无

* 返回值：

无

### triger() - 触发继电器

* 函数功能：

触发继电器。此时，继电器处于非常置状态。

常开端**闭合**，常闭端**打开**。

**常开端与公共端形成回路**

**常闭端与公共端断开**

* 函数原型：

> Relay.triger()

* 参数说明：

无

* 返回值：

无

## 四、接口案例

board.json 配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "relayIO": {
            "type": "GPIO",
            "port": 21,
            "dir": "output",
            "pull": "pulldown"
        },
    }
}
```

```python
from relay import Relay
from driver import GPIO

relayIO = GPIO()
relayIO.open("relayIO")

relayObj = Relay(relayIO, 1)    # 高电平触发

relayObj.triger()
relayObj.untriger()
```

## 五、工作原理

<div align="center">
<img src=./../../docs/images/继电器原理.png width=100%/>
</div>

&emsp;&emsp;
以传统的电磁继电器为例：

&emsp;&emsp;
电磁继电器是利用电磁铁控制工作电路通断的开关.

&emsp;&emsp;
如图所示，电磁继电器的主要部件是电磁铁、衔铁、弹簧和一个动触点（公共触点）、两个静触点（常闭触点及常开触点）。

&emsp;&emsp;
工作电路可分为低压控制电路和高压工作电路两部分，低压控制电路包括电磁继电器线圈(电磁铁)，低压电源，开关；高压工作电路包括高压电源，电动机，电磁继电器的触点部分。

&emsp;&emsp;
只要在线圈两端加上一定的电压，线圈中就会流过一定的电流，从而产生电磁效应，衔铁就会在电磁力吸引的作用下克服返回弹簧的拉力吸向铁芯，从而带动衔铁的动触点（公共触点）与静触点（常开触点）吸合。当线圈断电后，电磁的吸力也随之消失，衔铁就会在弹簧的反作用力返回原来的位置，使动触点（公共触点）与原来的静触点（常开触点）释放。这样吸合、释放，从而达到了在电路中的导通、切断的目的。对于继电器的“常开、常闭”触点，可以这样来区分：继电器线圈未通电时处于断开状态的静触点，称为“**常开触点**”；处于接通状态的静触点称为“**常闭触点**”。

## 参考文献及购买链接
[1] [电磁继电器](https://baike.baidu.com/item/电磁继电器/5975261)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=41231430731)
