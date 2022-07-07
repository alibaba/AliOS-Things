# 火焰检测系统
# 简介
火力发电系统会持续对炉膛中煤的燃烧情况进行监控，防止出现煤熄灭的情况，本案例针对火力发电中防止煤熄灭的场景进行实时监控，如果煤熄灭就报警。
当然也可以反向操作，作为对失火场景的监控。
## 火焰传感器
**1、原理**
火焰传感器通过检测火焰中产生的红外线来监测是否着火。远红外火焰传感器能够探测到波长在700纳米～1000纳米范围内的红外光，探测角度为60，其中红外光波长在880纳米附近时，其灵敏度达到最大。远红外火焰探头将外界红外光的强弱变化转化为电流的变化，通过A/D转换器后可以监测。
**2、器件说明**
市面上买到的有3根引脚的也有4根引脚的。案例适用4个引脚的。以下有几点需要说明：

 - **电源指示LED**--显示是否通电 	
 - **模拟信号输出**--输出模拟电压值 	
 - 红外线越强**模拟信号输出**越小，反之越大。
 - **灵敏度调节电位器**--用来调节判断阈值。
		当**模拟信号输出**大于该阈值（红外线弱，没火），**开关信号输出**为1（高电平），**开关指示LED**灯灭；
		当**模拟信号输出**小于该阈值（红外线强，着火），**开关信号输出**为0（低电平），**开关指示LED**灯亮。

<div align="center">
<img src=./../../../images\506火焰传感器\fir1.png width=80%/>
</div>
 
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
火焰传感器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506火焰传感器\fir2.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、重复读取火焰传感器电压，将数据上传阿里云。

3、使用**Web应用**功能，显示设备位置、火焰传感器电压变化、设备报警。

4、将报警信号发送至开发板，控制报警开关。

# 功能实现
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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/fire_detector.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506火焰传感器\fir3.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506火焰传感器\fir4.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506火焰传感器\fir5.png width=50%/>
</div>
 

## 2、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 
 - 搭建完后复制[火焰检测系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506火焰传感器\fir6.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印火焰传感器电压**
<div align="center">
<img src=./../../../images\506火焰传感器\fir9.png width=100%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506火焰传感器\fir10.png width=100%/>
</div>
 
## 3、物联网应用开发
以下是物联网应用开发流程，接下来按以下流程介绍Web端应用的开发。
<div align="center">
<img src=./../../../images\506火焰传感器\fir11.png width=60%/>
</div>
 
### 4.1新建‘普通项目’
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506火焰传感器\fir12.png width=100%/>
</div>
 
###  4.2创建‘Web应用’
 - 在新项目的首页新建一个Web应用
<div align="center">
<img src=./../../../images\506火焰传感器\fir13.png width=100%/>
</div>
 
 - Web应用创建成功后自动跳转到应用界面设计页面。点击左侧栏‘组件’按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html?spm=a2cpu.b17028610.0.0.17a360b1r241eh)
<div align="center">
<img src=./../../../images\506火焰传感器\fir14.png width=100%/>
</div>
 
### 4.3页面设计
添加三个组件：

	实时曲线 显示火焰传感器电压数据变化
	指示灯 提示报警状态
	设备地图 显示设备位置

 - 可以搜索组件，然后拖到画布区
<div align="center">
<img src=./../../../images\506火焰传感器\fir15.png width=100%/>
</div>
 
### 4.4关联产品
 - 在项目控制台，点击左侧‘产品’点击‘关联物联网平台产品’。并勾选‘关联产品同时关联其下所有设备’，以便该项目可以访问到所有设备的定位信息。
<div align="center">
<img src=./../../../images\506火焰传感器\fir16.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506火焰传感器\fir17.png width=100%/>
</div>
 
### 4.5关联数据源
关联数据源3个步骤：

	关联产品
	关联设备
	关联属性

 - 接下来对每个组件进行设置
