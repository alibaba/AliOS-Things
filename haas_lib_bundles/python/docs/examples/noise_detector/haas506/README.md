# 噪音检测系统
# 简介
当今社会，噪音污染已经影响到了人们日常生活的方方面面，办公室的噪音污染会影响员工的工作效率，生活中的噪音勿扰会影响大家的睡眠和情绪，从而影响我们的身心健康。

噪音检测系统目的是检测办公室和生活环境的噪音，当噪音超过设定的峰值时，发出报警。根据报警提示，办公室里的人可以通过降低打电话，讨论的声音，或者主动到会议室去，以避免打扰别人；家里的人也可以据此作出相应的对策，比如关门窗，联系物业或者警察等。

噪声标准

 1. 比较安静： 30～40 分贝
 2. 影响睡眠和休息：50分贝以上
 3. 影响工作效率： 70分贝以上
 4. 严重影响听力或导致其他疾病： 长期生活在90分贝以上环境

为了检测室内环境中声音的大小，本场景的示意图如下图所示，场景设计的行为如下：

 1. 通过声音传感器测量环境声音大小
 2. 当声音低于90分贝时，根据不同的声音大小亮不同颜色的灯：
   - 小于40分贝亮绿色
   - 50～70分贝亮蓝色
   - 大于70分贝亮红色
 3. 当声音大于90分贝时打开蜂鸣器

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
MK002508声音传感器|1
RGB三色LED灯|1
蜂鸣器|1
SIM卡|1
杜邦线|若干

# 硬件连接图

<div align="center">
<img src=./../../../images\506噪音检测系统\zy1.png width=100%/>
</div>
 
# 代码流程

1、连接阿里云平台。

2、重复读取声音传感器数据，将数据上传阿里云。

3、使用**业务逻辑**功能，根据声音分贝数值控制对应灯光与蜂鸣器。

## 1、物联网平台开发
第一次使用物联网平台的读者，需要开通实例后使用物联网平台功能。也可以使用免费的公共实例进行开发，在[阿里云物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择‘华东2-上海’，点击‘公共实例’，即可开通。

1、平台产品创建

① 登录[物联网平台控制台](https://iot.console.aliyun.com/lk/summary/new)
<div align="center">
<img src=./../../../images\506公共实例创建\1.png width=100%/>
</div>
 
② 在实例概览页面，找到对应的实例，单击实例进入实例详情页面。

**注意 在中国地域，目前仅华东2（上海）地域开通了公共实例服务。**

在左侧导航栏，选择设备管理 > 产品，单击创建产品。
<div align="center">
<img src=./../../../images\506公共实例创建\2.png width=100%/>
</div>
 
 输入产品信息
 <div align="center">
<img src=./../../../images\506公共实例创建\3.png width=100%/>
</div>
 
③ 添加设备
 <div align="center">
<img src=./../../../images\506公共实例创建\4.png width=100%/>
</div>
 
**注意：设备名称需要输入IMEI号，代码需要使用设备名称IMEI进行动态注册**
 <div align="center">
<img src=./../../../images\506公共实例创建\5.png width=100%/>
</div>
 
 点击确定
 <div align="center">
<img src=./../../../images\506公共实例创建\6.png width=100%/>
</div>
 
 ④ 打开动态注册，要连接云平台必须打开。在这个界面也可以查看需要复制的产品信息。
  <div align="center">
<img src=./../../../images\506公共实例创建\7.png width=100%/>
</div>
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/noise_detector.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506噪音检测系统\zy2.png width=100%/>
</div>
 
- 添加自定义功能

- 添加标识符与数据类型（标识符要与代码一致）

- 点击发布上线

 <div align="center">
<img src=./../../../images\506噪音检测系统\zy3.png width=100%/>
</div>
 
-选择读写类型
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy4.png width=100%/>
</div>
 
 - 点击确定
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy5.png width=100%/>
</div>
 
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 
 - 搭建完后复制[噪音检测系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy6.png width=100%/>
</div>
 
修改位置
```python
if __name__ == '__main__':
    ICCID=None
    g_connect_status = False
    net = None
    device = None
    deviceSecret = None
    deviceName = None
	#复制产品证书内容替换
    productKey = "your-productKey"
    productSecret = "your-productSecret"
    device_dyn_resigter_succed = False

```

### 调试

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**重复打印当前音量**。
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy7.png width=100%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy8.png width=100%/>
</div>
 
## 4、物联网应用开发

 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
### 4.1创建项目
 - 在项目管理新建空白项目
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy9.png width=100%/>
</div>
 

### 4.2关联产品和设备
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy10.png width=100%/>
</div>
 
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy11.png width=100%/>
</div>
 
### 4.3业务逻辑开发
业务逻辑的主要目的是为了方便用户设定物联网设备端的行为逻辑，常规的物联网系统都是在设备端固话行为逻辑，出厂后如果需要修改设备行为，则需要进行OTA升级。
新建一条噪音监测的规则。

 <div align="center">
<img src=./../../../images\506噪音检测系统\zy12.png width=100%/>
</div>
 
系统自动进入到业务逻辑页面，点击左侧‘节点’，右侧是业务逻辑介绍。
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy13.png width=100%/>
</div>
 
本案例是个多分支的选择结构，需要用到多个条件判断。逻辑如下
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy14.png width=100%/>
</div>
 
将所需节点拖入画布区，用线连接
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy15.png width=100%/>
</div>
 
节点配置
需要配置的节点比较多，一个一个来配置
* 设备触发节点
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy16.png width=100%/>
</div>
 
* 条件判断1
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy17.png width=100%/>
</div>
 
* 条件判断2
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy18.png width=100%/>
</div>
 
* 条件判断3
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy19.png width=100%/>
</div>
 
* 设备1
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy20.png width=100%/>
</div>
 
* 设备2
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy21.png width=100%/>
</div>
 
* 设备3
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy22.png width=100%/>
</div>
 
* 设备4
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy23.png width=100%/>
</div>
 
### 4.4保存与部署
 <div align="center">
<img src=./../../../images\506噪音检测系统\zy24.png width=100%/>
</div>
 
