# 播报音箱
## 简介
&emsp;&emsp;
随着移动支付的普及，日常生活中很多时候我们都是采用扫码进行支付，当商家收到付款信息以后，会通过播报音箱将收款信息播放出来，从而快速的完成收款信息确认。

&emsp;&emsp;
“千里传音”服务，是阿里云IoT针对带有语音播报能力的AIoT设备，提供的一个云端一体的解决方案。为播报提醒类设备应用提供从播报语料合成，语料管理，语料推送到设备，播报设备管理等完善功能，配合集成了端侧播报能力的HaaS设备，帮助用户高效完成播报类设备应的开发和长期运行。

&emsp;&emsp;
本节内容就是通过使用阿里云的千里传音和Python轻应用，快速的打造一个自己的播报音箱产品。

## 准备
&emsp;&emsp;
完成本节内容的学习需要如下硬件：
1. HaaS EDU K1一套
2. 喇叭一个

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/Audio_3_2_HaaS_EDU_K1_硬件连线.png width=60%/>
</div>


## 物联网平台开发
### **开通千里传音服务**
&emsp;&emsp;
登录[阿里云物联网平台](https://iot.console.aliyun.com/)控制台页面。
<div align="center">
<img src=./../../../images/Audio_3_1_控制台界面.png width=80%/>
</div>

&emsp;&emsp;
 如下图所示，依此点击“增值服务”->“选购增值服务”->“语音智能”，找到“千里传音语音播报服务”，点击“开通服务”。
<div align="center">
<img src=./../../../images/Audio_3_1_开通千里传音服务.png width=80%/>
</div>

### **创建项目**
1. 开通了千里传音服务以后，再点击“增值服务”，页面就会有千里传音服务的管理界面，点击“前往服务”。
    <div align="center">
    <img src=./../../../images/Audio_3_1_使用千里传音服务.png width=80%/>
    </div>
2. 在左侧栏选项中选择“项目管理”功能，后点击“创建项目”创建一个新项目。
<div align="center">
<img src=./../../../images/Audio_3_2_项目管理入口.png width=80%/>
</div>

3. 在项目创建过程中，需要输入项目名称，并对项目进行简单描述，以便后期维护。
<div align="center">
<img src=./../../../images/Audio_3_3_创建项目.png width=40%/>
</div>

4. 在完成项目创建后，系统将帮助用户创建一个与项目名相同的产品，以便后续加入设备。同时，如果用户希望为设备增加千里传音以外的能力，也可以直接到物联网平台的设备管理界面中，为设备添加物模型能力。

### **新增语料**

1. 在项目列表中，点击需要配置的项目后面的“配置”按钮，可以对项目内容进行配置管理。
<div align="center">
<img src=./../../../images/Audio_3_4_项目配置.png width=80%/>
</div>

&emsp;&emsp;
在项目配置界面，有分别是服务配置和设备管理等选项，其中服务配置是用来配置千里传音服务的，主要是语料管理；设备配置是用来管理项目中设备列表的。

1. 如下图所示，在项目配置中，默认选中的就是“语料配置”
<div align="center">
<img src=./../../../images/Audio_3_6_新增语料.png width=80%/>
</div>

&emsp;&emsp;
在“语料配置”中的点击“新增语料”，新增自定义语料界面如下:

<div align="center">
<img src=./../../../images/Audio_3_7_自定义语料.png
width=40%/>
</div>

&emsp;&emsp;
其中语料标识和语料内容非常重要,语料标识必须是项目级别唯一标识符，它是作为语料组合调用时候的唯一标签，同时也作为设备端保存语料的文件名。语料内容指的就是我们要播报的文本。

&emsp;&emsp;
除此之外，我们还需要选择此语料的“语音场景”，包括方言场景，童声场景，客服场景以及通用场景（选择不同的语音场景后，生成的语料特色也各不相同）；自定义语料的播放速度；自定义语料的播报音量等。

&emsp;&emsp;
选择不同的“语音场景”、“播报速度”及“播报音量”后点击“试听”直到得到自己满意的播报语料（是不是很有趣？）。

### **创建设备**

1. 在完成语料构建和管理后，需要在项目中创建设备，以便最终用户的播报应用可以将命令发送到设备端，完成整个播报链路。在“项目管理”界面，点击“设备管理”标签，将进入设备管理界面。

<div align="center">
<img src=./../../../images/Audio_3_8_创建设备.png
width=80%/>
</div>

&emsp;&emsp;
在设备管理界面，有多个标签页可选，默认选中的是“设备列表”标签。

2. 设备创建可以有“创建单个设备”和“批量创建设备”两种方式，点击“创建设备”会打开创建单个设备弹窗，并要求用户输入设备相关信息。

<div align="center">
<img src=./../../../images/Audio_3_9_添加设备.png
width=40%/>
</div>

* DeviceName：英文字符组成的设备名称，设备名称在一个项目中不可重复。

* 备注名称：为了便于用户区分设备，给设备赋予的的信息，比如设备所部署的位置等信息。

2’. 如果需要创建多个设备可以点击“批量添加”按钮，此时会打开批量创建设备弹窗。

<div align="center">
<img src=./../../../images/Audio_3_10_批量添加设备.png width=40%/>
</div>

* 自动生成：指系统将为用户自动生成DeviceName
* 批量上传：需要用户通过.csv文件上传自定义的DeviceName
* 设备数量：需要批量添加的设备个数

### **语料推送**

1. 在创建完成设备以后，回到“设备管理”标签页，可以显示设备列表，点击要操作的设备后面的“鉴权信息”按钮，可以查看设备的三元组信息。

<div align="center">
<img src=./../../../images/Audio_3_11_鉴权信息.png
width=80%/>
</div>

2. 在创建完成设备以后，我们就可以回到“语料配置”界面，将新增的语料推送到相应的设备上面进行测试了。

&emsp;&emsp;
如下图所示，点击需要推送的语料，操作栏中的推送到设备按钮(下图中我们推送的语料id是yuan)

<div align="center">
<img src=./../../../images/Audio_3_12_推送到设备.png
width=80%/>
</div>

3. 进入语料推送界面，如下图所示，默认支持“单个设备”，“分组推送”和“全部设备”三种形式的推送。

<div align="center">
<img src=./../../../images/Audio_3_13_推送选项.png
width=80%/>
</div>

&emsp;&emsp;
“单个设备”和“分组推送”模式，需要选择自己的设备名或者分组名，选中以后点击确定即可完成推送（需要等到**设备端开发**步骤完成后语料才能成功的推送到设备上）。

## 设备端开发

### 开发环境准备

&emsp;&emsp;
在进行下一步之前请确保HaaS EDU K1开发板的开发环境已经搭建完毕。详情请参考[HaaS EDU K1快速开始](../../../startup/HaaS_EDU_K1_startup.md)的说明。

### 创建解决方案

#### 工程创建
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个python工程，设定好工程名称（“remote_speaker”）及工作区路径。

<div align="center">
<img src=./../../../images/Audio_3_14_创建播报音箱工程.png width=80%/>
</div>

* 硬件类型：选择**haaseduk1**
* 解决方案：选择**remote_speaker_v2**

&emsp;&emsp;
点击"立即创建"，创建一个Python轻应用的解决方案。这个步骤完成后，HaaS-Studio会自动从云端拉取remote_speaker_v2模板的代码到本地。
> 注：模板中的remote_speaker_v2使用的是HaaS Python 2.0标准的API

#### 代码修改
&emsp;&emsp;
本地工程创建完成后，只需要如下2步，就可以完成设备端的开发。

1. **修改路由器名称及密码**

&emsp;&emsp;
修改broadcast_speaker工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

 ```python
 # Wi-Fi SSID和Password设置
 wifiSsid = "请填写您的路由器名称"
 wifiPassword = "请填写您的路由器密码"
 ```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的代码就会连接读者自己设定的路由器。

1. **修改设备端三元组**

&emsp;&emsp;
修改broadcast_speaker工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息(三元组信息获取可以参考“语料推送”的鉴权信息图)，如下所示：

```python
# 三元组信息
productKey = "产品密钥"
deviceName = "设备名称"
deviceSecret = "设备密钥"
```
> Python脚本的详细说明请参考脚本内嵌的文字版注释。

## 运行结果
### 本地查看

&emsp;&emsp;
推送脚本到HaaS EDU K1上并运行，脚本运行后输出如下日志。其中:
* “remote speaker demo version - v2.0” 是程序启动成功后打印的版本信息
* ”wifi_connected: False“ 代表循环等待wifi连接成功
* “物联网平台连接成功” 代表成功连接到物联网平台

```log
...
             Welcome to AliOS Things   
...
 ==== python execute from /data/pyamp/main.py ====
 Welcome to MicroPython 
...
remote speaker demo version - v2.0
wifi_connected: False
wifi_connected: False
wifi_connected: False
Wi-Fi connected
开始连接物联网平台
sleep for 1 s
establish tcp connection with server(host='xxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
success to establish tcp, fd=54
物联网平台连接成功
...
```

&emsp;&emsp;
在出现“物联网平台连接成功”之后就可以查看“语料推送”小节中的说明从云端推送语料到设备端。

## 语料播报控制
&emsp;&emsp;
我们已经将常用的音频文件放在了“broadcast_speaker”工程的resource目录下，在推送工程到设备的时候推送到了设备上，因此我们不需要再去重新创建语料，可以直接通过云端进行播报调试。

1. 打开[OpenAPI调试](https://api.aliyun.com/#/?product=Iot&api=SpeechByCombination)页面并登录阿里云账号。

2. 如下如所示，选中SpeechByCombination,选择右侧服务器华东(上海)，设置我们需要发送的语音序列信息依次为"zfbGet","{$10000}","yuan"，{$xxx}里边的xxx代表实际要播报的数字，最后设置需要调试的设备DeviceName、ProductKey和千里传音的服务IotInstanceId。
<div align="center">
<img src=./../../../images/Audio_3_15_在线调试.png
width=80%/>
</div>

&emsp;&emsp;
其中IotInstanceId在企业物联网平台的控制台主页可以看到，如下图:
<div align="center">
<img src=./../../../images/Audio_3_1_实例ID.png
width=80%/>
</div>

3. 点击"发起调试"，设备端就可以听到**支付宝到账10000元**的播报提示。此时可以看到设备突出如下日志，其中：
on service：代表收到云端的播报发送的服务请求，后面跟的是云端请求播报的语料的标识符。
['xxx', 'yyy', 'zzz']：设备将接收到的云端标识符和本地语料路径进行组合获得的音频文件列表。

    ```log
    ****** on service ********
    zfbGet
    {$10000}
    yuan
    ['/data/pyamp/resource/zfbGet.wav', '/data/pyamp/resource/SYS_TONE_1.wav', '/data/pyamp/resource/SYS_TONE_MEASURE_WORD_wan.wav', '/data/pyamp/resource/yuan.wav']
    ```

4. 如果需要播报其它内容，需要先创建语料，然后推送到设备中去，推送完成以后修改CombinationList 中**语料ID** 进行组合即可进行播放。

