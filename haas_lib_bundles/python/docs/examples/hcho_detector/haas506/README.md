# 甲醛浓度检测系统
# 简介
甲醛是一种有特殊刺激气味的气体，对人的眼睛有强烈的刺激作用。若空气中甲醛浓度过高，比如新装修的房子、新买的汽车等，可引起中毒反应，严重的可致癌。家去拿时空气中的杀手之一，需要时刻提防。本案例的核心在于准确测量家庭中的甲醛浓度。

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
电化学甲醛模组|1
SIM卡|1
杜邦线|若干
# 硬件连接图

 - 使用TTL口下载完成后，用TTL口连接甲醛传感器进行数据读取。

<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh5.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、上传云端报警状态，重复读取传感器数据，并上传云端。
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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/hcho_detector.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh6.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh7.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh8.png width=60%/>
</div>
 

 - 点击确定
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh9.png width=60%/>
</div>
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 
 - 搭建完后复制[甲醛浓度检测系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh10.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印读取的数字及PPM数值**
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh13.png width=90%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh14.png width=100%/>
</div>
 
点击查看数据，可以看到每次上传的具体数值。
<div align="center">
<img src=./../../../images\506甲醛浓度检测\ocoh15.png width=80%/>
</div>
 

