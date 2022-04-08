# 跌倒检测系统

&emsp;&emsp;
下图是本案例除硬件连线外的 3 步导学，每个步骤中实现的功能请参考图中的说明。

<div align="center">
<img src=./../../../images/跌倒检测系统_步骤概述.png width=90%/>
</div>

## 简介

&emsp;&emsp;
据 Forbes 报道，到 2050 年，全球 60 岁以上的人口预计将达到 20 亿。根据美国疾病控制与预防中心的数据，每年有近四分之一的老年人跌倒，而跌倒是老年人因创伤入院的主要原因。跌到检测系统能够第一时间检测到老人跌到的情况并及时报警，配合及时的救治措施能很大程度降低老人收到的伤害。本案例通过 MPU 传感器采集到的惯性数据来检测老人跌倒时的位姿，来实现非接触、非隐私的跌倒检测并及时报警。

### 背景知识

&emsp;&emsp;
本系统的核心在于如何确定老年人的跌倒状态，并及时通知给家人或亲戚朋友。

&emsp;&emsp;
MPU6886(6-Axis IMU Unit)是一款 6 轴姿态传感器，可以获取器件当前的三轴加速度分量和三轴旋转角速度。由于其体积小巧，功能强大，精度较高，适合集成到跌倒检测系统中，用于检测老人行走过程中的位姿。此外，依靠 IoT Studio 的 **`业务逻辑`** 开发工具，可以及时把跌倒状态通知给家人或亲戚朋友的钉钉账号，实现对老人跌倒的远程监控。

<div align="center">
<img src=./../../../images/跌倒检测系统_加速度角速度示意图.png
 width=30%/>
</div>

&emsp;&emsp;
三轴加速度：加速度计的三轴分量 ACC_X、ACC_Y 和 ACC_Z，均为 16 位有符号浮点数，分别表示器件在三个轴向上的加速度，取负值时加速度沿坐标轴负向，取正值时沿坐标轴正向。

&emsp;&emsp;
三轴角速度：绕 X、Y 和 Z 三个坐标轴旋转的角速度分量 GYR_X、GYR_Y 和 GYR_Z，均为 16 位有符号浮点数。从原点向旋转轴方向看去，取正值时为顺时针旋转，取负值时为逆时针旋转。

## 准备

&emsp;&emsp;
M5StackCore2 开发板一套，已内置 MPU6886 传感器，无需外接传感器。

<br>

## 物联网平台开发

### 开通公共实例

&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东 2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的 3 个步骤完成云端设备的创建：

1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品

&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **`创建产品`** 按钮，如下图所示。

<div align="center">
<img src=./../../../images/跌倒检测系统_空产品页.png
 width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA 标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。

<div align="center">
<img src=./../../../images/跌倒检测系统_新建产品.png width=70%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“fall_detection”的产品，如下图所示。

<div align="center">
<img src=./../../../images/跌倒检测系统_产品列表页.png width=80%/>
</div>

<br>

### 创建产品属性（物模型）

&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic 列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/跌倒检测系统_尚未添加任何功能.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合**数据定义**中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/2_甲醛检测_创建自定义物模型1.png width=80%/>
</div>

&emsp;&emsp;
然后按照下图添加“跌倒状态属性”。

<div align="center">
<img src=./../../../images/跌倒检测系统_添加自定义功能.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚才创建的物模型属性。其中 isFall 代表跌倒状态的属性，数据类型为 int32 整数型。此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中。功能定义完成后点击 **`发布`** 按钮完成发布。

<div align="center">
<img src=./../../../images/跌倒检测系统_发布物模型.png width=80%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）

&emsp;&emsp;
在产品列表页面中，点击 **`fall_detection`** 后的 **`管理设备`**，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/跌倒检测系统_设备管理.png width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。

<div align="center">
<img src=./../../../images/跌倒检测系统_添加设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称(这里我们填入设备名称 fall_detection)，也可以不填任何内容让系统自动生成设备名称，如下图所示。

<div align="center">
<img src=./../../../images/1_添加设备.png width=50%/>
</div>

&emsp;&emsp;
点击 **确认** 后弹出下面的“添加完成“提示框，点击 **`前往查看`** 按钮就可以看到此设备端详细信息了。

<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：

1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<div align="center">
<img src=./../../../images/跌倒检测系统_物模型数据.png width=80%/>
</div>

<br>

#### **获取设备三元组**

