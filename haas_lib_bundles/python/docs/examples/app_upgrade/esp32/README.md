# Python应用升级
&emsp;&emsp;
本案例导学步骤如下图所示。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-导学步骤.png width=80%/>
</div>

## 1、简介

### 1.1、背景

&emsp;&emsp;
智能物联网进展至今，要想形成竞争力，必须要有一套闭环的端云一体解决方案，IoT硬件端有HaaS开发板，可使用Python开发，做到一端开发多端应用，应用开发完成后要迭代，就需要升级，为解决应用升级问题，需要一套应用分发系统来帮助HaaS设备实现端云一体的闭环。本案例就是介绍如何使用HaaS Cloud平台帮助开发者实现应用升级功能的实践。

### 1.2、设计方案

&emsp;&emsp;
HaaS Cloud平台为开发者提供了升级的通道，并提供应用升级相关的产品\设备\应用\版本可视化操作页面，开发者只需界面点点就可以实现应用升级全流程。
核心设计如下：

&emsp;&emsp;
1、在阿里云物联网平台注册设备。

&emsp;&emsp;
2、HaaS Cloud控制台创建测试设备分组、创建应用、创建应用版本。

&emsp;&emsp;
3、设备端使用MQTT连接阿里云物联网平台，订阅应用升级服务。

&emsp;&emsp;
4、设备端接收并解析HaaS Cloud控制台推送的升级消息。

&emsp;&emsp;
5、下载升级包并安装至指定位置，然后启动运行程序。

&emsp;&emsp;
设计的详细流程图如下：
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-整体流程图.png width=80%/>
</div>
<br>


### 1.3、准备

1. M5Stack Core2开发板       一套
2. microUSB数据线            一条

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |

## 2、注册设备、创建测试设备分组

