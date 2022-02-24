# ucamera - 摄像头功能
- 模块功能：  
ucamera组件是摄像头图像数据处理中间框架，目前支持wifi摄像头以及Uart摄像头。

-  注意事项：
   - WiFi摄像头支持ESP32 EYE/CAM，[购买链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.1.150d6a6ftZ6h4K&id=611790371635&ns=1&abbucket=3)。
   - Uart摄像头支持M5Stack的Unit CAM，[购买连接](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244)，搭配M5Stack Core2开发板使用。

## init - 初始化摄像头

-  函数功能：  
初始化摄像头

-  函数原型： 
> ucamera.init(type, rx=None, Tx=None)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| type | string | 是 | 如果使用的是WiFi摄像头，type='wifi'，如果是Uart摄像头，type='uart' |
| rx | int | 否 | 仅Uart摄像头时使用，表示rx使用的pin，默认rx=33 |
| tx | int | 否 | 仅Uart摄像头时使用，表示tx使用的pin，默认tx=32 |

-  返回值：  
成功返回0，失败返回-1

-  示例： 

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

## capture - 获取摄像头画面

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

## save - 保存摄像头画面

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
## setProp - 设置摄像头属性


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

## 使用实例

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




