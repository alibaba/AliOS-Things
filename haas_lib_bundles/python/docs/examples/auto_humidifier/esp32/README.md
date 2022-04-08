# 全自动加湿器

## 简介
&emsp;&emsp;
我们会发现，在很多白领们的工位上都会摆一个加湿器，尤其是秋冬季的时候，不管北方还是南方，空气都会让人感觉非常干燥，就是因为空气的湿度比较低。所以结合HaaS案例框架的设计理念：能解决人们生活中刚需的需求就是好需求，是一个有用的需要。

&emsp;&emsp;
本课程就手把手教开发者如何自制一款自动温湿度加湿器。

&emsp;&emsp;
本课程的理念：希望开发者学习完本课程以后，可以模仿本案例，打造出属于自己的全自动DIY加湿器，摆放在办公室、家中等等的地方。

&emsp;&emsp;
本案例设置的默认湿度阈值是60%H，每个人可以根据自己的情况来更改：
1. 湿度高于60%H：关闭加湿器
2. 湿度低于60%H：打开加湿器

## 准备
&emsp;&emsp;
本案例打造需要如下硬件：
* ESP32一台
* sht3X温湿度传感器一个
* SSD1306显示屏一个
* 加湿器模块一套
* 杜邦线若干
* Micro-USB数据线一条

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/1_ESP32_huma_auto_humi_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知OLED和SHT3X温湿度模块是I2C总线连接，加湿器模块通过I/O控制开关。

<br>

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

## 创建解决方案

&emsp;&emsp;
点击下图中的"快速开始"按键(<img src=./../../../images/1_HaaS_Studio_创建工程按钮.png width=8%/>)会弹出HaaS Studio的欢迎页面，请点击“创建项目”按钮。

&emsp;&emsp;
在随后弹框中，设定好项目名称（“auto_humidifier”）及工作区路径之后，硬件类型选择ESP32，点击“立即创建”，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/1_创建auto_humidifier工程_ESP32.png width=80%/>
</div>

&emsp;&emsp;
将[脚本目录](./code)中的所有文件复制后覆盖auto_humidifier工程目录下的原有文件。其中main.py脚本的内容如下图所示：
```python
import utime   # 延时函数在utime库中
from driver import GPIO,I2C
import sht3x
from ssd1306 import SSD1306_I2C

HUMI_CTRL_VALUE = 60.0

hum_s = 0
oled = None
sht3xDev = None
humi_gpio = None

def sht3x_init():
    global sht3xDev

    i2cDev = I2C()
    i2cDev.open("sht3x")
    sht3xDev = sht3x.SHT3X(i2cDev)

def humi_ctrl_init():
    global humi_gpio

    humi_gpio = GPIO()
    humi_gpio.open("hum_ctrl")

def start_hum():
    humi_gpio.write(0)

def stop_hum():
    humi_gpio.write(1)

def oled_init():
    global oled

    i2cObj = I2C()
    i2cObj.open("ssd1306")
    print("ssd1306 inited!")

    oled = SSD1306_I2C(128, 64, i2cObj)
    oled.fill(0)   #清屏背景黑色
    oled.text('welcome haas', 30, 5)
    oled.text('auto humi', 30, 22)
    oled.text(str('----------------------'),3,32)
    oled.text('', 30, 40)

    oled.show()

def oled_data_show(status,humi,time_arr):
    global oled
    oled.fill(0)
    oled.text(str('%d-%02d-%02d'%(time_arr[0],time_arr[1],time_arr[2])),30,5)
    oled.text(str('%02d:%02d:%02d'%(time_arr[3],time_arr[4],time_arr[5])),30,22)
    oled.text(str('----------------------'),3,32)
    if status == 1:
        oled.text('open', 25, 40)
        oled.text(str('%02d'%(humi)+'%H'),75,40)
    elif status == 0:
        oled.text('close', 25, 40)
        oled.text(str('%02d'%(humi)+'%H'),75,40)
    oled.show()

if __name__ == '__main__':
    sht3x_init()
    humi_ctrl_init()
    oled_init()
    while True:
        humidity = sht3xDev.getHumidity()
        if humidity <= HUMI_CTRL_VALUE:
            if hum_s == 0:
                hum_s = 1
                print("start")
                start_hum()
        else :
            if hum_s == 1:
                hum_s = 0
                print("stop")
                stop_hum()
        timeArray = utime.localtime()
        oled_data_show(hum_s,int(humidity),timeArray)
        utime.sleep(1)

```

&emsp;&emsp;
<b>下面是本节添加的重点代码段落讲解：</b>

* SHT3X温湿度初始化以及获取当前湿度值
```python
##初始化温湿度驱动
def sht3x_init():
    global sht3xDev

    i2cDev = I2C()
    i2cDev.open("sht3x")
    sht3xDev = sht3x.SHT3X(i2cDev)

##获取湿度值
humidity = sht3xDev.getHumidity()
```

* 自动控制湿度
```python

##默认湿度阈值
HUMI_CTRL_VALUE = 60.0

if humidity <= HUMI_CTRL_VALUE:
    if hum_s == 0:
        hum_s = 1
        print("start")
        start_hum() ##打开加湿模块
else :
    if hum_s == 1:
        hum_s = 0
        print("stop")
        stop_hum()##关闭加湿模块
```

<br>

## 运行结果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* “ssd1306 inited!”说明OLED初始化完毕
* “start”说明加湿器模块打开
* “stop”说明加湿器模块关闭


```log
mount fs
 ==== python execute bootpy ====
enable OneMinuteOnCloud
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ssd1306 inited!
start
stop
start

```

> 思考：本案例只设计了60%H湿度这个阈值，在实际环境中，有可能会出现一会开加湿器，马上又关闭加湿器，这个地方大家自己思考如何去解决这个临界条件下跳动问题，然后自己优化好，欢迎留言交流～
