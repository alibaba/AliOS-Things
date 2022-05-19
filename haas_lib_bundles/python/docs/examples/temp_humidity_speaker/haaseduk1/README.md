# 本地语音播报温湿度系统
&emsp;&emsp;
下图是本案的三步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/temp_humidity_speaker/3部导学.png width=60%/>
</div>

## 简介

HaaSEDUK1是阿里云IoT事业部HaaS团队打造的一款物联网教育开发板，其K1C版本内置了CHT8305温湿度传感器，和OLED屏幕，并可以外接喇叭。 本案例利用这些能力，循环检测当前温湿度，显示到屏幕上，并能语音播报，同时上报到阿里云物联网云平台，记录数据以做进一步分析。

## 准备

1. haaseduk1开发板          一套
2. 喇叭   一个

### 硬件连线图
<div align="center">
<img src=./../../../images/Audio_3_2_HaaS_EDU_K1_硬件连线.png width=60%/>
</div>
<br>

## 物联网平台开发

### 开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png
 width=60%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“标准品类”，节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/temp_humidity_speaker/新建温湿度检测产品.png width=60%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“温湿度检测”的产品，如下图所示。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测产品列表.png width=60%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测产品功能定义.png width=60%/>
</div>

&emsp;&emsp;
其中标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择直接新建方式来创建此产品需要的物模型信息，点击上图中的”编辑草稿“按钮。然后如下图再点击“添加标准功能”。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测编辑草稿.png width=60%/>
</div>

点击确定后回到前一个页面，再点击“发布上线”

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测发布上线.png width=60%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”温湿度检测“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测产品管理设备.png width=60%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，
也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测添加设备.png width=60%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测设备信息.png width=60%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测物模型数据.png width=60%/>
</div>

<br>

### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击deviceSecret后面的“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测三元组信息.png width=50%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。
<br>

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保haaseduk1开发环境已经搭建完毕。详情请参考[haaseduk1快速开始](../../../startup/HaaS_EDU_K1_startup.md)的说明。
<br>

### 创建解决方案

&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“temp_humidity_speaker”）及工作区路径之后，硬件类型选择haaseduk1，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/设备端-project-create.png width=40%/>
</div>

&emsp;&emsp;
将[温湿度播报系统代码](./code/)文件夹下的所有脚本进行复制到“temp_humidity_speaker”工程根目录中。如下图代码区所示：
<div align="center">
<img src=./../../../images/temp_humidity_speaker/设备端-project.png width=80%/>
</div>

> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改temp_humidity_speaker工程里main.py中wifi_ssid和wifi_password的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifi_ssid = "请输入您的路由器名称"
wifi_password = "请输入您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifi_ssid, wifi_password) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改temp_humidity_speaker工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

<div align="center">
<img src=./../../../images/temp_humidity_speaker/device_tripe.png width=80%/>
</div>

3. **获取当前温湿度，转换后语音播报和显示**
利用CHT8305传感器获取当前的温度和湿度，进行转换后保留2位小数，显示在haaseduk1的屏幕上，并进行语音播报。
```python
def play_display_temperature_humidity(cht8305Dev):
    play_data = {"format":0, "speechs":[]}
    temphumidity = cht8305Dev.getTempHumidity()
    print("当前温度:", temphumidity[0], "当前湿度:", temphumidity[1])

    if (temphumidity[0] < 0):
        temperature = "{$%.2f}" % -temphumidity[0]
        humidity = "{$%.2f}" % temphumidity[1]
        play_data["speechs"] = ["temperature", "negative", temperature, "centigrade", "humidity", humidity]
    else:
        temperature = "{$%.2f}" % temphumidity[0]
        humidity = "{$%.2f}" % temphumidity[1]
        play_data["speechs"] = ["temperature", temperature, "centigrade", "humidity", humidity]

    temp_str = "T:%.2f" % temphumidity[0]
    humi_str = "H:%.2f%%" % temphumidity[1]
    oledShowText(temp_str, 3, 1, 1, True, 12)
    oledShowText(humi_str, 3, 16, 1, False, 12)

    speaker.play_voice(play_data,resDir)
    return temphumidity
```

4. **修改设备端上报数据所用标识符**

&emsp;&emsp;
temp_humidity_speaker工程里main.py中下面的代码实现的是上报温湿度到云平台, 确保属性的标识符与云平台上产品的属性标识符一致。
```python
def post_data_to_cloud(device, temphumidity):
    # 上报温湿度到云平台
    prop = ujson.dumps({
        "CurrentTemperature": temphumidity[0],
        "CurrentHumidity": temphumidity[1],
    })

    upload_data = {"params": prop}
    device.postProps(upload_data)
```
&emsp;&emsp;
确保这温湿度检测的标识符和物联网产品的物模型中属性标识符是一样的.

<br>

## 运行结果
### 本地查看

&emsp;&emsp;
推送此脚本到haaseduk1之后，串口会周期性的打印如下日志。
```log
......
当前温度: 33.47656 当前湿度: 30.20966
temperature
{$33.48}
SYS_TONE_4
SYS_TONE_8
centigrade
humidity
{$30.21}
SYS_TONE_2
SYS_TONE_1
['/data/pyamp/resource/temperature.wav', '/data/pyamp/resource/SYS_TONE_3.wav', '/data/pyamp/resource/SYS_TONE_MEASURE_WORD_shi.wav', '/data/pyamp/resource/SYS_TONE_3.wav', '/data/pyamp/resource/SYS_TONE_dian.wav', '/data/pyamp/resource/SYS_TONE_4.wav', '/data/pyamp/resource/SYS_TONE_8.wav', '/data/pyamp/resource/centigrade.wav', '/data/pyamp/resource/humidity.wav', '/data/pyamp/resource/SYS_TONE_3.wav', '/data/pyamp/resource/SYS_TONE_MEASURE_WORD_shi.wav', '/data/pyamp/resource/SYS_TONE_dian.wav', '/data/pyamp/resource/SYS_TONE_2.wav', '/data/pyamp/resource/SYS_TONE_1.wav']
player_set_source: idle->ready
player_task: ready->running
......
```
haaseduk1屏幕显示
<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测屏幕显示.jpg width=50%/>
</div>

**注意：温湿度传感器CHT8305是内置在HaaS EDU K1内部的，所以测量的温湿度是板子内部的温湿度，并不是环境中的温湿度。这是由于HaaS EDU K1是以物联网开发教育为目的，实际生产环境中，需要采用一定的硬件结构设计，保证温湿度传感器暴露在外部环境中，并通过实际测试进行软件校准。**
<br>

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。由于云平台与本地python代码显示浮点数的方式稍有不同，故其显示的小数点第2位不同，读者可以忽略。

<div align="center">
<img src=./../../../images/temp_humidity_speaker/温湿度检测设备状态及属性.png width=80%/>
</div>

&emsp;&emsp;
每隔10s循环检测当前温湿度，物联网平台的物模型数据会更新为设备上报的最新的属性值。

<br>

&emsp;&emsp;
到此为止，温湿度本地播报并数据上云的案例就已经完成了。
