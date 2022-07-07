# 家庭入侵报警系统

# 简介
 - 对于一些缺乏安全感的人而言，即使在家睡觉的时候也会担心会不会有人非法入侵到家里，对自己的人身和财产构成威胁。如果有一套便宜，便捷的入侵报警系统，能够在发现入侵人员的第一时间发出警报既能够起到威慑入侵者，让他们知难而退的作用，也能够第一时间通知到自己，起到提前准备自卫的作用，从而尽可能避免和减少损失。
 - 本案例通过一个5.8G雷达感应传感器模块来检测是否有人入侵，检测到了以后，就会通过一个蜂鸣器进行报警。同时为了避免误报，我们可以通过手机app来动态控制报警时间段以及开关警报的操作，让报警系统满足我们每个人的需求。

## 无源蜂鸣器
无源蜂鸣器内部没有激励源，只有给它一定频率的方波信号，才能让蜂鸣器的振动装置起振，从而实现发声，同时，输入的方波频率不同，发出的声音也不同（所以无源蜂鸣器可以模拟曲调实现音乐效果）。下图为无源蜂鸣器的工作原理图
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in1.png width=70%/>
</div>
 
本案例**使用PWM端口来驱动蜂鸣器。**
无源蜂鸣器需要使用2k~5k频率的脉冲，HaaS506开发板上使用PWM0进行驱动。

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
5.8G雷达感应传感器模块|1
无源蜂鸣器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in2.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、上传云端报警状态，报警开始时间与结束时间。

3、检测报警状态，当前时间和传感器状态，当同时满足三个条件，开始报警。

4、当关闭报警状态才能关掉报警系统。

5、上传云端下发的数据，实现设备端与云端数据统一。
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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/home_intrusion_alarm.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in3.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in4.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in5.png width=70%/>
</div>
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 - 搭建完后复制[家庭入侵报警系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in6.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印工作状态，当全部为真时开始报警**
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in9.png width=100%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in10.png width=100%/>
</div>
 
## 3、物联网应用开发
以下是物联网应用开发流程，接下来按以下流程介绍移动端应用的开发。

### 3.1新建‘普通项目’
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in11.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in12.png width=100%/>
</div>
 

### 3.2关联产品和设备
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in13.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in14.png width=100%/>
</div>
 
### 3.3新建‘移动应用’
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in15.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in16.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in17.png width=100%/>
</div>
 
点击上图红框中的‘组件’，就可以看到可用的组件列表。各组件的使用说明请参考[组件说明](https://help.aliyun.com/document_detail/181886.html)
### 3.4页面设计
案例使用三个组件：
将三个组件拖到中间画布区
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in18.png width=100%/>
</div>
 
1、文字编辑
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in19.png width=100%/>
</div>
 
2、开关组件设置
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in20.png width=100%/>
</div>
 
依次配置产品设备信息，开关用来控制‘’报警开关‘’，点击确定。
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in21.png width=100%/>
</div>
 
3、数字框组件设置
先配置组件数据来源，在设置交互
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in22.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in23.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in24.png width=100%/>
</div>

<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in25.png width=100%/>
</div>
 
配置交互完成
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in26.png width=100%/>
</div>
 
同理，报警结束时间同样设置
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in27.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in28.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in29.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in30.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in31.png width=100%/>
</div>
 
### 3.5预览和发布上线
在页面‘’保存‘’并点击‘预览’进行预览。
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in32.png width=100%/>
</div>
 
手机扫描二维码，可以在手机看查看数据。
<div align="center">
<img src=./../../../images\506家庭入侵报警系统\in33.png width=100%/>
</div>
 
测试警报开关与工作时间

 - 关闭警报开关，蜂鸣器无反应。
 - 打开警报开关，当前时间不在工作时间内，蜂鸣器无反应。
 - 打开警报开关，在工作时间内，传感器检测到人体活动，开始报警。