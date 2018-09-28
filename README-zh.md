# <img src="http://o7spigzvd.bkt.clouddn.com/aos-logo-compact-1502x272.png" height="60">

[![Join the chat at https://gitter.im/aliosthings/Lobby](https://img.shields.io/gitter/room/aliosthings/Lobby.svg?style=flat-square)](https://gitter.im/aliosthings/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[EN](./README.md) | 中文

AliOS Things发布于[2017年杭州云栖大会](https://yunqi.aliyun.com)， 是 AliOS 家族旗下的、面向IoT领域的、高可伸缩的物联网操作系统，于2017年10月20号宣布在[github](https://github.com/alibaba/AliOS-Things)上开源. 

## Architecture Overview

AliOS Things 支持多种CPU架构，包括：ARM，C-Sky，MIPS，rl78，rx600，xtensa等。同时AliOS Things也支持相当多的[开发板](./board/README.md).

从架构的角度来看，AliOS Things适配了分层架构和组件架构。从上到下，AliOS Things包括以下部分：

- BSP: 板级支持包，主要由SoC开发商开发和维护
- HAL: 硬件适配层，像是WiFi，UART等
- Kernel: 包括Rhino RTOS 内核，Yloop, VFS, KV Storage等
- Protocol Stack: 包括LwIP TCPIP 协议栈, uMesh mesh networking stack等
- Security: 包括TLS, TFS(Trusted Framework Service), TEE(Trusted Exexcution Environment)等
- AOS API: AliOS Things 提供给应用软件和中间件的API
- Middleware: 阿里巴巴增值和常用的物联网组件
- Examples: 手把手的示例代码，以及一些严格测试的应用程序，像是 [linkkitapp](./app/example/linkkitapp/README.md)

所有的模块都作为组件的形式存在，每一个组件都有自己的`.mk`文件，用来描述与其他组件的描述关系，应用程序可以很方便的通过这种形式选择需要的组件。


# 文档

AliOS Things文档主要在 [Github Wiki](https://github.com/alibaba/AliOS-Things/wiki).

### 贡献代码

请参考：[Contributing Guideline](https://github.com/alibaba/AliOS-Things/wiki/contributing).

### 命令行快速上手

使用ubuntu系统，参考 [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start) 可以快速上手AliOS Things

### IDE快速上手

AliOS Things使用 [vscode](https://code.visualstudio.com/) 作为IDE，支持Windows/MAC/Linux。
具体使用步骤请参考：[AliOS Things Studio](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio).

# 社区

  * [Technical Blog | 云栖社区](https://yq.aliyun.com/teams/184)
  * [Technical Forum | 开发者论坛](https://bbs.aliyun.com/thread/410.html)
  * [技术交流群(微信)](http://o7spigzvd.bkt.clouddn.com/qr_wechat_100+.jpeg) - 人数过多，请联系管理员手动拉入
  * [技术交流群(钉钉)](http://o7spigzvd.bkt.clouddn.com/qr_dingtalk_github.png)

# License

  AliOS Things 开源源码遵循 [Apache 2.0 license](LICENSE) 开源协议。
