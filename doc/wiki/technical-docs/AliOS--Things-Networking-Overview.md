EN | [中文](AliOS--Things-Networking-Overview.zh)

# AliOS Things Networking Overview

AliOS Things released by Alibaba is an open-source implementation of operating system (OS) for Internet of Things (IoT). It is not only a traditional RTOS, but an embedded operating system with plenty of connecting components, including:

- uMesh network
- Socket Adapter Layer (SAL)
- TCP/IP network protocol Stack (LwIP)
- Connecting protocol 
  - WiFi
  - Bluetooth Low Energy (BLE)
  - LoRaWAN
  - GPRS
  - NB-IoT

In the first three parts (uMesh, SAL and LwIP), we will mainly introduce their functions and application scenarios. In the last part, about connecting protocol, suggested ways of using AliOS Things network components will also be given based on different scenarios.

## uMesh network

One of the core components of AliOS Things, uMesh, provides AliOS Things with capabilities of organizing network by themselves and achieving local interconnection among devices. uMesh is characterized as self-organized, self-healing and multi-hop, and It's suitable for scenarios that need large-scale deployment. Its main capabilities and features include:

- support IPv4  IPv6
- support various communication media, such as WiFi, BLE, 802.15.4
- support heterogeneous networking among different communication media
- support tree topology and mesh topology
- support low power
- can authenticate equipments by using ID2, and encrypt data through AES-128

uMesh is located above MAC layer and under IP layer. It mainly consists of three parts: the core part of the protocol, the connecting protocol abstraction layer and IP protocol adaptation layer. It's suitable for scenarios that need large-scale deployment, such as indoor positioning, smart lighting and business environment. Currently, it mainly supports WiFi, but supports for low power and BLE mesh are also under development.

## Socket Adapter Layer (SAL)

SAL is used to accelerate the development of MCU communication chips with different connecting abilities. In such scenario,  MCU is connected to communication chips such as WiFi and NB-IoT through UART or SPI line. The operating system of AliOS Things and users' APP are operated in MCU, and when network data access is needed, the receiving and transmitting of network load can be enabled through externally-connected communication chips. The communication between MCU and communication chips can be realized through AT Command channel or other private protocol channels.

## TCP/IP network protocol Stack (LwIP)

AliOS Things's TCP/IP network Protocol Stack is a deeply customized and optimized one based on LwIP 2.0.0. Its main capabilities and features include:

- IPv4
- IPv6
- ICMP (Internet Control Message Protocol)
- UDP (User Datagram Protocol)
- TCP (Transport Control Protocol)
- ARP (Address Resolution Protocol)
- NDP (Neighbor Discovery Protocol)
- MLD (Multicast Listener Protocol)
- IGMP (Internet Group Management Protocol)
- DHCP client
- TFTP Server
- TFTP Client

Besides these, it has been further optimized under the condition of multi-concurrent connections and large data volume.

## Connecting Protocol

AliOS Things supports plenty of connecting chips and protocols. From now on, AliOS Things has already supported WiFi, BLE and LoRaWAN, and new support is increasingly added. 

### WiFi

AliOS Things offers a simple and convenient WiFi Hardware Abstraction Layer(HAL). Once developers complete the connection with HAL, they can use all the functional components in application layer. It also supports externally-connected communication chips, where SAL works for connecting and abstracting. Once the connection is completed, components in application layer are also available for developers.

WiFi chip is one of the widest-supported connecting chips, and it can mainly be divided into four types:

- Integrated WiFi chip, doesn't support TCP/IP Connecting Protocol
- Integrated WiFi chip, supports TCP/IP Connecting Protocol
- External-connected WiFi chip, doesn't support TCP/IP Connecting Protocol
- External-connected WiFi chip, supports TCP/IP Connecting Protocol

Based on the four different types, AliOS Things offers various combinations of functional components. LwIP and SAL are must-options decided by the type of WiFi chips, while network configuration and support of WiFi uMesh are set according to actual needs. Detailed introduction of SAL can be seen here.https://yq.aliyun.com/articles/292612?spm=5176.10695662.1996646101.searchclickresult.7e64b1b8O39f8y

### LoRaWAN

LoRaWAN can support both Class A and Class C mode in AliOS Things. Developers can use the interfaces of LoRa to achieve the sending and receiving of data between end side and server side. Besides, they can use AliOS Things and LoRaWAN sample to complete the basic connection and communication between LoRa's gateway and server.

LoRa users often externally connect Semtech LoRa chip with MCU. AliOS Things can support sx1276, and chip developers can directly use LoRaWAN Protocol Stack provided by AliOS Things to carry out development. If other Semtech LoRa chips are used, the connection with the interface can be realized according to LoRa radio layer interface integrated in AliOS Things.

The LoRaWAN Protocol Stack of AliOS Things provides not only LoRaWAN protocol on the end side, but also the whole test environment. When developers complete the development of AliOS Things LoRaWAN Protocol Stack, they can carry out pre-test and verification by using the test environment provided by AliOS Things. You can click here to know more about the construction and usage of our test environment.AliOS-Things-lorawanapp%E7%A4%BA%E4%BE%8B

Nowadays, LoRaWAN developers can use the interface in LoRa protocol layer for further development, and we are still enriching interface in application layer based on application scenarios, so that application developers can achieve minimalist development with no need to understand too many details of LoRaWAN protocol.

### Bluetooth

BLE components can now support Bluetooth 4.2, the application programming framework and Application Programing Interface (API) on mk3239. The application programming framework can abstract the details of BLE protocle and provide API for application developers. For application developers, there's no needs to care about details of BLE protocol stack, and thus minimalist development can be achieved. For network developers, BLE components provide a whole series of interfaces in protocol layer, enabled them to use the whole BLE protocol. 

The application framework can mainly provide:

- connection with BLE
- reading and updating of sensor data
- broadcasting of data

Developers can use the interface provided by application development framework to send the data collected by the peripherals from GATT server to the GATT client. By offering the framework, Bluetooth application developers can directly build their applications based on our given interface. By now, AliOS Things has managed to offer application development interface for peripheral ports, which can read and update the sensor data, and developers can build their own applications on it.

We are still working on the enrichment of this framework, developing Bluetooth 5.0, and expanding what Bluetooth chips can support. In addition, support for BLE mesh is also going to be added in uMesh, which will completely support SIG BLE mesh in the future.

### GPRS and NB-IoT

NB-IoT and GPRS are now externally connected to the master chip as communication chips. The talk to communication chips via AT command set has been supported. Efforts are also made in integrating NB-IoT and GPRS to operate directly in a same SoC.

## Summary

AliOS-Things has already supported plenty of communication protocols and chips, and it is still under rapid development. If you want to know more about it,  please click https://github.com/alibaba/AliOS-Things.
