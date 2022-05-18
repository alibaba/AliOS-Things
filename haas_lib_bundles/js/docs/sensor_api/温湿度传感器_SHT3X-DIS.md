# 温湿度传感器 - SHT3X-DIS

## 一、产品简介
Sensirion SHT3x-DIS湿度和温度传感器基于CMOSens®传感器芯片，更加智能、可靠，精度更高。SHT3x-DIS具有增强的信号处理能力、两个独特的用户可选I2C地址，通信速度高达1MHz。SHT35-DIS的典型相对湿度 (RH) 精度为±1.5%，典型温度精度为±0.2°C。SHT3x-DIS具有2.5mm x 2.5mm x 0.9mm（长x宽x高）占位面积，电源电压范围为2.4V至5.5V。

## 二、技术参数
* 供电电压：2.4V～5.5V
* I2C接口，通信速度高达1MHz，具有两个用户可选地址
* 完全校准、线性化和温度补偿的数字输出
* 启动和测量速度极快
* 湿度精度：0 ~ 100%，精度为±2%RH
* 温度精度：-40度～125度，精度±0.3℃
* 封装：DFN-8，2.5mm * 2.5mm 0.9mm

## 三、软件接口
### SHT3X(i2cObj) - 创建SHT3X驱动对象
* 函数原型：
> var sht3xDev = sht3x.SHT3X(i2cObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|i2cObj|I2C|是|调用此函数前需确保i2cObj对象已经处于open状态|

* 返回值：
返回SHT3X对象；

* 示例代码：

```js
import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
```

* 输出：
```log
Testing sht3x ...
i2c: open success
```

### getTemperature - 测量温度

* 函数功能：
量测周围温度并返回测量结果

* 函数原型：
> SHT3X.getTemperature()

* 参数说明：
无

* 返回值：
返回温度测量结果（数据类型：浮点型；单位：摄氏度）；

* 示例：

```js
import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
var temperature = sht3xDev.getTemperature();
console.log('temperature is ', temperature, '°C');

```

* 输出：
```log
Testing sht3x ...
i2c: open success
temperature is 25.3°C
```

### getHumidity - 测量相对湿度

* 函数功能：
量测周围相对湿度并返回测量结果

* 函数原型：
> SHT3X.getHumidity()

* 参数说明：
无

* 返回值：
返回相对湿度测量结果（数据类型：整型；单位：%RH）；

* 示例：

```js
import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
var humidity = sht3xDev.getHumidity();
console.log('humidity is ', humidity, '%H');

```

* 输出：
```log
Testing sht3x ...
i2c: open success
humidity is 60%H
```

### getTempHumidity - 量测温度相对湿度

* 函数功能：
同时量测周围温度和相对湿度并返回测量结果

* 函数原型：
> SHT3X.getTempHumidity()

* 参数说明：
无

* 返回值：
返回温度和相对湿度测量结果（数据类型：数组，[温度值, 相对湿度值]；温度单位：摄氏度；相对湿度单位：%RH）；

* 示例：

```js
import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
var data = sht3xDev.getTempHumidity();
console.log('temperature is ', data[0], '°C');
console.log('humidity is ', data[1], '%H');
```

* 输出：
```log
Testing sht3x ...
i2c: open success
temperature is 25.3°C
humidity is 60%H
```

## 四、接口案例
此使用实例在app.json中定义了名为sht3x的I2C类型的对象。在Python脚本中周期性的获取温湿度测量结果并打印在日志中。

* 代码：
```js
# app.json配置：
{
  "name": "esp32",
  "version": "1.0.0",
  "io": {
      "sht3x": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 68
      }
  },

  "debugLevel": "ERROR",
  "repl": "disable"
}

```
```js
import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
setInterval(function(){
    var temperature = sht3xDev.getTemperature();
    var humidity = sht3xDev.getHumidity();
    console.log('temperature is ', temperature, '°C');
    console.log('humidity is ', humidity, '%H');
    var data = sht3xDev.getTempHumidity();
    console.log('temperature is ', data[0], '°C');
    console.log('humidity is ', data[1], '%H');
}, 5000);
```

* 输出：
```log
Testing sht3x ...
i2c: open success
The temperature is 25.3°C
The humidity is 60%H
The temperature is 25.3°C
The humidity is 60%H
...

```

<br>

## 五、通信协议
主控芯片和SHT3X-DIS传感器之间通信所用命令字如下表所示：
|序号|命令字|功能说明|缩写|
|:-:|:-:|:-:|:-|
|1|0x3780|读取芯片序列号|CMD_READ_SERIALNBR|
|2|0xF32D|读取状态寄存器|CMD_READ_STATUS|
|3|0x3041|清除状态寄存器|CMD_CLEAR_STATUS|
|4|0x306D|开启加热功能|CMD_HEATER_ENABLE|
|5|0x3066|关闭加热功能|CMD_HEATER_DISABLE|
|6|0x30A2|软件复位|CMD_SOFT_RESET|
|7|0x2B32|加速响应时间|CMD_ART|
|8|0x3093|停止周期性测量|CMD_BREAK|
|9|0x2400|高频率周期性测量|CMD_MEAS_POLLING_H|
|10|0x240B|中频率周期性测量|CMD_MEAS_POLLING_M|
|11|0x2416|低频率周期性测量|CMD_MEAS_POLLING_L|

实际控制此传感器的流程可参考如下伪代码。
</br>

* 初始化传感器并开始进行温湿度测量
```
    清除状态寄存器（发送CMD_CLEAR_STATUS）
    等待20ms
    发送周期性测量指令（CMD_MEAS_POLLING_H、CMD_MEAS_POLLING_M或CMD_MEAS_POLLING_L）
    等待20ms
```

* 读取温湿度测量结果
```
    发起I2C读取数据（6个字节）的操作
    将读取到的测量结果转换成实际温室度值
```
读取到的6个字节格式如下表所示：
|字节序号|1|2|3|4|5|6|
|:-:|:-:|:-:|:-|:-|:-|:-|
|意义|温度(高位)|温度(低位)|温度CRC-8校验值||湿度(高位)|湿度(低位)|湿度CRC-8校验值|

转换过程可以使用如下公式：
温度值 = ((data[0]<<8) | data[1]) * 0.0015259022
相对湿度值 = -45.0 + ((data[3]<<8) | data[4]) * 175.0 / (0xFFFF - 1)

* 停止测量
```
    发送停止周期性测量指令字（CMD_BREAK）
```

## 六、工作原理

&emsp;&emsp;
从下图可以看到SHT3x内部集成了湿度传感器和温度传感器，支持两种通信方式。<br>
通过ADC采样输入到数据处理和线性化单元，同时带有校正储存器，处理环境对器件测量的影响。<br>
通过数字接口IIC读取数据。带警报引脚，可通过修改寄存器的值设定阈值，当测量的温湿度超过阈值时它会被置位。<br>
<div align="center">
<img src=./../images/ext_sht3x-dis_工作原理.png  width=50%/>
</div>

引脚分布如下，<br>
1和4号脚是IIC总线接口；<br>
2号脚是决定地址的引脚，当ADDR接VSS时芯片地址为0x44，接VCC时芯片地址为0x45；<br>
3号脚警报引脚（当不使用时浮空），当温湿度超过设定的阈值则该脚会被置位；<br>
5号和8号脚是电源引脚；<br>
6号引脚是复位引脚（当不使用时接VDD），低电平有效；<br>
7号脚是为了封装而保留的引脚。<br>
 <div align="center">
<img src=./../images/ext_sht3x-dis_pin.png  width=50%/>
 </div>

下图是设备地址分配情况.
<div align="center">
<img src=./../images/ext_sht3x-dis_address.png  width=50%/>
</div>
&emsp;&emsp;
典型电路如下，由于SCL和SDA为开漏输出，驱动能力不足，需要接上拉电阻。VDD和VSS之间接一个小电容滤除高频杂波，另外nRESET和ALERT根据情况进行选择，若不需要使用，nRESET接高电平，ALERT浮空。Die Pad接地即可。

<div align="center">
<img src=./../images/ext_sht3x-dis_apply.png  width=50%/>
</div>

 ## 参考文献
-[1] [SHT3X-DIS温湿度传感器](https://www.mouser.cn/new/sensirion/sensirion-sht3x-dis-sensors/)<br>
[2] [SHT3x-DIS驱动及应用详解](https://blog.csdn.net/qq_33194301/article/details/103652311)

