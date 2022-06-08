# 智慧路灯系统
&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/4_智慧路灯_步骤概述.jpg width=60%/>
</div>

## 1、简介
&emsp;&emsp;
随着城市化不断扩大，城市的灯光秀越来越多，让我们居住的城市生活变得五彩缤纷，灯火辉煌，让城市显得越来越繁华。但是，我们也会经常发现有些路边的灯，在天很黑了不会自动打开，或者到清晨，天很亮了，马路边的灯也不会自动熄灭，只会在固定时间统一开关控制，这样既不人性化也不环保。于是现在很多地方的路灯会根据周围环境亮度自动开启或关闭路边灯光，那这样的路灯就非常人性化，同时也非常环保，这才是真正的城市智慧路灯。

### 1.1、背景知识
&emsp;&emsp;
本章课程打造的智慧路灯控制系统是当检测到周围环境亮度变暗的时候，系统会自动打开灯，当周围环境亮度变亮的时候，系统就会自动关闭灯。本智慧路灯控制系统默认设置一个亮度阈值，当亮度值超过阈值，关灯，当亮度值低于阈值，开灯，当然，这个阈值可以根据每个人对光的亮度感觉不同而调整。

### 1.2、准备
&emsp;&emsp;
本案例之需要HaaS EDU K1开发一套。实验过程中会使用HaaS EDU K1内置的AP3216C传感器和红色LED。

## 2、物联网平台开发
### 2.1、开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

### 2.2、云端创建产品
1. **创建产品**

&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/2_3_创建产品.jpg
 width=40%/>
</div>

&emsp;&emsp;
这里创建了一个名称为“智慧路灯”的产品。
<div align="center">
<img src=./../../../images/5_1_新建智慧路灯产品.jpg
 width=40%/>
</div>

&emsp;&emsp;
点击确认，就可以在产品列表中出现智慧路灯。

<div align="center">
<img src=./../../../images/5_1_完成产品创建.jpg
 width=40%/>
</div>

&emsp;&emsp;
查看产品详情。
<div align="center">
<img src=./../../../images/5_1_产品功能定义.jpg
 width=40%/>
</div>

&emsp;&emsp;
编辑草稿，选择自定义功能。
<div align="center">
<img src=./../../../images/5_1_自定义产品物模型定义.jpg
 width=40%/>
</div>

&emsp;&emsp;
如下图设置对应产品的物模型，分别设置光强度电压值和灯开关两个模型。
<div align="center">
<img src=./../../../images/5_1_2产品物模型.jpg
 width=40%/>
 <img src=./../../../images/5_1_3产品物模型onoff.jpg
 width=40%/>
</div>

&emsp;&emsp;
在产品详情中增加产品的功能，可以看到刚才设置的光强度电压值和灯开关两个功能属性，点击发布上线。
<div align="center">
<img src=./../../../images/5_1_3完成物模型设置.jpg
 width=80%/>
</div>

&emsp;&emsp;
这样，整个智慧路灯产品在物联网平台也创建好了。

2. **设备管理**

&emsp;&emsp;
选中最左边栏设备管理中的设备，然后添加设备，
<div align="center">
<img src=./../../../images/2_3_添加设备.jpg
 width=40%/>
</div>

&emsp;&emsp;
选择的产品为刚刚创建的智慧路灯，设备名设置为smartlight,点击确认，
<div align="center">
<img src=./../../../images/5_1_智慧路灯设备.jpg
 width=40%/>
</div>

&emsp;&emsp;
进入设备详情，获取设备三元组信息，
<div align="center">
<img src=./../../../images/5_1_设备三元组信息.jpg
 width=40%/>
</div>

&emsp;&emsp;
将设备三元组信息一键拷贝出来，在设备开发中会用到。
<div align="center">
<img src=./../../../images/2_3_设备证书.jpg
 width=40%/>
</div>

3. **设备引擎**

&emsp;&emsp;
如何通过光感自适应控制灯的亮灭呢？可以在物联网平台上通过引擎规则实现，本课程使用的光强度传感器AP3216C返回值越低，说明周围的环境光强度越低；反之，返回值越高，说明周围的环境光强度越高。
<div align="center">
<img src=./../../../images/5_1_haaseduk1_规则引擎1.jpg
 width=70%/>
</div>

&emsp;&emsp;
当光强度大于100，说明周围的环境变亮，执行关灯的指令。<br />

<div align="center">
<img src=./../../../images/5_1_haaseduk1_规则引擎2.jpg
 width=70%/>
</div>

&emsp;&emsp;
当光强度电压值小于100，说明周围的环境变暗，执行开灯的指令。<br />
<div align="center">
<img src=./../../../images/5_1_haaseduk1_规则引擎3.jpg
 width=70%/>
</div>

