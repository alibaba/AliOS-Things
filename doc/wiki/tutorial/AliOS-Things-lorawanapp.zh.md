[EN](AliOS-Things-lorawanapp) | 中文

AliOS Things 是 AliOS 家族旗下的、面向 IoT 领域的、轻量级物联网嵌入式操作系统。文本旨在给大家示范一下其中的lorawanapp的示例，完成一个LoRaWAN网络的构建和数据传输，并通过该示例让大家对AliOS Things有一个粗浅的了解。

## 系统框架图：
![AOS Lorawanapp](https://img.alicdn.com/tfs/TB1ltoUm26H8KJjy0FjXXaXepXa-1126-405.png)

## 系统特性:
- 基于AliOS Things的Lorawan全功能支持，用户可通过设置Class A达到最低的应用功耗特性
- 增加AliOS Things内核特性支持
- 可选多bin特性支持：实际硬件资源十分有限，通过多bin特性可以单独对应用端代码做升级，提供升级效率并大大降低升级功耗
- 可选KV特性支持：增加用户Flash的使用安全和使用寿命，提高应用数据的保存安全性
- 可选CLI特性支持：增加命令接口，方面代码调试提高开发效率
- 以上可选特性需根据实际硬件资源做适配

## 如下分别介绍了基于VSC, IAR, Keil三个Toolchain的工程示例：
- [基于VSC](AliOS-Things-lorawanapp-@VSC)
- [基于IAR](AliOS-Things-lorawanapp-@IAR)
- [基于Keil](AliOS-Things-lorawanapp-@Keil)

## 不同Toolchain下代码大小比较
   |    Compiler    | Flash(KB) | RAM(KB) | Optimization Level |
   | -------------- | --------- | ------- | ------------------ |
   | gcc            |  61.26    | 6.67    | -Os                |
   | iccarm(IAR)    |  43.83    | 6.18    | -O3                |
   | armcc(Keil)    |  42.64    | 6.20    | -O3                |


*因为LoRaWAN协议栈内有浮点运算，而大部分跑LoRaWAN协议栈的mcu均不含硬件浮点能力，所以需要软件实现，不同编译器会有比较大的代码差异。

## 结语

这边给大家介绍了一个AliOS Things的LoRaWAN示例，因为是系统级的示例，步骤比较多，难免有错漏的地方，请指正，此外因为该示例是LoRaWAN网络的初始实现，细节部分不完善，如API的接口，函数封装不够细致等等，但主要目的是让大家了解AliOS Things的使用和LoRaWAN网络的应用，让大家有一个感性的认识，希望大家可以以此为起点，去感受AliOS Things的易用和强大，以及快速实现LoRaWAN的部署和测试。
