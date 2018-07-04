本文以Linux下C语言版SDK为例, 演示如何快速体验让设备连接到阿里云IoT, 并通过MQTT协议的PUB/SUB实现一个简单的M2M通信过程.

## 详细说明请访问[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)和[官方SDK首页](https://github.com/aliyun/iotkit-embedded)

## 第一步: 在控制台中创建设备

登录[IoT控制台](http://iot.console.aliyun.com), 创建产品及在产品下创建设备和Topic类, 具体步骤如下:

   + 创建产品, 可得到`ProductKey`
   + 在产品下创建设备, 可得到`DeviceName`, `DeviceSecret`
   + 定义Topic: `$(PRODUCT_KEY)/$(DEVICE_NAME)/data`, 并设置权限为: 设备具有发布与订阅 **(此步骤非常重要)**

具体请参考[控制台使用手册](https://help.aliyun.com/document_detail/42714.html)文档中的`创建产品`, `添加设备`以及`获取设备Topic`部分.

## 第二步: 填充设备参数

> **备注:**
> 如果您还没有SDK源码, 请到[SDK下载](https://help.aliyun.com/document_detail/42648.html)页面, 下载最新版本.

将sample程序文件中的设备参数替换为您在控制台申请到的设备参数.

将 `sample/mqtt/mqtt-example.c` 中以下星号字符串表示的设备参数替换为第一步中获得的值:

    #if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
        #define PRODUCT_KEY             "*******************"
        #define DEVICE_NAME             "*******************"
        #define DEVICE_SECRET           "*******************"
    #else
        // TODO: 在以下段落替换下列宏定义为你在IoT控制台申请到的设备信息
        #define PRODUCT_KEY             "*******************"
        #define DEVICE_NAME             "*******************"
        #define DEVICE_SECRET           "*******************"
    #endif

完成编辑并保存后, 进行下一步

## 第三步: 编译SDK

* 返回顶层目录
* 执行make指令, 编译SDK, 命令如下

        make distclean
        make

编译成功后, 在相应目录生成样例可执行程序.

## 第四步: 执行样例程序

执行目录 `output/release/bin/` 下的可执行程序:

    cd output/release/bin
    ./mqtt-example

样例程序的基本逻辑流程为:

1. 创建一个MQTT客户端
2. 订阅主题 `$(PRODUCT_KEY)/$(DEVICE_NAME)/data`
3. 向该主题发布消息

## 其它
### 编译输出的说明

编译顺利完成后, 输出在 `output/release/` 目录:

    +-- bin
    |   +-- coap-example
    |   +-- ...
    |   +-- mqtt-example
    +-- include
    |   +-- exports
    |   |   +-- iot_export_coap.h
    |   |   +-- ...
    |   |   +-- iot_export_shadow.h
    |   +-- imports
    |   |   +-- iot_import_coap.h
    |   |   +-- ...
    |   |   +-- iot_import_ota.h
    |   +-- iot_export.h
    |   +-- iot_import.h
    +-- lib
    |   +-- libiot_platform.a
    |   +-- libiot_sdk.a
    |   +-- libiot_tls.a
    +-- src
        +-- coap-example.c
        +-- http-example.c
        +-- Makefile
        +-- mqtt-example.c

说明如下:

| 文件/目录               | 说明                                                     |
|-------------------------|----------------------------------------------------------|
| bin/mqtt-example        | 用MQTT协议连接阿里云IoT的样例程序                        |
| include/                | 使用`libiot_sdk.a`时需要的头文件, 存放在这个目录         |
| lib/libiot_platform.a   | 硬件平台抽象层, `libiot_sdk.a`的工作是建立在它的基础上的 |
| lib/libiot_sdk.a        | SDK的核心层, 基于`libiot_platform.a`提供连接云端的能力   |
| src/Makefile            | 示例用Makefile, 演示如何在SDK之外链接本SDK提供的库文件   |
| src/mqtt-example.c      | 样例对应C代码, 可在此目录下输入make, 编译生成可执行程序  |

### Java版本
Java版请参考 [JAVA-SDK使用(MQTT)](https://help.aliyun.com/document_detail/42693.html)
