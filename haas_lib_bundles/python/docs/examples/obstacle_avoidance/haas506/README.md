# 雷达避障系统
# 简介
近年来，随着智能汽车、智能机器人的发展，雷达避障技术得到了较快的发展。避障雷达是防撞设备的辅助装置，可以根据需要对避障距离和避障范围进行调整。避障雷达通常安装在物体可移动方向的前部位置，用于探测物体移动方向前方障碍物横截面与自身的距离，由此来判断是否让物体减速或停止移动，从而避免发生碰撞。

本系统检测前方障碍物，当距离小与设定安全距离会发出警报，并将当前距离上传云端，提示采取相应措施，以避免发生碰撞。安全距离可以通过云端进行更改。

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
AJ-SR04M- T-X超声波测距模块|1
蜂鸣器|1
SIM卡|1
杜邦线|若干

# 硬件连接图
<div align="center">
<img src=./../../../images\506雷达避障系统\ld4.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、开机将默认安全距离上传云端。

3、重复检测前方距离，当前方物体距离小与安全距离，蜂鸣器发出警报，并将数据上传阿里云。

4、传感器有效范围20cm-600cm，当超过这个范围无法检测正确距离。

5、可以通过云端自行设置蜂鸣器报警安全距离。

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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/obstacle_avoidance.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506雷达避障系统\ld5.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506雷达避障系统\ld6.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506雷达避障系统\ld7.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506雷达避障系统\ld8.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506雷达避障系统\ld9.png width=60%/>
</div>
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 - 搭建完后复制[雷达避障系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506雷达避障系统\ld10.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**打印安全距离**--**重复打印当前监测距离**。

<div align="center">
<img src=./../../../images\506雷达避障系统\ld11.png width=80%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506雷达避障系统\ld12.png width=100%/>
</div>
 
3、通过在线调试更改安全距离
<div align="center">
<img src=./../../../images\506雷达避障系统\ld13.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506雷达避障系统\ld14.png width=100%/>
</div>
 