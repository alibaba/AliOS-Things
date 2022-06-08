# 气压检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的的 2 步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/气压检测系统_开发步骤.png width=50%/>
</div>
<br>

## 1、简介
&emsp;&emsp;
气压是作用在单位面积上的大气压力，即在数值上等于单位面积上向上延伸到大气上界的垂直空气柱所受到的重力。气压的国际制单位是帕斯卡，简称帕，符号是Pa。气象学中，人们一般用千帕（kPa）或百帕（hPa）作为单位。气压不仅随高度变化，也随温度而异。

### 1.1、背景知识
&emsp;&emsp;
本系统的核心在于如何精准的检测环境中的大气压值，测量大气压值一般使用气压传感器。气压传感器其实就是一个惠斯通电桥，扩散在单晶硅片上，用于检测电阻值的微小变化。电压阻效应使桥壁电阻值发生变化，产生一个差动电压信号。根据差动电压信号，即可计算出气体压强是多大。

&emsp;&emsp;
本案例选用的是BMP280气压传感器，可精确测量空气中的气压和温度，具有稳定性高、抗干扰能力强等特点。

<div align="center">
<img src=./../../../images/气压检测系统_BMP280气压传感器.png width=10%/>
</div>
<br>

### 1.2、准备
&emsp;&emsp;
完成本案例需要如下硬件：

&emsp;&emsp;
1、E-SP32开发板一套

&emsp;&emsp;
2、BMP280气压传感器一个

&emsp;&emsp;
3、杜邦连接线若干

&emsp;&emsp;
硬件连线图如下图所示，六个引脚的名称与功能如下：

&emsp;&emsp;
1、pin1：VCC（3.3V供电，切记不可以接5v）

&emsp;&emsp;
2、pin2：GND

&emsp;&emsp;
3、pin3：SCL（I2C通信模式时钟信号）

&emsp;&emsp;
4、pin4：SDA（I2C通信模式数据信号）

&emsp;&emsp;
5、pin5：CSB（SPI通信模式下用到的引脚，片选引脚，拉低后启用）

&emsp;&emsp;
6、pin6：SDO（传感器地址控制位）

<div align="center">
<img src=./../../../images/气压检测系统_ESP32_连线.png width=70%/>
</div>
<br>

## 2、开发步骤

### 2.1、搭建开发环境

&emsp;&emsp;
在进行开发之前请确保ESP32开发环境已经搭建完毕。详情请参考[M5StackCore2开发环境](../../../startup/M5StackCore2_startup.md)的说明。

### 2.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“气压检测系统”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
推送此工程到 M5StackCore2 设备上，如下图所示。

<div align="center">
<img src=./../../../images/气压检测系统-推送步骤.png width=90%/>
</div>

&emsp;&emsp;
推送过程中的日志输出如下图所示。如果需要选择串口号，请选择“/dev/cu.wchusbserial53190134851 'USB Single Serial'”，直接输入串口号前面的数字即可。

<div align="center">
<img src=./../../../images/气压检测系统-推送脚本log.png width=90%/>
</div>

## 3、运行结果
&emsp;&emsp;
推送此脚本到M5StackCore2之后并运行，串口会周期性的打印如下日志，包括温度、气压和海拔高度：

```log
>>> execfile("/data/pyamp/main.py")
E (5848) disp_lvgl: app_init
E (6048) disp_lvgl: register touch driver
Testing bmp280
26.47 摄氏度
1009.46 hPa
32.82712 m
26.46 摄氏度
1009.47 hPa
32.7436 m
26.47 摄氏度
1009.48 hPa
32.67326 m
26.47 摄氏度
1009.43 hPa
33.03867 m
```

&emsp;&emsp;
M5StackCore2界面显示的结果如下图所示：

<div align="center">
<img src=./../../../images/气压检测系统_气压测试结果.png width=15%/>
</div>
<br>

## 4、设备端代码说明

