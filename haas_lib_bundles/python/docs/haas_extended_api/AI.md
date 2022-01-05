# AI  - 人工智能

- 模块功能：  
本模块集成阿里云视觉智能开放平台中的视觉AI识别功能。

-  注意事项：  
在使用前，请确保已经开通阿里云账号以及OSS对象存储，以及视觉智能开放平台。

## 常量说明

| 常量 | 常量数值 | 常量说明 |
| --- | --- | --- |
| AI_ENGINE_ALIYUN | 1 | 阿里云AI能力 |
| AI_ENGINE_NATIVE | 2 | 本地AI能力 |


## 模块导入

```python
import uai
```

请使用下面的代码连接wifi:

```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('<AP_name>', '<password>')
utime.sleep_ms(500)
while not sta_if.isconnected():
   utime.sleep_ms(500)
```
直到sta_if.isconnected()返回 True 则表明网络连接成功。

## AI  - 创建AI对象

-  函数原型： 
> ai = uai.AI(type=uai.AI_ENGINE_NATIVE, accessKey=None, accessSecret=None, ossEndpoint=None, ossBucket=None)

- 参数说明：
| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| type | string | 是 | 阿里云或者本地AI |
| accessKey | string | 否 | 如果是阿里云AI，填写阿里云访问密钥 |
| accessSecret | string | 否 | 如果是阿里云AI，填写阿里云访问密匙 |
| ossEndpoint | string | 否 | oss访问节点，如果待识别文件在本地，则填写该字段 |
| ossBucket | string | 否 | 存储Bucket，如果待识别文件在本地，则填写该字段 |

-  返回值：  
AI对象

-  示例： 

```python
import uai

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET)
```

## compareFace - 人脸比对

-  函数功能：  
人脸比对1:1

-  函数原型： 
> compareFace(imagePath, compareFacePath)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| imagePath | string | 是 | 待比对人脸图片路径，可以是url或本地路径 |
| compareFacePath | string | 是 | 待比对原始人脸图片路径，可以是url或本地路径 |

-  返回值：  
比较结果，是以dict形式进行存储

```json
{'confidence': 60, 'x': 40, 'y': 40, 'w': 100, 'h': 120}
```

-  示例1： 

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET)

image = '/face1.jpg'
compareImage = 'face2.jpg'
resp = ai.compareFace(image, compareImage)

print(resp)
```

-  示例2：

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET)

image = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/facebody/CompareFace/CompareFace-left1.png'
compareImageUrl = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/facebody/CompareFace/CompareFace-left2.png'

resp = ai.compareFace(image, compareImageUrl)

print(resp)
```

## detectPedestrian - 人体检测

-  函数功能：  
人体检测

-  函数原型：
> detectPedestrian(imagePath)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| imagePath | string | 是 | 待比对人脸图片路径，可以是url或本地路径 |

-  返回值：  
比较结果，是以dict形式进行存储

```json
{'type': 'person', 'score': 70, 'x': 40, 'y': 40, 'w': 100, 'h': 120}
```

-  示例1： 

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET)

image = '/body.jpg'
resp = ai.detectPedestrian(image)

print(resp)

image = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/facebody/DetectPedestrian/DetectPedestrian1.jpg'
resp = ai.detectPedestrian(image)

print(resp)
```

## detectFruits - 水果识别


-  函数功能：  
水果识别

-  函数原型： 
> detectFruits(imagePath)

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| imagePath | string | 是 | 待识别水果图片路径，本地或url路径 |

-  返回值：  
比较结果，是以dict形式进行存储

```json
{'name': '橙子','score': 70, 'x': 40, 'y': 40, 'w': 100, 'h': 120}
```

-  示例1：

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET)

image = '/fruits.jpg'
resp = ai.detectFruits(image)

print(resp)

image = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/imagerecog/DetectFruits/DetectFruits1.jpg'
resp = ai.detectFruits(image)

print(resp)
```

## recognizeLicensePlate - 车牌识别

-  函数功能：  
车牌识别

-  函数原型：   
> detectFruits(imagePath)

- 参数说明：
| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| imagePath | string | 是 | 待识别车牌图片路径，本地或url路径 |

-  返回值：  
比较结果，是以dict形式进行存储

```json
{'name': '橙子','score': 70, 'x': 40, 'y': 40, 'w': 100, 'h': 120}
```

-  示例1： 

```python
import oss

# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'

# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-Bucket"

ai = uai.AI(uai.AI_ENGINE_ALIYUN, ACCESS_KEY, ACCESS_SECRET, OSS_ENDPOINT, OSS_BUCKET)

image = '/carlicense.jpg'
resp = ai.recognizeLicensePlate(image)

print(resp)

image = 'http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/ocr/RecognizeLicensePlate/cpsb1.jpg'
resp = ai.recognizeLicensePlate(image)

print(resp)
```
