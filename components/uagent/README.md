@page uagent uagent

# 概述
uagent是AliOS Things的模块，支持其他客户模块(ulog，cli，ota，uND)将其服务注册到其中。 从云(面向维护平台)的服务调用可以通过uagent调用设备的服务，uagent还可以帮助客户模块发送信息，以帮助显示其工作状态。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
uagent/
├── doc
│   └──README.md
├── example
│   └──uagent_example.c
├── include
│   ├── uagent_config.h
│   ├── uagent_type.h
│   └── uagent.h
├── internal
│   └── uagent_inner_api.h
├── src
│   ├── uagent_comm_mqtt.c
│   ├── uagent_core.c
│   ├── uagent_send.c
│   └── uagent_utility.c
└── package.yaml
```

## 依赖组件
* linksdk

# 常用配置

# API说明
@ref uagent_init
@ref uagent_mqtt_client_set
@ref uagent_ext_comm_start
@ref uagent_register
@ref uagent_unregister
@ref uagent_request_service
@ref uagent_send


# 使用示例
示例代码参考example/uagent_example.c，以运行linkitsdk_demo为例，具体步骤如下：

## 添加示例代码
> uagent组件的package.yaml中添加example示例代码
```sh
source_file:
  - "src/uagent_comm_mqtt.c"
  - "src/uagent_core.c"
  - "src/uagent_send.c"
  - "src/uagent_utility.c"
  - "example/uagent_example.c"
```
> linkitsdk_dmeo solutions的package.yaml中添加依赖组件uagent组件
```sh
depends:                                       # 该组件依赖其他的组件
  - linksdk: dev_aos
  - haas100: dev_aos
  - mbedtls: dev_aos
  - netmgr: dev_aos
  - uagent: dev_aos   #添加uagent组件
```
>在data_model_basic_demo.c中修改三元组信息为阿里云物联网平台创建的设备信息
```c
    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a1jMbw3BI9t";
    char *device_name       = "HAAS100SDFASDFAS";
    char *device_secret     = "61eda487b3ffcfa7d08eae012e40ba28";
```

>在data_model_basic_demo.c中添加uagent设置mqtthandle，在连接上阿里云物联网平台(MQTT)后添加
```c
    uagent_mqtt_client_set(mqtt_handle); //设置uagent用于连接mqtt的handle，必须在linksdk连接上云后设置
    uagent_ext_comm_start(product_key, device_name);  //设置uagent的PK与DN，与linksdk中的pk，dn一致
    /* 主循环进入休眠 */
    while (1) {
        aos_msleep(1000);
    }
```

## 编译
```sh
cd solutions/linksdk_demo && aos make
```
## 烧录固件
> 参考具体板子的快速开始文档。

## uagent示例测试

### CLI命令行输入：
>连接网络
```sh
netmgr -t wifi -c <wifi_ssid> <wifi_pwd>
```
> CLI关键日志：
```sh
[49.030][LK-0313] MQTT connect success in 202 ms
```
### CLI命令行输入：
> uagent测试cli
```sh
uagent_example
```
# FAQ