&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_设备三元组_马赛克.png width=80%/>
</div>

<br>

#### **查看设备属性信息**

&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“跌倒状态“属性值就会显示设备最新的属性信息。

<div align="center">
<img src=./../../../images/跌倒检测系统_属性状态数据.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>

### 配置业务逻辑

&emsp;&emsp;
接下来就是配置业务逻辑，以便将设备端发送到物联网平台的数据通过钉钉的方式发送给疫情监控人员。这里我们采用物联网`业务逻辑`开发工具

&emsp;&emsp;
物联网应用开发（IoT Studio）提供了物联网业务逻辑的开发工具，支持通过编排服务节点的方式快速完成简单的物联网业务逻辑的设计。具体细节可以参考物联网应用开发中的[业务逻辑](https://help.aliyun.com/document_detail/141614.html)章节。

#### 1. 创建钉钉机器人

&emsp;&emsp;
点击钉钉头像，在弹出的对话框中点击`设置`按钮

<div align="center">
<img src=./../../../images/疫情防控系统_创建钉钉机器人_1.png width=40%/>
</div>

&emsp;&emsp;
点击 `机器人管理`按钮

<div align="center">
<img src=./../../../images/疫情防控系统_创建钉钉机器人_2.png width=40%/>
</div>

&emsp;&emsp;
在机器人管理页面点击 `自定义`按钮

<div align="center">
<img src=./../../../images/疫情防控系统_创建钉钉机器人_3.png width=40%/>
</div>

&emsp;&emsp;
点击 `确定`按钮

<div align="center">
<img src=./../../../images/疫情防控系统_创建钉钉机器人_4.png width=40%/>
</div>

&emsp;&emsp;
输入机器人名称及机器人想要添加的钉钉群组(如果没有可以自行创建)，在安全设置一项添加"自定义关键词"。

&emsp;&emsp;
需要注意的是：**_自定义关键词用来过滤向钉钉群推送的信息，只有包括了该关键词的推送信息才会在群里显示，不包含该关键词的推送信息会被过滤掉。可以定义多个关键词，只要推送的信息中有一个词跟关键词匹配即可被群接收并显示_**。这里我们定义"发生跌倒"关键词，也就是说后续的推送信息里面必须包含"发生跌倒"这个词语。

&emsp;&emsp;
点击 `完成`按钮

<div align="center">
<img src=./../../../images/跌倒检测系统_创建钉钉机器人_5.png width=60%/>
</div>

<span id="webhook_copy">
&emsp;&emsp;
创建完成后会弹出提示框，点击``复制``按钮复制Webhook地址，使用该地址可以向钉钉群推送消息。点击完成。
<div align="center">
<img src=./../../../images/疫情防控系统_创建钉钉机器人_6.png width=40%/>
</div>

### 配置 IoT Studio 物联网

&emsp;&emsp;
登录[IoT Studio](https://studio.iot.aliyun.com/projects)平台，在左侧的侧边栏点击`项目管理`，在项目管理页面点击`新建项目`按钮。

<div align="center">
<img src=./../../../images/疫情防控系统_iotstudio_项目管理.png width=60%/>
</div>

&emsp;&emsp;
在`新建项目`页面点击`创建空白项目`。

<div align="center">
<img src=./../../../images/疫情防控系统_iotstudio_新建项目.png width=60%/>
</div>

&emsp;&emsp;
弹出`新建空白项目`页面，输入项目名称及描述后点击`确定`按钮。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_新建空白项目.png width=60%/>
</div>

&emsp;&emsp;
项目创建完成后会自动导航到新创建的项目，新项目需要关联产品，设备，并创建`业务逻辑`，以实现云端上报信息到钉钉的信息推送。

&emsp;&emsp;
点击左侧的`产品`侧边栏，点击`关联物联网平台产品`，在弹出的窗口中选择之前创建的产品并关联。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_关联物联网平台产品.png width=80%/>
</div>

&emsp;&emsp;
点击左侧的`设备`侧边栏，点击`关联物联网平台设备`，在弹出的窗口中选择关联产品下的设备。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_关联物联网平台设备.png width=80%/>
</div>

&emsp;&emsp;
点击左侧的`主页`侧边栏，在弹出的项目详情页选择`业务逻辑`，点击`新建`按钮，在弹出的下拉框选择`新建空白业务服务`。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_新建业务逻辑.png width=60%/>
</div>

&emsp;&emsp;
在新建业务服务窗口输入业务服务名称及描述后点击`确定`按钮。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_新建业务服务.png width=60%/>
</div>

&emsp;&emsp;
业务服务创建完成后自动导航到业务逻辑开发页面。点击“节点”按钮显示所有原子功能，并以此拖住`设备触发`，`条件判断` 和 `钉钉机器人`到右侧的窗口。按住鼠标左键连接各个模块，注意`条件判断`逻辑输出要连接正确的`钉钉机器人`，以免逻辑混乱。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发.png width=60%/>
</div>

&emsp;&emsp;
点击`设备触发`模块，在弹出的对话框中选择产品、设备及触发条件。
选择产品

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_选择产品.png width=80%/>
</div>

选择设备

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_选择设备.png width=80%/>
</div>

选择属性

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_选择触发条件.png width=80%/>
</div>

&emsp;&emsp;
点击`条件判断`模块，在弹出的对话框中条件配置。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_条件配置.png width=80%/>
</div>

&emsp;&emsp;
点击`疫情安全`模块，在弹出的对话框中设置[Webhook](#webhook_copy)，选择配置方式及消息类型。Webhook 对话框输入之前创建“疫情安全”钉钉机器人时候拷贝的字符串。在“消息类型”的内容配置项里填写`content`内容。(内容需要包括钉钉机器人设置的"自定义关键词")

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_疫情安全机器人.png width=80%/>
</div>

&emsp;&emsp;
业务逻辑开发配置完成后，我们可以先使用软件调试的方式验证相关逻辑链路的正确性。点击网页右上角的`保存`按钮后点击`部署调试`按钮。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_部署调试.png width=80%/>
</div>

&emsp;&emsp;
在弹出的对话框选择`前往`按钮：

<div align="center">
<img src=./../../../images/疫情防控系统_iotstudio_业务逻辑开发_前往部署调试.png width=80%/>
</div>

&emsp;&emsp;
在弹出的设备模拟器页面点击`启动设备模拟器`按钮：

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_启动设备模拟器.png width=80%/>
</div>

&emsp;&emsp;
选择`属性上报`选项并在跌倒状态属性栏输入 0（门关闭）或者 1（发生跌倒），点击`发送命令`按钮：

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_设备模拟调试.png width=80%/>
</div>

&emsp;&emsp;
发送命令后，我们便可以在钉钉群接收到相关的通知：

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_钉钉发生跌倒通知.png width=50%/>
</div>

&emsp;&emsp;
到这里我们已经完成了 IoT 云端的所有配置，接下来就是完成设备端开发并完成设备属性的上报。

<br>

## 设备端开发

### 开发环境

&emsp;&emsp;
在进行下一步之前请确保 M5StackCore2 开发环境已经搭建完毕。详情请参考[M5StackCore2 开发环境](../../../startup/M5StackCore2_startup.md)的说明。

### 开发原理

    步骤(1) MPU6886模块采集加速度和角速度信号；
    步骤(2) 计算加速度标准差，如果大于阈值，进入步骤(3)，否则回到步骤(1)；
    步骤(3) 计算角速度标准差，如果大于阈值，则发出跌倒预警，否则回到步骤(1)；

### 创建解决方案

&emsp;&emsp;
如下图所示，打开 VS Code 之后在新建一个基于 hellworld 的 python 工程，设定好工程名称（`fall_detection`）及工作区路径之后，硬件类型选择 M5StackCore2，点击立即创建，创建一个 Python 轻应用的解决方案。

<div align="center">
<img src=./../../../images/跌倒检测系统_esp32_1.png width=40%/>
</div>

### 案例代码

&emsp;&emsp;
将[fall_detection](./code/)文件夹下所有脚本代码复制后，覆盖刚创建的`fall_detection`工程根目录下的文件。main.py代码区如下图所示：

<div align="center">
<img src=./../../../images/跌倒检测系统_esp32_2.png width=80%/>
</div>

> Python 脚本的详细说明请参考脚本内嵌的文字注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改 fall_detection 工程里 main.py中 wifiSsid 和 wifiPassword 的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后 get_wifi_status 函数中的 nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里 main.py中 productKey、deviceName 和 deviceSecret 的值为读者创建的物联网设备的三元组信息，如下图所示：

<div align="center">
<img src=./../../../images/跌倒检测系统_修改设备端三元组信息.png
 width=80%/>
</div>

3. **修改设备端上报数据所用标识符**

&emsp;&emsp;
main.py中下面的代码实现的是上传跌倒状态到云端的功能。其中 isFall 便是将跌倒状态上报云端所用的标识符。

```python
    # 网络初始化
    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象
    get_wifi_status()
    connect_lp(productKey, deviceName, deviceSecret)

    # 硬件初始化
    i2cObj = I2C()
    i2cObj.open("mpu6886")                 # 按照board.json中名为"mpu6886"的设备节点的配置参数（主设备I2C端口号，从设备地址，总线频率等）初始化I2C类型设备对象
    if not isinstance(i2cObj, I2C):
        raise ValueError("parameter is not an I2C object")
    print("mpu6886 inited!")
    mpu6886Dev = mpu6886.MPU6886(i2cObj)   # 初始化MPU6886传感器

    # 跌倒检测算法初始化
    fallDetection = fall_detection.fall_detection(mpu6886Dev)
    fallDetection.calibrate()              # 校准传感器

    # 跌倒检测循环
    while True:
        fall = fallDetection.detect_fall() # 检测是否跌倒
        if (fall==True):                   # in event of a fall detection
            print("FALL DETECTED using MPU sensor")
            upload_data = {'params': ujson.dumps({
                'isFall': 1,
                })
            }
            # 上传跌倒状态到物联网平台
            device.postProps(upload_data)
        utime.sleep_us(10)
    i2cObj.close()                                      # 关闭I2C设备对象
    del mpu6886Dev

```

3. **阈值参数说明**

&emsp;&emsp;
在 fall_detection.py 中，可以通过以下阈值参数来调节跌倒检测的灵敏度：
| 加速度 | 阈值说明 | 角速度 | 阈值说明 |
| :-----| ----: | :----: | :----: |
| ACCELERATION_LOW_THREADHOLD | 越大越灵敏，默认值 4 |ANGULAR_VELOCITY_LOW_THREADHOLD | 越小越灵敏，默认值 1 |
| ACCELERATION_UP_THREADHOLD | 越小越灵敏，默认值 12 |ANGULAR_VELOCITY_UP_THREADHOLD | 越大越灵敏，默认值 40 |

确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：

<div align="center">
<img src=./../../../images/跌倒检测系统_属性标识符修改.png
 width=80%/>
</div>

<br>

docs/examples/tap_detection/esp32/README.md

## 推送脚本
1. **推送脚本**

&emsp;&emsp;
推送此工程到 M5StackCore2 设备上，如下图所示。

<div align="center">
<img src=./../../../images/跌倒检测系统-推送步骤.png width=90%/>
</div>

2. **推送脚本时的日志**

&emsp;&emsp;
推送过程中的日志输出如下图所示。如果需要选择串口号，请选择“/dev/cu.wchusbserial53190134851 'USB Single Serial'”，直接输入串口号前面的数字即可。

<div align="center">
<img src=./../../../images/跌倒检测系统-推送脚本log.png width=90%/>
</div>

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到 M5StackCore2 之后并运行，将 M5StackCore2 粘在一个物体上，以模拟老人跌倒的状态。

<div align="center">
<img src=./../../../images/跌倒检测系统_模拟跌倒动作.png width=80%/>
</div>

&emsp;&emsp;
串口一开始会打印连接阿里云 IoT 平台及校准 MPU6886 传感器的日志，如下所示：

```log
>>> execfile("/data/pyamp/main.py")
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.3.199', '255.255.255.0', '192.168.3.1', '192.168.3.1')
sleep for 1 s
establish tcp connection with server(host='xxxxxxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
mpu6886 inited!
Now you can start fall detection!
FALL DETECTED using MPU sensor
FALL DETECTED using MPU sensor
FALL DETECTED using MPU sensor
```

当打印“Now you can start fall detection!”后，即可开始进行跌倒检测。打印“FALL DETECTED using MPU sensor”表示检测到一次“跌倒动作”，同时我们也会收到钉钉端机器人的相关通知。

<div align="center">
<img src=./../../../images/跌倒检测系统_iotstudio_业务逻辑开发_钉钉发生跌倒通知.png width=50%/>
</div>

<br>

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/跌倒检测系统_设备状态及属性.png width=80%/>
</div>

<br>

&emsp;&emsp;
到此为止，跌倒检测系统的案例就已经完成了。感谢您的阅读!
