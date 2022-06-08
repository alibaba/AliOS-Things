# 低碳节能系统

&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/低碳节能系统/步骤概述.png width=80%/>
</div>

## 注意事项
**1、该案例有涉及一定的强电，仅作演示使用，未做详尽的用电安全保护。具体商业使用请做好安全保护。**

**2、该案例指导如何使用nodemcu32s通过Modbus控制外围slave设备。**

</br>

## 1、简介
&emsp;&emsp;
2020年我国首次提出“中国将提高国家自主贡献力度，采取更加有力的政策和措施，二氧化碳排放力争于2030年前达到峰值，努力争取2060年前实现碳中和”。低碳节能已经上升为国家战略性决策，日常生活中节约用电就是最好的低碳节能方式。

### 1.1、背景知识
&emsp;&emsp;
本系统的核心在于监控一定区域内是否有人员存在，如果没有人员存在则自动关闭电器设备，有人员存在的时候则打开电器设备，实现电器的开关与人员是否存在相关联，最大限度的实现电能节约。

&emsp;&emsp;
本案例采用红外感应器，配合继电器控制实现人员检测与电路开关，能够满足该案例所需的监控范围及条件。

### 1.2、准备

&emsp;&emsp;
本案例需要如下硬件。
* ESP32开发板一套
* 红外感应器一个
* [TTL转485转换器](https://detail.tmall.com/item.htm?spm=a230r.1.14.11.777c5e36UX0aj6&id=590488524313&ns=1&abbucket=4&skuId=4360468488162)一个
* [单路继电器模块](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.43e12e8dzrkmMK&id=600430232101&_u=o3k06vo9c03e)一个
* 直流电源一个
* 杜邦连接线若干
* 12V灯泡一个

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/低碳节能系统/ESP32_连线.png width=100%/>
</div>
<br>

## 2、物联网平台开发
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能，这里可以使用免费的公共实例进行开发。

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

### 2.1、创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **``创建产品``** 按钮，如下图所示。
<div align="center">
<img src=./../../../images/疫情防控系统_空产品页.png
 width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/低碳节能系统/新建产品.png width=80%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“低碳节能系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/低碳节能系统/产品列表.png width=80%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/低碳节能系统/尚未添加任何功能.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合**数据定义**中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/2_甲醛检测_创建自定义物模型1.png width=80%/>
</div>

然后按照下图添加属性。
<div align="center">
<img src=./../../../images/低碳节能系统/添加自定义功能.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚才创建的物模型属性。其中is_person_exist代表监控区域内是否有人。点击确认后再点击“发布上线”按钮，按照系统提示一步一步进行下去就可以完成之前创建的物模型属性设置。功能定义完成后点击 **``发布``** 按钮完成发布。


<div align="center">
<img src=./../../../images/低碳节能系统/发布物模型.png width=80%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 2.3、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击 **``低碳节能系统``** 后的 **``管理设备``**，就会进到设备管理页面。在“设备”页面点击“添加设备”按钮，
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称(这里我们填入设备名称power1)，也可以不填任何内容让系统自动生成设备名称。

<div align="center">
<img src=./../../../images/低碳节能系统/设备管理.png width=80%/>
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
<img src=./../../../images/低碳节能系统/设备信息.png width=80%/>
</div>

<br>

**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_设备三元组_马赛克.png width=80%/>
</div>

<br>

**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“监控区域内是否有人”属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/低碳节能系统/物模型数据.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>

## 3、设备端开发
### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。

### 开发原理
    (1)人体红外感应器探测到固定区域内是否存有人存在并给出两个状态变化：无人存在->有人存在，有人存在->无人存在；
    (2)设备端分别判断两个状态变化，并发送Modbus命令通知继电器开/关；
    (3)继电器链接家用电器，实现家电的开关控制以最终实现低碳节能;

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“低碳节能系统”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

> Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**

&emsp;&emsp;
修改main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，具体获取方式如下：

<div align="center">
<img src=./../../../images/低碳节能系统/获取三元组.png
 width=80%/>
</div>

3. **修改设备端上报数据所用标识符**

&emsp;&emsp;
此工程里main.py中下面的代码实现的是上传入户门开关状态到云端的功能。其中is_person_exist便是监控区域内是否有人员存在的标识符。

```python
    thisIRExist = irDev.irDetect()
    print('thisIRExist = ', thisIRExist)

    if(lastIRExist != thisIRExist):
        # 生成上报到物联网平台的属性值字串，此处的属性标识符"is_person_exist"必须和物联网平台的属性一致
        # "is_person_exist" - 表示入户门开关状态
        upload_data = {'params': ujson.dumps({
            'is_person_exist': thisIRExist,
            })
        }
        # 上传状态到物联网平台
        device.postProps(upload_data)

        if(thisIRExist == 1):
            open_relay(0)
        else:
            close_relay(0)

    lastIRExist = thisIRExist
    utime.sleep(CHECK_PERIOD_SEC)      # 打印完之后休眠3秒
```

&emsp;&emsp;
确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/低碳节能系统/属性标识符修改.png
 width=80%/>
</div>

<br>

## 4、运行结果

### 4.1、本地查看

&emsp;&emsp;
推送此脚本到ESP32之后并运行。当监控区域内有人员出现或者无人员出现的时候，设备会监控相关状态并控制家用电器的开关。串口会周期性的打印如下日志：

```log
>>>
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
wifi_connected: False
('192.168.3.196', '255.255.255.0', '192.168.3.1', '192.168.3.1')
establish tcp connection with server(host='xxxxxxxx.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
sleep for 1 s
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
物联网平台连接成功
sleep for 2s
thisIRExist =  0
thisIRExist =  1
thisIRExist =  1
thisIRExist =  1
thisIRExist =  0
thisIRExist =  0
```
同时我们也能够听到继电器的开合，看到继电器控制的灯泡的亮灭。
<br>

### 4.2、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从“未激活状态”变为“上线”，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/低碳节能系统/设备状态及属性.png width=80%/>
</div>

&emsp;&emsp;
通过点击"查看数据"按钮也可以看到一段时间监测到的区域内是否有人员存在的相关状态。
<div align="center">
<img src=./../../../images/低碳节能系统/查看数据.png width=60%/>
</div>

<br>

&emsp;&emsp;
到此为止，低碳节能系统的案例就已经完成了。
