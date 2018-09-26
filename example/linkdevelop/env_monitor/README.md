


## 简介

本文介绍一个模拟”环境监测“的简单常用示例，搭载 AliOS Things 的设备，并结合 Link Develop (以下简称LD) 可快速实现手机远程打开web app进行环境监测。
本示例源码: [env_monitor](https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/env_monitor)
```
env_monitor
├── devkit_display
│   ├── AliOS_Things_logo.c
│   ├── lv_conf_developerkit.h
│   ├── sensor_display.c
│   └── sensor_display.h
├── webapp
├── env_monitor.c
└── env_monitor.mk
```

## 功能介绍

本系统涉及到了3个部分：
- 设备端。 设备端运行AliOS Things嵌入式系统，设备端上电启动后初始化与 LD 云端的 MQTT 连接，然后在串口 cli 中输入 `netmgr connect ssid password` 命令后连接网络，然后定时上报设备端的温湿度值到云端。
- LD平台。 设备端的传感器数据会上传到LD平台。
- web APP。 LD平台使用<服务编排>功能创建API，然后`web app`调用创建的API实时显示温湿度。

系统框图如下所示：

![](https://img.alicdn.com/tfs/TB1oPPzXYvpK1RjSZPiXXbmwXXa-498-381.png)

## 环境要求

### 设备端环境
任意具有接入互联网能力，且已完成移植 AliOS Things 移植的设备，如：[ST开发板](https://linkdevelop.aliyun.com/device-doc#dev-st-guide.html), [NXP开发板](https://linkdevelop.aliyun.com/device-doc#dev-nxp-guide.html), [TI开发板](https://linkdevelop.aliyun.com/device-doc#dev-ti-guide.html), [Developerkit开发板](http://aliosthings.io/#/developerkit)。
亦可请参考其他[已支持的硬件](https://github.com/alibaba/AliOS-Things/tree/master/board)进行适配。

### 云端环境

* 在 [Link Develop](https://linkdevelop.aliyun.com) 注册账号(与阿里云账号通用)。
* 在 LD 产品创建好以后，在设备模型的自定义功能处，新增一个属性(report_sensor), 用于上行消息, 设为json复合类型，里面是两个温湿度数据。如图：
![](https://img.alicdn.com/tfs/TB1TaS_XNYaK1RjSZFnXXa80pXa-1672-804.png)
同时，这些publish的内容需要按要求填写在设备端代码(example/linkdevelop/env_monitor/env_monitor.c)：
    ```
    #define PROP_POST_FORMAT_HUMITEMP    "{\"report_sensor\":{\"Humi\":%f,\"Temp\":%f}}"
    ```
* 设备模型创建完成后，新增一个测试设备，记下如下所示的四元组信息。
  该组信息会用于烧写到实际设备中，而这个测试设备，则会是实际设备在云端的一个影子。
	* PRODUCT_KEY
	* PRODUCT_SECRET
	* DEVICE_NAME
	* DEVICE_SECRET

## 设备端编译构建

前置开发环境搭建请移步[开发准备](https://linkdevelop.aliyun.com/device-doc#dev-prepare.html)。

在 `example/linkdevelop/env_monitor/env_monitor.c` 中, 将以下几个字段更新为上述生成的对应信息，如：
```c
#define PRODUCT_KEY             "b1VxeCKSQeO"
#define DEVICE_NAME             "simple"
#define DEVICE_SECRET           "x9D0k0DjwtYG734unBm0YYsyoMRxgCNR"
```

然后在 AliOS Things 代码环境中编译出目标.bin文件，如：
```sh
aos make linkdevelop.env_monitor@developerkit
```
烧录到板子中：
```sh
aos upload linkdevelop.env_monitor@developerkit 
```
_注意: 此处烧录方式根据不同硬件会有所不同，此处以 [Developer kit](https://github.com/alibaba/AliOS-Things/tree/developer/board/developerkit) 为例_

## 设备端运行

### 启动并连网
烧录完成以后重新上电板子，在串口中 cli 中使用 `netmgr` 命令进行连网：
```
# netmgr connect ssid password
```
其中 `ssid` 和 `password` 是WiFi路由器的名字和密码。
看到如下信息以后即说明连网完成，并已经和 LD 云端建立好 MQTT 连接。
```
[inf] _TLSConnectNetwork(357):   . Setting up the SSL/TLS structure...
[inf] _TLSConnectNetwork(367):  ok
[inf] _TLSConnectNetwork(402): Performing the SSL/TLS handshake...
[inf] _TLSConnectNetwork(410):  ok
[inf] _TLSConnectNetwork(414):   . Verifying peer X.509 certificate..
[inf] _real_confirm(93): certificate verification result: 0x00
[005598]<V> mqtt service
[005652]<V> subscribe success, packet-id=1
# 
```

### 接收上行消息
![](https://img.alicdn.com/tfs/TB1vYgYXQvoK1RjSZFDXXXY3pXa-1687-473.png)
按上图所示，在LD控制台的在线调试里可看到上行消息。同样，本地串口可看到如下信息：
```
[084590]<V> packet-id=0, publish topic msg={"id":"71","version":"1.0","method":"thing.event.property.post","params":{"report_sensor":{"Humi":62.700000,"Temp":30.600000}}}.

[085640]<V> packet-id=0, publish topic msg={"id":"72","version":"1.0","method":"thing.event.property.post","params":{"report_sensor":{"Humi":62.700000,"Temp":30.600000}}}.

```

## webapp开发
### 服务编排

[服务编排](https://yuque.antfin-inc.com/ld-doc/service/index)用来创建API提供给web app使用，服务编排相关的文档请参考https://yuque.antfin-inc.com/ld-doc/service/index 。
本示例创建了2个api：

|API|说明|
|---|---|
|`/getDevices`|获取设备列表|
|`/getSensor`|获取传感器信息|

#### 创建API：getDevices

1. 首先`新建服务流`，如图所示：

![](https://img.alicdn.com/tfs/TB1xvzmXVzqK1RjSZFvXXcB7VXa-921-481.jpg)


2. 添加`编排`：

![](https://img.alicdn.com/tfs/TB1RR2nX4TpK1RjSZFGXXcHqFXa-799-531.jpg)

3. `设备查询`设置，按照下图设置：

![](https://img.alicdn.com/tfs/TB17XYmX3HqK1RjSZJnXXbNLpXa-799-531.jpg)

4. `API输出`设置：

![](https://img.alicdn.com/tfs/TB1cBroXZfpK1RjSZFOXXa6nFXa-691-512.jpg)

5. 点击部署->运行，可以看到运行效果:

![](https://img.alicdn.com/tfs/TB16bjpX7zoK1RjSZFlXXai4VXa-1425-887.jpg)

#### 创建API：getSensor

创建过程和`getDevices`类似，其中`可用API`进行如下设置：

![](https://img.alicdn.com/tfs/TB1Rh_mXVzqK1RjSZFCXXbbxVXa-788-636.jpg)

#### 查看创建的API详情页

点击如下页面可以看到创建的API详情：

![](https://img.alicdn.com/tfs/TB1VyDoXVzqK1RjSZFzXXXjrpXa-725-535.jpg)

API详情如下所示：

![](https://img.alicdn.com/tfs/TB1.Q6oX6TpK1RjSZKPXXa3UpXa-820-530.jpg)

#### 服务发布
点击页面的右上角的<发布版本>即可发布API。

> 如果<发布版本>显示为灰色，那么说明API还存在问题，或者没有部署。

#### 设置自定义域名

**LD平台**服务编排创建的API对自动创建的域名有限制，**每天只能调用1000次**，如果调用次数很多的话，那么可以设置自定义的域名，这样可以无限制调用：

![](https://img.alicdn.com/tfs/TB1_e_uX7voK1RjSZFDXXXY3pXa-945-524.png)

### web应用

`web 应用`开发这边不进行详细说明，具体可以参考：https://yuque.antfin-inc.com/ld-doc/web/index

本示例的webapp代码在[https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/env_monitor/webapp](https://github.com/alibaba/AliOS-Things/tree/developer/example/linkdevelop/env_monitor/webapp) 中

代码结构：
```sh
webapp
├── app
│   └── pages
│       ├── home			# 主页web页面
│       │   ├── index.css
│       │   └── index.js
│       └── not-found
│           ├── index.css
│           └── index.js
├── app.js
├── bone-config.json
├── package
│   ├── app.css
│   ├── app.js
│   ├── common.js
│   └── manifest.json
├── package.json
└── public
    └── index.html
```
#### 开通自研服务

`web应用`想要调用刚才创建的2个API的话，就需要在web 应用页面的`服务授权`里面添加自研服务：

![](https://img.alicdn.com/tfs/TB1XtnIX4naK1RjSZFBXXcW7VXa-1690-439.jpg)

#### 更改代码中的API路径

编辑文件`webapp/app/pages/home/index.js`,把`config.domain`的`api.xiaxiaowen.top`替换为自定义的域名即可。

```c
const config = {
    domain: 'http://api.xiaxiaowen.top'
}
```

#### 编译 调试 打包 上传

调试：

```sh
bnpm install
bone start
```

打包上传：

```sh
bone build
bone pack
```

执行`bone pack`之后会在目录中生成`bundle.zip`，把这个压缩包上传到web应用中，然后打开下图的`开发环境域名`即可体验：

![](https://img.alicdn.com/tfs/TB13EPOX5LaK1RjSZFxXXamPFXa-1699-479.jpg)

#### 网页体验

温湿度每隔1s刷新一次。

![](https://img.alicdn.com/tfs/TB12s_QX4jaK1RjSZKzXXXVwXXa-662-246.jpg)

## 结语
至此，已经完成了一个最常用的物联网的应用：环境监测。
