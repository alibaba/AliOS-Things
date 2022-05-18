# 16路PWM模块 - PCA9685

## 一、产品简介
PCA9685芯片，是16通道12bit PWM舵机驱动，主控板通过I2C控制芯片，进而可以驱动16个舵机，这样可以解决需要很多电机控制的项目，会大量占用主控板的引脚，也会影响主控板的处理能力。

<div align="center">
<img src=./../../docs/images/pca9685-front.png  width=50%/>
</div>

引脚定义
* GND：接地<br>
* OE：GPIO（低电平有效，板载已经将OE拉低，使用时可以不接）<br>
* SCL：I2C时钟<br>
* SDA：I2C数字<br>
* VCC：5V<br>
* V+：NC<br>

## 二、技术参数
* 供电电压：5V<br>
* 接口：I2C接口，最大支持1MHz速率<br>
* 地址：6地址位I2C总线上可挂载62个设备<br>
* PWM输出支持可调频，1.6KHz<br>
* 分辨率：12位<br>
* IC待机电流：20mA<br>
* 模块尺寸：25mm * 61mm * 10mm<br>

## 三、软件接口
PC9685 PWM舵机控制器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/pca9685)
### PCA9685(i2cObj) - 创建PCA9685驱动对象
* 函数原型：
> pca9685Obj = PCA9685(i2cObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
PCA9685对象创建成功，返回PCA9685对象；PCA9685对象创建失败，抛出Exception

* 示例代码：

```python
from driver import I2C
import pca9685

i2cObj = I2C()
i2cObj.open("pca9685")
robot=pca9685.PCA9685(i2cObj)
print('pca5860 inited')
```

* 输出：
```log
pca5860 inited!
```

### setServo(index,pos) - 设置各路Servo电机的位置

* 函数功能：
设置各路Servo电机的位置

* 函数原型：
> PCA9685.setServo(index,pos)

* 参数说明：

|返回值|类型|说明|
|-----|----|----|
|index|整型|index：0-15，分别代表第index+1路PWM|
|pos|浮点|-pi/2~pi/2 <br> 单位：度<br>pi约等于3.141|

* 返回值：
无

* 示例：

```python
from math import pi
from driver import I2C
import pca9685
import utime

i2cObj = I2C()
i2cObj.open("pca9685")
robot=pca9685.PCA9685(i2cObj)
print('pca5860 inited')
robot.setServo(0,-pi/4)
utime.sleep(1)
robot.setServo(0,pi/4)
utime.sleep(1)
robot.setServo(1,-pi/4)
utime.sleep(1)
robot.setServo(1,pi/4)
utime.sleep(1)
robot.setServo(2,-pi/4)
utime.sleep(1)
robot.setServo(2,pi/4)
utime.sleep(1)
robot.setServo(3,-pi/4)
utime.sleep(1)
robot.setServo(3,pi/4)
utime.sleep(1)
```

* 输出：
```log
pca5860 inited
```

### reset(index) - 复位各路PWM

* 函数功能：
复位各路PWM

* 函数原型：
> PCA9685.reset(index)

* 参数说明：

|返回值|类型|必选参数？|说明|
|-----|----|:---:|----|
|index|整型|是|index：0-15，分别代表第index+1路PWM|

* 返回值：
无

* 示例：

```python
from math import pi
from driver import I2C
import pca9685
import utime

i2cObj = I2C()
i2cObj.open("pca9685")
robot=pca9685.PCA9685(i2cObj)
print('pca5860 inited')
robot.setServo(0,-pi/4)
utime.sleep(1)
robot.setServo(0,pi/4)
utime.sleep(1)
robot.reset(0)
```

* 输出：
```log
pca5860 inited
```

## 四、接口案例
此使用实例在board.json中定义了名为pca9685的I2C类型的对象。在Python脚本中周期性的设置第1路的舵机从0度到负90度，然后从负90度回到0度位置并打印在日志中。

* 代码：

board.json配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "pca9685": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 1000000,
        "mode": "master",
        "devAddr": 64
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

* 示例：
```python
from math import pi
from driver import I2C
import pca9685
import utime

i2cObj = I2C()
i2cObj.open("pca9685")
robot=pca9685.PCA9685(i2cObj)
print('pca5860 inited')
while True:
    robot.setServo(0,0)
    print(0)
    utime.sleep(1)
    robot.setServo(0,-pi/2)
    print(-pi/2)
    utime.sleep(1)
```

* 输出：
```log
pca5860 inited
0
-pi/2
0
-pi/2
0
-pi/2
0
-pi/2
```

<br>

## 五、工作原理

PCA9685传感器寄存器描述如下表所示：
|序号|寄存器地址|功能说明|
|:-:|:-:|:-:|
|1|0x00|MODE1|
|2|0x01|MODE2|
|3|0x02|SUBADR1|
|4|0x03|SUBADR2|
|5|0x04|SUBADR3|
|6|0x05|ALLCALLADR|
|7|0x06|LED0_ON_L|
|8|0x07|LED0_ON_H|
|9|0x08|LED0_OFF_L|
|10|0x09|LED0_OFF_H|
|11|...|...|
|12|0x06+4*X|LEDX_ON_L|
|13|0x06+4*X+1|LED0_ON_H|
|14|0x06+4*X+2|LED0_OFF_L|
|15|0x06+4*X+3|LED0_OFF_H|
|16|0xFA|ALL_LED_ON_L|
|17|0xFB|ALL_LED_ON_H|
|18|0xFC|ALL_LED_OFF_L|
|19|0xFD|ALL_LED_OFF_H|
|19|0xFE|PRE_SCALE|
|19|0xFF|TestMode|
</br>

### 通道的四个寄存器
在上述的寄存器总览表中可看出：16个通道中，每个都有LEDX_ON_L、LEDX_ON_H、LEDX_OFF_L、LEDX_OFF_H 四个寄存器，芯片中12位的计数器CNT，会根据PRE_SCALE设置的值进行计数。<br>
* 当LEDX_ON_H[3:0]:LEDX_ON_L < CNT < LEDX_OFF_H[3:0]:LEDX_OFF_L时，输出高电平；
* 当LEDX_OFF_H[3:0]:LEDX_OFF_L < CNT < 4096时，输出低电平。
<br>

如下图所示，假设LEDX_ON=410,LEDX_OFF=1300,那么在计数器从0计数到409，开始输出高电平，高电平持续LEDX_OFF-LEDX_ON，然后开始输出低电平，到一个PWM周期结束，后面周而复始循环动作。
<br>

<div align="center">
<img src=./../../docs/images/pca9685_led.png  width=70%/>
</div>

### PRE_SCALE寄存器
prescale = round(osc_clock/4096∗update_rate) - 1
* osc_clock为选用的时钟频率，如使用内部25MHz时钟，即为25000000Hz。
* update_rate为PWM的频率，比如舵机PWM周期为20ms，则update_rate=50Hz。
* 4096是因为计数器是12位。
* prescale为计数器加一所需要的时钟数

<br>

### 舵机控制
舵机的PWM信号控制周期为20ms（50Hz的频率），想要控制角度只需控制一个周期中高电平持续的时间。以180°舵机为例，对应关系如下：
|一个周期中高电平持续时间|舵机保持的角度|
|:-:|:-:|
|0.5ms|0度|
|1ms|45度|
|1.5ms|90度|
|2ms|135度|
|2.5ms|180度|
<br>

 ## 参考文献及购买链接
[1] [PCA9685模块使用说明](https://blog.csdn.net/qq_44897194/article/details/105441545)<br>
[2] [PCA9685模块购买](https://item.taobao.com/item.htm?spm=a230r.1.14.16.366113985GLmJm&id=528707992631&ns=1&abbucket=17#detail)
