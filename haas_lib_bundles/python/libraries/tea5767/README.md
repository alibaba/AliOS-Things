# FM收音机模块 - TEA5767

## 一、产品简介
TEA5767 FM收音机模块是工作频率在76MHz～108MHz的自动数字调谐收音机。其特点高灵敏度、高稳定、低噪声，内部集成了中频选频和解调网络。

<div align="center">
<img src=./../../docs/images/tea5767-front.png  width=30%/>
</div>

引脚定义
* GND：接地<br>
* SCL：I2C时钟<br>
* SDA：I2C数字<br>
* VCC：5V<br>

## 二、技术参数
* 供电电压：5V DC<br>
* 接口：I2C接口<br>
* 频率：76MHz-108MHz<br>
* 电路：AGC（高放自动增益控制）<br>
* 特性：支持SNC（立体声消噪）、锁相环调谐系统 <br>
* 模块尺寸：30mm * 30mm<br>

## 三、软件接口

TEA5767 FM收音机模块HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/tea5767)。

### TEA5767(i2cObj) - 创建TEA5767驱动对象
* 函数原型：
> tea5767Obj = TEA5767(i2cObj)

* 参数说明：
<br>
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
<br>
TEA5767对象创建成功，返回TEA5767对象；TEA5767对象创建失败，抛出Exception

* 示例代码：

```python
from driver import I2C
import tea5767

i2cObj = I2C()
i2cObj.open("tea5767")
robot=tea5767.TEA5767(i2cObj)
print('tea5767 inited')
```

* 输出：
```log
tea5767 inited!
```

### setFrequency(freq) - 设置FM工作频率

* 函数功能：
<br>
设置FM收音机工作频率

* 函数原型：
> TEA5767.setFrequency(freq)

* 参数说明：

|返回值|类型|说明|
|-----|----|----|
|freq|float|76.0~108.0，单位（MHz）|

* 返回值：
无

* 示例：

```python
from driver import I2C
import tea5767

i2cObj = I2C()
i2cObj.open("tea5767")
robot=tea5767.TEA5767(i2cObj)
print('tea5767 inited')
robot.setFrequency(91.8)
print('set freq = 91.8Mhz')
```

* 输出：
```log
tea5767 inited
set freq = 91.8Mhz
```

## 四、接口案例
<br>
此使用实例在board.json中定义了名为tea5767的I2C类型的对象。在Python脚本中设置FM频率为91.8Mhz的频道并打印在日志中。

* 代码：
board.json配置
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
      "tea5767": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 96
      }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

* 示例：
```python
from driver import I2C
import tea5767

i2cObj = I2C()
i2cObj.open("tea5767")
robot=tea5767.TEA5767(i2cObj)
print('tea5767 inited')
robot.setFrequency(91.8)
print('set freq = 91.8Mhz')
```

* 输出：
```log
tea5767 inited
set freq = 91.8Mhz
```

<br>

 ## 参考文献及购买链接
[1] [TEA5767收音机模块介绍说明](https://www.mwrf.net/tech/communications/2013/12576.html)<br>
[2] [TEA5767收音机模块购买](https://item.taobao.com/item.htm?spm=a230r.1.14.16.6eda2f4bsR3IjR&id=542111885602&ns=1&abbucket=8#detail)
