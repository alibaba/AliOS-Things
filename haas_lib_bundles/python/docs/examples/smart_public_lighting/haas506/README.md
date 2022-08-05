# 智能路灯系统
# 简介
本案例打造的智能路灯系统能检测周围环境的明亮变化。光线变暗灯就会打开，光线变亮灯关闭。路灯控制系统默认设置一个亮度阈值，阈值数值100 ，可以在阿里云“场景联动”功能中更改。
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
BH1750光照强度传感器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506智能路灯\lig1.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、重复读取光照强度，将数据上传阿里云。

3、使用**场景联动**功能，根据光照强度执行开灯或关灯逻辑。

4、将开关灯逻辑发送至开发板，控制路灯开关。

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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/smart_public_lighting.zip)，以下为手动添加的操作流程。
选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506智能路灯\lig2.png width=100%/>
</div>
 
1、**添加自定义功能**
2、添加**标识符**与**数据类型**（标识符要与代码一致）
3、点击**发布上线**
<div align="center">
<img src=./../../../images\506智能路灯\lig3.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506智能路灯\lig4.png width=60%/>
</div>
 

## 2、场景联动设定
点击**规则引擎**--**场景联动**，点击**创建规则**
<div align="center">
<img src=./../../../images\506智能路灯\lig5.png width=100%/>
</div>
 
创建关灯联动规则
<div align="center">
<img src=./../../../images\506智能路灯\lig6.png width=80%/>
</div>
 
进行规则设定，点击**保存**
<div align="center">
<img src=./../../../images\506智能路灯\lig7.png width=100%/>
</div>
 


创建开灯联动规则
<div align="center">
<img src=./../../../images\506智能路灯\lig8.png width=80%/>
</div>
 
进行规则设定，点击**保存**
<div align="center">
<img src=./../../../images\506智能路灯\lig9.png width=100%/>
</div>
 
启动场景联动
<div align="center">
<img src=./../../../images\506智能路灯\lig10.png width=100%/>
</div>
 

## 3、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 - 搭建完后复制[智能路灯代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506智能路灯\lig11.png width=100%/>
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
 
## 4、调试

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印光照强度**，改变光照强度，光照低于100灯会亮起，高于100灯关闭。
<div align="center">
<img src=./../../../images\506智能路灯\lig14.png width=90%/>
</div>
 

2、阿里云平台，打开**实时刷新**，物模型数值会即时改变。
<div align="center">
<img src=./../../../images\506智能路灯\lig15.png width=100%/>
</div>
 
通过在线调试，也可以短暂关闭或开启灯光。
<div align="center">
<img src=./../../../images\506智能路灯\lig16.png width=100%/>
</div>
 
