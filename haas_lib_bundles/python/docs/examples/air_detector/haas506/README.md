# 气体检测系统
## 1.简介
&emsp;&emsp;
本案例将向大家介绍如何使用Python语言在HaaS 506上实现读取传感信息，上报到阿里云物联网平台，并实现云端下发控制继电器。

## 2.准备
+ 硬件环境
<br>
(1)	haas506开发板 Usb转uart模块，Hy转杜邦线4pin 6pin线材，购买链接：
https://item.szlcsc.com/2794903.html
<br>
(2)	继电器，购买链接：
https://item.taobao.com/item.htm?spm=a1z09.2.0.0.17d92e8d3T5rbF&id=596058726555&_u=5msb3vr09cb
<br>
(3)	空气传感器，购买链接：https://item.szlcsc.com/2794903.html
<br>
+ 软件环境
<br>
(1)	Haas506固件，下载链接：https://hli.aliyuncs.com/o/config/haas506/haas506.zip
<br>
(2)	Haas506固件烧录方式，烧录方式链接：
https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/quickstart_haas506.html#i
<br>
(3)	Vs code版本应大于1.57，下载链接：https://code.visualstudio.com/
<br>
(4)	Haas studio Vs code插件，安装方法：
https://blog.csdn.net/HaaSTech/article/details/118997742
<br>
(5)	Haas506快速开始，查看链接：
https://blog.csdn.net/HaaSTech/article/details/119379840
<br>
(6)	Haas506 Python api手册，查看链接：
https://g.alicdn.com/HaaSAI/PythonDoc/library/index.html

## 3.涉及知识点
&emsp;&emsp;（1）uart通讯原理
<br>
&emsp;&emsp;&emsp;&emsp;串口全称叫做串行接口，通常也叫做 COM 接口，串行接口指的是数据一个一个的顺序传输，通信线路简单。使用两条线即可实现双向通信，一条用于发送，一条用于接收。串口通信距离远，但是速度相对会低，串口是一种很常用的工业接口。
<br>
&emsp;&emsp;(2)	空气传感器-TB600B
<br>
&emsp;&emsp;&emsp;&emsp;TB600B 系列 SMELL异味检测模组汇集了诸多来自德国的高精度检测技术，以及德国团队的设计理念，核心传感器采用全球电化学领域体积最小的德国 EC Sense 固态聚合物传感器。可以替代我们的鼻子夫精准嗅出气体浓度，实现精准监测。UART数字式信号输出，省去了客户对传感器应用的了解，以及校准的繁琐工作。应用领域：家居装修材料释放异味检测、养殖场环境恶臭检测、空气新鲜测量评估、办公楼宇及公共空间异味检测、污水恶臭，垃圾恶臭检测、厕所环境异味检测、口腔异味检测、汽车装饰材料异味检测、纺织品异味检测。
 <br>
&emsp;&emsp;(3)	阿里云物联网平台
<br>
&emsp;&emsp;&emsp;&emsp;阿里云物联网平台为设备提供安全可靠的连接通信能力，向下连接海量设备支撑设备数据采集上云；向上提供云端API，服务端通过调用云端API将指令下发至设备端，实现远程控制。
<br>

## 4.代码流程图
&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_1.jpg width=80%/>
</div>
<br>



## 5.接线方式
&emsp;&emsp;（1）空气传感器连接4pin HY线材对接HaaS506 开发板TTL接口(J8)。
注意事项：空气传感器接收接HaaS506开发板发送(TX),空气传感器发送接HaaS506开发板接收(RX),空气传感器地接HaaS506开发板GND，空气传感器VCC接HaaS506	开发板3.3V
<br>
&emsp;&emsp;（2）	将6pin HY线材插入SPI接口(J9)。
 注意事项：继电器VCC接Haas506开发板3.3V，继电器Gnd接HaaS506开发板GND,继电器In1接HaaS506开发板MOSI。HaaS506开发板CS接HaaS506开发板GND。
<br>

## 6.使用
+ 阿里云物联网产品创建

&emsp;&emsp;&emsp;&emsp;请参见如下文档：
<br>
&emsp;&emsp;&emsp;&emsp;https://help.aliyun.com/document_detail/156179.html?spm=5176.12818093.help.dexternal.243316d0YqXqgu
<br>
+ 创建IOT Studio移动端应用
  
&emsp;&emsp;&emsp;&emsp;（1）创建阿里云物联网产品
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;A：在公共实例中，根据项目需求创建产品：

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_2.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;注意：在选择联网方式时，请选择蜂窝（2G / 3G / 4G / 5G）；创建设备时建议使用模块镭雕上的15-17位IMEI号作为设备DeviceName，后期产品可	以使用模块唯一识别号IMEI进行动态注册。
<br>

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_3.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;B:导入物模型数据

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_4.jpg width=80%/>
</div>
<br>

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_5.jpg width=80%/>
</div>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_6.jpg width=80%/>
</div>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_7.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;导入完成后如下图所示：
<br>
&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_8.jpg width=80%/>
</div>
<br>
<br>
&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_9.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;c:添加设备
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_10.jpg width=80%/>
</div>
<br>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_11.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;选择刚创建好的设备，点击查看设备物模型（此处为案例演示设备，显示的为测试数据，需自行上报数据）

