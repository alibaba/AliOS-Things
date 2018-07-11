EN | [中文](AliOS-Things-lorawanapp.zh)

AliOS Things is a light-weight embedded operating system for Internet of Things (IoT). This article will give you an example of Lorawanapp, and the network construction and data transfer in LoRaWAN, giving you a general knowledge of AliOS Things.

## system framework:

![AOS Lorawanapp](https://img.alicdn.com/tfs/TB1ltoUm26H8KJjy0FjXXaXepXa-1126-405.png)

## system characteristics:

-  Since AliOS Things can fully support Lorawan, users can achieve the lowest application power consumption by setting Class A
- increase AliOS Things's support for kernels
- (Optional) support for multi bin: Hardware resources are very limited. Characterized as multi bin, code in application side can be upgraded separately, improving upgrade efficiency and greatly reducing power consumption.
- (optional) support for KV: Increase use security and service life of Flash and improve the security of application data
- (optional) support for CLI: increase command interfaces, making code debugging easier and improving development efficiency
- the above optional features need to be configured according to actual hardware resources

Optional multi bin feature support: the actual hardware resources are very limited. Through multi bin features, it can upgrade the application code separately, provide upgrading efficiency and greatly reduce the power consumption of upgrading.

- optional KV feature support: increase user Flash's use security and service life, improve the security of application data

- optional CLI feature support: increase command interface, code debugging and improve development efficiency

- the above optional features need to be adapted to the actual hardware resources

Optional multi bin feature support: the actual hardware resources are very limited. Through multi bin features, it can upgrade the application code separately, provide upgrading efficiency and greatly reduce the power consumption of upgrading.

- optional KV feature support: increase user Flash's use security and service life, improve the security of application data

- optional CLI feature support: increase command interface, code debugging and improve development efficiency

- the above optional features need to be adapted to the actual hardware resources

Optional multi bin feature support: the actual hardware resources are very limited. Through multi bin features, it can upgrade the application code separately, provide upgrading efficiency and greatly reduce the power consumption of upgrading.

- optional KV feature support: increase user Flash's use security and service life, improve the security of application data

- optional CLI feature support: increase command interface, code debugging and improve development efficiency

- the above optional features need to be adapted to the actual hardware resources

## The following are three projects based on VSC, IAR, and Keil, respectively

- [based on VSC](AliOS-Things-lorawanapp-@VSC)
- [based on IAR](AliOS-Things-lorawanapp-@IAR)
- [based on Keil](AliOS-Things-lorawanapp-@Keil)

## Code size comparison under different Toolchain

| Compiler    | Flash(KB) | RAM(KB) | Optimization Level |
| ----------- | --------- | ------- | ------------------ |
| gcc         | 61.26     | 6.67    | -Os                |
| iccarm(IAR) | 43.83     | 6.18    | -O3                |
| armcc(Keil) | 42.64     | 6.20    | -O3                |

*There are floating point operations in the LoRaWAN stack, but most MCU does not contain that capability, so software implementation is needed, and different compiler will generate different code.

## Summary

This article introduces a LoRaWAN example in AliOS Things. There maybe omissions or mistakes, and not perfect in details such as API and function package. The main purpose is to give you a general idea of usage of AliOS Things and LoRaWAN. Hope you can experience the convenience of AliOS Things and achieve the deployment and testing of LoRaWAN based on it.

