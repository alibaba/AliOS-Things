[EN](stm32-networking) | 中文

# 手把手教你stm32上如何使用mqtt上云

本文基于[AliOS Things](https://github.com/alibaba/AliOS-Things) 1.2.0版本，手把手教你如何在stm32L475上使用mqtt上云。

## 1、硬件环境搭建

1、STM32 B-L475E-IOT01A开发板。

·       开发板使用的是ST低功耗MCU系列中的STM32L475这款芯片，拥有1M字节的Flash和128K字节的SRAM。

·       支持WiFi，蓝牙，SubG, NFC等多种无线连接方式。 因为它还支持Arduino接口，所以使用者也可以很方便的通过Arduino接口来扩展其他的无线连接模块，比如GSM模块。 

·       板上还集成了64Mbits的Quad-SPI Flash。

·       板上还搭载了多种传感器，比如温湿度传感器，高性能3轴磁力计，加速度传感器，陀螺仪，接近传感器和压力传感器等。方便开发者用来进行多种应用的演示。

·       板子上还有两个数字全角度麦克风。若需要，还可以实现语音控制的功能。

硬件详细信息请移步：[STM32官网硬件详细介绍](http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-discovery-kits/b-l475e-iot01a.html)
![](https://img.alicdn.com/tfs/TB1J6KLmZLJ8KJjy0FnXXcFDpXa-3968-2976.jpg)

2、庆科MK3060通信模组一块

![](https://img.alicdn.com/tfs/TB1HYMxmRfH8KJjy1XbXXbLdXXa-2976-3968.jpg)

3、双公头杜邦线三根

4、连接方法：

将STM32L475 D0--->MK3060 UART TX， STM32L475 D1--->MK3060 UART RX， STM32L475 GND--->MK3060 GND。具体连接方式如下图所示；两个开发板中相同颜色的点通过杜邦线连接起来即可(红色对红色，白色对白色，黄色对黄色)。

![](https://img.alicdn.com/tfs/TB1k.Sfm3DD8KJjy0FdXXcjvXXa-3968-2976.jpg)



## 2、接入阿里云物联网套件

使用mqttapp快速接入阿里云。请参考[阿里云物联网设备接入准备](https://help.aliyun.com/document_detail/42714.html?spm=5176.doc30530.2.5.eofHfK)和[设备接入](https://help.aliyun.com/document_detail/30530.html?spm=5176.doc42714.2.7.LjopzI)

### 2.1、在云端主要包括以下几步：

1、注册一个阿里云账号

2、开通物联网套件

3、创建产品，拿到`ProductKey`

4、创建设备，拿到`DeviceName`和`DeviceSecret`

5、定义Topic`$(PRODECT_KEY)/$(DEVICE_NAME)/data`，并设置权限为：设备具有发布和订阅

### 2.2、设备端参数修改

mqttapp程序所在源码为AliOS-Things/example/mqttapp/mqtt-example.c，接入mqtt服务的信息在[framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h](https://github.com/alibaba/AliOS-Things/blob/master/framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h)。此时在云端获取的三个参数`ProductKey`，`ProductSecret`，`DeviceName`和`DeviceSecret`分别对应代码中的`PRODUCT_KEY`，`PRODUCT_SECRET`，`DEVICE_NAME`和`DEVICE_SECRET`三个宏。

```
#elif  MQTT_TEST
#define PRODUCT_KEY             "b1eszMRbDvz"                                                         
#define DEVICE_NAME             "mqtt_test"
#define DEVICE_SECRET           "CAaQz8Fc1JkFEyuzFhu4NpHSTlRSmRxV"
#define PRODUCT_SECRET          "Fxx6nyYptOugnS6H"
#else
```

## 3、AliOS Things Mqttapp编译

基于目前AliOS Things mqttapp编译命令行如下：

aos make mqttapp@b_l475e

在该命令执行完成后可在`out/mqttapp@b_l475e/binary/` 目录找到生成的bin文件和hex文件。

## 4、固件烧入

### 4.1、庆科MK3060固件烧入

请到庆科官方提供的MK3060 [AT固件](http://developer.mxchip.com/developer/md/bWljby1oYW5kYm9vay9Eb3dubG9hZC8zLTIuTWlDT19BVF92Mi4wX0NNRC5tZA)下载网址，请选择型号为MOC108/EMW3060对应的固件。烧写方法请参考 [MK3060固件烧入指导](http://developer.mxchip.com/handbooks/63)中“1.更新应用程序固件”部分

### 4.2、STM固件烧入

使用STM32固件烧入工具[ST-LINK](http://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stsw-link004.html)进行固件烧入。烧入步骤按下图所示

![](https://img.alicdn.com/tfs/TB1QprXm46I8KJjy0FgXXXXzVXa-937-241.jpg)

## 5、AliOS Things配网及数据连接阿里云

经过以上4步以后相应的mqttapp binary已经正常烧入到stm32L475开发板中，启动串口打印如下图所示：

通过命令行使得wifi模组能正确连接到对应的AP。

`netmgr connect *ssid* *password* *open|wep|wpa|wpa2*`

正常联网后，mqttapp会真正开始运行。下图为mqtt运行日志截图：

![](https://img.alicdn.com/tfs/TB1_9CBm8DH8KJjSspnXXbNAVXa-1708-284.jpg)

进一步在云端查询到设备相关的日志信息如下所示

![](https://img.alicdn.com/tfs/TB1kk_ImZLJ8KJjy0FnXXcFDpXa-1875-833.jpg)

