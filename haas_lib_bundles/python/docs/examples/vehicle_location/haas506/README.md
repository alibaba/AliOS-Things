# GPS定位系统

# 简介
GPS为GlobalPositioningSystem(全球定位系统 )的缩写。实际上 ,这是一种全球性无线电卫星导航系统 ,由 2 4个人造卫星星座及其地面接收站组成。GPS就是利用人造卫星这类“人造星体”作为参考点来确定一个物体的位置 ,其精确程度达几米到几十米。
# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
GNSS定位模块 HT2828Z3G5L|1
SIM卡|1
杜邦线|若干
# 硬件连接图

 - 使用先使用TTL口下载程序，下载完了之后TTL口连接GPS模块。
<div align="center">
<img src=./../../../images\506gps定位系统\gps1.png width=100%/>
</div>
 
# 代码流程

1、连接阿里云平台。

2、读取GPS位置信息，上传经纬度上阿里云。

3、阿里云‘Web应用’显示位置信息。

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
 

2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/vehicle_location.zip)，以下为手动添加的操作流程。

 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506gps定位系统\gps2.png width=100%/>
</div>
 
 - **添加自定义功能**
设置**标识符**、**数据类型**、**读写类型**参数，标识符要与代码保持一致。点击**确定**。
 **发布上线**，点击**确定**。
<div align="center">
<img src=./../../../images\506gps定位系统\gps3.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506gps定位系统\gps4.png width=100%/>
</div>
 
3、产品发布，动态注册一定要打开。
<div align="center">
<img src=./../../../images\506gps定位系统\gps5.png width=100%/>
</div>
 
 - 全部打钩，点击发布
<div align="center">
<img src=./../../../images\506gps定位系统\gps6.png width=100%/>
</div>
 
## 2、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 - 搭建完后复制[GPS定位系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506gps定位系统\gps7.png width=100%/>
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
1、由于TTL串口需要连接GPS模块，使用USB port5号口对应串口读取log

串口调试工具log，**传感器每秒钟打印GPS读取信息**。
<div align="center">
<img src=./../../../images\506gps定位系统\gps10.png width=100%/>
</div>
 
## 4、Web应用
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)实现设备位置在地图上实时显示。
### 新建‘普通项目’
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506gps定位系统\gps11.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506gps定位系统\gps12.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506gps定位系统\gps13.png width=100%/>
</div>
 
### 关联产品
 - 在项目控制台，点击左侧‘产品’点击‘关联物联网平台产品’。并勾选‘关联产品同时关联其下所有设备’，以便该项目可以访问到所有设备的定位信息。
<div align="center">
<img src=./../../../images\506gps定位系统\gps14.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506gps定位系统\gps15.png width=100%/>
</div>
 
###  创建‘Web应用’
 - 返回上一页，在项目控制台，在新项目的首页新建一个Web应用
<div align="center">
<img src=./../../../images\506gps定位系统\gps16.png width=100%/>
</div>
 
 - Web应用创建成功后自动跳转到应用界面设计页面。点击左侧栏‘组件’按钮图标，就可以看到可用的组件列表。各组件的说明请参考[IoT Studio组件说明](https://help.aliyun.com/document_detail/125196.html?spm=a2cpu.b17028610.0.0.17a360b1r241eh)
 - 为了能够将设备显示在地图上，选用‘设备地图’组件。鼠标悬停在组件上可以查看详细信息。
<div align="center">
<img src=./../../../images\506gps定位系统\gps17.png width=100%/>
</div>
 
 - 将‘设备地图’组件拖拽至画布，并调整大小。点击右侧‘编辑设备地图’
<div align="center">
<img src=./../../../images\506gps定位系统\gps18.png width=100%/>
</div>
 
 - ‘选择产品’，添加产品，点击确定。
<div align="center">
<img src=./../../../images\506gps定位系统\gps19.png width=100%/>
</div>
 
 - 此时，地图上出现了定位点，点击产品，可以看到绑定的所有设备。点击对应设备可以在地图上定位到每个设备的具体位置。
<div align="center">
<img src=./../../../images\506gps定位系统\gps20.png width=100%/>
</div>
 
 - 点击产品上设置，‘配置属性’，勾选‘经度’、‘纬度’点击 确定。
<div align="center">
<img src=./../../../images\506gps定位系统\gps21.png width=100%/>
</div>
 
 - 点击**保存**
<div align="center">
<img src=./../../../images\506gps定位系统\gps22.png width=100%/>
</div>
 
### 发布上线
回到Web应用页面，可以看到地图上的设备定位已经正常显示，可以进行**发布**。在发布前可以点击上图的**预览**查看运行效果。
<div align="center">
<img src=./../../../images\506gps定位系统\gps23.png width=100%/>
</div>
 
设备上显示出上传云端的经纬度。
<div align="center">
<img src=./../../../images\506gps定位系统\gps24.png width=100%/>
</div>
 