<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_13.jpg width=80%/>
</div>
<br>

+ 创建移动端应用


&emsp;&emsp;&emsp;&emsp;（1）打开Iot Studio开发界面
产品和设备创建完成后，在控制台选择增值服务，进入Iot Studio开发界面。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_14.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;（2）新建移动端应用如下图所示，新建一个移动端的应用（此处的项目为事先创建好的，如有所需	也可以新建项目）
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_15.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;（3）创建移动端应用，点击确定以后会自动进入创建好的移动端应用中，选择创建空白页面。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_16.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;完成创建后进入正式页面编辑界面。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_17.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;（4）选择组件，在组件中选择想要的基础组件，将拖拽至页面中，进行相应的布局设置。（本案例中只用到折线图和开关控制组件）

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_18.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;（5）组件配置，选择好对应的组件后，还需要对其进行数据源的配置。
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;A：点击组件，在右侧样式中选择配置数据源
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_19.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;B：数据源配置选择

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_20.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;C：选择关联产品，关联刚刚创建的产品HaaS506
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_21.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;D：选择关联设备

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_22.jpg width=80%/>
</div>
<br>

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_23.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;E：控制开关组件数据源配置,返回移动端开发界面，刷新列表即可进入选择界面。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_24.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;设置完成后，可以看到右侧样式栏显示开关数据已配置数据源，即为配置完成。


+ 折线图组件数据源配置
  
&emsp;&emsp;&emsp;&emsp;折线图组件需要创建一个SQL分析的API接口。
<br>
&emsp;&emsp;&emsp;（1）Iot Studio控制台中选择数据任务，新建一个SQL分析，如下图所示：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_25.jpg width=80%/>
</div>
<br>

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_26.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;（2）进入数据分析界面，配置数据源
<br>

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_27.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;如下图所示，选择设备名称和气体值，并点击生成SQL查询语句：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_28.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;（3）SQL语句生成后，点击运行即可查询所选数据内容，分别点击配置、	测试以及发布。如下图所示:
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_29.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;点击配置后，设置对应的参数类型:
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_30.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;点击测试，测试数据是否正常，显示测试成功，即可点击发布

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_31.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;发布后就可以返回移动端应用开发界面。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_32.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;（4）返回开发界面后点击折线图，在右侧样式中将刚发布的API接口作为折线图的数据源，如下图所示:
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_33.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;进入折线图配置界面，将所需维度拖拽至横纵轴，完成配置
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_34.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;（5）配置完成后，如下图所示，可以保存、预览以及发布
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_35.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;（6）点击预览后会生成一个预览二维码，手机扫描二维码进行预览控制操作
<div align="center"
<img src=./../../../images/haas506案例图片/haas506_gas_36.jpg width=80%/>
</div>



## 7.测试
+ 准备硬件：

&emsp;&emsp;（1）请参考上述准备 硬件环境。
<br>
&emsp;&emsp;（2）HaaS506 连接usb typeC 到pc，此时在pc端会生成8组usb serail port
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_gas_37.jpg width=80%/>
</div>

&emsp;&emsp;（3）其中port5即Unisoc Usb Serial Port 5对应的COM口为板子日志输出以及python命令行交互口，此例为COM36。
<br>
&emsp;&emsp;（4）HaaS506 使用HY转杜邦线4pin线材连接uart1 即板子丝印J8 TTL接口，杜邦线连接usb转uart模块，请参考上述接线方式。
以上硬件准备完毕。
<br>
+ 软件准备：


&emsp;&emsp;（1）	HaaS506最新固件，一般拿到开发板，厂家烧录最新的固件，无需重新烧录。下载地址以及烧录方式：HaaS 506 — Python轻应用 (alicdn.com)
<br>
&emsp;&emsp;（2）VScode（大于1.57），HaaS studio VScode插件。
<br>
&emsp;&emsp;（3）代码实现保存为main.py，board.json（HaaS506硬件驱动配置文件）
<br>
&emsp;&emsp;（4）新建python轻应用请参见：
HaaS506快速开始_HaaS技术社区的博客-CSDN博客
<br>
&emsp;&emsp;（5）将示例文件main.py以及硬件驱动配置文件board.json，一起放置到创建haas轻应用时的工作区路径下solutions\test_demo（test_demo为项目名字）下，按照步骤4文档进行烧录验证。
<br>
&emsp;&emsp;（6）扫描iot studio 预览二维码进行预览以及控制操作。
