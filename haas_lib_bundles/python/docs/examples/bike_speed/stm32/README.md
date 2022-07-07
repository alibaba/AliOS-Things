# 自行车测速系统
&emsp;&emsp;
下图是本案例除硬件连线外的2步导学，每个步骤中实现的功能请参考图中的说明。在硬件连线完成之后我们建议您先使用“一分钟上云体验”功能预先体验本案例的实际运行效果。
<div align="center">
<img src=./../../../images/hall_步骤概述.png width=70%/>
</div>

## 简介
&emsp;&emsp;
随着经济的快速发展, 生活节奏日益加快, 由于缺少运动许多人被亚健康所困扰, 骑行运动作为新时尚, 成为越来越多人的首选运动方式。随着经济的快速发展, 生活节奏日益加快, 由于缺少运动许多人被亚健康所困扰, 骑行运动作为新时尚, 成为越来越多人的首选运动方式。随着经济的快速发展, 生活节奏日益加快, 由于缺少运动许多人被亚健康所困扰, 骑行运动作为新时尚, 成为越来越多人的首选运动方式

### 背景知识
&emsp;&emsp;
本文拟设计一种基于霍尔传感器的的简易自行车测速系统,可显示速度,并增设速度报警模块,对自行车使用者在车速过快时及时做出提醒,防止因速度过快而对骑行者和交通参与者的安全造成威胁。

&emsp;&emsp;
霍尔器件是一种磁传感器。用它们可以检测磁场及其变化，可在各种与磁场有关的场合中使用。霍尔器件以霍尔效应为其工作基础。霍尔器件具有许多优点，它们的结构牢固，体积小，重量轻，寿命长，安装方便，功耗小，频率高（可达1MHZ），耐震动，不怕灰尘、油污、水汽及盐雾等的污染或腐蚀。

&emsp;&emsp;
霍尔线性器件的精度高、线性度好；霍尔开关器件无触点、无磨损、输出波形清晰、无抖动、无回跳、位置重复精度高（可达μm 级）。取用了各种补偿和保护措施的霍尔器件的工作温度范围宽，可达－55℃～150℃。按照霍尔器件的功能可将它们分为:霍尔线性器件和霍尔开关器件。前者输出模拟量，后者输出数字量。按被检测的对象的性质可将它们的应用分为:直接应用和间接应用。前者是直接检测出受检测对象本身的磁场或磁特性，后者是检测受检对象上人为设置的磁场，用这个磁场来作被检测的信息的载体，通过它，将许多非电、非磁的物理量例如力、力矩、压力、应力、位置、位移、速度、加速度、角度、角速度、转数、转速以及工作状态发生变化的时间等，转变成电量来进行检测和控制。

&emsp;&emsp;
本节选用的是Rysym的3144E的霍尔传感器模块，支持3.3~5V宽电压输入，体积小，灵敏度高，响应速度快。温度性能好，准确度高，可靠性好。具备良好的兼容性，几乎可兼容所有的主控器。

<div align="center">
<img src=./../../../images/hall_霍尔传感器图.jpg  width=30%/>
</div>



## 准备
* 01Studio Columbus开发板一套
* MH系列3144E霍尔测速传感器模块一个
* 磁铁（圆柱形要求直径<25mm,立方体要求最长边<25mm
）
* 自行车一辆
* 杜邦连接线若干

### 硬件连线图
&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/hall_硬件连接图.png width=90%/>
</div>
<br>

### 自行车连接图
&emsp;&emsp;
将磁铁固定在车轮上，将传感器与单片机固定在车座，用线引出霍尔传感器并用线固定住，保证霍尔传感器与车轮磁铁的距离小于5mm。pc端和单片机通过type-C连接，暂时单片机要通过有线网连接，后续WiFi功能开发完毕后可通过无线联网。如下图是某共享单车的实际连接：
<div align="center">
<img src=./../../../images/hall_实际连接.jpg width=90%/>
</div>
<br>

&emsp;&emsp;
由于当前stm32仅仅支持有限联网功能，因此测试需要在网线范围内，因此仅进行车轮转动进行等效的测试。后续stm32支持WiFi联网之后，即可自由进行骑行。

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

### 创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。
<div align="center">
<img src=./../../../images/hall_创建产品.png
 width=100%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/hall_新建检测设备.png width=50%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“自行车测速”的产品，如下图所示。

<div align="center">
<img src=./../../../images/hall_产品列表.png width=100%/>
</div>

<br>

### 创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

<div align="center">
<img src=./../../../images/hall_产品详情.png width=100%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/2_甲醛检测_创建自定义物模型1.png width=100%/>
</div>

然后按照下图选择速度属性添加。
<div align="center">
<img src=./../../../images/hall_创建物模型.png width=50%/>
</div>

&emsp;&emsp;
物模型添加成功之后可以看到网页出现了我们刚刚创建的物模型属性。其中speed代表自行车的速度值，数据类型为float浮点数型。此时点击“发布”按钮，按照系统提示一步一步进行下去就可以将刚刚创建的物模型属性发布到产品中。


<div align="center">
<img src=./../../../images/hall_发布物模型.png width=100%/>
</div>
&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击”自行车测速“后的“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/hall_管理设备.png width=100%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/hall_添加设备.png width=100%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/1_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<div align="center">
<img src=./../../../images/hall_设备详情.png width=100%/>
</div>

<br>

