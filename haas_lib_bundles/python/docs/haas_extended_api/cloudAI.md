# cloudAI  - 云端AI

- 模块功能：
本模块为HaaS Python标准中的云端AI模块, 即通过达摩院视觉智能平台上的服务完成AI的功能服务。截止目前cloudAI模块对外透出的API可以完成如下功能：
* 人体检测
* 水果识别
* 车牌识别
* 名片识别
* 手势识别
* 车型检测
* 车辆拥塞检测
* 垃圾分类
* 物体检测
* 动作行为识别

<br>
-  注意事项：
使用此模块时，需要搭配HaaS增值服务这个云端平台一起。


## 模块导入

```python
from cloudAI import *
```

## CloudAI  - 创建云端AI对象

-  函数原型:
> engine = CloudAI(key_info, recognize_cb)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| key_info | dictionary | 是 | 设备信息，例如key_info = { <br>'region' : 'cn-shanghai' ,<br>'productKey': productKey ,<br>'deviceName': deviceName ,<br>'deviceSecret': deviceSecret ,<br>'keepaliveSec': 60} |
| recognize_cb | 函数对象 | 是 | 用于接收AI结果的回调函数 |

-  返回值：
AI对象

<br>

其中recognize_cb函数原型如下：
> recognize_cb(commandReply, result)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| commandReply | string | 是 | 表示结果返回类型，取值为'handGestureReply','recognizeActionReply','ocrCarNoReply','DetectPedestrianReply','detectFruitsReply','recognizeBusinessCardReply','classifyingRubbishReply','detectObjectReply','recognizeVehicleReply', 'recognizeLogoReply'  |
| result | string | 是 | 表示返回结果，详细请看具体案例实现 |

-  返回值：
无

-  示例:

```python
from cloudAI import *

# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

# 识别回调函数
def recognize_cb(commandReply, result) :
    print('接收识别的结果')

engine = CloudAI(key_info, recognize_cb)
```

## detectPedestrian(frame)   - 人体检测

-  函数功能：
检测图像中的人体，并输出图像中人体所在坐标，可同时识别图片中的多个人体。
检测结果将通过回调函数异步返回。

-  函数原型：
> detectPedestrian(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1:

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
detected = False

key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': productKey ,
    'deviceName': deviceName ,
    'deviceSecret': deviceSecret ,
    'keepaliveSec': 60
}

# 识别回调函数
def recognize_cb(commandReply, result) :
    global detected
    detected = False

    if commandReply == 'DetectPedestrianReply' :
        detected = result
    else :
        print('unknown command reply')

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.detectPedestrian(frame)

```

## detectFruits - 水果识别


-  函数功能：
检测出图像中是否含有水果并识别出水果的种类，目前支持60种水果和16种坚果。具体包括：八月瓜、百香果、菠萝、菠萝蜜、草莓、橙子、脆瓜、甘蔗、桂圆、哈蜜瓜、海底椰、海红果、红毛丹、火龙果、桔子、蓝莓、梨、荔枝、李子、榴莲、蔓越莓、芒果、梅子、猕猴桃、木瓜、柠檬、牛油果、欧李、枇杷、苹果、葡萄、茄瓜、桑葚、沙果、沙棘果、山楂、山竹、蛇皮果、石榴、柿子、树莓、桃、桃胶、西瓜、西梅、仙人掌果、香瓜、香蕉、橡子、小番茄、杏子、雪莲果、杨梅、洋蒲桃、杨桃、椰子、银杏果、樱桃、柚子、枣、南瓜子、夏威夷果、巴旦木、开心果、杏仁、松子、板栗、核桃、榛子、白果、碧根果、腰果、花生、葡萄干、葵花子、西瓜子。

<br>
&emsp;&emsp;检测结果将通过aliyunIoT的回调函数异步返回。

<br>

-  函数原型:
> detectFruits(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.detectPedestrian(frame)
```

## recognizeLicensePlate - 车牌识别

-  函数功能：
识别出图像中车车牌类型、车牌号码信息。
检测结果将通过回调函数异步返回。

-  函数原型：
> recognizeLicensePlate(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.recognizeLicensePlate(frame)
```

## recognizeGesture - 手势识别

-  函数功能：
静态手势识别可以识别图片中的手势动作，包含：确认、点赞、噤声、手掌等姿势。
检测结果将通过回调函数异步返回。

-  函数原型：
> recognizeGesture(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1:

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.recognizeGesture(frame)
```

## recognizeBussinessCard - 名片识别

-  函数功能：
识别各类名片关键字段内容，关键字段包括：姓名、职位、公司、部门、座机号、手机号、地址、邮箱，共8个关键字段信息。
检测结果将通过回调函数异步返回。

-  函数原型：
> recognizeBussinessCard(frame)

- 参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.recognizeBussinessCard(frame)
```

## recognizeVehicleType - 车型识别

-  函数功能：
识别汽车图片（完整或部件图片）的类型，目前主要有小轿车、多用途汽车、SUV等类别。
检测结果将通过回调函数异步返回。

-  函数原型：
> recognizeVehicleType(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.recognizeVehicleType(frame)
```

## detectVehicleCongestion - 车辆拥堵检测

-  函数功能：
车辆拥堵检测, 根据图片中的车辆，判断是否发生拥堵。
检测结果将通过回调函数异步返回。

-  函数原型：
> detectVehicleCongestion(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1:

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.detectVehicleCongestion(frame)
```

## classifyRubbish - 垃圾分类

-  函数功能：
对输入图片中的垃圾进行分类，并给出具体的物品名称。
检测结果将通过回调函数异步返回。

-  函数原型：
> classifyRubbish(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.classifyRubbish(frame)
```

## detectObject - 物体检测

-  函数功能：
检测图像中的物体。可识别90类物体，例如：人体、椅子、篮球、摩托车、旗帜、斑马等。
检测结果将通过回调函数异步返回。

-  函数原型：
> detectObject(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.detectObject(frame)
```

## recognizeAction - 动作行为识别

-  函数功能：
判断输入授权图片的人体动作行为，当前可以识别的行为类别包括：举手、吃喝、吸烟、打电话、玩手机、趴桌睡觉、跌倒等动作。检测结果将通过回调函数异步返回。

-  函数原型：
> recognizeAction(frame)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| frame | frame对象 | 是 | 待检测的图像帧 |

-  返回值：
调用成功：0； 调用失败：1

-  示例1：

```python
from cloudAI import *
import ucamera      # 摄像头库
from driver import GPIO

# 初始化摄像头
ucamera.init('uart', 33, 32)
ucamera.setProp(ucamera.SET_FRAME_SIZE, ucamera.SIZE_320X240)

engine = CloudAI(key_info, recognize_cb)

frame = ucamera.capture()

engine.recognizeAction(frame)
```
