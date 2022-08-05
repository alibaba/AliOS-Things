# WS2812个性氛围灯
# 简介
LED灯带广泛应用于家具、汽车、广告、照明、轮船、酒吧等行业。跳动的彩色灯光也变成了城市夜晚的靓丽风景。
本案例通过手机控制RGB灯光模式，点击相应的按钮就可以改变LED灯光模式。
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
WS2812灯带（30灯）|1
SIM卡|1
杜邦线|若干
# 硬件连接图
连接时注意灯带信号方向，本案例使用SPI控制LED灯带。

<div align="center">
<img src=./../../../images\506灯带\ws1.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。

2、上传关闭状态，编写灯光的5种不同的模式。

3、配置iot云端下发属性，当云端数据改变时，发送到设备端，再上传云端。

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
 
2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/ws2812_strip.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506灯带\ws2.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506灯带\ws3.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506灯带\ws4.png width=60%/>
</div>
 
## 2、设备端开发
 
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 - 搭建完后复制[创意灯带代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。

<div align="center">
<img src=./../../../images\506灯带\ws5.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**
<div align="center">
<img src=./../../../images\506灯带\ws8.png width=100%/>
</div>
2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506灯带\ws9.png width=100%/>
</div>

## 3、物联网应用开发
以下是物联网应用开发流程，接下来按以下流程介绍移动端应用的开发。

### 3.1新建‘普通项目’
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506灯带\ws10.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506灯带\ws11.png width=100%/>
</div>
 
### 3.2关联产品和设备
<div align="center">
<img src=./../../../images\506灯带\ws12.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506灯带\ws13.png width=100%/>
</div>
 
### 3.3新建‘移动应用’
<div align="center">
<img src=./../../../images\506灯带\ws14.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506灯带\ws15.png width=100%/>
</div>
 
点击上图红框中的‘组件’，就可以看到可用的组件列表。各组件的使用说明请参考[组件说明](https://help.aliyun.com/document_detail/181886.html)
### 3.4页面设计
将组件拖到中间画布区
<div align="center">
<img src=./../../../images\506灯带\ws16.png width=100%/>
</div>
 
**开关组件设置**
<div align="center">
<img src=./../../../images\506灯带\ws17.png width=100%/>
</div>
 
 - 配置交互
<div align="center">
<img src=./../../../images\506灯带\ws18.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506灯带\ws19.png width=100%/>
</div>
 
依次设置其他开关组件
**彩色跑马灯**
<div align="center">
<img src=./../../../images\506灯带\ws20.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506灯带\ws21.png width=100%/>
</div>
 
**蓝色跑马灯**
<div align="center">
<img src=./../../../images\506灯带\ws22.png width=100%/>
</div>
 
**反向彩色跑马灯**
<div align="center">
<img src=./../../../images\506灯带\ws23.png width=100%/>
</div>
 
**霓虹灯**
<div align="center">
<img src=./../../../images\506灯带\ws24.png width=100%/>
</div>
 
**常亮蓝色**
<div align="center">
<img src=./../../../images\506灯带\ws25.png width=100%/>
</div>
 
### 3.5预览和发布上线
在页面‘’保存‘’并点击‘预览’进行预览。
<div align="center">
<img src=./../../../images\506灯带\ws26.png width=100%/>
</div>
 
手机扫描二维码，可以在手机看查看数据。

<div align="center">
<img src=./../../../images\506灯带\ws27.png width=100%/>
</div>
 
