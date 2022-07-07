# 自动感应门
# 简介
很多公共场所采用自动感应门，当有人靠近时门会自动打开，远离时自动关门；有时候需要将门常开比如搬运货物，不方便将门关闭；或者场所夜晚关门时需要常关，不允许他人进入。

本案例模拟相应场景，设计一款可以使用手机APP来控制自动感应门的系统。
## 红外避障传感器

 - 红外避障是避障传感器的一种，具有一对红外线发射与接收管，发射管发射出一定频率的红外线，当检测方向遇到障碍物（信号输出接口输出低电平信号，可通过电位器旋钮调节检测距离）。
 - 规格参数：

	1.工作电压：DC 3.3V-5V
	2.工作电流：≥20mA
	3.工作温度：－10℃—＋50℃
	4.检测距离：2-30cm
	5.输出信号：有障碍物或反射到地面输出低电平，无障碍物高电平
	6.调节方式：多圈电阻式调节
	7.有效角度：35°
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
红外避障传感器|1
SG90舵机|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506自动感应门\zd01.png width=100%/>
</div>

# 代码流程
1、连接阿里云平台

2、启用两个线程，避障传感器每秒将检测状态上传阿里云；舵机根据条件判断是否开门。

3、创建‘移动应用’功能，控制门禁正常、常闭、常开模式，并实时检测累计人流量。

4、将门禁状态上传阿里云平台。

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
 
 
2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/auto_control_door.zip)，以下为手动添加的操作流程。

选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506自动感应门\zd02.png width=100%/>
</div>

1、**添加自定义功能**
2、按照图2显示添加**标识符**与**数据类型**（标识符要与代码一致）
3、点击**发布上线**
<div align="center">
<img src=./../../../images\506自动感应门\zd03.png width=100%/>
</div>

 - 点击确定
<div align="center">
<img src=./../../../images\506自动感应门\zd04.png width=100%/>
</div>

## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 
 - 搭建完后复制[自动感应门代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506自动感应门\zd05.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印红外避障传感器的状态**，当有人进入打印‘open the door’，控制舵机开门，并显示上传云端当前人数；当没人进入打印‘no object detected’。
<div align="center">
<img src=./../../../images\506自动感应门\zd08.png width=100%/>
</div>

2、阿里云平台，打开**实时刷新**，会显示当前 进入人数。
<div align="center">
<img src=./../../../images\506自动感应门\zd09.png width=100%/>
</div>

## 4、创建移动应用
### 创建‘普通项目’
<div align="center">
<img src=./../../../images\506自动感应门\zd10.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd11.png width=100%/>
</div>

创建完自动跳转主页，关联相应产品
<div align="center">
<img src=./../../../images\506自动感应门\zd12.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd13.png width=100%/>
</div>

返回主页，新建移动应用
<div align="center">
<img src=./../../../images\506自动感应门\zd14.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd15.png width=100%/>
</div>

自动跳转应用编辑界面，选择组件→基础组件→图表→实时曲线，或者直接搜索。
添加完成后，配置组件信息。
<div align="center">
<img src=./../../../images\506自动感应门\zd16.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd17.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd18.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd19.png width=100%/>
</div>

添加‘单选’组件，并配置组件信息。
<div align="center">
<img src=./../../../images\506自动感应门\zd20.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd21.png width=100%/>
</div>

配置交互信息。
<div align="center">
<img src=./../../../images\506自动感应门\zd22.png width=100%/>
</div>
<div align="center">
<img src=./../../../images\506自动感应门\zd23.png width=100%/>
</div>

配置完成后保存，点击预览可以查看设置好的移动应用。
<div align="center">
<img src=./../../../images\506自动感应门\zd24.png width=100%/>
</div>

手机扫描二维码，可以在手机上选择自动门模式，并且实时查看人流量。
<div align="center">
<img src=./../../../images\506自动感应门\zd25.png width=100%/>
</div>
