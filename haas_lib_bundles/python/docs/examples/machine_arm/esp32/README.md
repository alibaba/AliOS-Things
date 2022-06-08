# 远程控制机械臂

# 1、简介

## 1.1、背景
&emsp;&emsp;
机器人和人工智能是目前很热门的两个学科，未来也是被很看好的行业之一，而机械臂就是通往机器人和人工智能的基础。本机械臂是入门级的自由度金属机械手臂，主要包括舵机驱动板、金属机械手臂支架和四个金属齿轮舵机，同时结合HaaS开发框架，就可以通过远程控制机械臂的一些基础动作啦。

&emsp;&emsp;
本课程就手把手教开发者如何自制一台远程控制机械臂。

&emsp;&emsp;
本课程的理念：希望开发者学习完本课程以后，可以模仿本案例，打造出属于自己的机械臂，知道其运作原理，然后通过不断挖掘，打造更加智能增加进阶的智能机器人。

## 1.2、准备
&emsp;&emsp;
本案例打造需要如下硬件：
* [NodeMCU-32S](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C0A5C37AADCDB00000001&dataId=800C0A5C37AADCDB)一台<br>
* [Servo舵机](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5A3F64C2F88300000001&dataId=800C5A3F64C2F883)四个<br>
* [PCA9685多路PWM传感器](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C882BBA684B1500000001&dataId=800C882BBA684B15&s=)一个<br>
* 机械臂一套<br>
* 杜邦线若干<br>
* 外部6-12V直流电源<br>
* Micro-USB数据线一条<br>

&emsp;&emsp;
硬件连线图如下图所示：

<div align="center">
<img src=./../../../images/ESP32_machine_arm_节点图.png width=80%/>
</div>

<br>

&emsp;&emsp;
根据ESP32的硬件连线图，可以得知机械臂驱动板是PCA9685是I2C总线连接，另外机械臂驱动板需要一个6-12V直流电源单独供电。

<br>

## 2、物联网平台开发
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。

<div align="center">
<img src=./../../../images/5_3_开通公共实例.png width=80%/>
</div>

开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的设定：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

<br>

### 2.1、创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击创建产品按钮，如下图所示。

<div align="center">
<img src=./../../../images/1_创建产品.png width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（如果创建的产品品类为标准品类，可以选择标准品类），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/machine_arm_新建产品页面.png width=40%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“机械臂”的产品，如下图所示。

<div align="center">
<img src=./../../../images/machine_arm_产品列表页.png width=80%/>
</div>

<br>

### 2.2、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义，点击“添加自定义功能”，其中功能名称、标识符、数据类型和定义如下所示。

<div align="center">
<img src=./../../../images/machine_arm_产品详情页面.png width=80%/>
</div>

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合“数据定义”中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 2.3、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击“管理设备”，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/machine_arm_管理设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，如下图所示。
<div align="center">
<img src=./../../../images/1_hit_添加设备入口.png width=80%/>
</div>

&emsp;&emsp;
在“添加设备”页面中设定“deviceName”，这里开发者可以自己填入自己想设定的设备名称，也可以不填任何内容让系统自动生成设备名称，如下图所示。
<div align="center">
<img src=./../../../images/machine_arm_添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备添加完成后，点击“前往查看”按钮，就可以看到此设备端详细信息了。
<div align="center">
<img src=./../../../images/1_完成添加设备.png width=40%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组
2. 物模型属性信息

<div align="center">
<img src=./../../../images/machine_arm_设备详细信息.png width=80%/>
</div>

<br>

#### 2.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息，三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/1_三元组信息.png width=60%/>
</div>

<br>

#### 2.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备时间上报情况及设备服务调用情况，如下图所示。待物联网设备按照设备属性对应的标识符上报设备属性的时候，本图片中的“机械臂控制“等属性值就会显示设备最新的属性信息。
<div align="center">
<img src=./../../../images/machine_arm_调试信息.png width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

## 3、设备端开发

### 3.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

### 3.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“远程控制机械臂”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

&emsp;&emsp;
<b>下面是重点代码段落讲解：</b>

* PCA9685初始化以及控制
```python
def pca9685_init():
    global robotDev

    i2cObj = I2C()
    i2cObj.open("pca9685")
    robotDev=pca9685.PCA9685(i2cObj)
    print('pca9865 inited')

def left_ctrl():
    robotDev.setServo(0,-pi/2)

def right_ctrl():
    robotDev.setServo(0,pi/2)

def front_ctrl():
    robotDev.setServo(1,-pi/2)

def back_ctrl():
    robotDev.setServo(1,pi/2)

def catch_ctrl():
    robotDev.setServo(3,-pi/2)

def uncatch_ctrl():
    robotDev.setServo(3,0)
```

* 云端控制机械臂
```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    payload = ujson.loads(request['params'])
    # print (payload)
    # 获取dict状态字段 注意要验证键存在 否则会抛出异常
    if "operate_machine" in payload.keys():
        value = payload["operate_machine"]
        if value == 1:
            left_ctrl()
            print("左移")
        elif value == 2:
            right_ctrl()
            print("右移")
        elif value == 3:
            front_ctrl()
            print("前移")
        elif value == 4:
            back_ctrl()
            print("后移")
        elif value == 5:
            catch_ctrl()
            print("夹住")
        elif value == 6:
            uncatch_ctrl()
            print("放开")
        else:
            print("No operate")
```

<br>