1、‘指示灯’，点击配置数据源。
<div align="center">
<img src=./../../../images\506火焰传感器\fir18.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506火焰传感器\fir19.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506火焰传感器\fir20.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506火焰传感器\fir21.png width=100%/>
</div>
 
 - 修改‘指示灯’展示样式，‘报警’/‘停止报警’用不同图片显示。(发电系统正常工作着火，熄火报警用灰色图标显示）
<div align="center">
<img src=./../../../images\506火焰传感器\fir22.png width=100%/>
</div>
 

 - 火焰图片如下：
 
<figure>
<img src=./../../../images\506火焰传感器\fir23.png width=30%/>
<img src=./../../../images\506火焰传感器\fir24.png width=30%/>
</figure>


2、同样方法设置‘实时曲线’显示当前的火焰传感器电压。
<div align="center">
<img src=./../../../images\506火焰传感器\fir25.png width=100%/>
</div>
 
3、‘设备地图’设置属性
<div align="center">
<img src=./../../../images\506火焰传感器\fir26.png width=100%/>
</div>
 

给设备进行点位
<div align="center">
<img src=./../../../images\506火焰传感器\fir27.png width=100%/>
</div>
 
保存设备的位置信息，查看每个位置的设备信息。
<div align="center">
<img src=./../../../images\506火焰传感器\fir28.png width=100%/>
</div>
 
配置设备显示信息，再保存一次
<div align="center">
<img src=./../../../images\506火焰传感器\fir29.png width=100%/>
</div>
 
### 4.6业务逻辑开发
业务逻辑的主要目的是让用户设定物联网设备端的行为逻辑，常规的物联网系统都是在设备端固化行为逻辑，出厂后修改设备行为需要OTA升级，需要与开发者沟通。云端的业务逻辑用户可以自行更改。

 - 新建一条业务逻辑
<div align="center">
<img src=./../../../images\506火焰传感器\fir30.png width=100%/>
</div>
 
 - 创建完成后，系统会自动进入到业务逻辑界面。点击节点开始逻辑设计。
<div align="center">
<img src=./../../../images\506火焰传感器\fir31.png width=100%/>
</div>
 

逻辑需求，对上报的火焰传感器电压进行判断，高于设定数值报警，否则关闭报警。
需要设定一下四个节点：

		设备触发节点
		条件判断节点
		开启报警节点
		关闭报警节点
①从左侧拖出相应节点
<div align="center">
<img src=./../../../images\506火焰传感器\fir32.png width=100%/>
</div>
 
②建立节点间的关联关系，拖动鼠标将节点连接
<div align="center">
<img src=./../../../images\506火焰传感器\fir33.png width=70%/>
</div>
 
③业务逻辑编辑

	设备触发设置
<div align="center">
<img src=./../../../images\506火焰传感器\fir34.png width=100%/>
</div>
 

	条件判断设置
		用户可以在这个位置设定判断阈值
<div align="center">
<img src=./../../../images\506火焰传感器\fir35.png width=100%/>
</div>
 

	设备行为设定
<div align="center">
<img src=./../../../images\506火焰传感器\fir36.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506火焰传感器\fir37.png width=100%/>
</div>
 

 - 依次点击保存与部署
<div align="center">
<img src=./../../../images\506火焰传感器\fir38.png width=100%/>
</div>
 
### 4.7预览与发布上线
 - 返回到火焰监控报警界面，点击保存，点击预览后可以看到界面效果。
<div align="center">
<img src=./../../../images\506火焰传感器\fir39.png width=100%/>
</div>
 
 - 可以看到设备位置，当前传感器状态，报警状态，传感器电压的变化曲线信息
<div align="center">
<img src=./../../../images\506火焰传感器\fir40.png width=100%/>
</div>
 
### 4.8在线调试
通过阿里云平台在线调试也可以远程控制报警开关。
<div align="center">
<img src=./../../../images\506火焰传感器\fir41.png width=100%/>
</div>
 
