@page linksdk linksdk

# 概述

Link SDK由阿里云提供给设备厂商，由设备厂商集成到设备上后通过该SDK将设备安全地接入到阿里云IoT物联网平台，继而让设备可以被阿里云IoT物联网平台进行管理。设备需要支持TCP/IP协议栈才能集成Link SDK，Zigbee、433、KNX这样的非IP设备需要通过网关设备接入到阿里云IoT物联网平台，网关设备需要集成Link SDK。

说明:Link SDK以前名称为Link Kit SDK，现更名为Link SDK。

该组件支持以下功能：
- MQTT连云
- HTTP连云
- 设备认证
- 物模型
- 时间同步
- RRPC
- 设备连接异常告警
- 日志上报
- 设备引导服务
- 子设备管理
- 设备诊断
- 设备取证
- OTA(在AliOS THings场景建议使用OS自带OTA)

更多详情，请参考阿里云Link SDK[说明文档](https://help.aliyun.com/document_detail/163755.html?spm=a2c4g.11186623.6.558.38557748p9kUy6) 。

# 版权信息
> Apache 2.0 License

# 目录结构
```sh
.
├── ChangeLog.md           #变更日志
├── components
│   ├── bootstrap          #设备引导服务
│   ├── das                #设备取证
│   ├── data-model         #物模型
│   ├── devinfo            #设备信息
│   ├── diag               #设备诊断
│   ├── dynreg             #基于HTTP动态注册
│   ├── dynreg-mqtt        #基于MQTT的动态注册
│   ├── logpost            #日志上报
│   ├── ntp                #ntp时间
│   ├── ota                #ota功能
│   ├── shadow             #设备影子
│   └── subdev             #子设备
├── core
│   ├── aiot_http_api.c    #HTTP核心api实现
│   ├── aiot_http_api.h    #HTTP对外头文件
│   ├── aiot_mqtt_api.c    #MQTT核心api实现
│   ├── aiot_mqtt_api.h    #MQTT对外头文件
│   ├── aiot_state_api.c   #状态码
│   ├── aiot_state_api.h   #状态码头文件
│   ├── aiot_sysdep_api.h  #系统相关头文件
│   ├── README.md
│   ├── sysdep             #系统相关实现
│   └── utils              #工具类接口
├── external
│   ├── ali_ca_cert.c      #cert证书
│   └── README.md
├── package.yaml           #编译文件
├── portfiles
│   ├── aiot_port          #AliOS Things适配层
│   └── README.md
└── README.md
```

# 依赖组件

* osal_aos
* cjson
* mbedtls

# 常用配置
无

# API说明
@ref linksdk API
请参考阿里云Link SDK[编程手册](https://help.aliyun.com/document_detail/163764.html?spm=a2c4g.11186623.6.568.3d00316fqDi9YJ)。

# 使用示例
solutions中提供了以下demo供用户选择使用：
- link_sdk_demo 物模型单品用例
- link_sdk_gateway_demo 物模型网关代理子设备上云用例

本文以link_solo_demo为例讲解快速接入。

## 调试设备创建
在https://iot.console.aliyun.com/创建自己的产品及设备。
详细参考[本文](https://help.aliyun.com/document_detail/163756.html?spm=a2c4g.11186623.6.559.af29557f0aLXPC#title-hod-eo1-u22)。

## 修改设备端三元组
将上一步骤中创建的设备三元组信息填写到solutions/link_solo_demo/data_model_basic_demo.c的313-316行处：
```sh
    /* TODO: 替换为自己设备的三元组 */
    char *product_key       = "a18wPzZJzNG";
    char *device_name       = "data_model_basic_demo";
    char *device_secret     = "uwMTmVAMnGGHaAkqmeDY6cHxxBMTP3TD";
```
## 编译
```sh
cd solutions/link_solo_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make link_solo_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## 示例测试
> CLI命令行输入联网命令：
```sh
netmgr -t wifi -c ssid password
```
注意：ssid和password需要替换成自己的路由的ssid及密码

连上路由器后会自动连接到阿里云物联网平台。

## 关键日志
> CLI日志：
```sh
success to establish mbedtls connection, fd = 3(cost 0 bytes in total, max used 0 bytes)
[104.526][LK-0313] MQTT connect success in 1908 ms
AIOT_MQTTEVT_CONNECT
```

# FAQ

常见问题可参考阿里云官网设备接入[常见问题](https://help.aliyun.com/document_detail/96598.html?spm=a2c4g.11186623.6.554.2042557fWpe2Ps)。
