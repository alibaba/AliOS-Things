@page linksdk linksdk

[更正文档](https://gitee.com/alios-things/linksdk/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

Link SDK由阿里云提供给设备厂商，由设备厂商集成到设备上后通过该SDK将设备安全地接入到阿里云IoT物联网平台，继而让设备可以被阿里云IoT物联网平台进行管理。设备需要支持TCP/IP协议栈才能集成Link SDK。另外Zigbee、433、KNX这样的非IP设备需要通过网关设备接入到阿里云IoT物联网平台，网关设备需要集成Link SDK。

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
- OTA(在AliOS THings场景建议使用OS自带OTA)

更多详情，请参考阿里云Link SDK[说明文档](https://help.aliyun.com/document_detail/163755.html?spm=a2c4g.11186623.6.558.38557748p9kUy6) 。

## 版权信息
> Apache 2.0 License

## 目录结构
```tree
.
├── ChangeLog.md           #变更日志
├── components
│   ├── bootstrap          #设备引导服务
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

## 依赖组件

* osal_aos
* cjson
* mbedtls

# 常用配置
无

# API说明

请参考阿里云Link SDK[编程手册](https://help.aliyun.com/document_detail/163764.html?spm=a2c4g.11186623.6.568.3d00316fqDi9YJ)。

# 使用示例
solutions中提供了以下demo供用户选择使用：
- [link_sdk_demo 物模型单品用例](https://g.alicdn.com/alios-things-3.3/doc/linksdk_demo.html)
- [link_sdk_gateway_demo 物模型网关代理子设备上云用例](https://g.alicdn.com/alios-things-3.3/doc/linksdk_gateway_demo.html)


## 添加组件
默认仅使能能物模型及网关子设备功能。如需编译其他功能，请在package.yaml中包含对应的头文件路径及源码编译路径。如需使能动态注册：
```yaml
source_file:
  - "components/dynreg/*.c"

include:
  - components/dynreg

```
# FAQ

常见问题可参考阿里云官网设备接入[常见问题](https://help.aliyun.com/document_detail/96598.html?spm=a2c4g.11186623.6.554.2042557fWpe2Ps)。
