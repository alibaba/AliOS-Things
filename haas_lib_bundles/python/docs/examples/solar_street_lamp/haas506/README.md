# 太阳能路灯远程监控
&emsp;&emsp;
本案例开发需要经过4步。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-导览.png width=90%/>
</div>

## 简介

&emsp;&emsp;
太阳能路灯已经被广泛使，如何远程监控已安装路灯的电池状态？ 如何优化路灯工作时长？如何保证最大限度利用太阳能资源？ 本案例将基于HaaS506 DTU开发板以及阿里云物联网平台和阿里云IoT Studio组态软件搭建云端一体太阳能路灯运行状态监控案例。


&emsp;&emsp;
在本案例中,手机小程序可以远程查看路灯的电池状态、当前光照强度、远程开关路灯、查看当前供电类型、光伏电和市电占比等信息。

<br>

## 准备

**1. 硬件器材**

<div align="center">

|配件名称|数量|功能|购买链接|
|-----|:---:|:-----|----|
|HaaS506开发|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.1.712a1a7bHThfbt&id=651798670356&ns=1&abbucket=18#detail)|
|3.3V继电器|1|通过gpio口控制线路通断|[点我购买](https://detail.tmall.com/item.htm?id=530650857798&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cj6d75)|
|光伏发电套装（18V发电板20W + 12V控制器）|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.193.51d852b1owN1A2&id=660886029445&ns=1&abbucket=18#detai)|
|12V蓄电池|1||[点我购买](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.741c2e8dtOYS6q&id=645279478844&_u=oh5g3cj7f2b)|
|12V直流供电路灯|1||[点我购买](https://detail.tmall.com/item.htm?id=668423563320&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc9a7&sku_properties=20397:3234583)|
|DCDC降压模块|1||[点我购买](https://detail.tmall.com/item.htm?id=19905413537&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc239)|
|1M电阻|若干||[点我购买](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.1.d4f7629475swvF&id=13302997879&skuId=3756188445694&areaId=330100&user_id=738263294&cat_id=2&is_b=1&rn=76e39d7f60e7768027b8149ac84e8bbb)|
|12V DC电源|1||根据灯泡功率自备|
|光照传感器|1|采集当前环境光线强度|
|杜邦线/导电线|若干||

</div>
<br>

**2. 硬件连线图**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-连线图.png width=100%/>
</div>
<br>


## 物联网平台开发

物联网平台开发需要依次完成以下5步
1. 开通物联网平台实例
2. 创建云端产品
3. 定义产品功能
4. 为产品创建云端设备
5. 基于IoT Studio开发移动应用

### 开通物联网平台实例
&emsp;&emsp;
登陆[物联网平台](https://iot.console.aliyun.com/lk/summary/new)。
第一次使用物联网平台时候，首先需要在物联网平台创建一个实例。本案例选择使用免费的公共实例进行开发。如果您需要保证更多设备同时上云，需要购买企业实例。物联网平台创建公共实例的方式如下图所示， 左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

<br>

### 创建云端产品
&emsp;&emsp;
点击上一小节创建的公共实例就可以进入实例管理页面，在页面的左侧菜单中选中“设备管理” -> “产品”菜单项开始创建物联网产品。具体创建过程如下图所示：


**1. 点击创建产品按钮**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建产品1.png
 width=100%/>
</div>

&emsp;&emsp;

**2. 填写产品基础信息以后点击“确认”按钮**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建产品2.png
 width=60%/>
</div>

&emsp;&emsp;
创建产品成功以后，页面的左侧菜单中选中“设备管理” -> “产品”菜单，可以查看已经创建的产品

### 定义产品功能

&emsp;&emsp;
点击“设备管理” -> "产品"菜单进入产品列表页，双击刚才创建的“太阳能路灯控制”产品 并 点击 “功能定义” 菜单，开始定义产品功能。首先下载[物模型文件](./link_platform/model.zip)。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能1.png width=90%>
</div>


**1. 功能定义页面点击“编辑草稿”**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能2.png width=90%>
</div>



**2. 点击“快速导入”**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能3.png width=60%>
</div>


**3. 点击“上传物模型”**

&emsp;&emsp;
选择上面下载的物模型文件进行上传。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能4.png width=60%>
</div>


**3. 物模型成功以后，效果如下图，点击“发布上线”**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-定义产品功能5.png width=100%>
</div>

<br>

### 为产品创建云端设备


**1. 在产品列表页面中，点击”太阳能路灯监控“对应的“管理设备”按钮，进入设备管理页面。**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备1.png
 width=100%/>
</div>


**2. 点击“添加设备”按钮**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备2.png width=80%/>
</div>


**3. 弹框中不填写任何信息，直接点击“确认”完成设备添加**
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备3.png width=50%/>
</div>


**4. 创建完云端设备以后，点击“设备管理”-> “设备” 菜单可以看到刚才创建的设备， 点击设备对应的“查看”按钮进入设备详情页面。 在详情页点击“查看” 按钮获取设备三元组。 设备三元组信息需要填写到设备端代码中。**

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建云端设备4.png width=90%/>
</div>
<br>

### 基于IoT Studio开发移动应用


**1. 新建一个空白项目**

&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，点击屏幕左侧“项目管理”菜单 -> 点击“新建项目”按钮 -> 点击“创建空白项目“，项目名称填写“传感器数据采集”。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建IoTStudio项目.png width=90%/>
</div>

<br>

**2. 关联物联网产品和物联网设备”**

&emsp;&emsp;
点击“产品”旁边的“关联”按钮，然后选中前一章节创建的物联网产品完成关联。

&emsp;&emsp;
点击“设备”旁边的“关联”按钮，然后选中前一章节创建的物联网设备完成关联。

<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-关联设备和产品.png width=90%/>
</div>

**3. 新建“移动应用”**

&emsp;&emsp;
点击“移动应用 ”按钮 ->点击“新建“按钮， 开始创建项目，选择“空白项目”。 
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建移动应用.png width=90%/>
</div>
&emsp;&emsp;
创建完毕以后自动跳转到应用UI可视化搭建页面。


**4. 可视化搭建”**

&emsp;&emsp;
需要从左侧组件列表中依次拖动道UI面板对应位置
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-可视化搭建.png width=90%/>
</div>

&emsp;&emsp;
设定“实时曲线组件”的数据源
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-设定实时曲线.png width=90%/>
</div>

&emsp;&emsp;
参考“实时曲线组件”数据源的设定方式完成其他组件的数据源设定

<br>


## 设备端开发

### 设备端开发流程
    1. 搭建开发环境
    2. 创建HaaS Studio工程
    3. 拷贝案例代码
    4. 填写三元组信息
    5. 部署运行


### 搭建开发环境
&emsp;&emsp;
参考[HaaS506开发环境](../../../startup/HaaS506_startup.md)说明文档搭建软件开发环境。

&emsp;&emsp;
参考本文章开始处的“硬件连接图”连接各硬件模块。

<br>

### 创建HaaS Studio工程

&emsp;&emsp;
如下图所示，打开HaaS Studio之后在新建一个基于helloworld的python工程，设定好工程名称（“solar_street_lamp”）及工作区路径之后，硬件类型选择HaaS506，点击立即创建，创建一个Python轻应用的解决方案。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-创建项目.png width=60%/>
</div>


### 拷贝案例代码

&emsp;&emsp;
将[本案例代码](./code)复制到“solar_street_lamp”工程根目录下并覆盖工程原来的同名文件。 代码详细逻辑可以参考代码中的注释。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-代码工程.png width=100%/>
</div>

<br>

### 填写三元组信息
&emsp;&emsp;
根据创建云端设备章节中获取到的设备三元组信息 修改main.py中 "productKey" "deviceName" "deviceName"三个变量。 然后点击部署运行按钮并查看运行结果。

<br>

### 部署运行
&emsp;&emsp;
点击IDE左下角的“部署运行”按钮，部署应用到haas506开发板。部署完毕以后，自动连接云平台

<br>


## 调试验证

&emsp;&emsp;
再次进入iot studio 移动应用界面，点击"预览"。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-预览.png width=90%/>
</div>

&emsp;&emsp;
移动应用机界面会实时显示环境数据，远程切换供电模式，远程开关等。
<div align="center">
<img src=./../../../images/太阳能路灯监控/太阳能路灯监控-运行效果.png width=40%/>
</div>

<br>

### 代码
本案例的代码整体流程如下，详细代码请参考代码中的注释。
1. 连接网络，网络连接成功以后，haas506开发版 网络状态led灯会点亮
2. 连接物联网平台，并注册平台消息监听器， 连接成功以后会打印“物联网平台连接成功”
3. 周期上报太阳能路灯信息
