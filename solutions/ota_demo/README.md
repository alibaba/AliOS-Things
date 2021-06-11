@page ota_demo HaaS物联网设备OTA解决方案

[更正文档](https://gitee.com/alios-things/ota_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
OTA升级是很多嵌入式产品必备的一个功能。HaaS提供了完备的OTA解决方案。本案例就是一个端云一体的例子。
本文将分几个部分介绍，具体包括：
设备端代码的修改、编译、烧录。
云端服务器的配置、新增、OTA升级包上传、OTA升级。
本文的设备连接情况如下：
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01U7iUQh1k4aSEzSpK4_!!6000000004630-0-tps-4032-3024.jpg" style="max-width:800px;" />
</div>

本文的主要目标是通过修改设备端代码和配置云端，完成对设备端版本的升级，如下图所示：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01ScRC9M1GS0EMBJG76_!!6000000000620-0-tps-1448-984.jpg" style="max-width:800px;" />
</div>

# 2. 基础知识
OTA：（over the air)已成为物联网设备的刚需功能, AliIOS Things OTA有完备的升级方案，对各种升级场景都有很好的支持。
升级模式：整包升级、压缩升级、差分升级、安全升级。
支持的升级通道：http、https、BLE、3G/4G，NB等；
复杂场景支持：网关及子设备升级，连接型模组升级非连接主设备的间接升级；
OTA工具：差分工具、本地签名工具、ymodem辅助升级工具，多固件打包工具等；

HaaS100进行升级流程，如下图所示，当用户开启阿里云IOT物联网平台的安全升级功能，对应的产品就启动了针对这个产品的安全升级功能，云端会对这个产品的升级固件做秘钥、公钥管理并对这个产品的固件做数字签名；对应的设备端，在OTA的过程中，会用从云端获取的公钥对升级的固件做数字签名的验证；整个流程，用户不需要管理公私钥，使用起来也非常方便，下图为HaaS100 安全升级使用流程图：
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01Az1bpN1yg3TWgq2qC_!!6000000006607-0-tps-1818-1178.jpg" style="max-width:800px;" />
</div>

# 3. 物料清单
本实验不依赖于其他外围设备，主要是HaaS100开发板
|名称|数量|
|:-:|:-:|
|HaaS100 开发板|1|

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01bQf9611vvg8cQED0M_!!6000000006235-2-tps-3000-2000.png" style="max-width:800px;" />
</div>

# 4. 案例实现
本案例依赖如下几个组件，具体定义放到了solutions/ota_demo/package.yaml的文件中。
|依赖组件|作用|
|:-:|:-:|
|ota|ota功能的组件|
|cjson|和云端通信使用的json格式的组件|
|cli|命令行的组件|
|netmgr|用于网络连接的组件|
|lwip|用于网络协议栈的组件|
|haas100|用于haas100开发板的组件|
|ulog|用于串口日志的组件|

**1.打开云端安全升级功能并获取公钥**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01cXHMmB1dPvabuoJzv_!!6000000003729-2-tps-1837-867.png" style="max-width:800px;" />
</div>

**2.将公钥内嵌到代码中**
如步骤1图示，通过复制按钮获取的公钥内嵌到 ```
AliOS-Things/components/dm/ota/hal/ota_hal_digest.c ```文件中，覆盖如下的两个数组：
```
/* RSA Public Key:User needs sign in alibaba cloud to get and replace them. */
static const unsigned char ota_pubn_buf[256];
static const unsigned char ota_pube_buf[3];
```
**3.编译烧录到HaaS100的基础固件**
* 开发环境搭建
开发环境的搭建请参考 [HaaS100快速开始 (搭建开发环境章节)](https://g.alicdn.com/alios-things-3.3/doc/haas100_quick_start.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程;
* 用例和开发板选择
参考[HaaS100快速开始 (选择解决方案和开发板章节)](https://g.alicdn.com/alios-things-3.3/doc/haas100_quick_start.html)，其中解决方案选择：**ota_demo**; 开发板：**HaaS 100**
* 配置固件版本号
根据需求修改版本号,如app-1.0.0等
修改位置：solutions/ota_demo/otaappdemo.c中的，MY_APP_VER宏定义。
* 配置四元祖
修改位置：solutions/ota_demo/otaappdemo.c中的，mqtt_main函数中的char *product_key, char *device_name, char *device_secret;填入自己的pk,dn,ds;
* 编译固件
完成上面步骤后，就可以编译ota_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。
* 烧录固件
ota_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。
* 打开串口
固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。
当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。
* 配网连云
输入wifi账号和密码配网：在终端输入：
`netmgr -t wifi -c wifi_ssid wifi_password`
注意修改其中的wifi_ssid和wifi_password为需要连接的wifi名字和wifi密码。
连网成功后，登录[物联网平台](http://iot.console.aliyun.com/)可以看到对应的设备在线:

<div align=left display=flex>
    <img src="https://img.alicdn.com/tfs/TB18SN737T2gK0jSZFkXXcIQFXa-592-45.png" style="max-width:800px;" />
</div>

**4.编译上云固件及云端操作**
本地烧录完成后，需要做一个高版本固件上传到云端，通过云端操作完成固件的升级，所以需要按照步骤3中修改版本号的方法，修改固件版本号，其他不用修改，再编译生成一个高版本的固件，然后登录[物联网平台](http://iot.console.aliyun.com/)平台，按如下图顺序操作:
<div align=left display=flex>
    <img src="https://img.alicdn.com/tfs/TB1NM2iU7T2gK0jSZFkXXcIQFXa-1110-861.png" style="max-width:800px;" />
</div>

点击**添加固件**后，如下图将`platform/mcu/haas1000/release/write_flash_gui/ota_bin/ota_bin/ota_rtos_ota.bin`上传到云端:

<div align=left display=flex>
    <img src="https://img.alicdn.com/tfs/TB19BoDhmslXu8jSZFuXXXg7FXa-719-1196.png" style="max-width:800px;" />
</div>

点击确定后，选择验证固件即可开始固件升级；
**5.升级结果验证**
按照上面的操作步骤完成后，可以完成固件的数字签名验签进而实现固件升级，端侧的log如下图：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01nCJgW423op8DkE8Vg_!!6000000007303-2-tps-1170-385.png" style="max-width:800px;" />
</div>

如果开启了安全升级，但HaaS100没有内嵌公钥，触发升级会怎么样呢？答案是HaaS100会数字签名验证失败，禁止固件升级，端侧的log会如下图所示：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01g5P8gy1t33fTWNqur_!!6000000005845-2-tps-1297-490.png" style="max-width:800px;" />
</div>

云端升级结果可以通过点击“查看”获取详情；
物联网平台的OTA操作可参考文档[阿里云物联网平台固件升级文档](https://help.aliyun.com/document_detail/58328.html) 

# 5. 总结
本文仅仅针对OTA升级在HaaS100上的的全链路进行了简单介绍。使用ota_demo代码例子外加一块HaaS100开发板，熟悉HaaS开发框架中对于OTA升级的能力支持。关于OTA升级的更多丰富功能，欢迎访问阿里云官网介绍 https://help.aliyun.com/document_detail/184188.html。
