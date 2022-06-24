# GPS模块

## 一、产品简介
HT2828Z3G5L是一款高性能的面向车载组合导航领域的定位G-MOUSE，系统包含同时支持北斗和GPS的高性能卫星接收机芯片。具备全方位功能，能满足专业定位的严格要求。体积小巧，可以装置在汽车内部任何位置，低功耗，能适应个人用户的需要。<br>
该产品采用了新一代AT6558D低功耗芯片，高灵敏度，在城市峡谷、高架下等信号弱的地方，都能快速、准确的定位。可广泛应用开发多种GPS终端产品，如：汽车导航汽年保全系统、车辆监控以及其他卫星定位应用等。
<div align="center">
<img src=./../../docs/images/ext_GPS传感器_示意图.png  width=50%/>
</div>

<br>

接口说明
| 标号  | 名称     | 功能描述                                          |
| :---: | :------- | :------------------------------------------------ |
|   1   | VBAT/PPS | 兼容备用电池/时间标准脉冲输出                     |
|   2   | VCC      | 系统主电源，供电电压为3.3~5V,工做时消耗电流约25mA |
|   3   | TX       | UART/TTL接口，可选RS232_TXD                       |
|   4   | RX       | UART/TTL接口，可选RS232_RXD                       |
|   5   | GND      | 接地                                              |
|   6   | EN       | 电源使能，高电平/悬空模组工作，低电平模组关闭     |

## 二、技术参数
### 1、电气特性
| 输入电压 | 宽电压范围：主电源为3.3～5V |
| :------: | :-------------------------- |
| 工作电流 | 30mA                        |
| 备用电压 | 1.8～3.6V                   |
| 温度范围 | 5% to 95% non-condensing    |
| 工作温度 | -40度~100度                 |
| 存储温度 | -55度~100度                 |

### 2、技术指标
* 行业标准的25*25*4mm高灵敏度GPS天线<br>
* 采用0.5PPM高精度TCXO<br>
* 内建RTC晶体及皮法电容更快的热启动<br>
* 内建LNA,低噪声信号放大器<br>
* 采用6558D芯片，内置FLASH，支持温启动<br>
* 1～10Hz定位更新速率<br>
* 支持AssistNow Online和AssistNow Offline等A-GPS服务<br>
* GPS、北斗、GLONASS、（WMS，EGNOS,MSAS，GAGAN)混合引擎<br>

### 3、模块性能
| 标号  | 名称                 | 功能描述                                                      |
| :---: | :------------------- | :------------------------------------------------------------ |
|   1   | 芯片                 | AT65558D(内置FLASH,支持温启动)                                |
|   2   | 频率                 | BDS/GPS/1.5611-1.57542GHz                                     |
|   3   | C/A码                | 1.023MHz码流                                                  |
|   4   | 可用波特率           | 4800，19200，38400，57600，115200bps（默认9600）              |
|   5   | 通道                 | 三通道射频，支持GPS+BDS（默认）、BDS+GLONASS、GPS+GLONASS输出 |
|   6   | SWR                  | S11<=1.3                                                      |
|   7   | SWR                  | S22<=1.3                                                      |
|   8   | Log Mag              | S212>=20.0dB                                                  |
|   9   | Smith                | S11:50欧姆+5%                                                 |
|  10   | 灵敏度               | 跟踪：-162dBm,捕捉：-148dBm                                   |
|  11   | 冷启动               | 平均35秒                                                      |
|  12   | 冷启动灵敏度         | -148dBm                                                       |
|  13   | 温启动               | 平均10秒                                                      |
|  14   | 热启动               | 平均1秒                                                       |
|  15   | 热启动灵敏度         | -156dBm                                                       |
|  16   | AGPS网络辅助星历数据 | 3s（平均)                                                     |
|  17   | 定位精度             | <5m                                                           |
|  18   | 授时精度             | 30ns                                                          |
|  19   | 方向                 | <0.5Degree                                                    |
|  20   | 参照坐标系           | WGS-84                                                        |
|  21   | 速率                 | <0.1m/s                                                       |
|  22   | 最大海拔高度         | 50000米                                                       |
|  23   | 标准时钟脉冲         | 0.25Hz~1KHz                                                   |
|  24   | 最大速度             | 515m/s                                                        |
|  25   | 最大加速度           | <=4G                                                          |
|  26   | 更新频率             | 1～10Hz（默认1Hz）                                            |
|  27   | 端口界面             | UART:232/TTL                                                  |
|  28   | 输出语句             | NMEA 0183 V3.0(GGA，GSA，GSV，RMC,VTG，GLL)协议数据           |

<br>

## 三、软件接口
<br>

### Gnss(uartObj) - 创建全球导航定位传感器驱动对象
<br>

