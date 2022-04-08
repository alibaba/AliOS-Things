# RgbLed - RGB三色LED驱动

## 一、产品简介
&emsp;&emsp;
RGB三色LED是一种简单的三色集成的LED。红、绿、蓝三种颜色可以使用对应的三个引脚进行独立控制。

<div align="center">
<img src=./../../docs/images/RGBLED.jpeg  width=30%/>
</div>

引脚定义

* R：红色灯引脚
* G：绿色灯引脚
* B：蓝色灯引脚
* GND：电源地

## 二、软件接口

## RgbLed(pwmRObj, pwmGObj, pwmBObj) - 创建 RGB LED 驱动对象
* 函数原型：
> rgbLedObj = RgbLed(pwmRObj, pwmGObj, pwmBObj)

* 参数说明：

| 参数    | 类型 | 必选参数？ | 说明                                                            |
| ------- | ---- | :--------: | --------------------------------------------------------------- |
| pwmRObj | PWM  |     是     | 连接红色LED的引脚PWM对象，调用此函数前需确保pwmRObj对象已被open |
| pwmGObj | PWM  |     是     | 连接绿色LED的引脚PWM对象，调用此函数前需确保pwmRObj对象已被open |
| pwmBObj | PWM  |     是     | 连接蓝色LED的引脚PWM对象，调用此函数前需确保pwmRObj对象已被open |

* 返回值：
若RgbLed对象创建成功，返回RgbLed对象；若RgbLed对象创建失败，抛出相关Exception

* 示例代码：

```python
from driver import PWM
from RgbLed import RgbLed

rgbR = PWM()
rgbR.open("rgbR")

rgbG = PWM()
rgbG.open("rgbG")

rgbB = PWM()
rgbB.open("rgbB")

led = RgbLed(rgbR, rgbG, rgbB)

print("RgbLed inited!")
```

* 输出：
```
RgbLed inited!
```

## setColor - 设置 RGB LED 颜色

* 函数功能：
设置 RGB LED 颜色

* 函数原型：
> RgbLed.setColor(color)

* 参数说明：
color 为 RGB888 格式的色值，接受两种形式的入参

其一为长度为3的tuple类型，如
```python
RgbLed.setColor((255, 255, 255))
```
其中 (255, 255, 255) 表示白色；

其二为int类型，如
```python
RgbLed.setColor(0xFFFFFF)
```
其中 0xFFFFFF 表示白色；

* 返回值：
成功无返回；失败抛出Exception

## 三、使用实例

* board.json
```json
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "rgbR": {
            "type": "PWM",
            "port": 5
        },
        "rgbG": {
            "type": "PWM",
            "port": 18
        },
        "rgbB": {
            "type": "PWM",
            "port": 19
        }
    },
    "debugLevel": "ERROR",
    "repl": "disable"
}
```

```python
# Python代码
import utime
from driver import PWM
from rgbLed import RgbLed

if __name__ == '__main__':
    rgbR = PWM()
    rgbR.open("rgbR")

    rgbG = PWM()
    rgbG.open("rgbG")

    rgbB = PWM()
    rgbB.open("rgbB")

    led = RgbLed(rgbR, rgbG, rgbB)

    colorHex = 0x000000

    while True:
        colorHex = (colorHex + 1) if colorHex <= 0xffffff else 0
        led.setColor(colorHex)
        utime.sleep_ms(1)
```

## 参考文献及购买链接
[1] [购买链接](https://detail.tmall.com/item.htm?id=548418198148)
