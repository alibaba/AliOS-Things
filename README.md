# <img src="http://o7spigzvd.bkt.clouddn.com/aos-logo-compact-1502x272.png" height="60">

[![Join the chat at https://gitter.im/aliosthings/Lobby](https://img.shields.io/gitter/room/aliosthings/Lobby.svg?style=flat-square)](https://gitter.im/aliosthings/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![](https://img.shields.io/travis/alibaba/AliOS-Things/master.svg?style=flat-square)](https://travis-ci.org/alibaba/AliOS-Things)

EN | [中文](./README-zh.md)

AliOS Things is Alibaba's IoT version of AliOS Family, it was announced in [The Computing Conference 2017](https://yunqi.aliyun.com) in Hangzhou by Alibaba Cloud, and open sourced in 20<sup>th</sup>, October, 2017 at github:https://github.com/alibaba/AliOS-Things.

## Architecture Overview

AliOS Things supports multiple architectures, including ARM, C-Sky, MIPS, rl78, rx600 and xtensa, AliOS Things also supports a large number of [boards](./board/README.md).

From an architectural point of view, AliOS Things adapts Layered Architecture and Component Architecture. From bottom to top, AliOS Things includes:

- BSP: Board Support Package mainly developed and maintained by SoC Vendor
- HAL: Hardware Abstraction Layer, like WiFi, UART
- Kernel: Rhino RTOS Kernel, Yloop, VFS, KV Storage included
- Protocol Stack: LwIP TCPIP Stack, uMesh mesh networking stack included
- Security: TLS, TFS(Trusted Framework Service), TEE(Trusted Exexcution Environment)
- AOS API: AliOS Things exposed APIs for Application and Middleware
- Middleware: Alibaba's value-added and commonly seen IoT components included
- Examples: hands-on sample codes, and well tested applications such as [linkkitapp](./example/linkkitapp/README.md)

All modules have been organized as Components, and each component has its own .mk file to describe its dependency with other Components, which enables applications to choose components needed easily.

## Documentation

AliOS Things wiki: [Github Wiki](https://github.com/alibaba/AliOS-Things/wiki).
AliOS Things documentation: [Alibaba Cloud IoT Documentation](https://dev.iot.aliyun.com/doc) | AliOS Things

### How to contribute

Please refer to [Contributing Guideline](https://github.com/alibaba/AliOS-Things/wiki/contributing).

### Quick Start by Command Line using Ubuntu Machine

Please refer to [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start).

### Quick Start by IDE(supporting Windows/MAC/Linux)

Please refer to [AliOS Studio](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio).

### IoT Platform

AliOS Things can help you connect your devices to [Alibaba Cloud IoT platform](https://iot.console.aliyun.com/quick_start) more quickly.

## Community

* [DingTalk Group](https://img.alicdn.com/tfs/TB1X2HOhYPpK1RjSZFFXXa5PpXa-970-1280.png)
* [Alibaba Cloud IoT Community](https://dev.iot.aliyun.com/)

## License

  AliOS Things is released under the [Apache 2.0 license](LICENSE)