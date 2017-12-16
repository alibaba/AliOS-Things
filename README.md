# AliOS Things  

(中文描述在最后)

AliOS Things is Alibaba's IoT version of AliOS Family, it's announced in [The Computing Conference](https://yunqi.aliyun.com) 2017 Hangzhou by Alibaba Cloud, and open sourced in 2017/10/20.

AliOS Things is designed for low power, resource constrained MCU, connectivity SoC, greatly suitable for IoT devices. AliOS Things is not just a RTOS, it contains full stack of software components and tools for building IoT devices. 

# Architecture Overview

From architectural point of view, AliOS Things adapts Layered Architecture, and Component Architecture. From bottom to top, AliOS Things includes:

- BSP: Board Support Package mainly developed and maintained by SoC Vendor
- HAL: Hardware Abstraction Layer, like WiFi, UART
- Kernel: Rhino RTOS Kernel, Yloop, VFS, KV Storage included
- Protocol Stack: LwIP TCPIP Stack, uMesh mesh networking stack included
- Security: TLS, TFS(Trusted Framework Service), TEE(Trusted Exexcution Environment)
- AOS API: AliOS Things exposed APIs for Application and Middleware
- Middleware: Alibaba's value-added and commonly seen IoT components included
- Examples: hands-on sample codes, and well tested industry applications such as Alinkapp

And all of the modules are organized as Component, each component has its own .mk file to describe its dependecy with other Components, which make it easy for applications to choose the components needed.

# Documentation

AliOS Things' documentation are mainly hosted at [Github Wiki](https://github.com/alibaba/AliOS-Things/wiki).

### Quick Start by Command Line using Ubuntu Machine

```shell
$ pip install aos-cube
$ git clone https://github.com/alibaba/AliOS-Things.git
$ cd AliOS-Things
$ aos make helloworld@linuxhost
$ ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

Quick Start by IDE(supporting Windows/MAC/Linux)

Please refer to [AliOS Things Application Develop Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-APP-DEV-Guide).

# Community

  * [Technical Blog | 云栖社区](https://yq.aliyun.com/teams/184)
  * [Technical Forum | 开发者论坛](https://bbs.aliyun.com/thread/410.html)

# License

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
- 支持可信运行环境(支持ARMV8-M Trust Zone)  
- 支持预置ID2根身份证和非对称密钥以及基于ID2的可信连接和服务

**高度优化的性能**

- 内核支持Idle Task成本，Ram<1K,Rom<2k，提供硬实时能力  
- 提供YLOOP事件框架以及基于此整合的核心组件，避免栈空间消耗，核心架构良好支持极小FootPrint的设备

**解决IoT实际问题的特性演进**

- 更好的云端一体融合优化，更简单的开发体验，更安全，更优整体性能和算法支持，更多的特性演进，我们在路上  
  Stay Hungry，Stay Foolish！