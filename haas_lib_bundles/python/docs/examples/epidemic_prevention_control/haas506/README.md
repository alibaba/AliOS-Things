# 居家隔离监控系统
# 简介
新型冠状病毒持续危害着人类安全，居家隔离可以有效切断病毒的传播路径。

本系统的核心在于监测居家隔离过程中入户门的开关状态，并及时通知给社区管理人员。

# 准备
本案例需要的硬件
器材|数量
|--|--|
HaaS506开发板|1
AJ-SR04M- T-X超声波测距模块|1
SIM卡|1
杜邦线|若干
# 硬件连接图
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq4.png width=100%/>
</div>
 

# 代码流程
1、连接阿里云平台。

2、重复检测超声波距离，当距离大于40cm设定开门状态为1，将开门的状态上传云端；当距离小与40cm设定开门状态为0，将开门状态上传云端。（每次状态改变时才上传一次）。

3、创建钉钉机器人，使用IoT Studio业务逻辑根据开门状态发送对应消息。


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
 
 
2、创建产品属性（添加物模型）物模型可以导入或手动添加，[导入物模型文件](./link_platform/epidemic_prevention_control.zip)，以下为手动添加的操作流程。

选择产品**功能定义**--**编辑草稿**
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq5.png width=100%/>
</div>
 
1、**添加自定义功能**
2、按照图2显示添加**标识符**与**数据类型**（标识符要与代码一致）
3、点击**发布上线**
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq6.png width=100%/>
</div>
 
 <div align="center">
<img src=./../../../images\506居家隔离监控系统\yq7.png width=60%/>
</div>
 
 - 点击确定
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq8.png width=60%/>
</div>
 
## 2、设备端开发
- 第一次使用开发板的读者可以按照[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b92276856.0.0.17a360b1tEuLlx#/Python/docs/zh-CN/startup/HaaS506_startup)搭建开发环境。
 
 - 搭建完后复制[居家隔离监控系统代码](./code/)到Visual Studio Code，复制产品证书到代码相应位置。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq9.png width=100%/>
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

1、串口调试工具log，**网络连接成功**--**动态注册成功**--门的状态改变后**打印当前距离和门的状态**。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq10.png width=100%/>
</div>
 
2、阿里云平台，打开**实时刷新**，物模型数值会即时改变。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq11.png width=100%/>
</div>
 
## 4、创建钉钉机器人
群设置中找到智能群助手。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq12.png width=100%/>
</div>
 
添加机器人
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq13.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq14.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq15.png width=100%/>
</div>
 
点击添加
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq16.png width=100%/>
</div>
 
输入机器人名称及机器人想要添加的钉钉群组(如果没有可以自行创建)，在安全设置一项添加"自定义关键词"。

需要注意的是：自定义关键词用来过滤向钉钉群推送的信息，只有包括了该关键词的推送信息才会在群里显示，不包含该关键词的推送信息会被过滤掉。可以定义多个关键词，只要推送的信息中有一个词跟关键词匹配即可被群接收并显示。这里我们定义"入户门"关键词，也就是说后续的推送信息里面必须包含"入户门"这个词语。
提醒的两条消息“**入户门**关闭，疫情安全”，“**入户门**打开，危险”
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq17.png width=100%/>
</div>
 
 创建完成后会弹出提示框，点击``复制``按钮复制Webhook地址以备后用，使用该地址可以向钉钉群推送消息。点击“完成”。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq18.png width=100%/>
</div>
 
## 5、IoT Studio配置业务逻辑
登录[IoT Studio](https://studio.iot.aliyun.com/projects?spm=a2cpu.b17028610.0.0.1a6f60b1uGkdFA)平台，在左侧的侧边栏点击项目管理，在项目管理页面点击新建项目按钮。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq19.png width=100%/>
</div>
 
在新建项目页面点击创建空白项目。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq20.png width=100%/>
</div>
 
弹出新建空白项目页面，输入项目名称及描述后点击确定按钮。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq21.png width=100%/>
</div>
 
项目创建完成后会自动导航到新创建的项目，新项目需要关联产品，设备，并创建业务逻辑，以实现云端上报信息到钉钉的信息推送。

点击关联设备
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq22.png width=100%/>
</div>
 
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq23.png width=100%/>
</div>
 
创建业务逻辑
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq24.png width=100%/>
</div>
 
业务服务创建完成后自动导航到业务逻辑开发页面。点击“节点”按钮显示所有原子功能，并以此拖住设备触发，条件判断 和 钉钉机器人到右侧的窗口。按住鼠标左键连接各个模块，注意条件判断两个逻辑输出要连接正确的钉钉机器人，以免逻辑混乱。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq25.png width=100%/>
</div>
 
点击设备触发模块，在弹出的对话框中选择产品，设备及触发条件。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq26.png width=100%/>
</div>
 
点击条件判断模块，在弹出的对话框中条件配置。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq27.png width=100%/>
</div>
 
点击疫情安全模块，在弹出的对话框中设置Webhook，选择配置方式及消息类型。Webhook对话框输入之前创建“疫情安全”钉钉机器人时候拷贝的字符串。在“消息类型”的内容配置项里填写content内容。(内容需要包括钉钉机器人设置的"自定义关键词")
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq28.png width=100%/>
</div>
 
按照上述办法配置疫情危险模块(使用相同的Webhook信息)。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq29.png width=100%/>
</div>
 
业务逻辑开发配置完成后，我们可以先使用软件调试的方式验证相关逻辑链路的正确性。点击保存按钮后点击部署调试按钮。
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq30.png width=100%/>
</div>
 
在弹出的对话框选择前往按钮：
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq34.png width=100%/>
</div>
 
在弹出的设备模拟器页面点击启动设备模拟器按钮：
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq31.png width=100%/>
</div>
 
 选择属性上报选项并在开关状态栏输入0（门关闭）或者1（门打开），点击发送命令按钮：
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq32.png width=100%/>
</div>
 
发送命令后，我们便可以在钉钉群接收到相关的通知：
<div align="center">
<img src=./../../../images\506居家隔离监控系统\yq33.png width=100%/>
</div>
 



