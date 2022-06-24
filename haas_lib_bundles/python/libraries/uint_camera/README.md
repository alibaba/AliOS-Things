# 摄像头模块 - unit_camera

## 一、产品简介
摄像头模块是一种包含图像传感器的集成模块，模块上有主控芯片和Flash等器件，它一般是通过UART接口或者Wi-Fi无线的方式将图像传输给其他主控芯片做摄像头应用处理。开发者可以在不支持专业图像传感器接口的芯片上使用摄像头模块。目前unit_camera摄像头模块只支持M5StackCore2开发板。<br>

<div align="center">
<img src=./../../docs/images/摄像头模块.png  width=40%/>
</div>

引脚定义
* GND: 接地<br>
* VCC: 5V电源<br>
* RX: 串口接收<br>
* TX: 串口发送<br>
<br>
<div align="center">
<img src=./../../docs/images/unit_camera接口图.png  width=30%/>
</div>
<br>
## 二、技术参数<br>
* 供电电压：5.0V<br>
* 图像传感器：OV2640<br>
* 接口：UART串口<br>
* 自带Flash：4M<br>
* 自带RAM: 520KB<br>
* 最大分辨率：200万像素<br>
* 默认图片传输速率：12fps<br>
* OV2640支持输出格式：YUV(422/420)/YCbCr422, RGB565/555, 8位压缩数据<br>
* 产品尺寸：45mm*20mm*12mm<br>
<br>

## 三、软件接口

在M5StackCore2 HaaS Python标准固件中默认已包含unit_camera摄像头模块的驱动代码。
<br>

### init - 初始化摄像头模块
* 函数原型：
> ucamera.init(type, rx=None, Tx=None)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| type | string | 是 | 如果使用的是WiFi摄像头，type='wifi'，如果是UART摄像头，type='uart' |
| rx | int | 否 | 仅UART摄像头时使用，表示rx使用的pin，默认rx=33 |
| tx | int | 否 | 仅UART摄像头时使用，表示tx使用的pin，默认tx=32 |

* 返回值：
成功返回0，失败返回-1

* 示例代码：

```python
import ucamera

# 初始化WiFi摄像头
ret = ucamera.init('wifi')
```


```python
import ucamera

# 初始化UART摄像头
ret = ucamera.init('uart', 33, 32)
```

- 输出：

```
0
```

### capture - 获取摄像头画面

-  函数功能：
获取摄像头画面

-  注意事项：
获取的图像格式为JPEG

-  函数原型：
> ucamera.capture()

- 参数说明：
无

-  返回值：
正常返回bytes格式的jpeg图像数据，失败返回None

-  示例：

```python
import ucamera

ucamera.init('uart', 33, 32)

frame = ucamera.capture()
if (frame == None):
	print('frame is None')
else:
	print('get frame')
```

- 输出

```
get frame
```

### save - 保存摄像头画面

-  函数功能：
保存摄像头画面到文件系统中

-  函数原型：
> ucamera.save(frame, path)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | bytes | 是 | 待保存的jpeg图像数据 |
| path | string | 是 | 保存在文件系统中的路径 |

-  返回值：
正常返回0，失败返回-1

-  示例：

```python
import ucamera

ucamera.init('uart', 33, 32)

frame = ucamera.capture()
if (frame == None):
	print('frame is None')
else:
	print('get frame')
    ret = ucamera.save(frame, '/test.jpg')
    if ret == 0:
        print('save jpg file success')
    else:
        print('save jpg file fail')
```

- 输出：


```
get frame
save jpg file success
```
### setProp - 设置摄像头属性


-  函数功能：
设置摄像头属性

-  注意事项：
目前主要针对Uart摄像头进行设置，设置后重新启动生效

-  函数原型：
> ucamera.setProp(cmd, value)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| cmd | int | 是 | 设置摄像头属性命令ID <br> UCAMERA_CMD_SET_FRAME_SIZE： <br> 设置Uart摄像头分辨率 <br> UCAMERA_CMD_SET_MODE： <br> 设置Uart摄像头工作模式<br> 设置Uart摄像头上的LED亮度 |
| value | variable | 是 | 设置摄像头对应命令的值<br> UCAMERA_CMD_SET_FRAME_SIZE对应的值：<br> SIZE_160X120<br> SIZE_240X240<br>SIZE_320X240<br> SIZE_640X480<br> SIZE_800X600<br> UCAMERA_CMD_SET_LED_BRIGHTNESS对应的值：<br> 0~1024|

-  返回值：
正常返回0，失败返回-1

-  示例：

```python
import ucamera

ucamera.init('uart', 33, 32)

# 设置Uart摄像头分辨率
ret = ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)
if ret == 0:
    print('设置摄像头分辨率为320x240')
# 设置摄像头上LED灯亮度
ret = ucamera.setProp(ucamera.SET_LED_BRIGHTNESS, 1000)
if ret == 0:
    print('设置摄像头LED灯亮度为1000')
```

- 输出：

```
设置摄像头分辨率为320x240
设置摄像头LED灯亮度为1000
```

## 四、接口案例
此使用实例通过摄像头模块抓取一帧图像，并将图像显示在lcd屏幕上。
无需在board.json中定义了硬件接口。

* 代码：
```python
import ucamera
import display
"""
获取摄像头画面，并在显示屏上显示
"""
# 创建lcd display对象
disp = display.TFT()
disp.clear()

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)
# 采集摄像头画面
frame = ucamera.capture()
if frame != None:
    # 显示图片
    disp.image(0, 20, frame, 0)
```
<br>

## 五、工作原理
unit camera摄像头模块使用ESP32-WROOM32E作为核心处理芯片，搭载一颗OV2640的图像传感器，并且模块内有4M Flash，支持的最大分辨率为200万像素。

摄像头传感器将三原色的光信号进行转换，输出的是彩色图像，一般情况下，可以表示16777216种颜色（256×256×256=16777216），这种情况称为全彩色图像。OV2640的图像输出信号为数字信号。其内部集成了A/D转换电路，可直接将模拟量的图像信号转化为数字信号。其具有抗干扰能力强，输出图像格式多样。

想要了解更多的请参考参考文献！

<br>

## 参考文献及购买链接
[1] [机器视觉技术介绍](https://gitee.com/haasedu/haasedu/blob/release_2.0/%E6%8B%93%E5%B1%95%E7%9F%A5%E8%AF%86/HaaS%E8%A7%86%E8%A7%89%E4%BA%BA%E5%B7%A5%E6%99%BA%E8%83%BD/%E6%9C%BA%E5%99%A8%E8%A7%86%E8%A7%89%E6%8A%80%E6%9C%AF/%E6%9C%BA%E5%99%A8%E8%A7%86%E8%A7%89%E6%8A%80%E6%9C%AF%E4%BB%8B%E7%BB%8D.md)<br>
[2] [购买链接](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244)<br>
