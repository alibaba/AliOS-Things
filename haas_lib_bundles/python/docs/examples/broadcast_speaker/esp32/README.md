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
1. m5stackcore2一套


## 物联网平台开发

### **创建项目**

1. 登录[阿里云官网](https://cn.aliyun.com/)，如下图所示，搜索***企业物联网平台***，点击并跳转
    <div align="center">
    <img src=./../../../images/Audio_3_1_企业物联网平台入口.png
    width=80%/>
    </div>
2. 点击进入控制台，控制台界面如下
    <div align="center">
    <img src=./../../../images/Audio_3_1_控制台界面.png
    width=80%/>
    </div>
3. 增值服务->选购增值服务->扩展与定制服务，找到千里传音服务，点击免费试用开通该服务。开通了千里传音服务以后，增值服务页面就会有千里传音服务的管理界面，点击前往服务。
    <div align="center">
    <img src=./../../../images/Audio_3_1_千里传音服务入口.png
    width=80%/>
    </div>
4. 选择项目管理，创建项目
    <div align="center">
    <img src=./../../../images/Audio_3_2_项目管理入口.png
    width=80%/>
    </div>
5. 在项目创建过程中，需要输入项目名称，并对项目进行简单描述，以便后期维护。
    <div align="center">
    <img src=./../../../images/Audio_3_3_创建项目.png
    width=80%/>
    </div>
6. 在完成项目创建后，系统将帮助用户创建一个与项目名相同的产品，以便后续加入设备。同时，如果用户希望为设备增加千里传音以外的能力，也可以直接到物联网平台的设备管理界面中，为设备添加物模型能力。
### **新增语料**

1. 在项目列表中，点击需要配置的项目，可以对项目内容进行配置管理。
    <div align="center">
    <img src=./../../../images/Audio_3_4_项目配置.png
    width=80%/>
    </div>
    在项目配置界面，有两个选项，分别是服务配置和设备管理，其中服务配置是用来配置千里传音服务的，主要是语料管理，设备配置是用来管理项目中设备列表的。

2. 如下图所示，在项目配置中，默认选中的就是语料配置

    <div align="center">
    <img src=./../../../images/Audio_3_6_新增语料.png
    width=80%/>
    </div>

    在语料配置中的选中新增语料，新增自定义语料界面如下:

    <div align="center">
    <img src=./../../../images/Audio_3_7_自定义语料.png
    width=80%/>
    </div>

    其中语料标识和语料内容非常重要,语料标识必须是项目级别唯一，它是作为语料组合调用时候的唯一标签，同时也作为设备端保存语料的文件名。语料内容指的就是我们要播报的文本。
    除此之外，我们还可以自定义语料的场景，包括方言场景，童声场景，客服场景以及通用场景；自定义语料的播放速度；自定义语料的播报音量等。
### **创建设备**


1. 在完成语料构建和管理后，需要在项目中创建设备，以便最终用户的播报应用可以将命令发送到设备端，完成整个播报链路。在项目管理界面，点击“设备管理”标签，将进入设备管理界面

    <div align="center">
    <img src=./../../../images/Audio_3_8_创建设备.png
    width=80%/>
    </div>

    在设备管理界面，有多个TAB可选，默认选中的是设备列表。
2. 设备创建可以有单个创建和批量创建两种方式，点击“创建设备”会打开创建单个设备弹窗，并要求用户输入设备相关信息。

    <div align="center">
    <img src=./../../../images/Audio_3_9_添加设备.png
    width=80%/>
    </div>


    DeviceName：英文字符组成的设备名称，设备名称在项目中不可重复

    备注名称：为了便于用户区分设备，给设备赋予的别名。

3. 点击“批量添加”会打开批量创建设备弹窗。

    <div align="center">
    <img src=./../../../images/Audio_3_10_批量添加设备.png
    width=80%/>
    </div>

    * 自动生成：指系统将为用户自动生成DeviceName
    * 批量上传：需要用户通过.csv文件上传自定义的DeviceName
    * 设备数量：需要批量添加的设备个数

### **语料推送**

1. 在创建完成设备以后，设备管理界面，可以显示设备列表，点击设备对应的鉴权信息，可以查看设备的三元组信息

    <div align="center">
    <img src=./../../../images/Audio_3_11_鉴权信息.png
    width=80%/>
    </div>

2. 在创建完成设备以后，我们就可以回到语料配置界面，将新增的语料推送到相应的设备上面进行测试了。
如下图所示，点击需要推送的语料，操作栏中的推送到设备按钮(下图中我们推送的语料id是  yuan)

    <div align="center">
    <img src=./../../../images/Audio_3_12_推送到设备.png
    width=80%/>
    </div>

3. 进入语料推送界面，如下图所示，默认支持单个设备推送，分组推送和全部设备推送。

    <div align="center">
    <img src=./../../../images/Audio_3_13_推送选项.png
    width=80%/>
    </div>

    单个设备和分组模式，需要选择自己的设备名或者分组名，选中以后点击确定即可完成推送。




## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保m5stackcore2开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 创建解决方案


#### 工程创建
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个python工程，设定好工程名称（“broadcast_speaker”）及工作区路径


<div align="center">
<img src=./../../../images/Audio_3_14_创建在线工程.png width=80%/>
</div>

* 硬件类型选择**m5stackcore2**
* 解决方案选择**broadcast_speaker**


点击立即创建，创建一个Python轻应用的解决方案。


#### 代码修改

Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**

    修改broadcast_speaker工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

    ```
    # Wi-Fi SSID和Password设置
    wifi_ssid = "请填写您的路由器名称"
    wifi_password = "请填写您的路由器密码"
    ```


    修改完成之后get_wifi_status函数中的代码就会连接读者自己设定的路由器。

2. **修改设备端三元组**


    修改broadcast_speaker工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息(三元组信息获取可以参考语料推送章节1的鉴权信息图)，如下所示：

    ```
    # 三元组信息
    productKey = "产品密钥"
    deviceName = "设备名称"
    deviceSecret = "设备密钥"
    ```

## 运行结果
### 本地查看

&emsp;&emsp;
推送脚本到m5stackcore2上并运行，脚本运行后输出如下日志。
```log
==== python file check /data/pyamp/main.py ====
==== python execute from /data/pyamp/main.py ====
remote speaker demo version - 1.0.3
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.3.113', '255.255.255.0', '192.168.3.1', '192.168.3.1')
set on_connect and on_service callback, start connect
sleep for 1 s
establish tcp connection with server(host='a1Ba4rCO9iM.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
```
其中:
* “remote speaker demo version - 1.0.3” 代表程序启动成功
* ”wifi_connected: False“ 代表循环等待wifi连接成功
* “物联网平台连接成功” 代表成功连接到物联网平台

```

```


## 语料推送
&emsp;&emsp;
由于在HaaSStudio 创建项目的过程中，我们已经将常用的音频文件推送到了设备端，因此我们不需要重新创建语料，直接进行推送。

1. 打开[OpenAPI调试](https://api.aliyun.com/?spm=5176.21213303.J_6704733920.32.15a33edarKZ0Pu&scm=20140722.S_other%40%40%E7%BD%91%E7%AB%99%40%40httpsapialiyuncom._.ID_other%40%40%E7%BD%91%E7%AB%99%40%40httpsapialiyuncom-RL_%E5%8D%83%E9%87%8C%E4%BC%A0%E9%9F%B3%20OpenAPI-OR_main-V_2-P0_5#/?product=Iot&api=SpeechByCombination)并登录

2. 如下如所示，选中SpeechByCombination,选择右侧服务器华东(上海)，设置我们需要发送的语音序列信息，数字请使用{$xxx} 进行表示，其它语音请以此按照千里传音后台的语料ID 进行配置，最后设置需要调试的设备DeviceName 和 ProductKey 和 千里传音的服务IotInstanceId
    <div align="center">
    <img src=./../../../images/Audio_3_15_在线调试.png
    width=80%/>
    </div>
    其中IotInstanceId在企业物联网平台的控制台主页可以看到，如下图:
    <div align="center">
    <img src=./../../../images/Audio_3_1_实例ID.png
    width=80%/>
    </div>
3. 点击发起调试，设备端就可以听到**支付宝到账10000元**的播报提示
4. 如果需要播报其它内容，需要先创建语料，然后推送到设备中去，推送完成以后修改CombinationList 中**语料ID** 进行组合即可进行播放
