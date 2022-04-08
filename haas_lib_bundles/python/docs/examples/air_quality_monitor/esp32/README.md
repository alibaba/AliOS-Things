# 空气检测系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/空气检测系统/步骤概述.png width=90%/>
</div>

## 简介
&emsp;&emsp;
近几年空气污染逐渐变重，很多人对空气质量愈发关心。及时检测周报环境的空气质量并通过钉钉通知相关人员，则可最大程度降低对周边环境的担忧，有利于后期出行。

### 背景知识
&emsp;&emsp;
本系统的核心在于如何检测周边环境PM2.5浓度，并通过钉钉提前预警通知给用户。

&emsp;&emsp;
光学灰尘传感器GP2Y1014AU可以检测非常细的颗粒，如香烟烟雾，房屋粉尘等。检测出的数据可以及时上报到物联网平台。依靠IoT Studio的 **``业务逻辑``** 开发工具，可以及时把状态通知到用户移动钉钉终端，实现空气质量PM2.5的提前预警。

<div align="center">
<img src=./../../../images/空气检测系统/GP2Y1014AU传感器.png width=50%/>
</div>


## 准备
ESP32开发板一套

[GP2Y1014AU传感器](https://detail.tmall.com/item.htm?spm=a230r.1.14.1.5157a9aeImgM2t&id=670779174099&ns=1&abbucket=4l)一个

杜邦连接线若干

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/空气检测系统/ESP32_连线.png width=90%/>
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
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **``创建产品``** 按钮，如下图所示。
<div align="center">
<img src=./../../../images/空气检测系统/空产品页.png width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/空气检测系统/新建产品.png width=80%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“空气检测系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/空气检测系统/产品列表页.png width=80%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/空气检测系统/尚未添加任何功能.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合**数据定义**中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/空气检测系统/创建自定义物模型.png width=80%/>
</div>

然后按照下图属性添加。
<div align="center">
<img src=./../../../images/空气检测系统/添加自定义功能2.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚才创建的物模型属性。pm25_value代表PM2.5值，数据类型为int32整数型。此时点击“确定”按钮，按照系统提示一步一步进行下去就可以完成之前创建的物模型属性设置。功能定义完成后点击 **``发布``** 按钮完成发布


<div align="center">
<img src=./../../../images/空气检测系统/发布物模型.png width=80%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击 **``空气检测系统``** 后的 **``管理设备``**，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/空气检测系统/设备管理.png width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/空气检测系统/添加设备入口1.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称(这里我们填入设备名称dust1)，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/空气检测系统/添加设备.png width=50%/>
</div>

&emsp;&emsp;
点击 **确认** 后弹出下面的“添加完成“提示框，点击 **``前往查看``** 按钮就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<div align="center">
<img src=./../../../images/空气检测系统/物模型数据.png width=80%/>
</div>

<br>

#### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“PM25浓度”属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/空气检测系统/物模型属性信息.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>

### 配置业务逻辑
&emsp;&emsp;
接下来就是配置业务逻辑，以便将设备端发送到物联网平台的数据通过钉钉的方式发送相关人员。这里我们采用物联网``业务逻辑``开发工具

&emsp;&emsp;
物联网应用开发（IoT Studio）提供了物联网业务逻辑的开发工具，支持通过编排服务节点的方式快速完成简单的物联网业务逻辑的设计。具体细节可以参考物联网应用开发中的[业务逻辑](https://help.aliyun.com/document_detail/141614.html)章节。

#### 1. 创建钉钉机器人
&emsp;&emsp;
点击钉钉头像，在弹出的对话框中点击``设置``按钮
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_1.png width=40%/>
</div>

&emsp;&emsp;
点击 ``机器人管理``按钮
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_2.png width=40%/>
</div>

&emsp;&emsp;
在机器人管理页面点击 ``自定义``按钮
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_3.png width=40%/>
</div>

&emsp;&emsp;
点击 ``确定``按钮
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_4.png width=40%/>
</div>

&emsp;&emsp;
输入机器人名称及机器人想要添加的钉钉群组(如果没有可以自行创建)，在安全设置一项添加"自定义关键词"。

&emsp;&emsp;
需要注意的是：***自定义关键词用来过滤向钉钉群推送的信息，只有包括了该关键词的推送信息才会在群里显示，不包含该关键词的推送信息会被过滤掉。可以定义多个关键词，只要推送的信息中有一个词跟关键词匹配即可被群接收并显示***。这里我们定义"浓度"关键词，也就是说后续的推送信息里面必须包含"浓度"这个词语。

&emsp;&emsp;
点击``完成``按钮
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_5.png width=60%/>
</div>

<span id="webhook_copy">
&emsp;&emsp;
创建完成后会弹出提示框，点击``复制``按钮复制Webhook地址以备后用，使用该地址可以向钉钉群推送消息。点击“完成”。
<div align="center">
<img src=./../../../images/空气检测系统/创建钉钉机器人_6.png width=40%/>
</div>

### 配置 IoT Studio 物联网

&emsp;&emsp;
登录[IoT Studio](https://studio.iot.aliyun.com/projects)平台，在左侧的侧边栏点击``项目管理``，在项目管理页面点击``新建项目``按钮。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_项目管理.png width=60%/>
</div>

&emsp;&emsp;
在``新建项目``页面点击``创建空白项目``。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_新建项目.png width=60%/>
</div>

&emsp;&emsp;
弹出``新建空白项目``页面，输入项目名称及描述后点击``确定``按钮。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_新建空白项目.png width=60%/>
</div>

&emsp;&emsp;
项目创建完成后会自动导航到新创建的项目，新项目需要关联产品，设备，并创建``业务逻辑``，以实现云端上报信息到钉钉的信息推送。

&emsp;&emsp;
选择左侧的``产品``侧边栏，点击``关联物联网平台产品``，在弹出的窗口中选择之前创建的产品并关联。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_关联物联网平台产品_2.png width=80%/>
</div>

&emsp;&emsp;
选择左侧的``设备``侧边栏，点击``关联物联网平台设备``，在弹出的窗口中选择关联产品下的设备。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_关联物联网平台设备.png width=80%/>
</div>

&emsp;&emsp;
选择左侧的``主页``侧边栏，在弹出的项目详情页选择``业务逻辑``，点击``新建``按钮，在弹出的下拉框选择``新建空白业务服务``。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_新建业务逻辑.png width=60%/>
</div>

&emsp;&emsp;
在新建业务服务窗口输入业务服务名称及描述后点击``确定``按钮。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_新建业务服务.png width=60%/>
</div>

&emsp;&emsp;
业务服务创建完成后自动导航到业务逻辑开发页面。点击“节点”按钮显示所有原子功能，并以此拖住``设备触发``，``条件判断`` 和 ``钉钉机器人``到右侧的窗口。按住鼠标左键连接各个模块，注意``条件判断``两个逻辑输出要连接正确的``钉钉机器人``，以免逻辑混乱。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发.png width=80%/>
</div>

&emsp;&emsp;
点击``设备触发``模块，在弹出的对话框中选择产品，设备及触发条件。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_选择产品.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_选择设备.png width=80%/>
</div>

<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_选择触发条件.png width=80%/>
</div>

配置完设备后，修改条件判断。当PM2.5数值小于35的时候，我们认为空气质量优良
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_设置判决条件.png width=80%/>
</div>

&emsp;&emsp;
点击``空气优良机器人``模块，在弹出的对话框中设置[Webhook](#webhook_copy)，选择配置方式及消息类型。Webhook对话框输入之前创建“空气监测报警”钉钉机器人时候拷贝的字符串，按照下图中的设备配置机器人：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_空气质量优良机器人.png width=80%/>
</div>

同时我们也设置空气污染机器人，Webhook也输入上述机器人相同的字符串：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_空气污染机器人.png width=80%/>
</div>

&emsp;&emsp;
业务逻辑开发配置完成后，我们可以先使用软件调试的方式验证相关逻辑链路的正确性。点击``保存``按钮后点击``部署调试``按钮。
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_部署调试.png width=80%/>
</div>

&emsp;&emsp;
在弹出的对话框选择``前往``按钮：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_前往部署调试.png width=80%/>
</div>

&emsp;&emsp;
在弹出的设备模拟器页面点击``启动设备模拟器``按钮：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_启动设备模拟器.png width=80%/>
</div>

&emsp;&emsp;
选择``属性上报``选项并在PM25浓度填写合适的数值，点击``发送命令``按钮：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_设备模拟调试.png width=80%/>
</div>

&emsp;&emsp;
发送命令后，我们便可以在钉钉群接收到相关的通知：
<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_空气监测报警.png width=50%/>
</div>

&emsp;&emsp;
到这里我们已经完成了IoT云端的所有配置，接下来就是完成设备端开发并完成设备属性的上报。

<br>

## 设备端开发
### 开发环境
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于hellworld的python工程，设定好工程名称（``air_quality``）及工作区路径之后，硬件类型选择nodmcu32s，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/空气检测系统/esp32_1.png width=40%/>
</div>

### 案例代码
&emsp;&emsp;
将[air_quality_monitor](./code/)文件下所有脚本代码复制后，覆盖刚创建的``air_quality``工程根目录下的文件。main.py代码区如下图所示：
<div align="center">
<img src=./../../../images/空气检测系统/esp32_2.png width=80%/>
</div>

> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改epidemic_control工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，具体获取方式如下图所示：

<div align="center">
<img src=./../../../images/空气检测系统/获取三元组信息.png
 width=80%/>
</div>

3. **修改设备端上报数据所用标识符**

&emsp;&emsp;
air_quality工程里main.py中下面的代码实现的是上传PM2.5浓度到云端的功能。其中pm25_value和pollen_value便是上报云端所用的标识符。

```python
    # 这里的数据仅表示从ADC上读到的数据，未实现到真实PM2.5的转换，仅作案例参考
    dustValue = gp2y10Obj.getDustVal()
    print('dustValue = ', dustValue)

    # 生成上报到物联网平台的属性值字串，此处的属性标识符"pm25_value"必须和物联网平台的属性一致
    upload_data = {'params': ujson.dumps({
        'pm25_value': dustValue # 这里填充PM2.5浓度    
    })
    }
    # 上传状态到物联网平台
    device.postProps(upload_data)

    utime.sleep(60)      # 打印完之后休眠60秒
```

确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/空气检测系统/属性标识符修改.png
 width=80%/>
</div>

<br>

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到ESP32之后并运行，串口会周期性的打印如下日志：

```log
>>> 
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.3.196', '255.255.255.0', '192.168.3.1', '192.168.3.1')
establish tcp connection with server(host='xxxxxxxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
gp2y10Obj inited!
dustValue =  0
dustValue =  0
dustValue =  0
dustValue =  0
dustValue =  0
dustValue =  2200
dustValue =  431
dustValue =  0
dustValue =  0
dustValue =  20
dustValue =  0
dustValue =  0
dustValue =  0

```
同时我们也可以看到钉钉端的相关通知。

<div align="center">
<img src=./../../../images/空气检测系统/iotstudio_业务逻辑开发_空气监测报警.png width=50%/>
</div>

<br>

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/空气检测系统/设备状态及属性.png width=80%/>
</div>

&emsp;&emsp;
此时如果空气质量发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。

&emsp;&emsp;
到此为止，空气质量监控系统的案例就已经完成了。
