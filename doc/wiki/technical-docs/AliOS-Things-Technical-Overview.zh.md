[EN](AliOS-Things-Technical-Overview) | 中文

# 技术架构总览

[AliOS Things](https://github.com/alibaba/AliOS-Things) 是一款由阿里巴巴开发的轻量级物联网操作系统。它在2017年杭州云栖大会中问世，并在同年10月20号于github:<https://github.com/alibaba/AliOS-Things>开源。

## 架构概述

AliOS Things的架构可以适用于分层架构和组件化架构。从底部到顶部，AliOS Things包括：

- 板级支持包（BSP）：主要由SoC供应商开发和维护

- 硬件抽象层（HAL）：比如WiFi和UART

- 内核：包括Rhino实时操作系统内核、Yloop, VFS, KV 存储

- 协议栈：包括TCP/IP协议栈（LwIP），uMesh网络协议栈

- 安全：安全传输层协议（TLS），可信服务框架（TFS）、可信运行环境（TEE）

- AOS API：提供可供应用软件和中间件使用的API

- 中间件：包括常见的物联网组件和阿里巴巴增值服务中间件

- 示例应用：阿里自主开发的示例代码，以及通过了完备测试的应用程序（比如Alinkapp）

所有的模组都已经被组织成组件，且每个组件都有自己的.mk文件，用于描述它和其它组件间的依赖关系，方便应用开发者按需选用。 

### 结构框图

---

![block_digram](https://img.alicdn.com/tfs/TB1fKQMihrI8KJjy0FpXXb5hVXa-2330-1292.png)

### 文件夹结构

---

| 文件夹名称     | 内容描述                             |
| --------- | -------------------------------- |
| board     | 评估板（如STM32L496G-Discovery）       |
| build     | 编译框架                             |
| device    | 连接MCU/SoC的外设，比如支持使用AT命令的WiFi系列模组 |
| example   | 代码示例，通过了完备测试的应用程序（比如Alink）       |
| framework | IoT 通用组件                         |
| include   | 系统头文件                            |
| kernel    | 包括Rhino和协议栈                      |
| platform  | 芯片架构支持的相关文件                      |
| security  | 包括TLS，TFS， TEE在内的安全组件            |
| tools     | 命令行界面（CLI）和用于建立远程设备中心的testbed工具  |
| utility   | IoT通用软件库，比如 cJSON                |
| test      | UT测试用例                           |

## 内核介绍

### Rhino 实时操作系统内核 

Rhino是AliOS Things内部设计和开发的实时操作系统。它具有体积小、功耗低、实时性强和调试方便等特点。Rhino提供了丰富多元的内核原语，包括缓冲队列，环形缓冲区、定时器、信号量、互斥量、先入先出队列、事件等。

#### 体积小

Rhino为大多数内核对象提供静态和动态分配。为小内存块设计的内存分配器既支持固定块又支持可变块，它还可以支持多个内存区域。

大部分的内核特性，如work queue，和内存分配器，都可以通过修改k_config.h文件进行配置和裁剪。

由于组件可配置和可裁剪，可以让最终编译出的Rhino镜像尽可能小，使其可以被烧录进资源非常有限的设备中。

#### 功耗低

对于物联网设备来说，硬件功率至关重要，因为电量是有限的。如果系统消耗电量过快，它将很快没电。Rhino提供了CPU的tickless idle 模式来帮助系统节约电能和延长使用时间。

通常情况下，当CPU没有执行操作时，它将执行一个处理器指令（对于ARM来说的WFI，对于IA32位处理器来说的HLT），进入低功耗状态。此时，CPU寄存器的信息被保存，系统的tick clock interrupts会在每个tick时刻唤醒CPU。

为了比正常模式节省更多的电量，Rhino为CPU提供了tickless idle模式。当操作系统检测到有一个固定时间（多个ticks或更长时间）的空闲后，它将进入tickless idle模式。系统做好中断配置，并把CPU置于C1模式，那时system tick clock中断不再被触发，system tick的计数也将停止。CPU会保持低耗电状态直到tickless idle时间结束。然后，当system tick timer interrupt再次被触发时，唤醒CPU从C1模式回到C0模式，为ticks计算好补偿时间并继续计数。

#### 实时性

Rhino提供了两个调度策略，基于优先级的抢占式调度和round-robin循环调度策略。对于两个调度策略而言，具有最高优先级的任务都是被优先处理的。

基于优先级的抢占式调度器会在遇到比当前运行的任务具有更高优先级任务时抢占CPU。这意味着，如果出现一个任务比当前任务具有更高优先级，内核将立即保存当前任务的context，并切换到高优先级的任务的context。因此，内核保证CPU总是优先处理优先级最高的任务。

round-robin调度器通过时间片来为各任务分配CPU资源。在一组具有相同优先级的任务中，每个任务都会被安排运行一个固定的时间长度，或者说时间片，之后CPU将处理下一个任务。所以，在一个任务阻塞之前，其他任务不能抢夺到处理器资源。当时间片失效时，系统将运行该优先级就绪队列中的最后一个任务。

#### 方便调试

Rhino 可以支持stack溢出、内存泄漏、内存损坏的检测，这有助于开发人员找出棘手问题的根源。结合AliOS Studio的集成开发环境（IDE），Rhino的追踪功能将实现整个系统运行活动的可视化。

### Yloop 事件架构

Yloop是[AliOS Things](https://github.com/alibaba/AliOS-Things)的异步事件框架。借鉴了[libuv](https://github.com/libuv/libuv)及嵌入式业界常见的event loop，综合考虑使用复杂性，性能，及footprint，实现了一个适合于MCU的事件调度机制。Yloop提供了一套机制来统一调度管理IO（主要是socket），定时器，执行函数和事件，在大大降低了内存的使用的同时，避免了多线程编程的复杂性。

每个Yloop实例（aos_loop_t）与特定的任务上下文绑定。主任务以外的任务也可以创建自己的Yloop实例。多Yloop实例也可以被创建，其中每个instance都被绑定到一个单一的任务，让强大的硬件获得更好的性能。

### 键值对存储（KV）

KV组件是AliOS Things中一个以Key-Value方式进行持久化存储的轻量级组件，主要为基于Nor Flash的小型MCU设备提供通用的Key-Value持久化存储接口。它的优势和特征包括：

- 更少的擦写次数来延长flash的使用寿命
- 电源安全，没有中间状态将存在

- 方便使用，关键码值支持二进制格式数据
- 最低支持Flash的大小是8KB

### 协议栈介绍

为了帮助设备更容易地连接到云端，AliOS Things通过灵活的方式提供协议栈。

面向IP设备：

- 为直接连接的SoC提供了测试良好的LwIP协议栈，包括WiFi SoC，MCU+SDIO/SPI WiFi模块等。

- 为连接了通信模块（如WiFi，NB，GPRS）的MCU提供了SAL
- 提供uMesh去构建更复杂的网状网络拓扑


对于非IP设备：

- LoRaWAN协议栈已经集成到系统中

- 提供BLE标准的API和BLE 协议栈


在不久的将来，LoRaWAN和BLE将被集成。

### TCP / IP协议栈（LwIP）

AliOS Things拥有一个基于LwIP V2.0.0 的TCP/IP协议栈，支持IPv4，IPv6，IPv4和IPv6的共存。IPv4和IPv6已经在持续集成（CI）系统中经过良好测试，IPv6也已经在uMesh中被广泛应用和测试。

### 套接字适配层(SAL)

SAL为 WiFi/GPRS/NB-IoT系列模组提供了标准的Socket功能。特别地，考虑到AT命令是这个场景中最流行的形式，提供了AT Parser来帮助处理。

有了SAL，开发人员可以使用标准的Socket API访问网络，这将减少现有软件组件的集成工作。

### 自组织网络协议(uMesh)

uMesh是一个具有如下特征的mesh：

- 无缝支持IPv4 和IPv6
- RF标准独立，支持WiFi，BLE， 802.11和802.15.4等通信媒介
- 支持不同通信媒介间的异构组网
- 支持树状拓扑，网状拓扑和分层树状网格拓扑
- 支持低功耗特性
- 使用ID2对设备进行认证，AES-128对数据进行加密

## 安全性介绍

### 安全传输层协议（TLS）

继承于mbedtls, 对footprint进行了高度优化

### 信任框架服务（TFS）

可以对接大部分安全服务（比如 ID²）的框架

### 网络设备ID（ID²）

可信的IoT设备身份属性

### 密钥管理（KM）

通过使用硬件的安全功能提供可信的 runtime Root 

### Ali-Crypto

提供经典算法实现

### 可信执行环境（TEE）

提供完整的可信执行环境，现可支持C-Sky CK802T，不久的将来可支持ARMV8-M。

## 中间组件

### 空中固件升级（FOTA）

FOTA 使设备固件更新容易。AliOS Things可根据硬件配置给出FOTA解决方案，提供端到端的解决方案。

特征:

- 支持丰富的物联网协议 (Alink，MQTT ，COAP）

- 支持HTTP / HTTPS / COAP固件下载

- 支持多bin、delta和A／B更新

- 提供OTA HAL方便进入端口

### uData

uData框架是基于传统Sensor Hub概念，结合IoT的业务场景和AliOS Things物联网操作系统的特点设计而成的一个面对IoT的感知设备处理框架。

uData框架主要分kernel和framework两层，kernel层主要是负责传感器驱动，硬件端口配置和相关的静态校准，包括轴向校准等；framework层主要是负责应用服务管理、动态校准管理和对外模块接口等。

uData的目标范围服务是物联网商业服务，像无人机玩具、智能路灯、扫地机器人等。传感器驱动程序不仅能提供传感器SDK，还能提供传感器驱动像ALS，气压计，温度，加速度计，陀螺仪，磁力计等。

### 物联网协议

AliOS Things支持丰富的云端连接协议：

- Alink：阿里云平台，适用于智能生活； 也包括WiFi配置组件YWSS。

- MQTT：标准MQTT协议；已和阿里云物联网套件良好结合。
- COAP：基于UDP的轻量级协议。和COAP FOTA结合便可为NB-IoT设备建立一个只有UDP的系统。

### AT 解析器

AT解析器提供了处理AT命令连接通信模块的框架。AT解析器可处理串行流解析；回调OOB可以处理模块的特殊AT命令。与SAL一起使用，应用程序甚至可以在AT模块中使用BSD socket。.

## 工具

### AliOS Studio 集成开发环境（IDE）

作为VS代码插件执行，可提供编辑/编译/调试功能。详情请看 <AliOS-Things-Studio>

### uDevice Center

和AliOS Studio一起，它为开发者提供了一个远程和多设备的调试环境。借助backbone测试平台框架，基于真实硬件的持续集成（CI）系统可以被方便地建立。

## 小结

AliOS Things是为电量和资源有限的MCU，连接套接字SoC设计的，并非常适合于物联网设备。更多细节可以点击 <https://github.com/alibaba/AliOS-Things/wiki>

 
