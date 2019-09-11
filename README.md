<br/>
<div align="center">
  <img src="https://img.alicdn.com/tfs/TB1e1U7vyAnBKNjSZFvXXaTKXXa-973-200.png" height="60">
</div>
<br/>
<div align="center">

[![Join the chat at https://gitter.im/aliosthings/Lobby](https://img.shields.io/gitter/room/aliosthings/Lobby.svg?style=flat-square)](https://gitter.im/aliosthings/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

</div>
<br/>

EN | [中文](./README-zh.md)

AliOS Things is Alibaba's IoT version of AliOS Family, it was announced in [The Computing Conference 2017](https://yunqi.aliyun.com) in Hangzhou by Alibaba Cloud, and open sourced in 20<sup>th</sup>, October, 2017 at github:https://github.com/alibaba/AliOS-Things.

## Architecture Overview

AliOS Things supports multiple architectures, including ARM, C-Sky, MIPS, RISCV, rl78, rx600, xtensa etc. 200+ AliOS Things certified chips and modules, and 100+ certified sensors.

AliOS Things adapts Layered Architecture and Component Architecture including:

- BSP: Board Support Package
- HAL: Hardware Abstraction Layer, includes WiFi, UART, Flash etc.
- Kernel: includes Rhino RTOS Kernel, VFS, KV Storage, CLI, C++ etc. 
- Network: includes LwIP(Lightweight TCP/IP), uMesh mesh networking stack, BLE(Bluetooth Low Energy), LoRaWan stack, AT Commands Module etc.
- Security: includes TLS(mbedTLS and cutomized iTLS), ID2, SST(Trusted Storage), Crypto, TEE(Trusted Execution Environment) etc.
- AOS API: AliOS Things exposed APIs for Application and Middleware
- Middleware: Alibaba's value-added and commonly seen IoT components, includes Linkkit, OTA(Differential Incremental Security Upgrade), ulog(log service), uData(Sensor Framework), uLocation(Location Framework), WiFi Provision etc.
- Application: a lot of sample codes

All modules have been organized as Components, Configuring via menuconfig, which enables applications to choose components needed easily.

## Documentation

- AliOS Things wiki: [Github Wiki](https://github.com/alibaba/AliOS-Things/wiki)
- AliOS Things documentation: [Alibaba Cloud IoT Documentation](https://dev.iot.aliyun.com/doc) | AliOS Things

### How to contribute

Please refer to [Contributing Guideline](https://github.com/alibaba/AliOS-Things/wiki/contributing).

### Quick Start by Command Line using Ubuntu Machine

Please refer to [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start).

### Quick Start by AliOS Studio

AliOS Things uses [vscode](https://code.visualstudio.com/) as IDE, supporting Windows/MAC/Linux.
Please refer to [AliOS Studio](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio).

### IoT Platform

AliOS Things can help you connect your devices to [Alibaba Cloud IoT platform](https://iot.console.aliyun.com/quick_start) more quickly.

## Community

* [DingTalk Group](https://img.alicdn.com/tfs/TB1X2HOhYPpK1RjSZFFXXa5PpXa-970-1280.png)
* [Alibaba Cloud IoT Community](https://developer.aliyun.com/group/aliiot)

## License

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
  AliOS Things is released under the [Apache 2.0 license](LICENSE)
=======
  AliOS Things is released under the Apache 2.0 license.

    Copyright 2015-2017 Alibaba Group Holding Ltd.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at following link.
    
    http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

------

## AliOS Things概述

AliOS Things 是 AliOS 家族旗下的、面向 IoT 领域的、轻量级物联网嵌入式操作系统。 AliOS Things 将致力于搭建云端一体化 IoT 基础设施，具备极致性能、极简开发、云端一体、丰富组件、安全防护等关键能力，并支持终端设备连接到阿里云 Link，可广泛应用在智能家居、智慧城市、新出行等领域。

**极简开发**  

- 基于Linux平台，提供MCU虚拟化环境，开发者直接在Linux平台上开发硬件无关的IoT应用和软件库，使用GDB/Valgrind/SystemTap 等PC平台工具诊断开发问题
- 提供IDE，支持系统/内核行为Trace、Mesh组网图形化显示  
- 提供Shell交互，支持内存踩踏、泄露、最大栈深度等各类侦测  
- 提供面向组件的编译系统以及Cube工具，支持灵活组合IoT产品软件栈  
- 提供包括存储(掉电保护、负载均衡)在内的各类产品级别的组件

**即插即用的连接和丰富服务**  

- 支持umesh即插即用网络技术，设备上电自动连网  
- 通过Alink与阿里云计算IoT服务无缝连接

**细颗粒度的FOTA更新**  

- 支持应用代码独立编译映像，IoT App独立极小映像升级  
- 支持映像高度压缩

**彻底全面的安全保护**

- 提供系统和芯片级别安全保护  
- 支持可信运行环境(支持ARMV8-M Trust Zone，CK802 TEE)  
- 支持预置ID2根身份证和非对称密钥以及基于ID2的可信连接和服务

**高度优化的性能**

- 内核支持Idle Task成本，Ram<1K,Rom<2k，提供硬实时能力  
- 提供YLOOP事件框架以及基于此整合的核心组件，避免栈空间消耗，核心架构良好支持极小FootPrint的设备

**解决IoT实际问题的特性演进**

- 更好的云端一体融合优化，更简单的开发体验，更安全，更优整体性能和算法支持，更多的特性演进，我们在路上  
  Stay Hungry，Stay Foolish！
>>>>>>> 4536222e4 (update README.md)
=======
  AliOS Things is released under the [Apache 2.0 license](LICENSE)
>>>>>>> 3fc4718cc (update logo (#547))
=======
  AliOS Things is released under the [Apache 2.0 license](LICENSE)
>>>>>>> 2a6fe3e27 (update README (#862))
=======
  AliOS Things is released under the [Apache 2.0 license](LICENSE)
>>>>>>> 895a68ed8 (BugID:16944760: merge github master back)
=======
  AliOS Things is released under the [Apache 2.0 license](LICENSE)
>>>>>>> 2c8f3f7e5 (BugID: 17966605: update mainly README.md & README-zh.md)
