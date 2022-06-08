# 光伏面板监控
&emsp;&emsp;
本案例开发需要经过如下4步。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-导览.png width=90%/>
</div>

## 1、简介

### 1.1、背景
&emsp;&emsp;
光伏发电已经被广泛使，但是光伏板在使用过程中因为镜面堆积灰尘会导致发掉效率较低，所以，需要定期清洁，那么远程决策者如何获知光伏板的工作状态，并决策光伏板是否需要进行清洁呢？ 本案例将基于HaaS506 DTU开发板以及阿里云物联网平台搭建云端一体光伏运行效率监控案例，通过本案例可以远程监控光伏发电系统的运行状态。
<br>

### 1.2、准备

**1. 硬件器材**

<div align="center">

|配件名称|数量|功能|购买链接|
|-----|:---:|:-----|----|
|HaaS506开发|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.1.712a1a7bHThfbt&id=651798670356&ns=1&abbucket=18#detail)|
|光伏发电套装（18V发电板20W + 12V控制器）|1||[点我购买](https://item.taobao.com/item.htm?spm=a230r.1.14.193.51d852b1owN1A2&id=660886029445&ns=1&abbucket=18#detai)|
|12V蓄电池|1||[点我购买](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.741c2e8dtOYS6q&id=645279478844&_u=oh5g3cj7f2b)|
|DCDC降压模块|1||[点我购买](https://detail.tmall.com/item.htm?id=19905413537&spm=a1z09.2.0.0.741c2e8dtOYS6q&_u=oh5g3cjc239)|
|1M电阻|若干||[点我购买](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.1.d4f7629475swvF&id=13302997879&skuId=3756188445694&areaId=330100&user_id=738263294&cat_id=2&is_b=1&rn=76e39d7f60e7768027b8149ac84e8bbb)|
|光照传感器|1|采集当前环境光线强度|
|杜邦线/导电线|若干||

</div>
<br>

**2. 硬件连线图**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-连线图.png
width=90%/>
</div>
<br>


## 2、物联网平台开发

物联网平台开发需要依次完成以下5步
1. 开通物联网平台实例
2. 创建云端产品
3. 定义产品功能
4. 为产品创建云端设备
5. 基于IoT Studio开发移动应用

### 2.1、开通物联网平台实例
&emsp;&emsp;
登陆[物联网平台](https://iot.console.aliyun.com/lk/summary/new)。
第一次使用物联网平台时候，首先需要在物联网平台创建一个实例。本案例选择使用免费的公共实例进行开发。如果您需要保证更多设备同时上云，需要购买企业实例。物联网平台创建公共实例的方式如下图所示， 左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=100%/>
</div>

<br>

### 2.1、创建云端产品
&emsp;&emsp;
点击上一小节创建的公共实例就可以进入实例管理页面，在页面的左侧菜单中选中“设备管理” -> “产品”菜单项开始创建物联网产品。具体创建过程如下图所示：


**1. 点击创建产品按钮**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建产品1.png
 width=100%/>
</div>

&emsp;&emsp;

**2. 填写产品基础信息以后点击“确认”按钮**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建产品2.png
 width=60%/>
</div>

&emsp;&emsp;
创建产品成功以后，页面的左侧菜单中选中“设备管理” -> “产品”菜单，可以查看已经创建的产品

### 2.2、定义产品功能

&emsp;&emsp;
点击“设备管理” -> "产品"菜单进入产品列表页，双击刚才创建的“太阳能路灯控制”产品 并 点击 “功能定义” 菜单，开始定义产品功能。首先下载[物模型文件](./link_platform/model.zip)。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能1.png width=90%>
</div>


**1. 功能定义页面点击“编辑草稿”**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能2.png width=90%>
</div>



**2. 点击“快速导入”**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能3.png width=60%>
</div>


**3. 点击“上传物模型”**

&emsp;&emsp;
选择上面下载的物模型文件进行上传。

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能4.png width=60%>
</div>


**3. 物模型成功以后，效果如下图，点击“发布上线”**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-定义产品功能5.png width=100%>
</div>

<br>

### 2.4、为产品创建云端设备


**1. 在产品列表页面中，点击”太阳能路灯监控“对应的“管理设备”按钮，进入设备管理页面。**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备1.png
 width=100%/>
</div>


**2. 点击“添加设备”按钮**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备2.png width=80%/>
</div>


**3. 弹框中不填写任何信息，直接点击“确认”完成设备添加**
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备3.png width=50%/>
</div>


**4. 创建完云端设备以后，点击“设备管理”-> “设备” 菜单可以看到刚才创建的设备， 点击设备对应的“查看”按钮进入设备详情页面。 在详情页点击“查看” 按钮获取设备三元组。 设备三元组信息需要填写到设备端代码中。**

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建云端设备4.png width=90%/>
</div>
<br>

## 3、基于IoT Studio开发移动应用


**1. 新建一个空白项目**

&emsp;&emsp;
打开[IoT Studio官网](https://studio.iot.aliyun.com/)，点击屏幕左侧“项目管理”菜单 -> 点击“新建项目”按钮 -> 点击“创建空白项目“，项目名称填写“传感器数据采集”。
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建IoTStudio项目.png width=90%/>
</div>

<br>

**2. 关联物联网产品和物联网设备”**

&emsp;&emsp;
点击“产品”旁边的“关联”按钮，然后选中前一章节创建的物联网产品完成关联。

&emsp;&emsp;
点击“设备”旁边的“关联”按钮，然后选中前一章节创建的物联网设备完成关联。

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-关联设备和产品.png width=90%/>
</div>

**3. 新建“移动应用”**

&emsp;&emsp;
点击“移动应用 ”按钮 ->点击“新建“按钮， 开始创建项目，选择“空白项目”。 
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建移动应用1.png width=90%/>
</div>

<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-创建移动应用2.png width=90%/>
</div>
&emsp;&emsp;
创建完毕以后自动跳转到应用UI可视化搭建页面。



**4. 可视化搭建”**

&emsp;&emsp;
需要从左侧组件列表中依次拖动道UI面板对应位置
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-可视化搭建.png width=90%/>
</div>

&emsp;&emsp;
参考以下步骤设定每个卡片数据源
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-设定实时曲线.png width=90%/>
</div>

<br>


## 4、设备端开发

### 4.1、搭建开发环境
&emsp;&emsp;
参考[HaaS506开发环境](../../../startup/HaaS506_startup.md)说明文档搭建软件开发环境。

&emsp;&emsp;
参考本文章开始处的“硬件连接图”连接各硬件模块。

<br>

### 4.2、创建HaaS Studio工程
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“光伏面板监控”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

### 4.3、填写三元组信息
&emsp;&emsp;
根据创建云端设备章节中获取到的设备三元组信息 修改main.py中 "productKey" "deviceName" "deviceName"三个变量。 然后点击部署运行按钮并查看运行结果。

<br>

### 4.4、部署运行
&emsp;&emsp;
点击IDE左下角的“部署运行”按钮，部署应用到haas506开发板。部署完毕以后，自动连接云平台

<br>


## 5、调试验证

&emsp;&emsp;
再次进入iot studio 移动应用界面，点击"预览"
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-预览.png width=90%/>
</div>

&emsp;&emsp;
显示光伏面板状态，触发报警
<div align="center">
<img src=./../../../images/光伏面板监测/光伏面板监测-运行效果.png width=40%/>
</div>

<br>

&emsp;&emsp;
本案例主要代码流程如下，详细请参考代码中的注释。
1. 连接网络，网络连接成功以后，HaaS506开发板网络状态LED灯会点亮
2. 连接物联网平台，并注册平台消息监听器， 连接成功以后会打印“物联网平台连接成功”
3. 周期性上报太阳能路灯信息

