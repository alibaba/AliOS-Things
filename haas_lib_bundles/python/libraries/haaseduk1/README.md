# 教育开发板 - HaaS EDU K1

## 一、产品简介

HaaS EDU K1教育开发板的产品介绍及接口接口定义请参考[HaaS EDU K1硬件积木](https://haas.iot.aliyun.com/solution/detail/hardware?versionId=800C5AB3B8A4A88800000001&dataId=800C5AB3B8A4A888)中的说明。

HaaS EDU K1开发板硬件有两个版本，两个版本内置的的外围传感器芯片是不同的，传感器型号及Python驱动链接如下表所示：

|HaaS EDU K1硬件版本|v1.0|v1.1|
|-|-|-|
|温湿度传感器|[SI7006](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/si7006)|[CHT8305](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/cht8305)|
|磁力计传感器|[QMC5883L](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/qmc5883)|[QMC6310U](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/qmc6310)|
|6周陀螺仪传感器|[MPU6050](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/mpu6050)|[QMI8610](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/qmi8610)|
|大气压传感器|[SPL06](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/spl06)|[qmp6988](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/qmp6988)|
|光照及接近程度传感器|[AP3216C](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ap3216c)|[AP3216C](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/ap3216c)|
|OLED显示屏|[SH1106](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/sh1106)|[SH1106](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/sh1106)|

本文主要说明如何用判断HaaS EDU K1的硬件版本和固件版本。

## 二、判断方法
请参考[HaaS EDU K1快速开始](https://haas.iot.aliyun.com/haasapi/index.html/#/Python/docs/zh-CN/startup/HaaS_EDU_K1_startup)中的说明进行开发环境的搭建并创建一个helloworld工程，然后下载[此目录](./)中的所有文件后，覆盖到helloworld工程目录中的所有文件。

将此工程推送到HaaS EDU K1开发板上，系统启动后OLED屏幕上会显示HaaS EDU K1的硬件版本和固件的版本号。
* “K1 HW -”后面跟着的是硬件版本号，v1.0或者是v1.1
* “K1 FW -”后面跟着的是HaaS Python固件的版本号<ver>，如果屏幕显示v2.0.0，则代表固件版本号为HaaS Python v2.0.0


