
# 人体存在传感器


## 1.简介
&emsp;&emsp;
本案例使用了5.8G雷达感应传感器模块实现人体活动检测功能，当检测到有人活动时，会播报语音。当前音频文件被放置于工程文件夹中。

&emsp;&emsp;
硬件模块如下图所示：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_bodyDetect1.JPG width=40%/>
</div>
<br>

&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_body_detect2.JPG width=40%/>
</div>
<br>

&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_body_detect3.JPG width=40%/>
</div>
<br>

&emsp;&emsp;
硬件模块PIN管脚如下图所示：
<table align="center" class="table table-bordered table-striped table-condensed">
    <tr>
        <td>PIN</td>
	    <td>功能</td>
        <td>描述</td>
    </tr>
    <tr>
        <td>VCC</td>
	    <td>电源正</td>
        <td>2.7-4.8V（DC)</td>
    </tr>
    <tr>
        <td>GND</td>
	    <td>电源负</td>
        <td>0V</td>
    </tr>
    <tr>
        <td>OUT</td>
	    <td>信号输出</td>
        <td>输出高低电平（0/2.2V)  </td>
    </tr>
    <tr>
        <td>P2</td>
	    <td>GPIO2</td>
        <td>接收增益档位选择（悬空/拉低选择)</td>
    </tr>        
    <tr>
        <td>P3</td>
	    <td>GPIO3</td>
        <td>延时时间档为选择（悬空/拉低</td>
    </tr>  
</table>
<br>


## 2.准备

（1）硬件器材
1. HaaS 506开发板一套
2. 5.8G雷达感应传感器模块一个
3. 连接线若干

<br>



（2）硬件模块接线

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_雷达人体检测.jpg width=80%/>
</div>
<br>



 （3）将音频文件放置于当前工程文件夹内
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_body_detect4.jpg width=80%/>
</div>
<br>


## 3.程序实现

&emsp;&emsp;main.py
```
from driver import GPIO
import utime as time
import audio
import radar

#创建并打开一个GPIO实例
gpiorDev=GPIO()
gpiorDev.open('radar')
#创建一个radar实例
radarDev=radar.RADAR(gpiorDev)

#创建并打开一个Audio实例
aud=audio.Audio()
#开启音频播放使能
aud.set_pa()
#设置音量
aud.setVolume(5)
#循环检测是否有人经过
while True:
    if radarDev.detect()==1:
        print("detect body")
        aud.play('/data/pyamp/detect_body.mp3')
    else:
        print("no body")
    time.sleep(2)        

```
<br>

## 4.运行结果
### 本地查看

运行过程中日志如下:

```python
amp shakehand begin...

detect body
detect body
no body
detect body
detect body
no body
no body
no body
no body
detect body

......
```
