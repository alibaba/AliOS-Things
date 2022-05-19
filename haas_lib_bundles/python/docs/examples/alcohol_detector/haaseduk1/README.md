# 酒精检测系统

下图是本案例的3步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/alcohol_detector/三步导学.png width=70%/>
</div>

## 简介

酒精，化学名称乙醇，化学式C₂H₆O，乙醇在常温常压下是一种易挥发的无色透明液体，低毒性。酒精会使人意识不清晰，在很多场合都不适合饮酒，尤其是在需要驾车时。国家也出台了法律，明确禁止酒后驾驶，否则会给予非常严厉的处罚。MQ3传感器是汉威电子出品的乙醇传感器，它有快速相应、高敏感度、稳定、寿命长以及外围电路简单等多个优点，常用于酒精监测、呼气测试仪等场景。

### 背景知识

本系统的核心在于灵敏地检测到酒精气体，MQ3传感器内部使用二氧化锡（SnO₂)气敏材料。当其检测到酒精蒸汽时，传感器的电导率随空气中酒精蒸汽的浓度增加而增大。MQ3内部将这种电导率变化转换成随酒精蒸汽浓度变化而变化的输出信号。这种传感器对酒精的灵敏度高，可以抵抗汽油、烟雾、水蒸气等的干扰。

<div align="center">
<img src=./../../../images/ext_mq3_实物.png  width=50%/>
</div>

***酒精传感器在使用之前需要至少预热20秒，要想有很好的灵敏度，建议预热24小时以上。***


## 准备
* haaseduk1开发板一套
* MQ3酒精传感器一个
* 杜邦连接线若干

### 硬件连线图

硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/alcohol_detector/硬件接线图.png width=60%/>
</div>
<br>

## 物联网平台开发
### 开通公共实例

对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。


在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>


开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 创建云端产品

点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_创建产品.png width=100%/>
</div>


在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-创建产品.png width=50%/>
</div>


选择之后，点击“确认”按钮，即可完成产品创建。之后点击“前往定义物模型”按钮，为新建的产品定义物模型。

<div align="center">
<img src=./../../../images/alcohol_detector/前往定义物模型.png width=50%/>
</div>

<br>

### 创建产品属性（物模型）

点击下图中的“功能定义”标签页，再点击“编辑草稿”
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-功能定义.png width=60%/>
</div>

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-编辑草稿.png width=60%/>
</div>

标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-编辑自定义功能.png width=30%/>
</div>

把刚刚创建的物模型发布上线
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-发布上线.png width=60%/>
</div>

产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）

在云平台上为刚创建发布的产品创建一个设备，如下图
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-创建设备.png width=60%/>
</div>


设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>


设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-设备物模型信息.png width=60%/>
</div>

<br>

#### **获取设备三元组**

如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-设备三元组信息.png width=30%/>
</div>

<br>

#### **查看设备属性信息**

设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“酒精ADC电压值“属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-设备物模型数据.png width=50%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>


## 设备端开发
### 开发环境
在进行下一步之前请确保HaaSEDUK1开发环境已经搭建完毕。详情请参考[HaaSEDUK1开发环境](../../../startup/HaaS_EDU_K1_startup.md)的说明。

### 创建解决方案

如下图所示，打开VS Code之后在新建一个基于hellworld的python工程，设定好工程名称（“alcohol_detector”）及工作区路径之后，硬件类型选择haaseduk1，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/alcohol_detector/设备端-创建工程向导.png width=30%/>
</div>

<div align="center">
<img src=./../../../images/alcohol_detector/设备端-创建工程确认.png width=30%/>
</div>

### 案例代码

将[酒精检测系统](./code/)文件下所有脚本代码复制后，覆盖刚创建的“alcohol_detector”工程根目录下的文件。main.py代码区如下图所示：
<div align="center">
<img src=./../../../images/alcohol_detector/设备端-代码区.png width=80%/>
</div>

> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**


修改hcho_detector工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```


修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**


修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-设备三元组信息.png width=50%/>
</div>

3. **修改设备端上报数据所用标识符**


alcohol_detector工程里main.py中下面的代码实现的是上传酒精检测结果到云端的功能。其中alcohol_adc_voltage便是酒精检测结果上报云端所用的标识符。

```python
# 上传酒精浓度信息到物联网平台
def upload_alcohol_detector_state():
    global device
    alarming = False

    while True:
        data = get_alcohol_value()
        print('alcohol adc voltage:%d mv.' % data)
        if data < 0:
            print("Error: invalid alcohol adc voltage:", data)
            continue

        temp_str = "Alcohol:"
        temp_data_str = "%d" % data
        oledShowText(temp_str, 3, 1, 1, True, 16)
        oledShowText(temp_data_str, 40, 20, 1, False, 32)

        if (data > ALCOHOL_ALARM_VALUE):
            print("Info: alcohol detected, start alarm...")
            buzzer_alarm(True)
            alarming = True

        # "alcohol_adc_voltage" - 在云平台上创建产品时对应的酒精浓度属性的标识符
        upload_data = {'params': ujson.dumps({
            'alcohol_adc_voltage': data,
        })}
        device.postProps(upload_data)

        utime.sleep(3)
        if alarming:
            print("Info: stop alarm...")
            buzzer_alarm(False)

```

确保这个标识符和物联网产品的物模型中属性标识符是一样的。

<br>

## 运行结果

### 本地查看


推送此脚本到HaaSEDUK1之后并运行，串口会周期性的打印如下日志：

```log
success to establish tcp, fd=3
物联网平台连接成功
sleep for 2s
alcohol adc voltage:381 mv.
alcohol adc voltage:381 mv.
alcohol adc voltage:379 mv.
alcohol adc voltage:379 mv.
alcohol adc voltage:374 mv.
alcohol adc voltage:394 mv.
alcohol adc voltage:492 mv.
alcohol adc voltage:565 mv.
alcohol adc voltage:977 mv.
Info: alcohol detected, start alarm...
<W><0> [pwm_device_ioctl:57]pwm0 is off now, ignore OFF request
Info: stop alarm...
alcohol adc voltage:1140 mv.
Info: alcohol detected, start alarm...
Info: stop alarm...
alcohol adc voltage:1101 mv.
Info: alcohol detected, start alarm...
Info: stop alarm...
alcohol adc voltage:998 mv.
Info: alcohol detected, start alarm...
Info: stop alarm...
alcohol adc voltage:697 mv.
alcohol adc voltage:665 mv.
alcohol adc voltage:646 mv.
alcohol adc voltage:1045 mv.
Info: alcohol detected, start alarm...
Info: stop alarm...
alcohol adc voltage:1249 mv.
Info: alcohol detected, start alarm...
Info: stop alarm...
alcohol adc voltage:1167 mv.
Info: alcohol detected, start alarm...
```

根据测量值， 当检测到的ADC电压值大于800mv时，空气中有稍明显的酒精蒸汽，触发蜂鸣器报警。
不论是否报警，检测到的ADC电压值都会显示到HaaSEDUK1的屏幕上。
<div align="center">
<img src=./../../../images/alcohol_detector/设备端-测试结果显示.jpg width=50%/>
</div>

### 物联网平台端设备信息查看

物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/alcohol_detector/云平台-设备上线.png width=50%/>
</div>

此时如果开发板周围的酒精浓度发生变化，物联网平台的物模型数据会更新为设备上报的最新的属性值。通过点击查看数据，可以看到一段时间监测到代表酒精浓度的ADC电压值值。
<div align="center">
<img src=./../../../images/alcohol_detector/云平台-测试结果显示.png width=50%/>
</div>

<br>

到此为止，酒精检测系统的案例就已经完成了。