### 2.1、注册阿里云账号
&emsp;&emsp;
进入阿里云官网，[注册阿里云账号](https://account.aliyun.com/register/qr_register.htm) 。注册完成后登录官网。如果您已经有阿里云账号，直接登录即可。
<br>

### 2.2、注册设备

&emsp;&emsp;
进入[物联网平台](https://iot.console.aliyun.com/lk/summary/new)，如下图所示。

<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_物联网平台.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_空产品页.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_新建产品.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_添加设备.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_批量添加.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_设备列表.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_设备详情.png width=80%/>
</div>
<br>

&emsp;&emsp;
这里的设备三元组信息请注意，后面代码中会用到。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_设备证书.png width=80%/>
</div>
<br>

### 2.3、关联产品到HaaS Cloud控制台

&emsp;&emsp;
进入[HaaS Cloud控制台](https://haas.iot.aliyun.com/welcome)，将物联网平台上创建的产品关联到这里，步骤如下图所示。

<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_HaaS控制台绑定产品.png width=80%/>
</div>
<br>

&emsp;&emsp;
HaaS Cloud控制台上查看关联产品下的设备，如下图所示
在”设备管理-设备“菜单下也可以查看设备三元组，这里跟物联网平台上查看是一样的。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级_HaaS控制台查看设备.png width=80%/>
</div>
<br>


### 2.4、创建测试设备分组

&emsp;&emsp;
创建测试设备分组并向分组中添加设备，为后面推送应用升级做准备，在推送应用升级时需要选择该测试设备分组，那么该测试设备分组下在线的设备都会收到升级消息。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-创建测试分组.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-测试分组创建完成.png width=80%/>
</div>


<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-添加分组设备.png width=80%/>
</div>
<br>


## 3、设备端开发

&emsp;&emsp;
本章主要是搭建设备上需要运行的初始应用版本。

### 3.1、开发环境准备
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕。详情请参考[M5StackCore2快速开始](../../../startup/M5StackCore2_startup.md)的说明。
<br>

### 3.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”（本文选的是M5Stack-Core2），再从右侧的案例中选择“Python应用升级”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/Python应用升级/HaaS_Studio_创建工程示范.png width=80%/>
</div>
<br>

### 3.3、设备端代码修改

> Python脚本的详细说明请参考脚本内嵌的文字注释


1. **修改路由器名称及密码**

&emsp;&emsp;
修改工程根目录下的main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID="Your-AP-SSID"
PWD="Your-AP-Password"
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

2. **修改设备的三元组信息**

&emsp;&emsp;
按照[2.3获取设备的三元组]获取三元组信息，填入工程根目录下main.py中：
```python
# HaaS设备三元组
ProductKey = "Your-ProductKey"
DeviceName  = "Your-DeviceName"
DeviceSecret  = "Your-DeviceSecret"
```
3. **修改应用配置文件manifest.json**

&emsp;&emsp;
修改工程根目录下的manifest.json文件中appid对应的值，
必须为16位的数字，且开头的前八位为88001000。

&emsp;&emsp;
修改manifest.json文件注意事项：
``` 
1、该文件内不要添加任何注释；
2、关键字appName对应的值，只能使用英文字母、数字，长度不能超过16个字符，在HaaS Cloud平台全局范围内唯一；
3、关键字appid对应的值，必须为16位的数字，且开头为88001000xxxxpppp，该appid在HaaS Cloud平台全局范围内唯一；
4、关键字verison对应的值，只能使用数字和'.'，必须是3段式，例如0.0.1，在相同appName下唯一；
```
<br>

后续制作应用升级包时还会用到appName、version、appid关键字对应的值。到时请到manifest.json中查找。
<br>


## 4、运行设备

### 4.1、推送脚本到固件

&emsp;&emsp;
最后点击工程底部的部署运行，选择本地更新、电脑选择对应的端口编号烧录程序，如下图。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-程序推送到固件.png width=80%/>
</div>
<br>

### 4.2、设备本地查看

&emsp;&emsp;
程序启动后，会去连接网络，注册和监听HaaS Cloud服务推送的消息。详细log如下所示：
```log
==== python execute bootpy ====
==== python file check /data/pyamp/main.py ====
==== python execute from /data/pyamp/main.py ====
Python App Upgrade Program
{
    "appName": "python9000",
    "version": "0.0.1",
    "appid": "8800100099990000"
}
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: x.x.x.x
NTP start
NTP done
establish tcp connection with server(host='$YourProductKey.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
#订阅HaaS Cloud Python应用升级服务
subscribed topic success: /sys/$YourProductKey/$YourDeviceName/thing/service/hli_service_upgrade_push
#到这里设备启动完成，监听云端有没有升级消息推送。
```

## 5、制作升级包和推送升级

### 5.1、本地制作Python应用升级包

&emsp;&emsp;
本步骤目的是在本地项目中制作一个升级包，所以要对升配置文件需要做一些特定修改。
&emsp;&emsp;
本地制作升级包的步骤如下。打开命令行，进入到工程testUpgrade目录，修改testUpgrade/manifest.json文件
需要注意version必须要比工程根目录的manifest.json中version高，否则云端不会推送给设备。

&emsp;&emsp;
然后执行打包命令，如下所示。

```python
# 下面提到的$appName、$appid值必须跟工程根目录manifest.json中一致。
cd testUpgrade
tar cvzf $appName-$appid-$verison.tar.gz *
生成Python升级包样例：python9000-8800100099990000-0.0.2.tar.gz
```

### 5.2、创建Python应用

&emsp;&emsp;
进入[HaaS Cloud控制台](https://haas.iot.aliyun.com/welcome)，在左侧菜单栏进入”升级服务-应用升级“，创建应用。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-创建Python应用.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-创建应用步骤.png width=80%/>
</div>
<br>

### 5.3、创建升级版本

&emsp;&emsp;
在左侧菜单栏进入”升级服务-应用升级“，创建升级版本，把本地制作的应用升级包上传到云端。

<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-我的应用列表.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-创建版本步骤.png width=80%/>
</div>
<br>

&emsp;&emsp;
查看新创建的版本列表如下图所示。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-应用版本列表.png width=80%/>
</div>
<br>

### 5.4、创建测试任务
&emsp;&emsp;
创建测试任务如下图所示。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-开始测试.png width=80%/>
</div>
&emsp;&emsp;
测试设备分组请选择章节2.4中的分组。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-新建测试任务.png width=80%/>
</div>
<br>

### 5.5、推送升级
&emsp;&emsp;
推送Python应用升级包给测试设备分组的设备。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-推送应用.png width=80%/>
</div>

&emsp;&emsp;
设备端收到应用升级消息后，在串口中会打印下面log
```
#下面log是云端推送的消息
{'msg_id': 1657023114, 'service_id': 'hli_service_upgrade_push', 'params': '{"app":[{"size":1269,"appId":"8800100099990000","rollingBack":0,"version":"0.0.2","url":"https://iotx-haas-xxx.gz?Expires=xxx&OSSAccessKeyId=xxx&Signature=xxx","md5":"8366bb1b8ad9766335b733b226ad178c","needRestart":0}],"appUpdateCount":1,"errorCode":"200","needUpdateFirmware":0,"haasTraceId":"0bc1747b16546803930783531e538d"}', 'code': 0, 'params_len': 490}
#确定要升级
local version[0.0.1] low,upgrade to [0.0.2].
upgrade_result_cb needUpgrade
####准备重启设备，显示下面log后，请手动重启设备
Reset Device to upgarde app: 3
Reset Device to upgarde app: 2
Reset Device to upgarde app: 1

##重启设备后打印log如下
==== python execute bootpy ====
App is being upgraded. It will take about 10 seconds.
#这行log会停留大概10秒钟，是后台连接wifi后下载升级包的时间。
App upgrade finished.
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
#升级后的版本信息
{
  "appName": "python9000",
  "version": "0.0.2",
  "appid": "8800100099990000"
}
#表示成功运行应用升级包中的main.py
new version upgrade success.
new version upgrade success.
```

### 5.6、再次推送升级入口
HaaS Cloud控制台左侧菜单”升级服务-应用升级“，点击列表中右侧”版本列表“按钮，在版本列表对应行的最后点击”更多“中点击”测试"按钮即可进入推送升级页面。
<div align="center">
<img src=./../../../images/Python应用升级/Python应用升级-开始测试.png width=80%/>
</div>
<br>

### 5.7、删除版本

&emsp;&emsp;
请按下面步骤操作，即可删除正在测试中的版本。
1. HaaS Cloud控制台左侧菜单”升级服务-应用升级“，点击列表中右侧”版本列表“按钮，在版本列表对应行的最后点击”更多“中点击”废弃"按钮。
2. HaaS Cloud控制台左侧菜单”升级服务-应用升级“，点击列表中右侧”版本列表“按钮，在版本列表对应行的最后点击”更多“中点击”删除"按钮。
3. 重复本文[5.3、创建升级版本]和[5.4、创建测试任务]即可再次创建升级版本
<br>


## 后记

&emsp;&emsp;
本案例使用m5stack硬件实现，感兴趣的同学可以放到ESP32其他HaaS板子上也同样可以使用。

&emsp;&emsp;
本文只介绍了最简单应用升级流程，读者可以尝试修改代码，实现符合自己应用场景的升级方案，例如升级包正确性检验等。