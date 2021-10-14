@mainpage AliOS Things 概述

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/mainpage.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

<div align="center">
  <img src="https://img.alicdn.com/tfs/TB1e1U7vyAnBKNjSZFvXXaTKXXa-973-200.png" height="60">
</div>

[![Join the chat at https://gitter.im/aliosthings/Lobby](https://img.shields.io/gitter/room/aliosthings/Lobby.svg?style=flat-square)](https://gitter.im/aliosthings/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

**AliOS Things** 发布于[2017年杭州云栖大会](https://yunqi.aliyun.com)， 是 AliOS 家族旗下的、面向IoT领域的、高可伸缩的物联网操作系统，于2017年10月20号宣布在[github](https://github.com/alibaba/AliOS-Things)上开源，当前最新的版本是 **AliOS Things  3.3** 。

# 架构总览

**AliOS Things** 支持多种CPU架构，包括：ARM，C-Sky，MIPS，RISCV等。
<div align="center">
  <img src="https://img.alicdn.com/imgextra/i3/O1CN01PCABfS1bFjYu67i4Y_!!6000000003436-2-tps-2882-1646.png" style="max-width:800px;" >
</div>

**AliOS Things** 适配了分层架构和组件架构。包括以下部分：

- BSP: 板级支持包
- HAL: 硬件适配层，包括WiFi，蓝牙，I2C，SPI，UART，Flash 等
- Kernel: 包括Rhino RTOS 内核，VFS，KV Storage，CLI，C++ 等
- Network: 包括LwIP 轻量级TCP/IP协议栈，BLE 低功耗蓝牙协议栈，LoRaWAN 协议栈等
- Security: 包括TLS(mbedTLS and cutomized iTLS)，ID2，SST(Trusted Storage)，Crypto，TEE(Trusted Execution Environment) 等
- AOS API: AliOS Things 提供给应用软件和组件的API
- VFS驱动框架：设备驱动提供给组件和应用的服务接口
- Component: 阿里巴巴增值和常用的物联网组件，包括LinkSDK，OTA(安全差分升级)，ulog(日志服务)，uData(传感器框架)，uDisplay(图形接口)，WiFi配网 等
- Application: 丰富的示例代码

所有的模块都作为组件的形式存在，通过yaml进行配置，应用程序可以很方便的选择需要的组件。

# AliOS Things 3.3新功能介绍
- 统一的VFS接入方式，更标准的应用开发模式
- 更小的系统，yaml构建方式更直观
- 更全面的JavaScript和MicroPython轻应用开发框架的支持
- 全面完善的组件、解决方案和系统文档。格式更规范与开发者友好
- 升级了LinkSDK，新增设备引导服务、设备诊断、日志上报功能
- 新增蓝牙配网、Wi-Fi Camera、OLED等组件，解决方案能直接调用

# AliOS Things 3.3支持硬件
**AliOS Things 3.3** 版本支持官方已适配如下硬件

- HaaS100
- HaaS EDU K1
- HaaS200


# 文档

**快速上手**

- 参考：[HaaS100快速开始](https://g.alicdn.com/alios-things-3.3/doc/haas100_quick_start.html)

- 参考：[HaaS EDU K1快速开始](https://g.alicdn.com/alios-things-3.3/doc/haas_edu_k1_quick_start.html)

- 参考：[HaaS200快速开始](https://g.alicdn.com/alios-things-3.3/doc/haas200_quick_start.html)

**文档中心**
- 请到HaaS官方网站 [文档中心](https://haas.iot.aliyun.com/)查看。

**贡献代码**

- 参考：[开发者贡献](https://g.alicdn.com/alios-things-3.3/doc/contribute.html)。

**物联网平台**

- AliOS Things 能帮助你更加快速地接入[阿里云物联网平台](https://iot.console.aliyun.com/quick_start)。

# 社区

* [技术交流群(钉钉)](https://img.alicdn.com/imgextra/i3/O1CN017fYxQq1qXL0gLsnGg_!!6000000005505-2-tps-1658-682.png)
* [HaaS技术社区](https://blog.csdn.net/HaaSTech)

# License

  AliOS Things 开源源码遵循 [Apache 2.0 license](LICENSE) 开源协议。

# Release Note

## 2021-10-12
- JavaScript轻应用：IoT-Device修复publish内存泄漏问题、优化物模型初始化流程；IoT-Gateway增加NTP时间获取功能、完善拓扑管理、子设备注册和登陆登出等API；新增以太网配网功能，支持静态IP配网；新增UDP功能；更新JS轻应用版本信息获取接口；修复setTimeout、SPI的write问题等
- python轻应用：micropython引擎升级到V1.17；API对齐HaaS轻应用接口标准；新增BLE支持；GPIO适配层重构；封装Python视觉AI接口
- components：新增linkkit、ble_mesh、genie_service组件用于天猫精灵IoT生态的接入；
- ucloud_ai：增加车牌识别和人体检测接口
- solutions：根据系统性能决定-j线程数量提升编译速度;新增linkkit_genie_demo案例：基于HaaS200通过Wi-Fi接入天猫精灵IoT生态的案例；新增genie_mesh_demo案例：基于HaaS200实现可被天猫精灵连接的ble mesh
- posix：头文件标准化
- libc_stub：支持libc多线程
- rhino：支持libc多线程
- http：修复因为http buffer大小更新导致ota栈溢出问题
- ota：修复ota断点续传失效问题
- driver：新增PWM AOS接口；修复UART发送数据流中间会有过长的间隔的问题。

*特别感谢为本期迭代提出宝贵意见和建议的开发者：陈文源、莫学良等*

## 2021-08-26
- JavaScript轻应用：修改http对接问题、支持tcp及mqtt组件、新增智慧农业样板间示例代码等
- python轻应用：增加智能对话机器人python接口、增加token获取接口
- ucloud_ai：增加智能对话机器人、增加token获取接口
- ai_agent：增加“HaaS HaaS”语言唤醒功能、增加TFLite-Micro推理引擎
- solution：添加TFLite-Micro离线快捷词唤醒案例
- netmgr：支持根据指定channel进行扫描、修复wifi.conf内容为空导致异常问题等
- HaaS100：更新驱动库、优化二级boot、产测功能可配置等
- HaaS200：更新驱动库、优化二级boot等
- 文档：新增haas-studio使用说明和代码风格规范

*特别感谢为本期迭代贡献代码的开发者：WilliamArthur、忙碌的死龙等*

## 2021-07-15
- JavaScript轻应用：支持oss上传及下载功能、支持promise功能、添加ulog示例、修复问题若干
- python轻应用：支持ota功能、linksdk动态注册、支持在线文件传输、framebuf显示功能、kv功能
- ota：统一haas100和haas200的ota升级相关接口
- HaaS100：支持MTD驱动子系统、修复SPI初始化失败等问题
- vfs：修复多任务并发访问设备节点的互斥问题
- netmgr：支持AP功能、完善连接失败的重试机制、修复上电无法自动重连问题等
- cli：修复命令回显没有发送至云端问题
- 文档：优化贡献代码说明、添加ping/ifconfig/tftp/iperf等命令说明


*特别感谢为本期迭代提出宝贵意见和建议的开发者：空空、刘洪峰、刘超、莫学良、李大棚等*


## 2021-06-10
- JavaScript轻应用框架v3.0发布
- 新增案例：JavaScript和Python双引擎运行环境，一个镜像同时包含两套轻应用框架
- HaaS200支持PSRAM，4MB内存全解锁
- HaaS200 OTA支持压缩升级，增加更多空间给系统分区
- 优化HaaS200烧录成功率
- 增加HaaS200 windows编译支持
- 优化文档，优化各案例名称

## 2021-06-03
- 发布蓝牙配网小程序
- HaaS200 OTA功能发布
- HaaS200 支持Python/Javascript轻应用
- HaaS200 加强debug能力
- HaaS200 驱动子系统发布（UART/GPIO/MTD/Watchdog/I2C）

## 2021-05-25
- 增加HaaS200支持
- CSI驱动系统发布，支持I2C，flash，Uart，GPIO，watchdog
- 增加继电器外设案例
- 优化蓝牙协议栈
- 优化EDU K1烧录

## 2021-05-08
- 实现蓝牙配网的功能
- 优化文档
- 解决linksdk_gateway_demo案例无法配网问题
- mbedtls: 解决在package.yaml配置不生效问题
- audio_demo: 解决打包文件找不到问题，感激空空发现这个问题
- rfid_demo: 解决uart2打开失败问题，感激蒋才林发现这个问题
- audio_demo: 优化步骤，自动下载三方代码
- 新增语音播放、千里传音、屏幕显示、蓝牙配网、IDE一键热更新等众多功能

## 2021-04-28
- 使用支付宝小程序对HaaS100进行配网、配置三元组以及操控HaaS LED灯的案例 客户紧急需要支付宝小程序案例

## 2021-04-21
- 统一的VFS接入方式，更标准的应用开发模式
- 更小的系统，yaml构建方式更直观
- 更全面的JavaScript和MicroPython轻应用开发框架的支持
- 全面完善的组件、解决方案和系统文档。格式更规范与开发者友好
- 升级了LinkSDK，新增设备引导服务、设备诊断、日志上报功能
- 新增蓝牙配网、Wi-Fi Camera、OLED等组件，解决方案能直接调用
