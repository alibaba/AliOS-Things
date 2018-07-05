[EN](AliOS-Things-Porting-Guide) | 中文

如下图，AliOS Things是一个分层+组件架构，可以较简单的移植到各类MCU及Connectivity SoC平台上。
![block_digram](https://img.alicdn.com/tfs/TB1fKQMihrI8KJjy0FpXXb5hVXa-2330-1292.png)


## 典型的移植流程
### CPU支持
这部分工作一般由AliOS Things内核团队完成，主要是各类CPU架构如ARM Cortex-M系列，C-Sky 80x系列的支持。
同时也欢迎社区爱好者及各类合作伙伴根据[Rhino Porting Guide](AliOS-Things-Rhino-Porting-Guide)支持新的CPU，并贡献到github。

### BSP/HAL
这部分工作主要是芯片的驱动开发，以及和AliOS Things HAL的对接。
请参考[HAL Porting Guide](AliOS-Things-HAL-Porting-Guide)了解进一步细节。

### CLI
CLI是AliOS Things的交互命令行组件，辅助开发者调试开发，强烈建议作为移植的第一个里程碑。<br>
CLI依赖于UART HAL，当移植完成后，开发者可以通过命令行和AliOS Things交互，查看系统状态。

### 网络
根据芯片平台的连接能力，对于可以接入IP网络的设备
1. 内置连接能力的芯片，比如WiFi，对接[WiFi HAL](AliOS-Things-WiFi-Porting-Guide)
  及[LwIP协议栈](AliOS-Things-LwIP-Porting-Guide)
2. 外挂通信模块（TCPIP协议栈运行在通信模块上），可以通过[SAL](https://github.com/alibaba/AliOS-Things/tree/master/device/sal/README.md) 提供一个标准的Socket环境

对于非IP类设备，请参考：
- LoRaWAN
- BLE

关于AliOS Things的网络架构，请参考[云栖技术博客](https://yq.aliyun.com/articles/327862)。

### 更高级能力
包括FOTA，多BIN支持等，请参考附录
