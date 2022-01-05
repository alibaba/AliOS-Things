# 水果识别系统
&emsp;&emsp;
现在很多农场里边使用摘采机器人识别水果进行水果摘采，盒马超市也使用自动识别称来识别水果种类自动计费。

## 背景知识
&emsp;&emsp;
水果的种类繁多，识别种类越多，需要越多的水果图片数据进行模型训练。本章通过使用串口摄像头采集水果照片上传至云端进行识别。使用云端识别前请开通阿里云视觉智能开放平台水果识别功能。

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
3. 云端平台功能开通
4. 设备端开发
5. 设备运行

## 准备

1. M5Stack-Core2开发板    一套
2. M5Stack ESP32摄像头    一个
3. 连接线                 一根

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack-Core2开发版 | 1 | [M5Stack-Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack-Core2自带 |
| 摄像头 | 1 | [M5Stack ESP32摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/COM_M5Stack硬件连线图.png width=90%/>
</div>
<br>

## 云端平台功能开通
在本案例中涉及到两个云端平台功能：
1. 使用[对象存储OSS](https://help.aliyun.com/document_detail/52830.html?spm=5176.8465980.0.dexternal.4e7014509ELP9t)将图像上传到OSS中创建好的Bucket。
2. 使用[视觉智能开放平台](https://vision.aliyun.com/experience/detail?spm=a211p3.14020179.J_7524944390.8.66cd4850LUzVoZ&tagName=imagerecog&children=DetectFruits)免费开通水果检测识别功能。

&emsp;&emsp;
因此，整个步骤分为：
1. 注册阿里云账号获取AccessKey和Secret
2. 使用对象存储OSS创建Bucket并获取OSS Endpoint
3. 开通阿里云视觉智能开放平台水果检测识别功能
4. 设备端配置云端账号信息
5. 推送脚本到M5Stack Core2并运行水果检测识别

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。

<br>

1. **获取AccessKey和Secret**

&emsp;&emsp;
登录[Access Key管理页面](https://ram.console.aliyun.com/manage/ak)，创建并获取**AccessKey ID**和**AccessKey Secret**，使用子用户缩小权限范围，账号更加安全。

<br>

2. **创建子用户**
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1630931942553-f0649f29-162e-4d9d-8b1f-8d4c1d2a834a.png#clientId=u91568700-09eb-4&from=paste&height=488&id=i9IXd&margin=%5Bobject%20Object%5D&name=image.png&originHeight=976&originWidth=2874&originalType=binary&ratio=1&size=660204&status=done&style=none&taskId=uccb3ec0f-4d91-435c-b8c5-6f1cbeb04af&width=1437)
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1630932751015-c8227004-1aaf-470e-941a-e356dafac4fe.png#clientId=u91568700-09eb-4&from=paste&height=305&id=Ubs3m&margin=%5Bobject%20Object%5D&name=image.png&originHeight=610&originWidth=2878&originalType=binary&ratio=1&size=311231&status=done&style=none&taskId=u31d28fe8-6903-42b9-ba97-af687633334&width=1439)
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1630932878834-473b5812-5d0d-4b3c-856c-080c145c44e3.png#clientId=u91568700-09eb-4&from=paste&height=375&id=ixY5r&margin=%5Bobject%20Object%5D&name=image.png&originHeight=750&originWidth=2878&originalType=binary&ratio=1&size=265601&status=done&style=none&taskId=u1ccd53fb-dd44-4d4f-9a1c-e2e949d8905&width=1439)

<br>

3. **获取Key和Secret**
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1630933021351-6e87582c-221e-45f9-b880-1b460dfff392.png#clientId=u91568700-09eb-4&from=paste&height=331&id=MVsNC&margin=%5Bobject%20Object%5D&name=image.png&originHeight=662&originWidth=2878&originalType=binary&ratio=1&size=319964&status=done&style=none&taskId=u6928ce75-4a3c-48ae-95c9-2722c2e6273&width=1439)
通过下载csv文件，或者复制方式保存AccessKey ID和Secret，在设备端需要填入这两个信息。

<br>

## 对象存储OSS应用
1. **创建OSS Bucket**

&emsp;&emsp;
登陆OSS控制台[https://oss.console.aliyun.com/](https://oss.console.aliyun.com/overview)创建Bucket，创建时地域**一定要选择上海**。
<br />
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1611044636741-8cdf7526-5df4-4c07-9b7d-a607eb4a6f16.png#align=left&display=inline&height=298&margin=%5Bobject%20Object%5D&name=image.png&originHeight=596&originWidth=2552&size=333576&status=done&style=none&width=1276)
<br />
地域请选择上海：
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1611044802058-75241eeb-5ce8-4eec-ada2-ebfc3108ce91.png#align=left&display=inline&height=652&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1304&originWidth=2550&size=826075&status=done&style=none&width=1275)
<br />
创建完成后可以获得Endpoint及Bucket域名信息：
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1611044974987-ad66470c-c304-4b75-848d-77ad18cbf4b2.png#align=left&display=inline&height=642&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1284&originWidth=2554&size=565310&status=done&style=none&width=1277)<br />在Bucket创建好后，从上图我们可以看到：<br />**Endpoint：oss**-cn-shanghai.aliyuncs.com<br />**BucketName**：就是我们创建Bucket取的名字oss-ai-dev。

> 权限设置

&emsp;&emsp;
Bucket ACL设置为公共读，否则可能会出现访问不了的问题：<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1611817137461-d4ff15a8-6d28-4f11-aa81-ce8fcabcbb45.png#align=left&display=inline&height=311&margin=%5Bobject%20Object%5D&name=image.png&originHeight=622&originWidth=2556&size=249226&status=done&style=none&width=1278)

<br>

2. **获取OSS Endpoint**
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/106917/1611818799287-36f1eb93-3790-4c76-a4bf-cdf3c5e3992f.png#align=left&display=inline&height=1302&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1302&originWidth=2556&size=622552&status=done&style=none&width=2556)

<br>

## 开通阿里云视觉智能开放平台水果检测识别功能
&emsp;&emsp;
目前阿里云视觉智能开放平台的水果检测识别功能可以检测图像中的所有水果，并返回水果的名称和置信度。

> 功能描述

&emsp;&emsp;
水果识别可以识别60种常见的水果和16种坚果。比如：八月瓜、百香果、菠萝、菠萝蜜、草莓、橙子、脆瓜、甘蔗、桂圆、哈蜜瓜、海底椰、海红果、红毛丹、火龙果、桔子、蓝莓、梨、荔枝、李子、榴莲、蔓越莓、芒果、梅子、猕猴桃、木瓜、柠檬、牛油果、欧李、枇杷、苹果、葡萄、茄瓜、桑葚、沙果、沙棘果、山楂、山竹、蛇皮果、石榴、柿子、树莓、桃、桃胶、西瓜、西梅、仙人掌果、香瓜、香蕉、橡子、小番茄、杏子、雪莲果、杨梅、洋蒲桃、杨桃、椰子、银杏果、樱桃、柚子、枣、南瓜子、夏威夷果、巴旦木、开心果、杏仁、松子、板栗、核桃、榛子、白果、碧根果、腰果、花生、葡萄干、葵花子、西瓜子等。

> 输入限制
* 图片格式：JPEG、JPG、PNG、TIFF
* 图片分辨率：分辨率最小要求为100像素
* 图片URL地址中不能包含中文字符

### 功能开通
&emsp;&emsp;
登陆[视觉智能开放平台](https://vision.aliyun.com/experience/detail?spm=a211p3.14020179.J_7524944390.8.66cd4850LUzVoZ&tagName=imagerecog&children=DetectFruits)免费开通水果检测识别功能。
![](../../../图片/3_水果识别系统_免费开通1.png)

&emsp;&emsp;
选择地域为上海，勾选同意协议，每个账号2QPS免费调用。
![](../../../图片/3_水果识别系统_免费开通2.png)

### 功能体验
&emsp;&emsp;
开发者在实际在设备端使用前，可以尝试上传水果照片在产品体验页面进行体验。
![](../../../图片/3_水果识别系统_体验.png)


## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“fruits_recognization”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/3_2_水果识别_工程创建.png width=100%/>
</div>


&emsp;&emsp;
将[水果识别代码](./code/)文件下的所有脚本进行复制到“fruits_recognization”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改pedestrian_detection工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

2. **修改设备端访问Key和Secret**

&emsp;&emsp;
按照[获取Key和Secret]获取ACCESS_KEY和ACCESS_SECRET，填入main.py中：
```python
# 阿里云访问账号
ACCESS_KEY = 'Your-Access-Key'
ACCESS_SECRET = 'Your-Access-Secret'
```

3. **设备端配置云端账号信息**

&emsp;&emsp;
配置云端信息账号信息以及对象存储OSS信息：

&emsp;&emsp;
按照[创建Bucket]获取OSS_BUCKET，填入main.py中：
```python
# OSS对象存储配置
OSS_ENDPOINT = "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET = "Your-OSS-Bucket"
```

## 运行结果
### 本地查看

&emsp;&emsp;
在网络连接成功后，屏幕上将打印出IP地址和时间戳，时间显示当前北京时间，如果在网络连接成功后，显示的时间是(2000, 1, 1, 0, 1, 47, 5, 1)表示NTP同步不成功，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
推送此脚本到M5Stack之后，串口会周期性的打印如下日志，并且识别到水果后在屏幕上显示红色XXXX Detected的字样。
```log
I (71364) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
wifi is connecting...
wifi is connecting...
wifi is connected
IP: 192.168.43.168
NTP start
NTP done
[makeRequest]response_code: 200
content: http://oss-ai-dev.oss-cn-shanghai.aliyuncs.com/oss%2Ftest.jpg

imageseg describeInstances returned:
error code: InvalidImage.Content
requestId: 

None
[makeRequest]response_code: 200
content: http://oss-ai-dev.oss-cn-shanghai.aliyuncs.com/oss%2Ftest.jpg

imageseg describeInstances returned:
error code: InvalidImage.Content
requestId: 

None
[makeRequest]response_code: 200
content: http://oss-ai-dev.oss-cn-shanghai.aliyuncs.com/oss%2Ftest.jpg

imageseg describeInstances returned:
error code: 
requestId: F75D361A-3F9D-5D4D-B3B9-D75B4A3F5988

0score: 0.79189
0name: 橙子
box.xmin: 115.98448944091797
box.ymin: 1.16615891456604
box.xmax: 315.54144287109375
box.ymax: 192.87744140625
[{'name': '\u6a59\u5b50', 'h': 200.0, 'y': 115.0, 'score': 0.7918897, 'x': 115.0, 'w': 1.0}]
