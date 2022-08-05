# 番茄钟
# 简介
番茄工作法是一种可以有效管理时间的工具，在平时的工作学习中经常会被各种所谓的“事情”（抽根烟、吃点零食、说悄悄话等等）给打断，极大降低工作学习的效率。有了番茄工作法，我们可以在规定的番茄时间内专注于做一件事情，中途不能再做与该事情无关的事，直到番茄时间结束，然后就在系统中给这件事记录一个番茄。番茄工作法可以非常有效地提高工作学习效率.

本案例制作一个25分钟的番茄时钟,时钟有两种模式:

- 无限计时模式,倒计时到时间后,番茄数加一,记录总的番茄数,没到时间按按键时间会重启

- 手动计时模式,每次倒计时结束时钟停止,手动按按键后重新计时,没到时间按按键时间会重启

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
OLED显示屏|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506番茄钟\fq2.jpg width=100%/>
</div>

# 代码流程
1、连接阿里云平台,上传番茄数和模式默认0

2、按下按键,番茄钟开始倒计时.显示番茄数,模式,当前时间

3、倒计时结束上报当前番茄数

4、通过云端调节番茄钟模式

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
 
 
2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/model.zip).

选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506番茄钟\fq22.jpg width=100%/>
</div>

1、**导入物模型**
<div align="center">
<img src=./../../../images\506番茄钟\fq3.jpg width=100%/>
</div>

 - 发布上线
<div align="center">
<img src=./../../../images\506番茄钟\fq4.jpg width=100%/>
</div>

## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 
 - 搭建完后复制[番茄时钟代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506番茄钟\fq5.jpg width=100%/>
</div>


修改位置

aliyun.py

```python
# coding=utf-8
import network
import ujson
import utime as time
import modem
from  aliyunIoT import Device
import ota
import kv




#更改产品信息
###############################
productKey = "productKey"
productSecret = "productSecret"
###############################

```

## 3、调试

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印红外避障传感器的状态**，当有人进入打印‘open the door’，控制舵机开门，并显示上传云端当前人数；当没人进入打印‘no object detected’。
<div align="center">
<img src=./../../../images\506番茄钟\fq6.jpg width=100%/>
</div>

2、阿里云平台，打开**实时刷新**，会显示当前番茄数量和当前模式。
<div align="center">
<img src=./../../../images\506番茄钟\fq7.jpg width=100%/>
</div>
 
3、OLED显示屏显示番茄数,模式,当前倒计时时间
<div align="center">
<img src=./../../../images\506番茄钟\fq9.jpg width=100%/>
</div>
  
4、在线调试设置番茄钟模式
<div align="center">
<img src=./../../../images\506番茄钟\fq10.jpg width=100%/>
</div>
  
5、OLED显示屏模式改变
<div align="center">
<img src=./../../../images\506番茄钟\fq8.jpg width=100%/>
</div>
  