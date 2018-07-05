# AliOS Things LinkKit 通道自测指南
* [准备工作](#1)
  * [准备账号]()
  * [调试设备]()
* [功能测试](#2)
* [稳定性测试](#3)
* [性能测试](#4)

<a id="1"></a>
## 准备工作
进行LinkKit道测试前，需要准备阿里云账号并在阿里云飞燕平台创建测试设备，详细准备工作见下文。
### 准备阿里云账号
 [阿里云飞燕平台](https://living.aliyun.com)使用阿里云账号登录，在测试前若无阿里云账号请首先[注册](https://www.aliyun.com/)阿里云账号
### 创建测试设备
登录[阿里云飞燕平台](https://living.aliyun.com)网站，创建测试项目，在测试项目下创建测试产品
![](assets/linkkit_1.png)
![](assets/linkkit_2.png)
![](assets/linkkit_3.png)
![](assets/linkkit_4.png)
![](assets/linkkit_5.png)
![](assets/linkkit_6.png)
![](assets/linkkit_7.png)
> 上图中`ProductKey`,`DeviceName`,`DeviceScret`的组合是设备唯一性的身份标识，将三元组植入设备端，设备凭三元组连接到阿里云

### 设备调试
一站式开发平台提供设备调试的功能，当设备植入三元组连接到阿里云之后就可以通过调试功能验证设备功能是否正常。首先检查设备在线状态，若设备在线则进入设备调试。
![](assets/linkkit_8.png)
![](assets/linkkit_9.png)
设备日志显示设备接收到设置`LightSwitch`这个属性的指令
![](assets/linkkit_10.png)
同时调试界面日志打印了设备下发数据和设备上报数据
![](assets/linkkit_11.png)

### 自定义功能
Comming soon...
<a id="2"></a>
## 稳定性测试
稳定性测试用例主要是`5x24小时稳定工作`，设备上线后保持稳定工作5x24小时，通过一站式开发平台查看设备指定时间范围内设备上线次数来确定设备离线次数
![](assets/linkkit_12.png)
> 稳定测试时务必完整保存设备串口打印的日志，便于出现离线时快速定位问题

<a id="3"></a>
## 功能测试
通道的属性、服务、事件等功能测试可以通过`设备调试`验证

### 属性
设置属性
![](assets/linkkit_13.png)
设置属性后，可以通过`获取`犯法验证属性是否设置成功，或者在设备`运行状态`页面查看属性值及属性值的更新时间
![](assets/linkkit_14.png)
![](assets/linkkit_15.png)
### 服务
![](assets/linkkit_17.png)
### 事件
事件由设备主动上报，在页面上智能查询设备是否上报事件以及上报事件的时刻、输入输出参数等
![](assets/linkkit_16.png)
<a id="4"></a>
## 性能测试
Comming soon...

