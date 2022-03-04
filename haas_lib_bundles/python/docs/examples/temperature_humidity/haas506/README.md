# 温湿度传感器


## 1.简介
&emsp;&emsp;
本案例使用了GXHT30温湿度传感器来获取周围环境的温湿度数据。其中所用到的GXHT30传感器与SHT30传感器兼容。

&emsp;&emsp;
硬件模块如下图所示：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_GXHT30.jpg width=40%/>
</div>
<br>

&emsp;&emsp;
[温湿度模块（GXHT30）](https://detail.tmall.com/item.htm?id=657097046278&price=8.68&sourceType=item&sourceType=item&suid=0344a820-c038-4d9e-900f-5747508f0584&ut_sk=1.WGfWIj%20vg4sDABgbH0dov14E_21646297_1645611287433.Copy.ShareGlobalNavigation_1&un=2e9dd41a9eef2734d9a2d33a0bf32337&share_crt_v=1&un_site=0&spm=a2159r.13376460.0.0&tbSocialPopKey=shareItem&sp_tk=MVM4TDJWN1dtcTI=&cpp=1&shareurl=true&short_name=h.fnjX4bP&bxsign=scdEVmEzmxN3iPyJ4W5IjWGWIHk80w3t76xb8klY0nUtiBpPf50BJgGWu2Sg1Bc_K2tshcFURkNMC1HuDnnIM32mofmZn6_hDaWEOq92yDwtq6XH-mZoMwZNJBuhaj1c_wD&tk=1S8L2V7Wmq2&app=chrome
) |

&emsp;&emsp;
温湿度模块参数：
* 湿度测量范围：0-100%RH
* 湿度测量精度：±3%RH
* 温度测量范围：-40-125℃
* 温度测量精度：±0.3℃
* 工作电压：2.2-5.5V(DC)
* 通信方式：I2C




## 2.准备
&emsp;&emsp;
硬件器材：
1. HaaS 506开发板一套
2. 温湿度传感器（GXHT30）一个
3. 连接线若干

<br>

&emsp;&emsp;硬件接线表：
| 硬件 | 温湿度传感器 | HaaS 506开发板 |
| --- | --- | --- |
|端口标识 | VCC | VCC |
|端口标识 | GND | GND |
|端口标识 | SCL | SCL |
|端口标识 | SDA | SDA |

<br>

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_温湿度计.jpg width=90%/>
</div>
<br>


## 3.程序实现
&emsp;&emsp;main.py
```
import utime as time
from driver import I2C
import gxht30

#创建并打开一个I2C实例
i2cDev=I2C()
i2cDev.open('gxht30')
#创建一个温湿度实例
tem_hum_dev=gxht30.GXHT30(i2cDev)
#获取温湿度数据
while True :
    cTemp,fTemp,humidity=tem_hum_dev.measure()
    print ("Temperature in Celsius : %.2f C" %cTemp)
    print ("Temperature in Fahrenheit : %.2f F" %fTemp)
    print ("Relative Humidity : %.2f %%RH" %humidity)
    print("------------------------------------------\n")
    #数据显示的时间间隔
    time.sleep(1)
```
<br>

## 4.运行结果
### 本地查看

运行过程中日志如下:

```python
amp shakehand begin...

Temperature in Celsius : 21.94 C
Temperature in Fahrenheit : 71.49 F
Relative Humidity : 35.00 %RH
------------------------------------------

Temperature in Celsius : 22.68 C
Temperature in Fahrenheit : 72.83 F
Relative Humidity : 41.43 %RH
------------------------------------------

Temperature in Celsius : 23.78 C
Temperature in Fahrenheit : 74.80 F
Relative Humidity : 54.12 %RH
------------------------------------------

Temperature in Celsius : 24.18 C
Temperature in Fahrenheit : 75.52 F
Relative Humidity : 60.32 %RH
------------------------------------------
......
```


