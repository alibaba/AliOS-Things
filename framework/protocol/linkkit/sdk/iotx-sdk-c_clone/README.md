# 阿里云物联网套件

物联网套件是阿里云专门为物联网领域的开发人员推出的, 其目的是帮助开发者搭建安全且性能强大的数据通道, 方便终端(如传感器, 执行器, 嵌入式设备或智能家电等等)和云端的双向通信.

-   **[官方代码首页](https://github.com/aliyun/iotkit-embedded)**
-   **[官方维基首页](https://github.com/aliyun/iotkit-embedded/wiki)**

# 快速开始

本节描述如何申请自己的设备, 并结合本SDK快速体验该设备通过`MQTT`+`TLS/SSL`协议连接到阿里云, 上报和接收业务报文. 关于SDK的更多使用方式, 请访问[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)

> 实现原理:
>
> `MQTT协议`(`Message Queuing Telemetry Transport`, 消息队列遥测传输)是IBM开发的一个即时通讯协议, 是为大量计算能力有限, 且工作在低带宽, 不可靠的网络的远程传感器和控制设备通讯而设计的协议
>
> 利用MQTT协议是一种基于二进制消息的发布/订阅编程模式的消息协议, 下面的应用程序先在阿里云IoT平台订阅(`Subscribe`)一个`Topic`成功, 然后自己向该`Topic`做发布(`Publish`)动作
> 阿里云IoT平台收到之后, 就会原样推送回这个应用程序, 因为该程序之前已经通过订阅(`Subscribe`)动作成为该`Topic`的一个接收者, 发布到这个`Topic`上的任何消息, 都会被推送到已订阅该`Topic`的所有终端上

## 一. 开发环境准备

#### **1. 安装Ubuntu16.04**

本SDK的编译环境是`64位`的`Ubuntu16.04`, 在其它Linux上尚未测试过, 所以推荐安装与阿里开发者一致的发行版

如果您使用`Windows`操作系统, 建议安装虚拟机软件`Virtualbox`, 下载地址: [https://www.virtualbox.org/wiki/Downloads](https://www.virtualbox.org/wiki/Downloads)

然后安装64位的desktop版本`Ubuntu 16.04.x LTS`, 下载地址: [https://www.ubuntu.com/download/desktop](https://www.ubuntu.com/download/desktop)

#### **2. 安装必备软件**

本SDK的开发编译环境使用如下软件: `make-4.1`, `git-2.7.4`, `gcc-5.4.0`, `gcov-5.4.0`, `lcov-1.12`, `bash-4.3.48`, `tar-1.28`, `mingw-5.3.1`

可使用如下命令行安装必要的软件:

    apt-get install -y build-essential make git gcc

## 二. 在控制台创建设备

#### **1. 注册/登录阿里云账号**

访问阿里云[登录页面](https://account.aliyun.com/login/login.htm), 点击[免费注册](https://account.aliyun.com/register/register.htm), 免费获得一个阿里云账号. 若您已有账号, 可直接登录

#### **2. 访问物联网套件控制台**

登入之后, 鼠标悬停在**产品**上, 弹出层叠菜单

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/iotconsole-product.png)

然后向下滚动页面, 点击**物联网套件**

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/iotconsole-iotkit.png)

或直接访问[https://www.aliyun.com/product/iot](https://www.aliyun.com/product/iot), 之后点击**立即开通**, 或者**管理控制台**, 登入[控制台主界面](https://iot.console.aliyun.com/)

#### **3. 创建产品和设备**

点击页面右上角的**创建产品**按钮, 创建一个品类, 然后在左侧导航栏点击**设备管理**, 再到页面右侧点**添加设备**, 创建该品类下的一个设备, 如下图则得到创建成功后的**设备标识三元组**

- `productKey`: 标识产品的品类
- `deviceName`: 标识品类下的具体设备
- `deviceSecret`: 该设备的密钥, 需填写到SDK中, 用于连接阿里云服务器时完成认证

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/iotconsole-devinfo.png)

#### **4. 创建可订阅可发布的Topic**

点击左侧导航栏的**消息通信**, 再到页面右侧点**定义Topic类**, 创建一个新的`/${productKey}/${deviceName}/data`, 并设置为**可订阅可发布**权限

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/iotconsole-subpub.png)

## 三. 编译样例程序

#### **1. 下载SDK**

登录Linux, 运行如下命令从github克隆代码, 或者访问最新地址[下载页面](https://github.com/aliyun/iotkit-embedded/releases/latest), **将下载到的压缩包在Linux上解压缩**

    $ git clone https://github.com/aliyun/iotkit-embedded

#### **2. 填入设备信息**

编辑文件`sample/mqtt/mqtt-example.c`, 编辑如下代码段, 填入之前**创建产品和设备**步骤中得到的**设备标识三元组**:

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/sdk-devinfo.png)

#### **3. 编译SDK产生样例程序**

运行如下命令:

    $ make distclean
    $ make

编译成功完成后, 生成的样例程序在当前目录的`output/release/bin`目录下:

    $ tree output/release
    output/release
    ├── bin
    │   ├── coap-example
    │   ├── http-example
    │   ├── mqtt-example
    │   ├── mqtt_rrpc-example
    │   ├── ota_mqtt-example
    │   ├── sdk-testsuites
    │   ├── shadow-testsuites
    │   └── subdev-example
    ...
    ...

## 四. 运行样例程序

#### **1. 执行样例程序**

    $ ./output/release/bin/mqtt-example
    [inf] iotx_device_info_init(40): device_info created successfully!
    [dbg] iotx_device_info_set(50): start to set device info!
    [dbg] iotx_device_info_set(64): device_info set successfully!
    [dbg] _calc_hmac_signature(57): | source: clientId2UCRZpAbCGC.ExampleDevdeviceNameExampleDevproductKey2UCRZpAbCGCtimestamp2524608000000 (93)
    [dbg] _calc_hmac_signature(58): | secret: fbh47lGBSayncmTHEjF1E5x4CZdeJTO9 (32)
    [dbg] _calc_hmac_signature(61): | method: hmacsha1
    [dbg] _calc_hmac_signature(74): | signature: 326a4a6ed38b1bd5ddb6a5d11d27928bfb5a62d0 (40)
    [dbg] guider_print_dev_guider_info(236): ....................................................
    [dbg] guider_print_dev_guider_info(237):           ProductKey : 2UCRZpAbCGC
    [dbg] guider_print_dev_guider_info(238):           DeviceName : ExampleDev
    [dbg] guider_print_dev_guider_info(239):             DeviceID : 2UCRZpAbCGC.ExampleDev
    [dbg] guider_print_dev_guider_info(240):         DeviceSecret : fbh47lGBSayncmTHEjF1E5x4CZdeJTO9
    [dbg] guider_print_dev_guider_info(241): ....................................................
    ...
    ...
    _demo_message_arrive|136 :: ----
    _demo_message_arrive|140 :: Topic: '/2UCRZpAbCGC/ExampleDev/data' (Length: 28)
    _demo_message_arrive|144 :: Payload: '{"attr_name":"temperature", "attr_value":"1"}' (Length: 45)
    _demo_message_arrive|145 :: ----
    [inf] iotx_mc_unsubscribe(1416): mqtt unsubscribe success,topic = /2UCRZpAbCGC/ExampleDev/data!
    [dbg] iotx_mc_disconnect(2106): rc = MQTTDisconnect() = 0
    [inf] _network_ssl_disconnect(413): ssl_disconnect
    [inf] iotx_mc_disconnect(2114): mqtt disconnect!
    [inf] iotx_mc_release(2160): mqtt release!

    ---------------------------------------------------
    . bytes_total_allocated:    1292
    . bytes_total_freed:        1292
    . bytes_total_in_use:       0
    . bytes_max_allocated:      560
    . bytes_max_in_use:         1066
    . iterations_allocated:     20
    . iterations_freed:         20
    . iterations_in_use:        0
    . iterations_max_in_use:    11
    ---------------------------------------------------
    main|441 :: out of sample!

#### **2. 观察消息上报**

如下日志信息显示样例程序正在通过`MQTT`的`Publish`类型消息, 上报业务数据到`/${prodcutKey}/${deviceName}/data`

    mqtt_client|256 :: packet-id=3, publish topic msg={"attr_name":"temperature", "attr_value":"1"}

#### **3. 观察消息下推**

如下日志信息显示该消息因为是到达已被订阅的`Topic`, 所以又被服务器原样推送到样例程序, 并进入相应的回调函数

    _demo_message_arrive|136 :: ----
    _demo_message_arrive|140 :: Topic: '/2UCRZpAbCGC/ExampleDev/data' (Length: 28)
    _demo_message_arrive|144 :: Payload: '{"attr_name":"temperature", "attr_value":"1"}' (Length: 45)
    _demo_message_arrive|145 :: ----

#### **4. 观察控制台日志**

可以登录物联网套件控制台, 到[设备页面](https://iot.console.aliyun.com/#/product/detail), 找到刚才填写在SDK中的设备并点击进入, 点左边导航栏的**日志服务**, 可以看到刚才被上报的消息

![image](https://raw.githubusercontent.com/wiki/aliyun/iotkit-embedded/pics/iotconsole-publog.png)

# 关于SDK的更多使用方式, 请访问[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)