&emsp;&emsp;
规则引擎创建完成以后，启动引擎即可。<br />
<div align="center">
<img src=./../../../images/5_1_haaseduk1_规则引擎4.jpg
 width=70%/>
</div>

## 3、设备端开发
### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保HaaS EDU K1开发环境已经搭建完毕。详情请参考“[HaaS EDU K1快速开始](https://hli.aliyuncs.com/haas-static/haasapi/index.html#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup)”的说明。

### 3.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“智能路灯系统”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

> Python脚本的详细说明请参考脚本内嵌的文字注释

&emsp;&emsp;
之后对代码进行如下修改。

1. **修改路由器名称及密码**

&emsp;&emsp;
修改main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# wifi连接的的wifiSsid和wifiPassword定义
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

1. **修改设备端三元组**

&emsp;&emsp;
修改main.py中productKey、deviceName和deviceSecret的值为上面物联网平台创建的物联网设备的三元组信息。

```python
# 物联网平台相关的key和serect定义
productKey     = "产品密钥"
deviceName     = "设备名"
deviceSecret   = "设备密钥"
```

3. **修改设备端上报光强度和下发开关标识符**

&emsp;&emsp;
main.py中下面的代码实现的是上传光强度值到云端的功能，以及云端下发对应的开关命令。其中Brightness便是光强度值上报时所用的标识符，onoff为云端下发的开关标识符。

```python
def report_light_data(l_data):
    # 生成上报到物联网平台的属性值字串
    prop = ujson.dumps({
        'Brightness': l_data,
    })
    upload_data = {'params': prop}
    # 上传光强度信息到物联网平台
    device.postProps(upload_data)
```
&emsp;&emsp;
下面是采集光照数据的时候，采样值是跨阈值100就需要进行上报。
```python
# 采集打印光照强度电压值
def show_lightness():
    global lightness,i_light,last_light,system_reset
    i_light = int(lightness)
    if system_reset == 1:
        print("system first come")
        system_reset = 0    # 系统第一次启动
        report_light_data(i_light)
    elif (i_light >= 1000 and last_light < 1000) or (i_light < 1000 and last_light >= 1000):
        report_light_data(i_light)
        print("light has change")
    else:
        print('no need report')
    if i_light < 10:
        T_str = "亮度:" + str(round(i_light, 1))
    elif i_light >= 10 and i_light < 100:
        T_str = "亮度:" + str(round(i_light, 2))
    elif i_light >= 100 and i_light < 1000:
        T_str = "亮度:" + str(round(i_light, 3))
    elif i_light >= 1000 and i_light < 10000:
        T_str = "亮度:" + str(round(i_light, 4))
    print(T_str)
    last_light = i_light
```
&emsp;&emsp;
处理物联网平台过来的数据。
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    global alarm_on, device
    print(request)
    payload = ujson.loads(request['params'])
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "onoff" in payload.keys():
        alarm_on = payload["onoff"]
        if (alarm_on):
            print("开灯")
            ledOn()
        else:
            print("关灯")
            ledOff()
    # 要将更改后的状态同步上报到云平台
    upload_data = {'params': ujson.dumps({
        'onoff': alarm_on,
    })
    }
    # 上报本地报警灯状态到云端
    device.postProps(upload_data)
```
&emsp;&emsp;
确保Brightness和onoff两个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/5_1_完成物模型设置.jpg
 width=40%/>
</div>


## 4、运行结果

&emsp;&emsp;
推送此脚本到HaaS EDU K1之后，设备上打印光强度值，同时将相关的光强度值上传到云端，并且当光强度大于100，打开HaaS EDU K1开发板上的LED灯；当光强度小于100，关闭HaaS EDU K1开发板上的LED灯，如下日志所示：
```bash
...
物联网平台连接成功
...
ap3216c init finished
system first come
...
uploading data to the cloud: {'params': '{"Brightness": 304}'}
亮度:304
led off
uploading data to the cloud: {'params': '{"Brightness": 0}'}
light has change
亮度:0
led on
no need report
亮度:53
uploading data to the cloud: {'params': '{"Brightness": 302}'}
light has change
亮度:302
led off
uploading data to the cloud: {'params': '{"Brightness": 5}'}
light has change
亮度:5
led on
...
```

&emsp;&emsp;
到此为止，智慧路灯系统案例就已经完成了。如果想要学习智慧路灯实验更详细的操作步骤，请参考“[智慧路灯系统详解](https://gitee.com/haasedu/haasedu/blob/release_2.0/5-%E6%99%BA%E6%85%A7%E5%9F%8E%E5%B8%82/%E5%9C%BA%E6%99%AF1-%E6%99%BA%E6%85%A7%E8%B7%AF%E7%81%AF/README.md)”中的说明。