### 4.1、UI显示界面开发
&emsp;&emsp;
在做一个气压检测系统之前，首先需要做出气压表盘的UI显示界面，用于显示气压数据。HaaS Python提供了[UI Lite轻应用显示框架](../../../haas_extended_api/ui_lite/UI_Lite_widgets.md)，支持丰富的UI控件，开发者可以自行选择需要的控件进行UI界面开发，实现自己的业务逻辑。本案例选用了[UI Lite轻应用显示框架](../../../haas_extended_api/ui_lite/UI_Lite_widgets.md)中的"meter - 表盘"组件，作为气压检测系统的UI显示界面。

<div align="center">
<img src=./../../../images/气压检测系统_表盘.png width=15%/>
</div>
<br>

&emsp;&emsp;
以下是表盘界面对应的python代码：

```python
#
# A simple meter
#
meter = lv.meter(lv.scr_act())
meter.center()
meter.set_size(200, 200)

# Add a scale first
scale = meter.add_scale()
meter.set_scale_ticks(scale, 81, 2, 10, lv.palette_main(lv.PALETTE.GREY))
meter.set_scale_major_ticks(scale, 10, 4, 15, lv.color_black(), 10)
meter.set_scale_range(scale, 300, 1100, 270, 135)

indic = lv.meter_indicator_t()

# Add arc color for 300-400
indic = meter.add_arc(scale, 3, lv.color_hex(0x0000CD), 0)
meter.set_indicator_start_value(indic, 300)
meter.set_indicator_end_value(indic, 400)
# Add the tick lines color for 300-400
indic = meter.add_scale_lines(scale, lv.color_hex(0x0000CD), lv.color_hex(0x0000CD), False, 0)
meter.set_indicator_start_value(indic, 300)
meter.set_indicator_end_value(indic, 400)

# Add arc color for 400-500
indic = meter.add_arc(scale, 3, lv.color_hex(0x0000FF), 0)
meter.set_indicator_start_value(indic, 400)
meter.set_indicator_end_value(indic, 500)
# Add the tick lines color for 400-500
indic = meter.add_scale_lines(scale, lv.color_hex(0x0000FF), lv.color_hex(0x0000FF), False, 0)
meter.set_indicator_start_value(indic, 400)
meter.set_indicator_end_value(indic, 500)

# Add arc color for 500-600
indic = meter.add_arc(scale, 3, lv.color_hex(0x1E90FF), 0)
meter.set_indicator_start_value(indic, 500)
meter.set_indicator_end_value(indic, 600)
# Add the tick lines color for 500-600
indic = meter.add_scale_lines(scale, lv.color_hex(0x1E90FF), lv.color_hex(0x1E90FF), False, 0)
meter.set_indicator_start_value(indic, 500)
meter.set_indicator_end_value(indic, 600)

# Add arc color for 600-700
indic = meter.add_arc(scale, 3, lv.color_hex(0x00BFFF), 0)
meter.set_indicator_start_value(indic, 600)
meter.set_indicator_end_value(indic, 700)
# Add the tick lines color for 600-700
indic = meter.add_scale_lines(scale, lv.color_hex(0x00BFFF), lv.color_hex(0x00BFFF), False, 0)
meter.set_indicator_start_value(indic, 600)
meter.set_indicator_end_value(indic, 700)

# Add arc color for 700-800
indic = meter.add_arc(scale, 3, lv.color_hex(0x7FFFAA), 0)
meter.set_indicator_start_value(indic, 700)
meter.set_indicator_end_value(indic, 800)
# Add the tick lines color for 700-800
indic = meter.add_scale_lines(scale, lv.color_hex(0x7FFFAA), lv.color_hex(0x7FFFAA), False, 0)
meter.set_indicator_start_value(indic, 700)
meter.set_indicator_end_value(indic, 800)

# Add arc color for 800-900
indic = meter.add_arc(scale, 3, lv.color_hex(0x00FA9A), 0)
meter.set_indicator_start_value(indic, 800)
meter.set_indicator_end_value(indic, 900)
# Add the tick lines color for 800-900
indic = meter.add_scale_lines(scale, lv.color_hex(0x00FA9A), lv.color_hex(0x00FA9A), False, 0)
meter.set_indicator_start_value(indic, 800)
meter.set_indicator_end_value(indic, 900)

# Add arc color for 900-1000
indic = meter.add_arc(scale, 3, lv.color_hex(0x3CB371), 0)
meter.set_indicator_start_value(indic, 900)
meter.set_indicator_end_value(indic, 1000)
# Add the tick lines color for 900-1000
indic = meter.add_scale_lines(scale, lv.color_hex(0x3CB371), lv.color_hex(0x3CB371), False, 0)
meter.set_indicator_start_value(indic, 900)
meter.set_indicator_end_value(indic, 1000)

# Add arc color for 1000-1100
indic = meter.add_arc(scale, 3, lv.color_hex(0x2E8B57), 0)
meter.set_indicator_start_value(indic, 1000)
meter.set_indicator_end_value(indic, 1100)
# Add the tick lines color for 1000-1100
indic = meter.add_scale_lines(scale, lv.color_hex(0x2E8B57), lv.color_hex(0x2E8B57), False, 0)
meter.set_indicator_start_value(indic, 1000)
meter.set_indicator_end_value(indic, 1100)

# Add a needle line indicator
indic = meter.add_needle_line(scale, 4, lv.palette_main(lv.PALETTE.GREY), -10)

# Set pressure & height value indicator style
style = lv.style_t()
style.init()
style.set_pad_all(2)
style.set_x(120)
style.set_y(185)
style.set_text_color(lv.palette_main(lv.PALETTE.RED))
# Add a pressure & height value indicator
spans = lv.spangroup(lv.scr_act())
spans.set_width(300)
spans.set_height(300)
spans.add_style(style, 0)
spans.set_overflow(lv.SPAN_OVERFLOW.CLIP)
spans.set_mode(lv.SPAN_MODE.EXPAND)
span = spans.new_span()
```

