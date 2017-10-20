# AliOS Things  
　　AliOS家族旗下面向IoT领域的轻量级物联网嵌入式操作系统AliOS Things将致力于搭建云端一体化IoT基础设施，具备极致性能、极简开发、云端一体、丰富组件、安全防护等关键能力，并支持终端设备连接到阿里云Link，可广泛应用在智能家居、智慧城市、新出行等领域。
  
## AliOS Things　特性

**极简开发**  

　　基于Linux平台，提供MCU虚拟化环境，开发者直接在Linux平台上开发硬件无关的IoT应用和软件库，使用GDB/Valgrind/SystemTap 等PC平台工具诊断开发问题  
　　提供IDE，支持系统/内核行为Trace、Mesh组网图形化显示  
　　提供Shell交互，支持内存踩踏、泄露、最大栈深度等各类侦测  
　　提供面向组件的编译系统以及Cube工具，支持灵活组合IoT产品软件栈  
　　提供包括存储(掉电保护、负载均衡)在内的各类产品级别的组件
  
**即插即用的连接和丰富服务**  

　　支持umesh即插即用网络技术，设备上电自动连网  
　　通过Alink与阿里云计算IoT服务无缝连接

**细颗粒度的FOTA更新**  

　　支持应用代码独立编译映像，IoT App独立极小映像升级  
　　支持映像高度压缩

**彻底全面的安全保护**
  
　　提供系统和芯片级别安全保护  
　　支持可信运行环境(支持ARMV8-M Trust Zone)  
　　支持预置ID2根身份证和非对称密钥以及基于ID2的可信连接和服务

**高度优化的性能**
  
　　内核支持Idle Task成本，Ram<1K,Rom<2k，提供硬实时能力  
　　提供YLOOP事件框架以及基于此整合的核心组件，避免栈空间消耗，核心架构良好支持极小FootPrint的设备

**解决IoT实际问题的特性演进**
  
　　更好的云端一体融合优化，更简单的开发体验，更安全，更优整体性能和算法支持，更多的特性演进，我们在路上  
　　Stay Hungry，Stay Foolish！

-----

# 文档

  * [Coding Style Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Coding-Style-Guide)

  * [Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Porting-Guide)

  * [API Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-API-Guide)

  * [Application Development Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-APP-DEV-Guide)

  * [AliOS Studio](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio)

------

# 下载

  * [VS Code](https://code.visualstudio.com)

  * [Python](https://www.python.org/downloads/)

  * [GCC](https://launchpad.net/gcc-arm-embedded/+download)

  * [FTDI: UART driver](http://www.ftdichip.com/Drivers/D2XX.htm)

  * [STLink](http://www.st.com/content/st_com/en/products/development-tools/hardware-development-tools/development-tool-hardware-for-mcus/debug-hardware-for-mcus/debug-hardware-for-stm32-mcus/st-link-v2.html)

  * [CP2012 driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

------

# License

  Alios is released under the Apache 2.0 license.

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