* 函数原型

> gnssObj = Gnss(uartObj)

* 参数说明：

|  参数   | 类型  | 必选参数？ | 说明                                          |
| :-----: | :---: | :--------: | --------------------------------------------- |
| uartObj | UART  |     是     | 调用此函数前需确保uartObj对象已经处于open状态 |

* 返回值

> 若创建Gnss对象成功，返回Gnss对象；若创建Gnss对象创建失败，抛出Exception

* 示例代码
```python
import gnss
from driver import UART
print("Testing gnss ...")
uartDev = UART()
uartDev.open("gnss")
gnssDev = gnss.Gnss(uartDev)
```

* 输出
```
Testing gps ...
```
</br>

### getLocation() - 获取GPS信息值
<br>

* 函数功能：

> 解析更新GPS数据

* 函数原型：

> Gnss.getLocation()

* 参数说明：

无

* 返回值：

> 成功：返回gnss定位信息值，数据类型：字典，含义如下：<br> longitude(纬度)  <br> latitude(经度) <br> altitude(海拔)

* 示例：

```python
import gnss
from driver import UART
print("Testing gnss ...")
uartDev = UART()
uartDev.open("gnss")
gnssDev = gnss.Gnss(uartDev)
location = gnssDev.getLocation()
print("The gnss infor 纬度-%d 经度-%d 海拔-%d",location.longitude, location.latitude, location.altitude)
```

* 输出
```
Testing gnss ...
The gnss infor 纬度-xx 经度-xx 海拔-xx
```

</br>

## 四、接口案例

* board.json
```json
{
  "name": "esp32",
  "version": "1.0.0",
  "io": {
      "serial2": {
        "type": "UART",
        "port": 2,
        "dataWidth": 8,
        "baudRate": 9600,
        "stopBits": 1,
        "flowControl": "disable",
        "parity": "none"
      }
  },

  "debugLevel": "ERROR",
  "repl": "disable"
}
```

```python
import gnss
from driver import UART
print("Testing gnss ...")
uartDev = UART()
uartDev.open("gnss")
gnssDev = gnss.Gnss(uartDev)
location = gnssDev.getLocation()
print("The gnss infor 纬度-%d 经度-%d 海拔-%d",location.longitude, location.latitude, location.altitude)
```

* 输出
```
Testing gnss ...
The gnss infor 纬度-xx 经度-xx 海拔-xx
```

## 五、工作原理
GPS是全球定位系统（Global Position System）的简称，常见于汽车、手机中。民用GPS的定位精度在10~20米之间，这是为什么用手机的GPS定位，有时候明明在陆地上，却被认为在河里的原因。GPS定位的原理很简单，叫做三角定位法（Triangulation）。原理如下图所示：
<div align="center">
<img src=./../../docs/images/ext_GPS传感器_工作原理1.png  width=100%/>
</div>

(图片出处：https://www.nationalgeographic.org/photo/triangulation-sized/)

<br>

装在无人车上的GPS接收机，首先量测无线电信号到达卫星的传播时间，再将传播时间乘以光速，即可得到当前GPS接收机到达卫星的距离，有了距离，就可以根据几何原理求得位置了。

<div align="center">
<img src=./../../docs/images/ext_GPS传感器_工作原理2.png  width=100%/>
</div>


(图片出处：http://slideplayer.com/slide/5717261/)

<br>

若已知GPS接收机到达1号卫星和3号卫星的距离，那么1号和3号卫星根据距离产生两个球体（图中绿色和蓝色的球体），两个球体的相交的部分为圆形，该圆形与地球表面靠近的任何一个点都有可能是当前无人车（GPS接受装置）的位置。

<br>

因此仅根据这两个距离信息，还无法确定当前无人车的具体位置。此时通过引入第三个卫星的距离，就能较为准确地确定无人车的位置。这就是三角定位法的原理。
<br>



## 参考文献
[1] [GPS模块](https://detail.tmall.com/item.htm?id=624818869038&price=26.6&sourceType=item&sourceType=item&suid=ad59cf26-7f69-413a-b68b-aec2adf0ab2f&shareUniqueId=12327961394&ut_sk=1.Xiv3G2Rx9KsDAJMZlp0doD0d_21646297_1631675328230.DingTalk.1&un=f1829ead73825e46875dbdeffcd1c5a5&share_crt_v=1&cpp=1&shareurl=true&spm=a313p.22.1pw.1314950616524&short_name=h.f2n2K8t&bxsign=scdy4iisyiCmWe6Vjv3QqLEhXJuph-ngG8mQ_pa8_9bVy0a6Imtb21cchxHNdu1nQp1iJ2s-w-poP8sRFC_t7_4zKdp8Lx_hKdo0FUiP9Yve68&app=macos_safari)
