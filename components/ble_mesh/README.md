# Chang log

20210514
1. 新增Mesh Controlled Relay特性(根据周围邻居节点Relay状态决策是否开启自身Relay功能，以达到消减网络泛洪的目的)。
2. 修改RPL机制，当队列满时覆盖最早写入队列的元素，即最近有消息的节点优先留在队列中。
3. 修复Genie Service中genie OTA bug。

# 概述

Bluetooth Mesh Profile是一种基于BLE（Bluetooth low energy）的Mesh网络解决方案。它使用了一种称为managed-flood-based的技术，Bluetooth  Mesh 使用BLE广播信道传输消息，任意一个节点都可以接收或者转发相同网络里的数据，这样大大扩大了消息的传输范围，使能大规模的网络部署成为可能。

本组件实现了Bluetooth Mesh Profile 1.0版本定义的规范，提供自bearer layer 到 Model layer各个层级的功能。特性支持如下

- Bluetooth Mesh配网，PB-ADV，PB-GATT
- relay特性
- proxy特性
- friend特性
- LPN特性
- Configuration Client  Model & Configuration Server Model
- Mesh Controlled Relay特性


## 组件安装

```bash
yoc init
yoc install ble_mesh
```

# 配置

本组件的默认配置位于configs\ble_mesh_default_config.h文件中，对于开发者，不需要修改默认值，如果有修改的需求，需要将新的定义配置到相应Solution中的package.yaml文件，新的配置将覆盖默认值。

下面是本组件的一些配置选项

| 配置                             | 默认值 | 说明                                                         |
| -------------------------------- | ------ | ------------------------------------------------------------ |
| CONFIG_BT_MESH                   | 1      | 本组件功能宏，引入该组件后默认定义                           |
| CONFIG_BT_MESH_PB_ADV            | 1      | 启用PB-ADV配网功能，启用后支持通过蓝牙广播进行配网           |
| CONFIG_BT_MESH_PB_GATT           | 0      | 启用PB-GATT配网功能，启用后支持BLE GATT进行配网，通常需要支持手机来配网时，需要启用该功能 |
| CONFIG_BT_MESH_SUBNET_COUNT      | 1      | 默认子网数量为1                                              |
| CONFIG_BT_MESH_APP_KEY_COUNT     | 1      | 默认应用密钥数量为1                                          |
| CONFIG_BT_MESH_MODEL_KEY_COUNT   | 1      | 默认Model可绑定密钥数量为1                                   |
| CONFIG_BT_MESH_MODEL_GROUP_COUNT | 1      | 默认Model可绑定组地址数量为1                                 |
| CONFIG_BT_MESH_CRPL              | 10     | 默认RPL缓存数量为10条                                        |
| CONFIG_BT_MESH_MSG_CACHE_SIZE    | 10     | 默认底层消息缓存数量为10条                                   |
| CONFIG_BT_MESH_ADV_BUF_COUNT     | 9      | 默认Mesh广播可用资源数为10                                   |
| CONFIG_BT_MESH_TX_SEG_MSG_COUNT  | 1      | 默认并发消息发送数为1。如果同时发送消息超过该配置，将会返回错误。 |
| CONFIG_BT_MESH_RX_SEG_MSG_COUNT  | 1      | 默认并发消息接收数为1。                                      |
| CONFIG_BT_MESH_RX_SDU_MAX        | 72     | 默认最大消息收包长度72字节                                   |
| CONFIG_BT_MESH_TX_SEG_MAX        | 6      | 默认消息可分包数位6。注意CONFIG_BT_MESH_TX_SEG_MAX 至少需要比 CONFIG_BT_MESH_ADV_BUF_COUNT 小3 |
| CONFIG_BT_MESH_RELAY             | 0      | 启用Mesh Relay特性                                           |
| CONFIG_BT_MESH_CTRL_RELAY        | 0      | 启用Mesh Controlled Relay特性                                |
| CONFIG_BT_MESH_LOW_POWER         | 0      | 启用Mesh LPN特性                                             |
| CONFIG_BT_MESH_FRIEND            | 0      | 启用Mesh Friend特性                                          |
| CONFIG_BT_MESH_CFG_CLI           | 0      | 启用Mesh Configuration Client Model                          |
| CONFIG_BT_MESH_HEALTH_CLI        | 0      | 启用Mesh Health Client Model                                 |
| CONFIG_BT_MESH_SHELL             | 0      | 启用Mesh Shell调试命令                                       |
| CONFIG_BT_MESH_DEBUG             | 0      | 启用Mesh调试功能，该配置打开后，下面的调试宏才有效           |
| CONFIG_BT_MESH_DEBUG_NET         | 0      | 启用Mesh Network层调试                                       |
| CONFIG_BT_MESH_DEBUG_BEACON      | 0      | 启用Mesh Beacon层调试                                        |
| CONFIG_BT_MESH_DEBUG_CRYPTO      | 0      | 启用Mesh 加密层调试                                          |
| CONFIG_BT_MESH_DEBUG_PROV        | 0      | 启用Mesh 配网层调试                                          |
| CONFIG_BT_MESH_DEBUG_ACCESS      | 0      | 启用Mesh 接入层调试                                          |
| CONFIG_BT_MESH_DEBUG_MODEL       | 0      | 启用Mesh Beacon层调试                                        |
| CONFIG_BT_MESH_DEBUG_ADV         | 0      | 启用Mesh 广播层调试                                          |
| CONFIG_BT_MESH_DEBUG_LOW_POWER   | 0      | 启用Mesh LPN层调试                                           |
| CONFIG_BT_MESH_DEBUG_FRIEND      | 0      | 启用Mesh Friend层调试                                        |
| CONFIG_BT_MESH_DEBUG_PROXY       | 0      | 启用Mesh 代理层调试                                          |
| CONFIG_BT_MESH_DEBUG_SETTINGS    | 0      | 启用Mesh 存储调试                                            |

# 接口

本组件对外头文件位于components\ble_mesh\bt_mesh\core\inc\目录，通过#include <api/mesh.h>进行引用。其中API相关说明，参考《[MESH API](docs/mesh_api/MESH_Module_API.md)》文档

# 示例

应用示例参考Solutions中mesh_xxx相关示例程序。

# 错误码

MESH协议栈错误请参考 bt_errno.h 中定义

HCI底层返回错误码参考 hci_err.h 中定义 BT_HCI_ERR_XXX

# 运行资源

不同方案下资源消耗参考

| Solution              | Code size/Bytes | Ro data/Bytes | RW Data/Bytes | BSS/Bytes |
| ---------------------------- | --------------- | ------------- | ------------- | --------- |
| mesh_light_node_demo  | 55606           | 7605          | 2361          | 7128      |
| mesh_switch_node_demo | 42196           | 4150          | 1837          | 4498      |
| mesh_shell            | 53638           | 10412         | 1752          | 4242      |
| genie_mesh_light_ctl  | 38438           | 3004          | 1277          | 2970      |
| genie_mesh_node_ctl   | 38498           | 3032          | 1277          | 2970      |
| genie_mesh_switches   | 38442           | 3004          | 1277          | 2970      |

# 依赖资源

- ble_host
