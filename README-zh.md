<br/>
<div align="center">
  <img src="https://img.alicdn.com/tfs/TB1e1U7vyAnBKNjSZFvXXaTKXXa-973-200.png" height="60">
</div>
<br/>
<div align="center">

[![Join the chat at https://gitter.im/aliosthings/Lobby](https://img.shields.io/gitter/room/aliosthings/Lobby.svg?style=flat-square)](https://gitter.im/aliosthings/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

</div>
<br/>

[EN](./README.md) | 中文

AliOS Things发布于[2017年杭州云栖大会](https://yunqi.aliyun.com)， 是 AliOS 家族旗下的、面向IoT领域的、高可伸缩的物联网操作系统，于2017年10月20号宣布在[github](https://github.com/alibaba/AliOS-Things)上开源. 

## 架构总览

AliOS Things 支持多种CPU架构，包括：ARM，C-Sky，MIPS，RISCV，rl78，rx600，xtensa等。AliOS Things认证的芯片和模组数量有200+，认证的传感器数量有100+。

AliOS Things适配了分层架构和组件架构。包括以下部分：

- BSP: 板级支持包
- HAL: 硬件适配层，包括WiFi，UART，Flash 等
- Kernel: 包括Rhino RTOS 内核，VFS，KV Storage，CLI，C++ 等
- Network: 包括LwIP 轻量级TCP/IP协议栈，uMesh 自组网协议栈，BLE 低功耗蓝牙协议栈，LoRaWAN 协议栈，AT Commands Module 等
- Security: 包括TLS(mbedTLS and cutomized iTLS)，ID2，SST(Trusted Storage)，Crypto，TEE(Trusted Execution Environment) 等
- AOS API: AliOS Things 提供给应用软件和中间件的API
- Middleware: 阿里巴巴增值和常用的物联网组件，包括Linkkit，OTA(安全差分升级)，ulog(日志服务)，uData(传感器框架)，uLocation(定位框架)，WiFi配网 等
- Application: 丰富的示例代码

所有的模块都作为组件的形式存在，通过menuconfig配置界面进行配置，应用程序可以很方便的选择需要的组件。

## 文档

- AliOS Things wiki: [Github Wiki](https://github.com/alibaba/AliOS-Things/wiki)
- AliOS Things 文档: [开发者社区文档中心](https://dev.iot.aliyun.com/doc) | 基础产品 AliOS Things 专栏

### 贡献代码

请参考：[Contributing Guideline](https://github.com/alibaba/AliOS-Things/wiki/contributing).

### 命令行快速上手

使用ubuntu系统，参考 [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start) 可以快速上手AliOS Things

### IDE 快速上手

AliOS Things使用 [vscode](https://code.visualstudio.com/) 作为IDE，支持Windows/MAC/Linux。
具体使用步骤请参考：[AliOS Studio](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio).

### 物联网平台

AliOS Things 能帮助你更加快速地接入[阿里云物联网平台](https://iot.console.aliyun.com/quick_start)。

## 社区

* [技术交流群(钉钉)](https://img.alicdn.com/tfs/TB1KgBhjXY7gK0jSZKzXXaikpXa-1170-818.png)
* [阿里云IoT开发者社区](https://developer.aliyun.com/group/aliiot)

## License

  AliOS Things 开源源码遵循 [Apache 2.0 license](LICENSE) 开源协议。