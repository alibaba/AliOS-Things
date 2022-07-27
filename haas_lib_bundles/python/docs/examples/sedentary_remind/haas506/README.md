# 久坐提醒系统
# 简介
长期久坐会损害身体健康，本案例就是为了提醒人们不要坐太久而设计的一个提醒系统。当你长时间在工位上坐着，他会通过顶顶提醒你，让你每隔一段时间活动一下筋骨。久坐提醒设备是通过人体红外检测周围区域是否有人移动，当累计检测时长超过设定值，将会在钉钉群发来提醒，每次回到座位会重新开始计时。并且提醒时间可以自行调节，默认30分钟。
## 人体红外传感器

 - 传感器上有两个调节旋钮
 
		1、调节距离电位器顺时针旋转，感应距离增大，反之，感应距离减小。
		2、调节延时电位器顺时针旋转，感应延时加长，反之，感应延时减短。
		
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip1.png width=80%/>
</div>
 
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
人体红外传感器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip2.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、传感器检测人体活动，计算人体久坐时间，当时间超过设定值，将信号上传阿里云平台。

3、设置云端与钉钉机器人，当满足条件时发出提醒。
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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/sedentary_remind.zip)，以下为手动添加的操作流程。

选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip3.png width=90%/>
</div>
 

 - **添加自定义功能**
 - 设置**标识符**、**数据类型**、**读写类型**参数，标识符（light）要与代码保持一致。点击**确定**。
 - **发布上线**，点击**确定**。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip4.png width=90%/>
</div>
 

## 2、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 - 搭建完后复制[久坐提醒系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip5.png width=90%/>
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


## 3、调试

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**打印根据传感器状态打印信息**
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip8.png width=90%/>
</div>
 

2、阿里云平台，打开**实时刷新**，物模型会即时显示lLED当前状态。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip9.png width=90%/>
</div>
 

## 4、钉钉消息提醒
### 4.1添加钉钉机器人
在钉钉创建一个群组并进入群设置→智能助手→添加机器人→自定义。

 1
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip10.png width=90%/>
</div>

 2
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip11.png width=90%/>
</div>
 
 3
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip12.png width=90%/>
</div>
 
 4
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip13.png width=90%/>
</div>
 
 5
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip14.png width=90%/>
</div>
 

**注意：自定义关键词要与平台物模型一致**
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip15.png width=90%/>
</div>
 
复制webhook地址，后面需要用到。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip16.png width=90%/>
</div>
 
### 4.2、IoT Studio设置
进入[IoT Studio产品页](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1AlHtHl)，开通服务后
项目管理→新建项目
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip17.png width=90%/>
</div>

<div align="center">
<img src=./../../../images\506久坐提醒系统\tip18.png width=90%/>
</div>
 
添加关联产品、设备
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip19.png width=90%/>
</div>

<div align="center">
<img src=./../../../images\506久坐提醒系统\tip20.png width=90%/>
</div>
 
返回主页，新建空白业务逻辑。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip21.png width=90%/>
</div>
 
将节点拖入操作区
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip22.png width=90%/>
</div>
 
节点用线连接。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip23.png width=70%/>
</div>
 
配置设备触发节点
选择调试设备
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip24.png width=90%/>
</div>
 

配置钉钉机器人节点，复制钉钉webhook网址。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip25.png width=90%/>
</div>
 
依次点击保存、部署、发布。
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip26.png width=90%/>
</div>
 
### 4.3 在线调试
云平台在线调试发送数据1
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip27.png width=90%/>
</div>
 
钉钉群发出消息
<div align="center">
<img src=./../../../images\506久坐提醒系统\tip28.png width=90%/>
</div>
 

