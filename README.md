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

### Quick Start

Please refer to [Quick Start](https://help.aliyun.com/document_detail/161037.html)

### Docs

AliOS Things Docs: [Documentation](https://help.aliyun.com/document_detail/161023.html)

### Contributing

Please refer to [Contributing Guideline](https://github.com/alibaba/AliOS-Things/wiki/contributing).

### Connecting to IoT Platform

AliOS Things can help you connect your devices to [Alibaba Cloud IoT platform](https://iot.console.aliyun.com/quick_start) quickly.

## Community

* [DingTalk Group](https://img.alicdn.com/tfs/TB1KgBhjXY7gK0jSZKzXXaikpXa-1170-818.png)
* [Alibaba Cloud IoT Community](https://developer.aliyun.com/group/aliiot)

## License

  AliOS Things is released under the [Apache 2.0 license](LICENSE)