### 4.2、获取传感器数据并传入表盘
&emsp;&emsp;
定义set_value()接口，对表盘的气压值进行设置，以传入传感器数据实时显示当前气压值：
```python
# indic为表盘指针，v为气压值
def set_value(indic, v):
    meter.set_indicator_value(indic, v)
```

&emsp;&emsp;
定义convertPressure2Altitude()接口，对表盘的气压值进行转换，得到海拔高度值：
```python
# p为气压，t为温度
def convertPressure2Altitude(p, t):
    height = ((1-((p/101325)**(1/5.255)))*(t+273.15)/0.0065)
    return height
```

&emsp;&emsp;
通过set_value()接口，将获取到的气压值和海拔高度值设置到表盘内：
```python
if __name__ == "__main__":
    try:
        print("Testing bmp280")
        i2cDev = I2C()
        i2cDev.open("bmp280")
        baroDev = BMP280(i2cDev)
        while 1:
            # "pressure" - 代表气压传感器测量到的气压值
            pressure = baroDev.getPressure()
            # "temprature" - 代表气压传感器测量到的温度值
            temprature = baroDev.getTemperature()
            print(str(temprature) + ' 摄氏度')
            # 设置气压值
            set_value(indic, int(int(pressure) / 100))
            # 设置海拔高度值
            span.set_text(str(int(pressure) / 100) + ' hPa, ' +
                          str(convertPressure2Altitude(pressure, temprature)) + ' m')
            print(str(int(pressure) / 100) + ' hPa')
            print(str(convertPressure2Altitude(pressure, temprature)) + ' m')
            # 每2秒钟上报一次
            sleep_ms(2000)
        i2cDev.close()
        del baroDev
        print("Test bmp280 done")
    except OSError:
        print("make sure bmp280.py is in libs folder")
```
