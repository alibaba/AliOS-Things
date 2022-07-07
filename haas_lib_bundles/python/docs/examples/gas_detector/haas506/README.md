# 燃气检测系统
# 简介
天然气是家庭烹饪取暖主要燃料，其主要成分是甲烷（CH4），同时还含有少量的丙烷，乙烷、丁烷等其他少量气体。天然气本身无色无味，密度比空气少。天然气发生泄漏难以察觉，其在空气中浓度达到5%~15%时，遇明火会发生爆炸。家庭天然气的普及，每年因使用不当、人为因素造成燃气泄漏爆炸事故接连不断。
	本案例就是以此背景教开发者一步一步打造燃气泄漏检测系统。
## MQ2气体传感器
MQ-2常用于家庭和工厂的气体泄漏监测装置，适宜于液化气、苯、烷、酒精、氢气、烟雾等的探测。故因此，MQ-2可以准确来说是一个多种气体探测器。

**原理**
MQ-2型烟雾传感器属于二氧化锡半导体气敏材料，属于表面离子式N型半导体。处于200~300摄氏度时，二氧化锡吸附空气中的氧，形成氧的负离子吸附，使半导体中的电子密度减少，从而使其电阻值增加。当与烟雾接触时，如果晶粒间界处的势垒收到烟雾的调至而变化，就会引起表面导电率的变化。利用这一点就可以获得这种烟雾存在的信息，烟雾的浓度越大，导电率越大，输出电阻越低，则输出的模拟信号就越大。

**特性**

 - MQ-2型传感器对天然气、液化石油气等烟雾有很高的灵敏度，尤其对烷类烟雾更为敏感，具有良好的抗干扰性，可准确排除有刺激性非可燃性烟雾的干扰信息。
 - MQ-2型传感器具有良好的重复性和长期的稳定性。初始稳定，响应时间短，长时间工作性能好。需要注意的是：在使用之前必须加热一段时间，否则其输出的电阻和电压不准确。
 - 其检测可燃气体与烟雾的范围是100~10000ppm(ppm为体积浓度。 1ppm=1立方厘米/1立方米)
 - 电路设计电压范围宽，24V以下均可，加热电压5±0.2V
 
**注意：如果加热电压过高，会导致输入电流过大，将内部的信号线熔断，从而器件报废。**

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
MQ2气体传感器|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506燃气检测系统\gas1.png width=100%/>
</div>
 
# 代码流程
1、连接阿里云平台。
2、重复读取MQ2气体传感器电压，将数据上传阿里云。
3、使用**移动应用**功能，显示天气、MQ2气体传感器电压变化、设备报警。
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
 

2、2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/gas_detector.zip)，以下为手动添加的操作流程。
 - 选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506燃气检测系统\gas2.png width=100%/>
</div>
 
 - **添加自定义功能**
 - 添加**标识符**与**数据类型**（标识符要与代码一致）
 - 点击**发布上线**
<div align="center">
<img src=./../../../images\506燃气检测系统\gas3.png width=100%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506燃气检测系统\gas4.png width=60%/>
</div>
 
## 2、设备端开发

- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 
 - 搭建完后复制[燃气检测系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas5.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--**每秒打印MQ2气体传感器电压**
<div align="center">
<img src=./../../../images\506燃气检测系统\gas8.png width=100%/>
</div>
 

2、云端有数据传入，打开实时刷新，显示数据会自动更新。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas9.png width=100%/>
</div>
 

## 3、物联网应用开发
以下是物联网应用开发流程，接下来按以下流程介绍移动端应用的开发。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas10.png width=60%/>
</div>
 

### 3.1新建‘普通项目’
 - 使用阿里云[IoTStudio](https://studio.iot.aliyun.com/?spm=a2cpu.b17028610.0.0.17a360b1Rb7xdd)创建项目。
 - 在项目管理新建空白项目
<div align="center">
<img src=./../../../images\506燃气检测系统\gas11.png width=100%/>
</div>
 

### 3.2关联产品和设备
<div align="center">
<img src=./../../../images\506燃气检测系统\gas12.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas13.png width=100%/>
</div>
 
### 3.3新建‘移动应用’
<div align="center">
<img src=./../../../images\506燃气检测系统\gas14.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas15.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas16.png width=100%/>
</div>
 

点击上图红框中的‘组件’，就可以看到可用的组件列表。各组件的使用说明请参考[组件说明](https://help.aliyun.com/document_detail/181886.html)
### 3.4页面设计
案例使用三个组件：
 - 实时曲线  显示传感器实时数据
 - 指示灯  显示报警状态
 - 天气  用于显示目的地天气

将三个组件拖到中间画布区
<div align="center">
<img src=./../../../images\506燃气检测系统\gas17.png width=100%/>
</div>
 
### 3.5关联数据源
关联数据源分为如下三个步骤
关联产品
关联设备
关联属性
每个组件单独设置
1、‘指示灯’组件设置
<div align="center">
<img src=./../../../images\506燃气检测系统\gas18.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas19.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas20.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas21.png width=100%/>
</div>
 
设置好了之后，设置报警
<div align="center">
<img src=./../../../images\506燃气检测系统\gas22.png width=100%/>
</div>
 
2、同样的方法设置‘实时曲线’。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas23.png width=100%/>
</div>
 
3、设置‘天气’组件，选择需要的地点。

<div align="center">
<img src=./../../../images\506燃气检测系统\gas24.png width=100%/>
</div>
 
### 3.6业务逻辑开发
业务逻辑的主要目的是为了方便用户设定物联网设备端的行为逻辑，常规的物联网系统都是在设备端固话行为逻辑，出厂后如果需要修改设备行为，则需要进行OTA升级。
新建一条燃气超阈值就报警的规则。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas25.png width=100%/>
</div>
 
系统自动进入到业务逻辑页面，点击左侧‘节点’，右侧是业务逻辑介绍。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas26.png width=100%/>
</div>
 
1、选择目标节点
此逻辑需要判断设备上传的燃气电压值，当电压值超过阈值就打开报警，否则关闭报警。需要四个节点：

 - 设备触发节点
 - 条件判断节点
 - 开启报警灯节点
 - 关闭报警灯节点
 
分别从左侧拖出需要的节点。

<div align="center">
<img src=./../../../images\506燃气检测系统\gas27.png width=100%/>
</div>
 
2、建立节点间的关联关系，拖动鼠标将节点连接
<div align="center">
<img src=./../../../images\506燃气检测系统\gas28.png width=100%/>
</div>
 
3、业务逻辑编辑

 - 设备触发节点
<div align="center">
<img src=./../../../images\506燃气检测系统\gas29.png width=100%/>
</div>
 
 - 条件判断节点
<div align="center">
<img src=./../../../images\506燃气检测系统\gas30.png width=100%/>
</div>
 
设备节点行为设定
<div align="center">
<img src=./../../../images\506燃气检测系统\gas31.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506燃气检测系统\gas32.png width=100%/>
</div>
 
业务逻辑保存和部署
<div align="center">
<img src=./../../../images\506燃气检测系统\gas33.png width=100%/>
</div>
 
### 3.7预览和发布上线
业务逻辑设定完毕之后，可以在“燃气监控报警系统”页面‘’保存‘’并点击‘预览’进行预览。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas34.png width=100%/>
</div>
 
手机扫描二维码，可以在手机看查看数据。
<div align="center">
<img src=./../../../images\506燃气检测系统\gas35.png width=100%/>
</div>
 