#### **获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_设备三元组_马赛克.png width=50%/>
</div>

<br>

#### **查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“速度“属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/hall_设备详情.png width=100%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>


## 设备端开发
### 开发环境
在进行下一步之前请确保stm32 01Studio Columbus开发环境已经搭建完毕。

### 通信模式
该款霍尔传感器支持GPIO和ADC两种方式，本案例中采用了GPIO模式。


### 创建解决方案
&emsp;&emsp;
如下图所示，由于stm32暂未在VS Code之上发布。因此使用Thonny进行开发。Thonny的界面如下所示：

<div align="center">
<img src=./../../../images/hall_Thonny界面.png width=70%/>
</div>

### 案例代码
&emsp;&emsp;
将[自行车测速](./code/)文件下所有脚本代码复制到创建的“bike_speed”工程根目录下的文件。Python脚本的详细说明请参考脚本内嵌的文字版注释

1. **修改路由器名称及密码**

&emsp;&emsp;
stm32联网需要使用有线网线，连网线后使用如下的联网的方式。

```python
# 有线以太网设置
import network
.......
nic = network.LAN()
nic.active(1)
```


2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，如下图所示：

<div align="center">
<img src=./../../../images/hall_修改三元组.png
 width=80%/>
</div>

3. **修改设备端上报数据所用标识符**

&emsp;&emsp;
bike_speed工程里main.py中下面的代码实现的是上传速度检测结果到云端的功能。其中speed便是速度检测结果上报云端所用的标识符。

```python
    while True:
        #计算速度
        speed = halDev.speed_calulate()
        print("speed is", speed)
        upload_data = {'params': ujson.dumps({
            'speed': speed,
        })
        }
        if(speed > ALARM_DISTANCE):
            print("The speed is so fast!!!please control yourself!!!")
            buzzerdev.write(1)
        else:
            buzzerdev.write(0)
        #速度上传到云端
        device.postProps(upload_data)
        #每隔1s上传一次
        utime.sleep(1)
```

确保这个标识符和物联网产品的物模型中属性标识符是一样的，如下图所示：
<div align="center">
<img src=./../../../images/hall_属性标识符修改.png
 width=100%/>
</div>

<br>

4. **推送到设备端**

&emsp;&emsp;
打开设备端，点击上方菜单栏，选择运行，下拉选项中选择第一项“选择解释器”。具体配置如下：
<div align="center">
<img src=./../../../images/hall_解释器选择.png
 width=100%/>
</div>

<br>
上面解释器选择选择MicroPython（一般），端口选择自己pc机中的端口，不同的pc机端口不同，设置完成后点击确定按钮。连接打开设备端。

&emsp;&emsp;
打开设备后，分别将main.py上传到根目录，将board.json上传到data/pyamp路径下（如果文件不存在则自行创建文件），将剩余的hal55695.py和buzzer.py上传到lib目录下。以main.py为例，上传的方法如下图所示：
<div align="center">
<img src=./../../../images/hall_Thonny上传图.png width=50%/>
</div>

<br>
在本地电脑端进入源码目录，在设备端进入根目录，选中要上载的main.py，左键点击，选择上载到/。即可完成main.py的上载。同理，完成其他文件的上载也是类似的步骤。均上载完，选择main.py即可执行。

## 运行结果

### 本地查看

&emsp;&emsp;
推送此脚本到01Studio之后并运行，串口会周期性的打印如下日志：

```log
>>> %Run -c $EDITOR_CONTENT
HAL55695 sensor init success!
buzzer inited!
activation_report:245 report_data:{"activationStr":"V=3.3.0&O=Anolis Things&P=AliOS Things Team&A=HaaS Python&B=01Studio pyBoard Pro&C=STM32F407&N=Ethernet&X=HAAS Inside&S=MQTT/HTTPS&T=solo&M=02-02-78-43-59-f3&Y=aliyun"}
response data: HTTP/1.1 200 
Date: Thu, 23 Jun 2022 01:59:20 GMT
Content-Type: application/json;charset=UTF-8
Transfer-Encoding: chunked
Connection: keep-alive
Vary: Accept-Encoding
Server: Tengine
Vary: Accept-Encoding
EagleEye-TraceId: 0a98a37b16559495608273700ec898
Timing-Allow-Origin: *

24
{"success":true,"message":"success"}
0


activation report success
物联网平台连接成功
sleep time 2s
listen thread start
the time is 625
speed is 3.31785
the time is 589
speed is 3.520639
the time is 250
speed is 8.294624
The speed is so fast!!!please control yourself!!!
the time is 515
speed is 4.026517
the time is 526
speed is 3.942312
the time is 534
speed is 3.883251
the time is 514
speed is 4.03435
the time is 487
speed is 4.258021
the time is 410
speed is 5.057698
The speed is so fast!!!please control yourself!!!
the time is 351
speed is 5.907852
The speed is so fast!!!please control yourself!!!
the time is 836
speed is 2.48045

```

### 物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/hall_设备状态及属性.png width=10%/>
</div>

&emsp;&emsp;
经过一段时间的骑行，物联网平台的物模型数据会更新为设备上报的最新的属性值。通过点击查看数据，可以看到一段时间监测到的速度值。
<div align="center">
<img src=./../../../images/hall_查看数据.png width=100%/>
</div>

<br>

&emsp;&emsp;
到此为止，自行车测速的案例就已经完成了。