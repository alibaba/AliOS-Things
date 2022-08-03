# 智能风扇
# 简介
日常生活中，我们经常需要一些通过风扇降温的场景，比如说机房，电脑主机等等。人为的去打开或者关闭风扇，调节档位，操作起来不是很方便。本案例通过一个温度传感器实时的采集当前温度，然后依据温度值，动态调节风扇速度。
本案例通过一个温度传感器实时采集当前温度，然后依据温度值，动态的调节风扇的转速和开关，同时我们可以通过手机app实时的控制风扇转速所对应的温度档位，实现对风扇的远程控制，既能方便我们控制风扇，也能有效的节能，绿色环保。

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
AHT10温湿度传感器|1
电机模块|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506智能风扇\fan3.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、开机上传一次内置的温度阈值，并持续读取并上传当前温湿度传感器读取的温度数值

3、设置云端回调，接收来自云端的数据，更改程序中的判断数值
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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/smart_fan.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506智能风扇\fan4.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506智能风扇\fan5.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506智能风扇\fan6.png width=60%/>
</div>
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。

 - 搭建完后复制[智能风扇代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506智能风扇\fan7.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**重复打印当前温度，模式更改时打印更改的模式**
<div align="center">
<img src=./../../../images\506智能风扇\fan10.png width=80%/>
</div>
 
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506智能风扇\fan11.png width=100%/>
</div>
 
## 3、物联网应用开发
以下是物联网应用开发流程，接下来按以下流程介绍移动端应用的开发。
### 3.1新建‘普通项目’
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506智能风扇\fan12.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan13.png width=100%/>
</div>
 
### 3.2关联产品和设备
<div align="center">
<img src=./../../../images\506智能风扇\fan14.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan15.png width=100%/>
</div>
 
### 3.3新建‘移动应用’
<div align="center">
<img src=./../../../images\506智能风扇\fan16.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan17.png width=100%/>
</div>
 
点击上图红框中的‘组件’，就可以看到可用的组件列表。各组件的使用说明请参考[组件说明](https://help.aliyun.com/document_detail/181886.html)
### 3.4页面设计
按顺序将组件从上到下拖到中间画布区
<div align="center">
<img src=./../../../images\506智能风扇\fan18.png width=100%/>
</div>
 
**文本组件设置**
<div align="center">
<img src=./../../../images\506智能风扇\fan19.png width=100%/>
</div>
 

**开关组件设置**

 - 配置数据源
<div align="center">
<img src=./../../../images\506智能风扇\fan20.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan21.png width=100%/>
</div>
 
 - 配置交互
<div align="center">
<img src=./../../../images\506智能风扇\fan22.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan23.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan24.png width=100%/>
</div>
 
同样的方法，配置其他两个数字框
<div align="center">
<img src=./../../../images\506智能风扇\fan25.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan26.png width=100%/>
</div>
 
 - 数字框三
<div align="center">
<img src=./../../../images\506智能风扇\fan27.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan28.png width=100%/>
</div>
 
**实时曲线组件设置**
<div align="center">
<img src=./../../../images\506智能风扇\fan29.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506智能风扇\fan30.png width=100%/>
</div>
 
### 3.5预览和发布上线
在页面‘’保存‘’并点击‘预览’进行预览。
<div align="center">
<img src=./../../../images\506智能风扇\fan31.png width=100%/>
</div>
 
手机扫描二维码，可以在手机进行设置并查看实时温度数据。
<div align="center">
<img src=./../../../images\506智能风扇\fan32.png width=100%/>
</div>
 
