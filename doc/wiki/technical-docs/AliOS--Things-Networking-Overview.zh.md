[EN](AliOS--Things-Networking-Overview) | 中文

# AliOS Things 网络连接技术概述

AliOS Things具有丰富的连接组件和能力，主要包括:

* 自组织网络协议(uMesh)
* 套接字适配层(SAL)
* TCP/IP网络协议栈(LwIP)
* 连接协议(WiFi,BLE, LoRaWAN, GPRS和NB-IoT)

自组织网络(uMesh)，套接字适配层(SAL)和TCP/IP网络协议栈(LwIP)主要是功能和使用场景的介绍。连接协议部分除了功能性介绍，还会根据场景给出AliOSThings网络组件的建议使用方法。

## 自组织网络

AliOS Things核心组件之一uMesh提供了AliOS Things原生自组织网络能力，提供了AliOS Things本地互联互通的能力。uMesh具有自组织、自修复、多跳的特性。适用于需要大规模部署的场景。主要功能特性包括：

* 无缝支持IPv4 和IPv6
* 支持WiFi、BLE和802.15.4等通信媒介
* 支持不同通信媒介间的异构组网
* 支持树状拓扑和网状拓扑
* 支持低功耗特性
* 使用ID2对设备进行认证，AES-128对数据进行加密

uMesh自组织网络协议位于MAC层之上，及IP层之下。整个协议包括三个部分，协议核心部分，连接协议抽象层，及IP协议适配层。uMesh适用于需要大规模部署的场景，如室内定位，智能照明，及商业场景。目前，主要支持WiFi，正在开发对低功耗和BLE mesh的支持。

![mesh_stack](mesh_stack.png)

## 套接字适配层

AliOS Things为了能够适配不同的连接协议，和不同连接芯片的能力，提供了SAL开发组件，来加速MCU+通信连接芯片的应用场景开发和部署。在此类应用场景中，主控MCU芯片通过UART或SPI总线与WiFi、NB-IoT等通信芯片相连，AliOS Things操作系统和用户APP运行在主控MCU中，需要网络数据访问时，通过外接的通信芯片进行网络负载的接收和发射。主控MCU和外接通信芯片之间的通信，可以是AT Command通道，也可以是厂商私有协议通道。

![sal_datagram](SAL_DATAGRAM.png)

## IP网络协议栈

AliOS Things IP网络协议栈是基于LwIP 2.0.0，深度定制和优化的TCP/IP协议栈，主要功能特性包括：

* IPv4
* IPv6
* ICMP (Internet Control Message Protocol)
* UDP (User Datagram Protocol)
* TCP(Transport Control Protocol)
* ARP (Address Resolution Protocol)
* NDP(Neighbor Discovery Protocol)
* MLD (Multicast Listener Protocol)
* IGMP (Internet Group Management Protocol)
* DHCP client
* TFTP Server
* TFTP Client

除了上述提供的功能特性，还对TCP/IP协议栈在多并发连接，大数据量条件下进行了深度优化。

## 连接协议

AliOS Things支持丰富的连接芯片和协议，目前主要支持WiFi，BLE和LoRaWAN，并且在不断添加新的支持。

### WiFi

WiFi协议栈通常由芯片厂商提供，AliOS Things提供了极简的WiFi HAL层，开发者完成HAL对接后，即可使用AliOS Things提供的丰富的应用层功能组件。同时，AliOS Things也支持外接WiFi通信芯片，由SAL(Socket Adapter Layer)完成对接和抽象。完成SAL对接后，开发者同样可以使用丰富的应用层功能组件。

WiFi芯片是目前支持最广泛的连接芯片之一。主要可以分为四种不同类型：

* 集成WiFi芯片，不支持TCP/IP协议栈
* 集成WiFi芯片，支持TCP/IP协议栈
* 外接WiFi芯片，不支持TCP/IP协议栈
* 外接WiFi芯片，支持TCP/IP协议栈

根据这四种不同的形式，AliOS Things提供了不同的功能组件组合。LwIP和SAL是根据WiFi芯片类型的必选项。WiFi配网和WiFi uMesh的支持是根据实际的需要。

![net_components](network_components.png)

### LoRaWAN

LoRaWAN组件支持端侧Class A和Class C两种模式，开发者可以利用LoRa接口实现端侧与服务端数据的收发。开发者可以使用AliOS Things的LoRaWAN sample完成与LoRa网关和服务器之间的基本连接和通信。

LoRa通常是MCU外接Semtech LoRa芯片。目前AliOS Things已经集成支持sx1276，芯片开发者可以直接使用AliOS Things提供的LoRaWAN协议栈进行开发。如果外接其它Semtech LoRa芯片，可以根据AliOS Things集成的LoRa radio层接口完成接口的对接。

AliOS Things LoRaWAN协议栈不仅提供端上LoRaWAN协议，同时也提供整套测试环境，开发者使用AliOS Things LoRaWAN协议栈完成开发后，可以利用AliOS Things提供的测试环境进行前期测试和验证。应用示例测试环境搭建和使用请猛戳这里。

目前，LoRaWAN开发者可以使用LoRa协议层接口进行开发。我们正在根据LoRa应用及场景特点，开发应用层接口，让应用开发者使用了AliOS Things LoRa接口后，做到极简开发，不需要过多的理解LoRaWAN协议细节。

### Bluetooth

目前，BLE组件已经在mk3239上支持Bluetooth 4.2，应用编程框架和应用框架接口。应用编程框架对BLE协议细节进行抽象，对应用开发者提供应用框架接口。应用开发者不再需要关心BLE协议栈细节，专注于业务逻辑开发，做到极简开发。对于网络开发者，BLE组件提供完整的协议层接口，供网络开发者使用完整的BLE协议能力。

应用框架主要提供能力包括：

* BLE连接的建立
* 传感器数据的读取和更新
* 广播数据

开发者可以根据应用开发框架提供的接口，将外设采集到的数据由GATT服务端发送到GATT客户端。提供应用开发框架最主要的原因是蓝牙应用开发者很有可能不希望花费精力在蓝牙技术的细节上，而是希望通过极简的接口快速搭建起自己的应用。目前，AliOS Things提供外设端的应用开发接口，主要解决传感器数据的读取和更新，开发者可以利用这套接口在mk3239上快速搭建基于蓝牙的应用。

我们正在加速丰富应用开发框架，开发Bluetooth 5.0，并扩展蓝牙芯片支持的规模，同时uMesh也正在加入BLE mesh支持，它将完全支持SIG BLE mesh。

### GPRS和NB-IoT

目前，NB-IoT和GPRS作为通信芯片外接在运行AliOS Things主芯片上，通过SAL对接到通信芯片的AT命令集已经可以支持。

对于NB-IoT和GPRS作为AliOS Things支持协议族一部分，直接运行在一块SoC的相关工作正在进行中。
