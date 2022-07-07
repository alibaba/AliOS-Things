# 声控灯
# 简介
声控灯是通过声音控制开关的技术，适用于使用声音控制开灯的场景，特别是楼宇、住宅的公共场合，它可以减少人工开关电灯的麻烦，解放双手，避免双手接触公共开关，降低接触细菌的可能，疫情器件更是可以减少很多麻烦。
本案例实现一个简单的灯光控制功能，并把不同的灯光效果上传云端。
## 声音传感器
声音传感器的作用相当于一个话筒（麦克风）。它用来接收声波，显示声音的振动图像，但不能对噪声的强度进行测量。声音传感器输出高低电平信号，但本案例使用的是ADC端口。
<div align="center">
<img src=./../../../images\506声控灯\sou1.png width=80%/>
</div>
 
使用说明:
 - 1.声音模块对环境声音强度最敏感，一般用来检测周围环境的声音强度
- 2.在环境声音强度达不到设定阈值时，模块DO口输出高电平，当外界环境声音强度超过设定阈值时，模块DO输出低电平；
 - 3.输出DO可与单片机直接相连，通过单片机来检测高低电平，由此来检测环境的声音,可				直接制动继电器模块，组成声控开关。
 - 4.可通过电位器调节灵敏度。
 - 5.有电源指示灯，比较器输出有指示灯。
 - 6.只能识别声音的有无（根据震动原理）不能识别声音的大小或者特定频率的声音。   
 
 输出形式  数字开关量输出（0和1高低电平）
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
声音传感器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506声控灯\sou2.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、检测到声音时，轮流出现不同的灯光效果

3、将相应的效果上传云端

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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/sound_led.zip)，以下为手动添加的操作流程。
选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506声控灯\sou3.png width=100%/>
</div>
 
**添加自定义功能**
<div align="center">
<img src=./../../../images\506声控灯\sou4.png width=100%/>
</div>
 
设置**标识符**、**数据类型**、**读写类型**参数，标识符（light）要与代码保持一致。点击**确定**。
 - **发布上线**，点击**确定**。
<div align="center">
<img src=./../../../images\506声控灯\sou5.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506声控灯\sou6.png width=70%/>
</div>
 

## 2、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 - 搭建完后复制[声控灯代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506声控灯\sou7.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**当声音超过阈值，开启LED，每次开启不同模式**
<div align="center">
<img src=./../../../images\506声控灯\sou10.png width=80%/>
</div>
 
2、阿里云平台，打开**实时刷新**，物模型会即时显示lLED当前状态。
<div align="center">
<img src=./../../../images\506声控灯\sou11.png width=100%/>
</div>
 