## 4、运行结果

### 4.1、设备端运行效果
&emsp;&emsp;
推送此脚本到ESP32之后，运行此脚本，ESP32串口会周期性的打印如下日志。其中：
* pca9865 inited!”说明pca9865初始化完毕
* “start”说明加湿器模块打开
* “stop”说明加湿器模块关闭


```log
mount fs
 ==== python execute bootpy ====
enable OneMinuteOnCloud
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
ssd1306 inited!
左移
后移
右移
前移
左移
```

### 4.2、物联网平台端设备信息查看

&emsp;&emsp;
物联网设备的系统启动成功并连接到物联网平台之后，物联网平台上对应的设备状态会从”未激活状态“变为”上线“，在物模型数据标签页上会显示设备上报到物联网平台的属性值。

<div align="center">
<img src=./../../../images/设置机械臂设备属性.png width=50%/>
</div>

<div align="center">
<img src=./../../../images/查看机械臂设备状态及属性.png width=50%/>
</div>

## 5、物联网应用开发

&emsp;&emsp;
本案例通过物联网应用平台的界面控制机械臂的相关操作，开发者可以更加直观的对其操作。

<br>

### 5.1、中控大屏目标
&emsp;&emsp;
本节课程主要目标是设计一个总控大屏的web应用，能实时的设置机械臂的相关操作

<br>

### 5.2、茶花生长环境监控系统
&emsp;&emsp;
下图是一个典型的物联网应用程序开发的流程图，接下来本节就按照这个流程介绍如何花卉养植系统中控大屏的web应用程序的开发。

<div align="center">
<img src=./../../../images/1_物联网应用开发的一般流程.png width=80%/>
</div>

<br>

#### 5.2.1、**新建“普通项目”**
&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，在项目管理中创建一个空白项目，如下图所示，将此项目命名为“控制机械臂”。
<div align="center">
<img src=./../../../images/machine_arm_创建IoTStudio项目.png width=80%/>
</div>

<br>

#### 5.2.2、**新建“Web应用”**
&emsp;&emsp;
新建“普通项目”之后，在新项目的首页新建一个Web应用，命名为“机械臂实时控制”。
<div align="center">
<img src=./../../../images/machine_arm_机械臂实时控制.png width=80%/>
</div>

&emsp;&emsp;
Web应用创建成功后会进入到应用界面设计页面。
<div align="center">
<img src=./../../../images/machine_arm_页面编辑页.png width=80%/>
</div>

&emsp;&emsp;
点击上图红框中的“组件”按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html)。

<br>

#### 5.2.3、**页面设计**
&emsp;&emsp;
这里我们用到2种组件:
* 矩形
用于控制机械臂的前后左右
* 椭圆形
用于控制机械臂的抓取和松开

&emsp;&emsp;
将两种组件拖到中间的画布区，适当调整组件布局，如下图所示。

<div align="center">
<img src=./../../../images/machine_arm_初步布局.png width=80%/>
</div>

<br>

#### 5.2.4、**关联产品和设备**
&emsp;&emsp;
此时回到”控制机械臂“项目的主页，对产品和设备进行关联，如下图所示：
<div align="center">
<img src=./../../../images/machine_arm_关联产品和设备.png width=80%/>
</div>

&emsp;&emsp;
关联产品的过程如下：
<div align="center">
<img src=./../../../images/machine_arm_关联产品.png width=80%/>
</div>

&emsp;&emsp;
关联设备的过程如下：
<div align="center">
<img src=./../../../images/machine_arm_关联设备.png width=80%/>
</div>
&emsp;&emsp;
产品和设备关联完毕之后，就可以将把组件和设备的属性关联起来了。

<br>

#### 5.2.5、**关联数据源**
&emsp;&emsp;
关联数据源分为如下3个步骤，每个步骤的截图如下：
* 关联产品
* 关联设备
* 关联属性

&emsp;&emsp;
具体操作步骤如下：
1. 选中”向前“控制组件，点击右侧的“配置数据源”。

<div align="center">
<img src=./../../../images/machine_arm_front_配置数据源.png width=80%/>
</div>

2. 选择目标产品
<div align="center">
<img src=./../../../images/machine_arm_front_选择产品.png width=80%/>
</div>

3. 选择目标设备
<div align="center">
<img src=./../../../images/machine_arm_front_配置数据源_设备.png width=80%/>
</div>

4. 选择“向前“控制属性
<div align="center">
<img src=./../../../images/machine_arm_front_配置数据源_属性配置.png width=80%/>
</div>

> 其他的“向后”、“向左”、“向右”、“抓取”、“松开”与向前的配置一致，只需要修改相应的目标值。
* 向后：4<br>
* 向左：1<br>
* 向右：2<br>
* 抓取：5<br>
* 松开：6

<br>

#### 5.2.6、**发布上线**
<div align="center">
<img src=./../../../images/machine_arm_预览和发布.png width=80%/>
</div>

&emsp;&emsp;
在发布之前可以点击上图的“预览”查看应用的实际运行效果。实际运行效果如下图所示：

<div align="center">
<img src=./../../../images/machine_arm_运行效果.png width=80%/>
</div>

<br>

> 思考：本案例只设计远程控制机械臂的前后左右抓取放松等操作，开发者可以以此为基础，可以增加颜色识别传感器，这样可以通过颜色识别，抓取不同的物体，进一步让机械臂实现自动化能力，欢迎留言交流～
